#include "PCANode.h"
#include "../../Utilities/Logger.h"
#include <QFileDialog>
#include <QDateTime>
#include <sstream>
#include <QSettings>

#include "../../Labels/StringLabelValue.h"

#include <cmath> // for log()

PCANode::PCANode() : INode()
{
    m_specimensInput = NULL;

    m_eigenvalues = new Variables();
    m_loadingsOutput = new Loadings();
    m_pcscoreOutput = new PCScore();

    setupUi(this);
    QObject::connect(m_centerCheckBox, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(m_scaleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(radioButton, SIGNAL(clicked()), this, SLOT(controlsChanged()));
    QObject::connect(radioButton_2, SIGNAL(clicked()), this, SLOT(controlsChanged()));
    QObject::connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(controlsChanged()));
    QObject::connect(pushButton, SIGNAL(clicked()), this, SLOT(browseOutputFile()));

    m_updated = true;
}

PCANode::~PCANode()
{ } // need deletes

void PCANode::process()
{
    m_specimensInput = getInputPortData0();

    if( m_specimensInput == NULL || !m_specimensInput->isValid())
    {
        // no data connected
        return;
    }

    Logger::getInstance()->log("[PCA Node] Processing started");

    if(checkBox->isChecked())
    {
        // data is form
        // add a column of ln of centroid size
        if(m_specimensInput->getCentroidSizes().empty())
        {
            Logger::getInstance()->log("\tCentroid sizes not calculated yet", Logger::RUN_ERROR);
            m_updated = false;
            return;
        }

        int individuals = m_specimensInput->getSize();
        int landmarks = m_specimensInput->getLandmarkCount();
        int dimensions = m_specimensInput->getLandmarkDimensions();

        Matrix<double> data(individuals, landmarks * dimensions + 1);
        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *lands = m_specimensInput->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    data[i][j*dimensions+k] = (*lands)[j][k];
                }
                data[i][(j+1)*dimensions] = log(m_specimensInput->getCentroidSize(i));
            }
        }
        m_pca.LoadData(data.GetMatrix(), individuals, landmarks*dimensions+1);
    }
    else
    {
        // data is shape
        int individuals = m_specimensInput->getSize();
        int landmarks = m_specimensInput->getLandmarkCount();
        int dimensions = m_specimensInput->getLandmarkDimensions();

        Matrix<double> data(individuals, landmarks * dimensions);
        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *lands = m_specimensInput->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    data[i][j*dimensions+k] = (*lands)[j][k];
                }
            }
        }
        m_pca.LoadData(data.GetMatrix(), individuals, landmarks*dimensions);
    }

    m_pca.PerformPCA(m_centerCheckBox->isChecked(), m_scaleCheckBox->isChecked());

    vector<double> eigval;
    vector<vector<double> > eigvec;

    m_pca.GetEigenvalues(&eigval);

    if(eigval.size())
    {
        m_eigenvalues->ReSize(eigval.size(),1);
        m_eigenvalues->setColumnLabel(0,"Eigenvalues");
    }
    for (unsigned int i=0;i<eigval.size();i++)
    {
         stringstream sstr;
         sstr << "PC " << i+1;
         m_eigenvalues->setRowLabel(i,sstr.str().c_str());
        (*m_eigenvalues)[i][0] = eigval[i];
    }
    setOutputPort1(m_eigenvalues);

    Matrix<double> pcscores;
    m_pca.GetPCScores(&pcscores);

    int numgroupmeans = 0;
    for (unsigned int i=0;i<m_specimensInput->getGroupNumber();i++)
    {
        if(m_specimensInput->getGroup(i)->getMean() != 0)
        {
            numgroupmeans++;
        }
    }

    m_pcscoreOutput->ReSize(pcscores.GetRows()+numgroupmeans,pcscores.GetCols());

    for (unsigned int i=0;i<pcscores.GetRows();i++)
    {
        for (unsigned int j=0;j<pcscores.GetCols();j++)
        {
            (*m_pcscoreOutput)[i][j] = pcscores[i][j];
        }
    }

    for (unsigned int i=0;i<pcscores.GetCols();i++)
    {
        QString qstr;
        qstr.setNum(i+1);
        qstr = "PC " + qstr;
        m_pcscoreOutput->setColumnLabel(i,qstr);
    }
    for (unsigned int i=0;i<pcscores.GetRows();i++)
    {
        QString qstr = m_specimensInput->getSpecimenLabelValue(i,"ID");
        if(qstr=="")
        {
            qstr.setNum(i+1);
        }
        m_pcscoreOutput->setRowLabel(i,qstr);
    }
    //Add the specimen groups [A pointer to the group is passed. WARNING!! READ ONLY]
    m_pcscoreOutput->clearSpecimenGroups();

    if(m_specimensInput->getGroupNumber() == 1)
    {
        SpecimenGroup* group = m_specimensInput->getGroup(0);
        if(group!=0)
        {
            group->setGroupColor(Qt::black);
            m_pcscoreOutput->addSpecimenGroup(m_specimensInput->getGroupName(0), m_specimensInput->getGroup(0));
        }
    }

    int groupMeanNo = 0;
    for (unsigned int i=0;i<m_specimensInput->getGroupNumber();i++)
    {
        SpecimenGroup* group = m_specimensInput->getGroup(i);
        Matrix<double> *ls = group->getMean();
        if(ls!=0)
        {
            m_pcscoreOutput->addSpecimenGroup(m_specimensInput->getGroupName(i), m_specimensInput->getGroup(i));

            QString groupName = m_specimensInput->getGroupName(i);

            for (unsigned int j=0;j<m_specimensInput->getSize();j++)
            {
                if(group->contains(m_specimensInput->getSpecimenLabelValue(j,"ID")))
                {
                    for (unsigned int k=0;k<pcscores.GetCols();k++)
                    {
                        (*m_pcscoreOutput)[pcscores.GetRows()+groupMeanNo][k] += pcscores[j][k];
                    }
                }
            }
            if (group->getSize() > 1)
            {
            	for (unsigned int k=0;k<pcscores.GetCols();k++)
            	{
                	(*m_pcscoreOutput)[pcscores.GetRows()+groupMeanNo][k] /= group->getSize() - 1;
            	}
            }
            m_pcscoreOutput->setRowLabel(pcscores.GetRows()+groupMeanNo, groupName + " Mean");
            group->getMean()->setSpecimenIndex(m_specimensInput->getSize()+groupMeanNo);
            groupMeanNo++;
        }
        else
        {
            m_pcscoreOutput->addSpecimenGroup(m_specimensInput->getGroupName(i), m_specimensInput->getGroup(i));
        }
    }
    setOutputPort0(m_pcscoreOutput);

    bool scale = radioButton_2->isChecked();
    m_pca.GetPCLoadings(m_loadingsOutput,scale);

    for (unsigned int i=0;i<m_loadingsOutput->GetCols();i++)
    {
        QString qstr;
        qstr.setNum(i+1);
        qstr = "PC " + qstr;
        m_loadingsOutput->setColumnLabel(i,qstr);
    }
    if (m_specimensInput->getLandmarkDimensions()==3)
    {
        int num = 1;
        for (unsigned int i=0;i<m_loadingsOutput->GetRows();i+=3)
        {
            QString numstr;
            numstr.setNum(num);
            QString xstr = "x"+ numstr;
            QString ystr = "y"+ numstr;
            QString zstr = "z"+ numstr;
            m_loadingsOutput->setRowLabel(i,xstr);
            m_loadingsOutput->setRowLabel(i+1,ystr);
            m_loadingsOutput->setRowLabel(i+2,zstr);
            num++;
        }
    }
    else
    {
        int num = 1;
        for (unsigned int i=0;i<m_loadingsOutput->GetRows();i+=2)
        {
            QString numstr;
            numstr.setNum(num);
            QString xstr = "x"+ numstr;
            QString ystr = "y"+ numstr;
            m_loadingsOutput->setRowLabel(i,xstr);
            m_loadingsOutput->setRowLabel(i+1,ystr);
            num++;
        }
    }
    m_pca.GetMean(m_loadingsOutput->getMean());

    // get min and max scores for setting slider ranges in warper node
    vector<double> minscores(m_pcscoreOutput->GetCols());
    vector<double> maxscores(m_pcscoreOutput->GetCols());
    for (unsigned int i=0;i<m_pcscoreOutput->GetCols();i++)
    {
        double minval = (*m_pcscoreOutput)[0][i];
        double maxval = (*m_pcscoreOutput)[0][i];
        for (unsigned int j=1;j<m_pcscoreOutput->GetRows();j++)
        {
            if ((*m_pcscoreOutput)[j][i] < minval)
            {
                minval = (*m_pcscoreOutput)[j][i];
            }
            if ((*m_pcscoreOutput)[j][i] > maxval)
            {
                maxval = (*m_pcscoreOutput)[j][i];
            }
        }
        minscores[i] = minval;
        maxscores[i] = maxval;
    }

    m_loadingsOutput->setMin(minscores);
    m_loadingsOutput->setMax(maxscores);
    m_loadingsOutput->setNumScores(m_pcscoreOutput->GetCols());

    setOutputPort2(m_loadingsOutput);

    if(lineEdit->text() != "")
    {
        PrintOutput();
    }

    m_updated = false;

    Logger::getInstance()->log("[PCA Node] Processing finished");
}

