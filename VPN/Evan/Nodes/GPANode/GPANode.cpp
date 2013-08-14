#include "GPANode.h"
#include "../../Utilities/Logger.h"
#include <QFileDialog>
#include <QDateTime>
#include <QSettings>

#include <tps.h>
#include <svd.h>

using namespace std;

GPANode::GPANode(QWidget* parent) : QDialog(parent),  INode()
{
    m_specimensInput = NULL;
    m_surfaceInput = NULL;

    m_procCoordinatesOutput = new Specimens();
    m_transformsOutput = new TransformMatrix();
    m_residualsOutput = new Specimens();
    m_procDistancesOutput = new Variables();
    m_centroidSizes = new CentroidSizes();
    m_meanSpecimen = new LandmarkSet();
    m_uniformBasis = new Specimens();
    m_transformedSurface = new Surface();
    m_registeredSurface = new Surface();
    m_shapeVectors = new Variables();
    m_formVectors = new Variables();

    setupUi(this);

    // set alignment
    QObject::connect(radioButton, SIGNAL(clicked()), this, SLOT(controlsChanged()));
    QObject::connect(radioButton_2, SIGNAL(clicked()), this, SLOT(controlsChanged()));
    QObject::connect(radioButton_3, SIGNAL(clicked()), this, SLOT(controlsChanged()));

    // set scaling, rotation, translation and reflection flags
    QObject::connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(checkBox_2, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(checkBox_3, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(checkBox_4, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(checkBox_5, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(checkBox_8, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));

    // set method for calculating distances
    QObject::connect(radioButton_4, SIGNAL(clicked()), this, SLOT(controlsChanged()));
    QObject::connect(radioButton_6, SIGNAL(clicked()), this, SLOT(controlsChanged()));
    QObject::connect(radioButton_7, SIGNAL(clicked()), this, SLOT(controlsChanged()));
    QObject::connect(radioButton_5, SIGNAL(clicked()), this, SLOT(controlsChanged()));
    QObject::connect(radioButton_8, SIGNAL(clicked()), this, SLOT(controlsChanged()));

    // set distances as pairwise or not between all fitted specimens
    QObject::connect(checkBox_6, SIGNAL(clicked()), this, SLOT(togglePairwise()));
    QObject::connect(checkBox_7, SIGNAL(clicked()), this, SLOT(togglePairwise()));

    QObject::connect(pushButton, SIGNAL(clicked()), this, SLOT(browseOutputFile()));
    QObject::connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(controlsChanged()));
    QObject::connect(lineEdit_2, SIGNAL(textChanged(const QString&)), this, SLOT(controlsChanged()));
    QObject::connect(lineEdit_3, SIGNAL(textChanged(const QString&)), this, SLOT(controlsChanged()));

    m_updated = true;
    m_pairwise = true;

    lineEdit->setText("100");
    lineEdit_2->setText("1e-12");
    m_maxiter = 100;
    m_mintol = 1e-12;
}

void
GPANode::CalculateGPA()
{
    int alignment = 0;
    if (radioButton->isChecked())
        alignment = 1;  // align with principal axes
    else if (radioButton_2->isChecked())
        alignment = 2;  // align with first specimen
    else if (radioButton_3->isChecked())
        alignment = 3;  // align with plane of symmetry

    m_gpa.SetMaxIterations(m_maxiter);
    m_gpa.SetMinTolerance(m_mintol);

    bool scale = checkBox->isChecked();
    bool rotate = checkBox_2->isChecked();
    bool translate = checkBox_3->isChecked();
    bool reflect = checkBox_4->isChecked();

    vector<int> midline;

    if (alignment==3)
    {
        QVector<QPair<int,int> > pairs = m_specimensInput->getLandmarkPairs();
        if (pairs.empty())
        {
            Logger::getInstance()->log("No symmetry data available, aligning with principal axes", Logger::WARNING);
            alignment = 1;
        }
        else
        {
            for (unsigned int i=0;i<m_specimensInput->getLandmarkCount();i++)
            {
                midline.push_back(i+1);
            }
            for (int j=0;j<pairs.size();j++)
            {
                vector<int>::iterator endIt = midline.end();
                for ( vector<int>::iterator it = midline.begin(); it != endIt; ++it )
                {
                    if (*it==pairs[j].first || *it==pairs[j].second)
                    {
                        it = midline.erase(it);
                    }

                }
            }
            if (midline.empty())
            {
                Logger::getInstance()->log("[GPA Node] No unpaired landmarks, aligning with principal axes", Logger::WARNING);
                alignment = 1;
            }
        }
    }
    m_gpa.PerformGPA(alignment, scale, rotate, translate, reflect, midline);
}

void GPANode::process()
{
    m_specimensInput = getInputPortData0();
    m_surfaceInput = getInputPortData1();

    if (m_specimensInput == NULL || !m_specimensInput->isValid())
        return;

    Logger::getInstance()->log("[GPA Node] Processing started");

    int individuals = m_specimensInput->getSize();
    int landmarks = m_specimensInput->getLandmarkCount();
    int dimensions = m_specimensInput->getLandmarkDimensions();

    if (landmarks < dimensions)
    {
        Logger::getInstance()->log("[GPA Node] Landmarks need to be at least as many as number of dimensions", Logger::RUN_ERROR);
        return;
    }

    for (int i=0;i<individuals;i++)
    {
        if (!m_specimensInput->getLandmarkSet(i)->isComplete())
        {
            Logger::getInstance()->log("[GPA Node] Missing data in landmark set", Logger::RUN_ERROR);
            m_updated = false;
            return;
        }
    }
    Matrix<double> data(landmarks * individuals, dimensions);
    for (int i=0;i<individuals;i++)
    {
        LandmarkSet *lands = m_specimensInput->getLandmarkSet(i);
        for (int j=0;j<landmarks;j++)
        {
            for (int k=0;k<dimensions;k++)
            {
                data[i*landmarks+j][k] = (*lands)[j][k];
            }
        }
    }

    m_gpa.LoadData(&data, individuals, landmarks, dimensions);

    CalculateGPA();

    m_procCoordinatesOutput->clear();
    m_residualsOutput->clear();

    m_meanSpecimen->ReSize(landmarks,dimensions);
    m_gpa.GetProcrustesMean(m_meanSpecimen);

    for (int i=0;i<individuals;i++)
    {
        Matrix<double> proccoords;
        LandmarkSet lm(landmarks,dimensions);
        LandmarkSet res(landmarks,dimensions);

        m_gpa.GetProcrustesCoordinates(i,&proccoords);

        for (int j=0;j<landmarks;j++)
        {
            for (int k=0;k<dimensions;k++)
            {
                lm[j][k] = proccoords[j][k];
                res[j][k] = proccoords[j][k] - (*m_meanSpecimen)[j][k];
            }
        }
        m_procCoordinatesOutput->addLandmarkSet(new LandmarkSet(lm));
        m_residualsOutput->addLandmarkSet(new LandmarkSet(res));
    }

    m_procCoordinatesOutput->setSpecimenLabels(m_specimensInput->getSpecimenLabels());
    m_procCoordinatesOutput->setLandmarkLabels(m_specimensInput->getLandmarkLabels());
    m_residualsOutput->setSpecimenLabels(m_specimensInput->getSpecimenLabels());
    m_residualsOutput->setLandmarkLabels(m_specimensInput->getLandmarkLabels());

    m_centroidSizes->clearSpecimenGroups();
    m_procDistancesOutput->clearSpecimenGroups();

    for (unsigned int i=0; i<m_specimensInput->getGroupNumber(); ++i)    //Add Groups
    {
        SpecimenGroup* sg = m_specimensInput->getGroup(i);
        SpecimenGroup* newGroup = new SpecimenGroup;
        SpecimenGroup* newGroup2 = new SpecimenGroup;
        if(sg!=0)
        {
            m_centroidSizes->addSpecimenGroup(m_specimensInput->getGroupName(i), m_specimensInput->getGroup(i));
            m_procDistancesOutput->addSpecimenGroup(m_specimensInput->getGroupName(i), m_specimensInput->getGroup(i));
        }
        for (unsigned int j=0; j<sg->getSize(); ++j)
        {
            unsigned int specimenIndex = sg->getMember(j)->getSpecimenIndex();
            if (specimenIndex == m_specimensInput->getSize())
            {
                newGroup->addMean(m_specimensInput->getSize(),
                                  sg->getMemberID(j),
                                  sg->getMemberColor(j),
                                  sg->getMemberViewerSize(j),
                                  sg->getMemberPlotSymbol(j));
                newGroup2->addMean(m_specimensInput->getSize(),
                                   sg->getMemberID(j),
                                   sg->getMemberColor(j),
                                   sg->getMemberViewerSize(j),
                                   sg->getMemberPlotSymbol(j));
            }
            else
            {
                newGroup->addMember(  m_procCoordinatesOutput->getLandmarkSet(specimenIndex),
                                      sg->getMemberID(j),
                                      sg->getMemberColor(j),
                                      sg->getMemberViewerSize(j),
                                      sg->getMemberPlotSymbol(j));
                newGroup2->addMember(  m_residualsOutput->getLandmarkSet(specimenIndex),
                                       sg->getMemberID(j),
                                       sg->getMemberColor(j),
                                       sg->getMemberViewerSize(j),
                                       sg->getMemberPlotSymbol(j));
            }
        }
        newGroup->setGroupColor(sg->getGroupColor());
        newGroup2->setGroupColor(sg->getGroupColor());
        newGroup->setGroupLMKSize(sg->getGroupLMKSize());
        newGroup2->setGroupLMKSize(sg->getGroupLMKSize());
        newGroup->setGroupPlotSymbol(sg->getGroupPlotSymbol());
        newGroup2->setGroupPlotSymbol(sg->getGroupPlotSymbol());

        m_procCoordinatesOutput->addGroup(m_specimensInput->getGroupName(i), newGroup);
        m_residualsOutput->addGroup(m_specimensInput->getGroupName(i), newGroup2);
    }

    m_procCoordinatesOutput->initialize("Specimens after GPA");
    m_residualsOutput->initialize("GPA Residuals");

    setOutputPort0(m_procCoordinatesOutput);
    setOutputPort2(m_residualsOutput);

    m_transformsOutput->clear();
    for (int i=0;i<individuals;i++)
    {
        Matrix<double>* temp = new Matrix<double>;
        m_gpa.GetTransformationMatrix(i,temp);
        m_transformsOutput->addTransformMatrix(temp);
    }

    setOutputPort1(m_transformsOutput);

    vector<double> cs;
    m_gpa.GetCentroidSizes(&cs);

    m_centroidSizes->ReSize(cs.size(),1);
    double meanCS = 0.0;
    for (unsigned int i=0;i<cs.size();i++)
    {
        m_centroidSizes->setRowLabel(i,m_specimensInput->getSpecimenLabelValue(i,"ID"));
        (*m_centroidSizes)[i][0] = cs[i];
        m_procCoordinatesOutput->setCentroidSize((int)i,(double)cs[i]);
        meanCS += cs[i];
    }
    if(cs.size()>0)
    {
        meanCS /= cs.size();
        m_meanSpecimen->setCentroidSize(meanCS);
    }

    setOutputPort4(m_centroidSizes);

    // Rohlf, F. J. 1999. Shape statistics: Procrustes superimpositions and tangent spaces. Journal of Classification, 16:197-223.
    // use orthogonal projection
    // reference is the mean specimen
    Matrix<double> tanSpecimen;
    Matrix<double> tancoords(individuals,landmarks*dimensions);

    for (int i=0;i<individuals;i++)
    {
        LandmarkSet* lm = m_procCoordinatesOutput->getLandmarkSet(i);

        Matrix<double> meanSpecimen = (*m_meanSpecimen);
        Matrix<double> *specimen = (Matrix<double> *)(lm);

        // Procrustes distance (rho) from reference to specimen
        Matrix<double> X1TX2 = Transpose(meanSpecimen) * (*specimen);

        // Do Singular Value Decomposition on the X1TX2 matrix
        Row<double> lambda = svd(X1TX2);
        double sum = 0.0;

        for (int k=0;k<dimensions;k++)
        {
            sum += lambda[k];
        }

        // sum of lambda is cos(rho)
        if (sum > 0.0)
        {

            // stereographic projection
//                tanSpecimen = (*specimen) / sum;

            // orthogonal projection
            tanSpecimen = (*specimen) + (*m_meanSpecimen) * (1.0 - sum);
        }
        else
        {
            tanSpecimen = (*specimen);
        }
        for (int j=0;j<landmarks;j++)
        {
            for (int k=0;k<dimensions;k++)
            {
                tancoords[i][j*dimensions+k] = tanSpecimen[j][k];
            }
        }
        if (checkBox_8->isChecked())
        {
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    (*lm)[j][k] = tanSpecimen[j][k];
                }
            }
        }
    }

    m_meanSpecimen->initialize("GPA Mean");
    setOutputPort5(m_meanSpecimen);

    Matrix<double> dist;
    bool formSpace = checkBox_5->isChecked();

    if (checkBox_6->isChecked())
    {
        if (radioButton_5->isChecked())
        {
            // euclidean distances
            m_gpa.GetProcrustesDistances(&dist,0,formSpace);
        }
        else
        {
            // tangent distances
            dist.ReSize(individuals,individuals);

            for (int i=0;i<individuals;i++)
            {
                for (int j=0;j<individuals;j++)
                {
                    double sum2 = 0.0;

                    for (int k=0;k<landmarks*dimensions;k++)
                    {
                        double diff = tancoords[i][k] - tancoords[j][k];
                        sum2 += diff * diff;
                    }
                    dist[i][j] = sqrt(sum2);
                }
            }
        }
    }
    else
    {
        if (radioButton_4->isChecked())
        {
            // procrustes distances (rho)
            m_gpa.GetProcrustesDistances(&dist,3,formSpace);
        }
        else if (radioButton_7->isChecked())
        {
            // partial procrustes distances (Dp)
            m_gpa.GetProcrustesDistances(&dist,2,formSpace);
        }
        else if (radioButton_6->isChecked())
        {
            // full procrustes distances (Df)
            m_gpa.GetProcrustesDistances(&dist,1,formSpace);
        }
    }

    m_procDistancesOutput->ReSize(dist.GetRows(),dist.GetCols());
    if(dist.GetRows() == dist.GetCols())
    {
        for (unsigned int i=0;i<dist.GetRows();i++)
        {
            for (unsigned int j=0;j<dist.GetCols();j++)
            {
                (*m_procDistancesOutput)[i][j] = dist[i][j];
            }
            m_procDistancesOutput->setColumnLabel(i,m_specimensInput->getSpecimenLabelValue(i,"ID"));
            m_procDistancesOutput->setRowLabel(i,m_specimensInput->getSpecimenLabelValue(i,"ID"));
        }
    }

    setOutputPort3(m_procDistancesOutput);

    if(isOutputConnected(6))
    {
        m_uniformBasis->clear();
        Matrix<double> uniform;
        m_gpa.GetUniformBasis(&uniform);

        SpecimenGroup* defaultGroup = new SpecimenGroup;
        for (unsigned int i=0;i<uniform.GetRows();i++)
        {
            LandmarkSet *uni = new LandmarkSet(landmarks,dimensions);
            int l=0;
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    (*uni)[j][k] = uniform[i][l++];
                }
            }
            m_uniformBasis->addLandmarkSet(new LandmarkSet(*uni));

            QString num;
            num.setNum(i+1);
            m_uniformBasis->setLabelValue(i, "ID", "Uniform " + num);
            defaultGroup->addMember(uni, "Uniform " + num);
        }

        m_uniformBasis->addGroup("Ungrouped", defaultGroup);
        m_uniformBasis->initialize("Uniform Basis");
        setOutputPort6(m_uniformBasis);
    }

    if (m_surfaceInput != NULL && m_surfaceInput->isValid())
    {
        int transformIndex = m_surfaceInput->getSpecimenNumber() - 1;

        if (transformIndex >= 0 && transformIndex < (int)m_transformsOutput->getSize() )
        {
            Matrix<double>* transform = m_transformsOutput->getTransformMatrix(transformIndex);

            m_transformedSurface->clear();
            m_transformedSurface->initialize(m_surfaceInput, "Transformed Surface");
            m_transformedSurface->applyTransform(*transform);
            m_transformedSurface->refreshGeometry();

            setOutputPort7(m_transformedSurface);

            LandmarkSet* m_sourceLMKInput = m_specimensInput->getLandmarkSet(transformIndex);
            LandmarkSet* m_targetLMKInput = m_meanSpecimen;

            TPS tps;
            tps.LoadData(m_sourceLMKInput,m_targetLMKInput);

            int tpsresult = tps.PerformTPS();
            if(tpsresult == 0)
            {
                Matrix<double>* pointOutMat = new Matrix<double>(m_surfaceInput->getVertices()->GetRows(),3);
                tps.WarpPoints(m_surfaceInput->getVertices()->GetMatrix(),pointOutMat,(int)m_surfaceInput->getVertices()->GetRows());

                m_registeredSurface->clear();
                m_registeredSurface->initialize(m_surfaceInput, "Mean Surface");
                m_registeredSurface->setVertices(pointOutMat);
                m_registeredSurface->refreshGeometry();

                setOutputPort8(m_registeredSurface);
            }
            else if(tpsresult == 1)
            {
                Logger::getInstance()->log("[GPA Node] Error warping surface to the mean. Too few landmarks", Logger::RUN_ERROR);
            }
            else if(tpsresult == 2)
            {
                Logger::getInstance()->log("[GPA Node] Error warping surface to the mean. Bad sizes", Logger::RUN_ERROR);
            }
            else if(tpsresult == 3)
            {
                Logger::getInstance()->log("[GPA Node] Error warping surface to the mean. Data is singular", Logger::RUN_ERROR);
            }
        }
        else
        {
            Logger::getInstance()->log("[GPA Node] Associated specimen not found for surface", Logger::RUN_ERROR);
        }
    }

    m_shapeVectors->ReSize(individuals,landmarks*dimensions);
    m_formVectors->ReSize(individuals,landmarks*dimensions+1);

    for (int i=0;i<individuals;i++)
    {
        LandmarkSet* lm = m_procCoordinatesOutput->getLandmarkSet(i);

        m_shapeVectors->setRowLabel(i,m_specimensInput->getSpecimenLabelValue(i,"ID"));
        m_formVectors->setRowLabel(i,m_specimensInput->getSpecimenLabelValue(i,"ID"));

        for (int j=0;j<landmarks;j++)
        {
            for (int k=0;k<dimensions;k++)
            {
                (*m_shapeVectors)[i][j*dimensions+k] = (*lm)[j][k];
                (*m_formVectors)[i][j*dimensions+k] = (*lm)[j][k];


                QString num;
                num.setNum(j*dimensions+k+1);

                m_shapeVectors->setColumnLabel(j*dimensions+k,"x"+num);
                m_formVectors->setColumnLabel(j*dimensions+k,"x"+num);
            }
        }
        (*m_formVectors)[i][landmarks*dimensions] = log((long double)m_procCoordinatesOutput->getCentroidSize(i));
        m_formVectors->setColumnLabel(landmarks*dimensions,"Ln(CS)");
    }

    setOutputPort9(m_shapeVectors);
    setOutputPort10(m_formVectors);

    if(lineEdit_3->text() != "")
    {
        PrintOutput();
    }
    m_updated = false;
    Logger::getInstance()->log("[GPA Node] Processing finished");
}