bool PCANode::isUpdated() const
{
    return m_updated;
}

void PCANode::accept()
{
    if (m_updated)
        emit dataReady();
    QDialog::accept();
}

void PCANode::reject()
{
    m_updated = false;
    fromString(m_settings);
    QDialog::reject();
}

void PCANode::CalculatePCA()
{
    m_pca.PerformPCA(m_centerCheckBox->isChecked(), m_scaleCheckBox->isChecked());
}

QString PCANode::toString() const
{
    QString nodeData = "";

    if (m_centerCheckBox->isChecked())
        nodeData += "1";
    else
        nodeData += "0";

    nodeData += "$$";

    if (m_scaleCheckBox->isChecked())
        nodeData += "1";
    else
        nodeData += "0";

    nodeData += "$$";

    if (checkBox->isChecked())
        nodeData += "1";
    else
        nodeData += "0";

    nodeData += "$$";

    if (radioButton_2->isChecked())
        nodeData += "1";
    else
        nodeData += "0";

    nodeData += "$$";
    nodeData += lineEdit->text();

    return  nodeData;
}

void PCANode::fromString(const QString& params)
{
    QStringList nodeData = params.split("$$");

    if (nodeData.count() == 5)
    {
        m_centerCheckBox->setChecked(nodeData[0]=="1");
        m_scaleCheckBox->setChecked(nodeData[1]=="1");
        checkBox->setChecked(nodeData[2]=="1");

        if (nodeData[3]=="1")
            radioButton_2->setChecked(true);
        else
            radioButton->setChecked(true);

        lineEdit->setText(nodeData[4]);
    }
}


void PCANode::PrintOutput()
{
    QString fileName = lineEdit->text();
    QFile file(fileName);

    ofstream outfile(fileName.toStdString().c_str());

    if (outfile.fail())
    {
        Logger::getInstance()->log("[PCA Node] Error opening output file: " + fileName, Logger::RUN_ERROR);
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

    outfile << "EVAN Toolbox" << endl;
    outfile << "Principal Component Analysis (PCA)" << endl;

    outfile << endl;

    outfile << "Date:\t" << dateString.toStdString() << endl;
    outfile << "Time:\t" << timeString.toStdString() << endl;

    outfile << endl;

    outfile << "\tEigenvalues\tVariance (%)\tCumulative Variance (%)" << endl;
    double sumEigenvalues = 0.0;
    for(unsigned int i=0;i<m_eigenvalues->GetRows();i++)
    {
        sumEigenvalues += (*m_eigenvalues)[i][0];
    }
    double cumVariance = 0.0;
    for(unsigned int i=0;i<m_eigenvalues->GetRows();i++)
    {
        double variance = (*m_eigenvalues)[i][0] * 100.0 / sumEigenvalues;
        cumVariance += variance;
        outfile << m_eigenvalues->getRowLabel(i).toStdString()
        << "\t" << (*m_eigenvalues)[i][0] << "\t"
        << variance << "\t"<< cumVariance << endl;
    }
    outfile << endl;

    outfile << "PC Scores" << endl;
    for(unsigned int j=0;j<m_pcscoreOutput->GetCols();j++)
    {
        outfile << "\t" << m_pcscoreOutput->getColumnLabel(j).toStdString();
    }
    outfile << endl;
    for(unsigned int i=0;i<m_pcscoreOutput->GetRows();i++)
    {
        outfile << m_pcscoreOutput->getRowLabel(i).toStdString();
        for(unsigned int j=0;j<m_pcscoreOutput->GetCols();j++)
        {
                outfile << "\t" << (*m_pcscoreOutput)[i][j];
        }
        outfile << endl;
    }
    outfile << endl;

    outfile << "PC Loadings" << endl;
    for(unsigned int j=0;j<m_loadingsOutput->GetCols();j++)
    {
        outfile << "\t" << m_loadingsOutput->getColumnLabel(j).toStdString();
    }
    outfile << endl;
    for(unsigned int i=0;i<m_loadingsOutput->GetRows();i++)
    {
        outfile << m_loadingsOutput->getRowLabel(i).toStdString();
        for(unsigned int j=0;j<m_pcscoreOutput->GetCols();j++)
        {
            outfile << "\t" << (*m_loadingsOutput)[i][j];
        }
        outfile << endl;
    }

    Logger::getInstance()->log("[PCA Node] Output written to file: " + fileName);
}

void PCANode::browseOutputFile()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = QFileDialog::getSaveFileName(this, tr("Select a file for PCA output"),
                       fsLastDir,
                       tr("All files (*)\nText files (*.txt)"));

    if (fileName != "" && !fileName.isEmpty())
    {
        lineEdit->setText(fileName);

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