bool GPANode::isUpdated() const
{
    return m_updated;
}

void GPANode::accept()
{
    int iter = lineEdit->text().toInt();
    double tol = lineEdit_2->text().toDouble();
    if (iter > 0)
    {
        m_maxiter = iter;
    }
    else
    {
        QString itertext;
        itertext.setNum(m_maxiter);
        lineEdit->setText(itertext);
        Logger::getInstance()->log("[GPA Node] Bad value for max iterations",Logger::WARNING);
    }
    if (tol > 0.0)
    {
        m_mintol = tol;
    }
    else
    {
        QString toltext;
        toltext.setNum(m_mintol);
        lineEdit_2->setText(toltext);
        Logger::getInstance()->log("[GPA Node] Bad value for min tolerance",Logger::WARNING);
    }

    if (m_updated)
        emit dataReady();
    QDialog::accept();
}

void GPANode::reject()
{
    m_updated = false;
    fromString(m_settings);
    QDialog::reject();
}

QString GPANode::toString() const
{
    QString nodeData = "";

    if (checkBox->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (checkBox_2->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (checkBox_3->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (checkBox_4->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (checkBox_5->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (checkBox_6->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (checkBox_7->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (checkBox_8->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_2->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_3->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_4->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_5->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_6->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_7->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_8->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    nodeData += lineEdit->text();
    nodeData += "$$";
    nodeData += lineEdit_2->text();
    nodeData += "$$";
    nodeData += lineEdit_3->text();

    return  nodeData;
}

void GPANode::fromString(const QString& params)
{
    QStringList nodeData = params.split("$$");
    if (nodeData.count() > 17)
    {
        checkBox->setChecked(nodeData[0]=="1");
        checkBox_2->setChecked(nodeData[1]=="1");
        checkBox_3->setChecked(nodeData[2]=="1");
        checkBox_4->setChecked(nodeData[3]=="1");
        checkBox_5->setChecked(nodeData[4]=="1");
        checkBox_6->setChecked(nodeData[5]=="1");
        checkBox_7->setChecked(nodeData[6]=="1");
        checkBox_8->setChecked(nodeData[7]=="1");

        if(nodeData[6]=="1")
        {
            togglePairwise();
        }

        radioButton->setChecked(nodeData[8]=="1");
        radioButton_2->setChecked(nodeData[9]=="1");
        radioButton_3->setChecked(nodeData[10]=="1");
        radioButton_4->setChecked(nodeData[11]=="1");
        radioButton_5->setChecked(nodeData[12]=="1");
        radioButton_6->setChecked(nodeData[13]=="1");
        radioButton_7->setChecked(nodeData[14]=="1");
        radioButton_8->setChecked(nodeData[15]=="1");

        lineEdit->setText(nodeData[16]);
        lineEdit_2->setText(nodeData[17]);
        m_maxiter = nodeData[16].toInt();
        m_mintol = nodeData[17].toDouble();

        if (nodeData.count() == 19)
            lineEdit_3->setText(nodeData[18]);
    }
}


void GPANode::browseOutputFile()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = QFileDialog::getSaveFileName(this, tr("Select a file for GPA output"),
                       fsLastDir,
                       tr("All files (*)\nText files (*.txt)"));

    if (fileName != "" && !fileName.isEmpty())
    {
        lineEdit_3->setText(fileName);

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void GPANode::PrintOutput()
{
    QString fileName = lineEdit_3->text();
    QFile file(fileName);

    ofstream outfile(fileName.toStdString().c_str());

    if (outfile.fail())
    {
        Logger::getInstance()->log("[GPA Node] Error opening output file: " + fileName, Logger::RUN_ERROR);
        return;
    }

    //get current date
	QDate date = QDate::currentDate();
	QString dateString = date.toString();

	//get current time
	QTime time = QTime::currentTime();
	QString timeString = time.toString();

	//get current date and time
	QDateTime dateTime = QDateTime::currentDateTime();
	QString dateTimeString = dateTime.toString();

    outfile<<"EVAN Toolbox"<<endl;
    outfile<<"Generalised Procrustes Analysis (GPA)"<<endl;

    outfile << endl;

    outfile << "Date:\t" << dateString.toStdString() << endl;
    outfile << "Time:\t" << timeString.toStdString() << endl;

    outfile << endl;

    outfile << "Number of individuals:\t" << m_specimensInput->getSize() << endl;
    outfile << "Number of landmarks:\t" << m_specimensInput->getLandmarkCount() << endl;
    outfile << "Number of dimensions:\t" << m_specimensInput->getLandmarkDimensions() << endl;

    outfile << endl;

    outfile << "Procrustes mean shape:" << endl;
    outfile << endl;
    outfile << (*m_meanSpecimen) << endl;

    outfile << "Procrustes distances:" << endl;
    outfile << endl;
    for(unsigned int i=0;i<m_specimensInput->getSize();i++)
    {
        outfile << "\t" << m_specimensInput->getSpecimenLabelValue(i,"ID").toStdString();
    }
    outfile << endl;
    for(unsigned int i=0;i<m_specimensInput->getSize();i++)
    {
        outfile << m_specimensInput->getSpecimenLabelValue(i,"ID").toStdString();
        for(unsigned int j=0;j<m_specimensInput->getSize();j++)
        {
            outfile << "\t" << (*m_procDistancesOutput)[i][j];
        }
        outfile << endl;
    }
    outfile << endl;
    outfile << "\tCentroid Sizes\tNatural Log of Centroid Sizes" << endl;
    for(unsigned int i=0;i<m_centroidSizes->GetRows();i++)
    {
        double lncs = log((long double)(*m_centroidSizes)[i][0]);
        outfile << m_specimensInput->getSpecimenLabelValue(i,"ID").toStdString() << "\t" << (*m_centroidSizes)[i][0] << "\t" << lncs << endl;
    }

    Logger::getInstance()->log("[GPA Node] Output written to file: " + fileName);
}

