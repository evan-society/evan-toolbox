#include "PLSNode.h"
#include "../../Utilities/Logger.h"
#include <QFileDialog>
#include <QDateTime>
#include <QSettings>

#include <svd.h>
#include <algorithm>
#include <cmath> // for log()

#include "../../Labels/StringLabelValue.h"
#include "../../Labels/ScalarLabelValue.h"

PLSNode::PLSNode(QWidget* parent) : QDialog(parent),  INode()
{
    m_leftSpecimens = NULL;
    m_rightSpecimens = NULL;

    m_leftLoadings = new Loadings();
    m_rightLoadings = new Loadings();
    m_leftScores = new PCScore();
    m_rightScores = new PCScore();
    m_singularValues = new Variables();
    m_correlation = new Variables();
//    m_RVCoefficient = new Float();

    setupUi(this);

    checkBox_2->setChecked(true);
    checkBox_3->setChecked(false);
    checkBox_4->setChecked(true);
    checkBox_5->setChecked(false);

    listWidget->setDisabled(true);
    listWidget_2->setDisabled(true);

    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    listWidget_2->setSelectionMode(QAbstractItemView::MultiSelection);

    QObject::connect(checkBox_2, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(checkBox_3, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(checkBox_4, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));
    QObject::connect(checkBox_5, SIGNAL(stateChanged(int)), this, SLOT(controlsChanged()));

    QObject::connect(radioButton, SIGNAL(clicked()), this, SLOT(leftRadioChanged()));
    QObject::connect(radioButton_2, SIGNAL(clicked()), this, SLOT(leftRadioChanged()));
    QObject::connect(radioButton_3, SIGNAL(clicked()), this, SLOT(leftRadioChanged()));
    QObject::connect(radioButton_4, SIGNAL(clicked()), this, SLOT(rightRadioChanged()));
    QObject::connect(radioButton_5, SIGNAL(clicked()), this, SLOT(rightRadioChanged()));
    QObject::connect(radioButton_6, SIGNAL(clicked()), this, SLOT(rightRadioChanged()));

    QObject::connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(controlsChanged()));
    QObject::connect(listWidget_2, SIGNAL(itemSelectionChanged()), this, SLOT(controlsChanged()));

    QObject::connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(controlsChanged()));
    QObject::connect(pushButton, SIGNAL(clicked()), this, SLOT(browseOutputFile()));

    m_updated = true;
}

PLSNode::~PLSNode()
{}

void PLSNode::CalculatePLS()
{
    Matrix<double> templeft = m_leftBlock;
    Matrix<double> tempright = m_rightBlock;
    Row<double> meanl = Mean(templeft);
    Row<double> meanr = Mean(tempright);

    if ( radioButton_3->isChecked() &&  radioButton_6->isChecked() )
    {
        if ( listWidget->selectedItems().empty() && listWidget_2->selectedItems().empty() )
        {
            Logger::getInstance()->log("[PLS Node] No variables selected for left or right block.", Logger::WARNING);
            return;
        }
    }
    if (radioButton_3->isChecked() && listWidget->selectedItems().empty())
    {
        Logger::getInstance()->log("[PLS Node] No variables selected for left block.", Logger::WARNING);
        return;
    }
    if (radioButton_6->isChecked() && listWidget_2->selectedItems().empty())
    {
        Logger::getInstance()->log("[PLS Node] No variables selected for right block.", Logger::WARNING);
        return;
    }

    if (m_leftBlock.GetRows() != m_rightBlock.GetRows())
    {
        Logger::getInstance()->log("[PLS Node] Number of observations in left block and right block must be the same.", Logger::RUN_ERROR);
        return;
    }

    int rows = m_leftBlock.GetRows();

    vector<double> leftMean;
    leftMean.clear();
    if (checkBox_2->isChecked())
    {
        for (unsigned int i=0;i<m_leftBlock.GetCols();i++)
        {
            for (int j=0;j<rows;j++)
            {
                templeft[j][i] -= meanl[i];
            }
            leftMean.push_back(meanl[i]);
        }
    }
    else
    {
        for (unsigned int i=0;i<m_rightBlock.GetCols();i++)
        {
            leftMean.push_back(0.0);
        }
    }
    vector<double> rightMean;
    rightMean.clear();
    if (checkBox_4->isChecked())
    {
        for (unsigned int i=0;i<m_rightBlock.GetCols();i++)
        {
            for (unsigned int j=0;j<m_rightBlock.GetRows();j++)
            {
                tempright[j][i] -= meanr[i];
            }
            rightMean.push_back(meanr[i]);
        }
    }
    else
    {
        for (unsigned int i=0;i<m_rightBlock.GetCols();i++)
        {
            rightMean.push_back(0.0);
        }
    }
    if (checkBox_3->isChecked())
    {
        for (unsigned int i=0;i<m_leftBlock.GetCols();i++)
        {
            double sumsquared = 0.0;
            for (int j=0;j<rows;j++)
            {
                sumsquared += templeft[j][i] * templeft[j][i];
            }
            double stdev = 0.0;
            if (checkBox_2->isChecked())
            {
                stdev = sqrt(sumsquared / (rows-1));
            }
            else
            {
                stdev = sqrt(sumsquared);
            }
            for (unsigned int j=0;j<m_leftBlock.GetRows();j++)
            {
                templeft[j][i] /= stdev;
            }
        }
    }
    if (checkBox_5->isChecked())
    {
        for (unsigned int i=0;i<m_rightBlock.GetCols();i++)
        {
            double sumsquared = 0.0;
            for (int j=0;j<rows;j++)
            {
                sumsquared += tempright[j][i] * tempright[j][i];
            }
            double stdev = 0.0;
            if (checkBox_4->isChecked())
            {
                stdev = sqrt(sumsquared / (m_rightBlock.GetRows()-1));
            }
            else
            {
                stdev = sqrt(sumsquared);
            }
            for (int j=0;j<rows;j++)
            {
                tempright[j][i] /= stdev;
            }
        }
    }

    Matrix<double> temp = Transpose(templeft)*tempright;
    temp /= (rows-1);

    Matrix<double> U;
    Matrix<double> V;
    Matrix<double> D;
    svd(temp,&U,&D,&V);

    Matrix<double> leftScores = templeft*U;
    Matrix<double> rightScores = tempright*V;

    m_leftLoadings->ReSize(U.GetRows(),U.GetCols());
    m_rightLoadings->ReSize(V.GetRows(),V.GetCols());

    int numvals = min(D.GetCols(),D.GetRows());
    m_singularValues->ReSize(numvals,1);

    m_leftScores->ReSize(leftScores.GetRows(),leftScores.GetCols());
    m_rightScores->ReSize(rightScores.GetRows(),rightScores.GetCols());

    for (unsigned int i=0;i<U.GetRows();i++)
    {
        for (unsigned int j=0;j<U.GetCols();j++)
        {
            (*m_leftLoadings)[i][j] = U[i][j];
        }
    }

    for (unsigned int i=0;i<V.GetRows();i++)
    {
        for (unsigned int j=0;j<V.GetCols();j++)
        {
            (*m_rightLoadings)[i][j] = V[i][j];
        }
    }

    m_leftLoadings->setMean(leftMean);
    m_rightLoadings->setMean(rightMean);

    for (unsigned int j=0;j<leftScores.GetCols();j++)
    {
        QString text, num;
        num.setNum(j+1);
        text = "Left SW Score " + num;
        m_leftScores->setColumnLabel(j,text);
        for (unsigned int i=0;i<leftScores.GetRows();i++)
        {
            (*m_leftScores)[i][j] = leftScores[i][j];
        }
    }
    for (unsigned int i=0;i<leftScores.GetRows();i++)
    {
        QString text = m_leftSpecimens->getSpecimenLabelValue(i,"ID");
        if(text=="")
        {
            text.setNum(i+1);
        }
        m_leftScores->setRowLabel(i,text);
    }

    for (unsigned int j=0;j<rightScores.GetCols();j++)
    {
        QString text, num;
        num.setNum(j+1);
        text = "Right SW Score " + num;
        m_rightScores->setColumnLabel(j,text);
        for (unsigned int i=0;i<rightScores.GetRows();i++)
        {
            (*m_rightScores)[i][j] = rightScores[i][j];
        }
    }
    for (unsigned int i=0;i<rightScores.GetRows();i++)
    {
        QString text = m_rightSpecimens->getSpecimenLabelValue(i,"ID");
        if(text=="")
        {
            text.setNum(i+1);
        }
        m_rightScores->setRowLabel(i,text);
    }

    for (int i=0;i<numvals;i++)
    {
        (*m_singularValues)[i][0] = D[i][i];
    }

    // get min and max scores for setting slider ranges in warper node
    vector<double> minscores(m_leftScores->GetCols());
    vector<double> maxscores(m_leftScores->GetCols());

    for (unsigned int i=0;i<m_leftScores->GetCols();i++)
    {
        double minval = (*m_leftScores)[0][i];
        double maxval = (*m_leftScores)[0][i];
        for (unsigned int j=1;j<m_leftScores->GetRows();j++)
        {
            if ((*m_leftScores)[j][i] < minval)
            {
                minval = (*m_leftScores)[j][i];
            }
            if ((*m_leftScores)[j][i] > maxval)
            {
                maxval = (*m_leftScores)[j][i];
            }
        }
        minscores[i] = minval;
        maxscores[i] = maxval;
    }

    m_leftLoadings->setMin(minscores);
    m_leftLoadings->setMax(maxscores);

    // get min and max scores for setting slider ranges in warper node
    minscores.resize(m_rightScores->GetCols());
    maxscores.resize(m_rightScores->GetCols());

    for (unsigned int i=0;i<m_rightScores->GetCols();i++)
    {
        double minval = (*m_rightScores)[0][i];
        double maxval = (*m_rightScores)[0][i];
        for (unsigned int j=1;j<m_rightScores->GetRows();j++)
        {
            if ((*m_rightScores)[j][i] < minval)
            {
                minval = (*m_rightScores)[j][i];
            }
            if ((*m_rightScores)[j][i] > maxval)
            {
                maxval = (*m_rightScores)[j][i];
            }
        }
        minscores[i] = minval;
        maxscores[i] = maxval;
    }

    m_rightLoadings->setMin(minscores);
    m_rightLoadings->setMax(maxscores);
    m_rightLoadings->setNumScores(m_rightScores->GetCols());

    // Calculate Rv coefficient
    Matrix<double> varx = Transpose(templeft) * templeft;
    varx /= (rows-1);

    Matrix<double> vary = Transpose(tempright) * tempright;
    vary /= (rows-1);

    Matrix<double> covxy = Transpose(templeft) * tempright;
    covxy /= (rows-1);

    Matrix<double> covyx = Transpose(tempright) * templeft;
    covyx /= (rows-1);

    Matrix<double> tempxy = covxy * covyx;
    Matrix<double> tempx = varx * varx;
    Matrix<double> tempy = vary * vary;

    double sumxy = 0.0;
    for (unsigned int i=0;i<tempxy.GetRows();i++)
    {
        sumxy += tempxy[i][i];
    }
    double sumx = 0.0;
    for (unsigned int i=0;i<tempx.GetRows();i++)
    {
        sumx += tempx[i][i];
    }
    double sumy = 0.0;
    for (unsigned int i=0;i<tempy.GetRows();i++)
    {
        sumy += tempy[i][i];
    }
    m_rv = sumxy/sqrt(sumx*sumy);

//    m_RVCoefficient->setData(Rv);

    int lcols = m_leftScores->GetCols();
    int rcols = m_rightScores->GetCols();

    Matrix<double> lmat(rows,lcols);
    for (int i=0;i<rows;i++)
    {
        for (int j=0;j<lcols;j++)
        {
            lmat[i][j] = (*m_leftScores)[i][j];
        }
    }
    Matrix<double> rmat(rows,rcols);
    for (int i=0;i<rows;i++)
    {
        for (int j=0;j<rcols;j++)
        {
            rmat[i][j] = (*m_rightScores)[i][j];
        }
    }

    for (int i=0;i<lcols;i++)
    {
        double sumsquared = 0.0;
        for (int j=0;j<rows;j++)
        {
            sumsquared += lmat[j][i] * lmat[j][i];
        }
        double stdev = sqrt(sumsquared / (rows-1));
        for (int j=0;j<rows;j++)
        {
            lmat[j][i] /= stdev;
        }
    }

    for (int i=0;i<rcols;i++)
    {
        double sumsquared = 0.0;
        for (int j=0;j<rows;j++)
        {
            sumsquared += rmat[j][i] * rmat[j][i];
        }
        double stdev = sqrt(sumsquared / (rows-1));
        for (int j=0;j<rows;j++)
        {
            rmat[j][i] /= stdev;
        }
    }

    numvals = min(rcols,lcols);
    m_correlation->ReSize(numvals,1);

    for (int j=0;j<numvals;j++)
    {
        double sum = 0.0;
        for (int i=0;i<rows;i++)
        {
            sum += rmat[i][j] * lmat[i][j];
        }
        (*m_correlation)[j][0] = sum / (rows-1);
    }

    m_leftLoadings->setNumScores(numvals);
    m_rightLoadings->setNumScores(numvals);
}

void PLSNode::process()
{
    m_leftSpecimens = getInputPortData0();
    m_rightSpecimens = getInputPortData1();

    if (m_leftSpecimens == NULL || !m_leftSpecimens->isValid() || m_rightSpecimens == NULL || !m_rightSpecimens->isValid())
    {
        return;
    }
    Logger::getInstance()->log("[PLS Node] Processing started");

    if(listWidget->count() == 0)
    {
        FillLeftList();
    }
    if(listWidget_2->count() == 0)
    {
        FillRightList();
    }
    if (m_updated)
    {
        LoadData();
        CalculatePLS();

        //Add the specimen groups [A pointer to the group is passed. WARNING!! READ ONLY]
        m_leftScores->clearSpecimenGroups();
        m_rightScores->clearSpecimenGroups();

        if(m_leftSpecimens->getGroupNumber() == 1)
        {
            SpecimenGroup* group = m_leftSpecimens->getGroup(0);
            if(group!=0)
            {
                group->setGroupColor(Qt::black);
                m_leftScores->addSpecimenGroup(m_leftSpecimens->getGroupName(0), m_leftSpecimens->getGroup(0));
            }
        }

        if(m_rightSpecimens->getGroupNumber() == 1)
        {
            SpecimenGroup* group = m_rightSpecimens->getGroup(0);
            if(group!=0)
            {
                group->setGroupColor(Qt::black);
                m_rightScores->addSpecimenGroup(m_rightSpecimens->getGroupName(0), m_rightSpecimens->getGroup(0));
            }
        }

        for (unsigned int i=1;i<m_leftSpecimens->getGroupNumber();i++)
        {
            SpecimenGroup* group = m_leftSpecimens->getGroup(i);
            if(group!=0)
            {
                if(m_leftSpecimens->getGroupNumber()==2)
                {
                    group->setGroupColor(Qt::black);
                }

                m_leftScores->addSpecimenGroup(m_leftSpecimens->getGroupName(i), m_leftSpecimens->getGroup(i));
            }
        }
        for (unsigned int i=1;i<m_rightSpecimens->getGroupNumber();i++)
        {
            SpecimenGroup* group = m_rightSpecimens->getGroup(i);
            if(group!=0)
            {
                if(m_leftSpecimens->getGroupNumber()==2)
                {
                    group->setGroupColor(Qt::black);
                }
                m_rightScores->addSpecimenGroup(m_rightSpecimens->getGroupName(i), m_rightSpecimens->getGroup(i));
            }
        }

        setOutputPort0(m_leftScores);
        setOutputPort1(m_rightScores);
        setOutputPort2(m_leftLoadings);
        setOutputPort3(m_rightLoadings);
        setOutputPort4(m_singularValues);
        setOutputPort5(m_correlation);
//        setOutputPort6(m_RVCoefficient);

        if(lineEdit->text() != "")
        {
            PrintOutput();
        }
    }

    m_updated = false;

    Logger::getInstance()->log("[PLS Node] Processing finished");
}

void PLSNode::accept()
{
    if (m_updated)
        emit dataReady();
    QDialog::accept();
}

bool PLSNode::isUpdated() const
{
    return m_updated;
}

void PLSNode::LoadData()
{
    if (radioButton->isChecked())   // shape variables
    {
        int dimensions = m_leftSpecimens->getLandmarkDimensions();
        int landmarks = m_leftSpecimens->getLandmarkCount();
        int individuals = m_leftSpecimens->getSize();
        m_leftBlock.ReSize(individuals,dimensions*landmarks);

        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *llm = m_leftSpecimens->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    m_leftBlock[i][j*dimensions+k] = (*llm)[j][k];
                }
            }
        }
    }
    else if (radioButton_2->isChecked())   // form variables
    {
        int dimensions = m_leftSpecimens->getLandmarkDimensions();
        int landmarks = m_leftSpecimens->getLandmarkCount();
        int individuals = m_leftSpecimens->getSize();
        m_leftBlock.ReSize(individuals,dimensions*landmarks+1);

        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *llm = m_leftSpecimens->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    m_leftBlock[i][j*dimensions+k] = (*llm)[j][k];
                }
            }
            m_leftBlock[i][landmarks*dimensions] = log(m_leftSpecimens->getCentroidSize(i));
        }
    }
    else    // other variables
    {
        int individuals = m_leftSpecimens->getSize();
        int lcols = listWidget->selectedItems().count();

        if (individuals > 0 && lcols > 0)
        {
            m_leftBlock.ReSize(individuals,lcols);

            // label values
            Labels *lbls = m_leftSpecimens->getSpecimenLabels(0);
            QStringList labelNames = lbls->keys();

            if (!labelNames.empty())
            {
                // first create a matrix of label values
                Matrix<double> labelMatrix(individuals,labelNames.size());

                QVector<QStringList> strvalues(m_leftSpecimens->getSpecimenLabels(0)->size());
                for (unsigned int i=0;i<m_leftSpecimens->getSize();i++)
                {
                    Labels *lbl = m_leftSpecimens->getSpecimenLabels(i);
                    QList<ILabelValue*> labelValues = lbl->values();

                    for (int j=0;j<labelValues.size();j++)
                    {
                        if (labelValues[j]->getType()==ILabelValue::STRING_VALUE)
                        {
                            StringLabelValue * val = (StringLabelValue*)(labelValues[j]);
                            if (!strvalues[j].contains(val->getValue()))
                            {
                                labelMatrix[i][j] = strvalues[j].size();
                                strvalues[j].push_back(val->getValue());
                            }
                            else
                            {
                                labelMatrix[i][j] = strvalues[j].indexOf(val->getValue());
                            }
                        }
                        else if (labelValues[j]->getType()==ILabelValue::SCALAR_VALUE)
                        {
                            ScalarLabelValue * val = (ScalarLabelValue*)(labelValues[j]);
                            labelMatrix[i][j] = val->getValue();
                        }
                    }
                }

                QList<QListWidgetItem *> leftSelected = listWidget->selectedItems();

                vector<int> leftIndexes;
                for (int i=0;i<leftSelected.count();i++)
                {
                    leftIndexes.push_back(listWidget->row(leftSelected.at(i)));
                }
                sort(leftIndexes.begin(),leftIndexes.end());

                for (unsigned int j=0;j<leftIndexes.size();j++)
                {
                    if (leftIndexes[j]==0)
                    {
                        m_leftBlock.setColumnLabel(j,"Centroid sizes");
                        for (int i=0;i<individuals;i++)
                        {
                            m_leftBlock[i][j] = m_leftSpecimens->getCentroidSize(i);
                        }
                    }
                    else if (leftIndexes[j]==1)
                    {
                        m_leftBlock.setColumnLabel(j,"Ln of centroid sizes");
                        for (int i=0;i<individuals;i++)
                        {
                            m_leftBlock[i][j] = log(m_leftSpecimens->getCentroidSize(i));
                        }
                    }
                    else
                    {
                        m_leftBlock.setColumnLabel(j,labelNames[leftIndexes[j]-2]);
                        for (int i=0;i<individuals;i++)
                        {
                            m_leftBlock[i][j] = labelMatrix[i][leftIndexes[j]-2];
                        }
                    }
                }
            }
        }
    }

    if (radioButton_4->isChecked())   // shape variables
    {
        int dimensions = m_rightSpecimens->getLandmarkDimensions();
        int landmarks = m_rightSpecimens->getLandmarkCount();
        int individuals = m_rightSpecimens->getSize();
        m_rightBlock.ReSize(individuals,dimensions*landmarks);

        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *rlm = m_rightSpecimens->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    m_rightBlock[i][j*dimensions+k] = (*rlm)[j][k];
                }
            }
        }
    }
    else if (radioButton_5->isChecked())   // form variables
    {
        int dimensions = m_rightSpecimens->getLandmarkDimensions();
        int landmarks = m_rightSpecimens->getLandmarkCount();
        int individuals = m_rightSpecimens->getSize();
        m_rightBlock.ReSize(individuals,dimensions*landmarks+1);

        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *rlm = m_rightSpecimens->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    m_rightBlock[i][j*dimensions+k] = (*rlm)[j][k];
                }
            }
            m_rightBlock[i][landmarks*dimensions] = log(m_rightSpecimens->getCentroidSize(i));
        }
    }
    else    // other variables
    {
        int individuals = m_rightSpecimens->getSize();
        int rcols = listWidget_2->selectedItems().count();

        if (individuals > 0 && rcols > 0)
        {
            m_rightBlock.ReSize(individuals,rcols);

            // label values
            Labels *lbls = m_rightSpecimens->getSpecimenLabels(0);
            QStringList labelNames = lbls->keys();

            if (!labelNames.empty())
            {
                // first create a matrix of label values
                Matrix<double> labelMatrix(individuals,labelNames.size());

                QVector<QStringList> strvalues(m_rightSpecimens->getSpecimenLabels(0)->size());
                for (unsigned int i=0;i<m_rightSpecimens->getSize();i++)
                {
                    Labels *lbl = m_rightSpecimens->getSpecimenLabels(i);
                    QList<ILabelValue*> labelValues = lbl->values();

                    for (int j=0;j<labelValues.size();j++)
                    {
                        if (labelValues[j]->getType()==ILabelValue::STRING_VALUE)
                        {
                            StringLabelValue * val = (StringLabelValue*)(labelValues[j]);
                            if (!strvalues[j].contains(val->getValue()))
                            {
                                labelMatrix[i][j] = strvalues[j].size();
                                strvalues[j].push_back(val->getValue());
                            }
                            else
                            {
                                labelMatrix[i][j] = strvalues[j].indexOf(val->getValue());
                            }
                        }
                        else if (labelValues[j]->getType()==ILabelValue::SCALAR_VALUE)
                        {
                            ScalarLabelValue * val = (ScalarLabelValue*)(labelValues[j]);
                            labelMatrix[i][j] = val->getValue();
                        }
                    }
                }

                QList<QListWidgetItem *> rightSelected = listWidget_2->selectedItems();

                vector<int> rightIndexes;
                for (int i=0;i<rightSelected.count();i++)
                {
                    rightIndexes.push_back(listWidget_2->row(rightSelected.at(i)));
                }
                sort(rightIndexes.begin(),rightIndexes.end());

                for (unsigned int j=0;j<rightIndexes.size();j++)
                {
                    if (rightIndexes[j]==0)
                    {
                        m_rightBlock.setColumnLabel(j,"Centroid sizes");
                        for (int i=0;i<individuals;i++)
                        {
                            m_rightBlock[i][j] = m_rightSpecimens->getCentroidSize(i);
                        }
                    }
                    else if (rightIndexes[j]==1)
                    {
                        m_rightBlock.setColumnLabel(j,"Ln of centroid sizes");
                        for (int i=0;i<individuals;i++)
                        {
                            m_rightBlock[i][j] = log(m_rightSpecimens->getCentroidSize(i));
                        }
                    }
                    else
                    {
                        m_rightBlock.setColumnLabel(j,labelNames[rightIndexes[j]-2]);
                        for (int i=0;i<individuals;i++)
                        {
                            m_rightBlock[i][j] = labelMatrix[i][rightIndexes[j]-2];
                        }
                    }
                }
            }
        }
    }
}

QString PLSNode::toString() const
{
    QString nodeData = "";

    if (checkBox_2->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (checkBox_3->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (checkBox_4->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (checkBox_5->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_2->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_3->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_4->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_5->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_6->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    return  nodeData;
}

void PLSNode::fromString(const QString& params)
{
    QStringList values = params.split("$$");

    if (values.size() == 10)
    {
        checkBox_2->setChecked(values[0]=="1");
        checkBox_3->setChecked(values[1]=="1");
        checkBox_4->setChecked(values[2]=="1");
        checkBox_5->setChecked(values[3]=="1");
        radioButton->setChecked(values[4]=="1");
        radioButton_2->setChecked(values[5]=="1");
        radioButton_3->setChecked(values[6]=="1");
        radioButton_4->setChecked(values[7]=="1");
        radioButton_5->setChecked(values[8]=="1");
        radioButton_6->setChecked(values[9]=="1");
        listWidget->setDisabled(!radioButton_3->isChecked());
        listWidget_2->setDisabled(!radioButton_6->isChecked());
    }
}

bool PLSNode::inputChanged(int inputIndex)
{
    switch (inputIndex)
    {
    case 0: // Left Specimens
        FillLeftList();
        break;
    case 1: // Right Specimens
        FillRightList();
        break;
    default:
        break;
    }
    return true;
}

void PLSNode::FillLeftList()
{
    m_leftSpecimens = getInputPortData0();
    if (m_leftSpecimens == NULL || !m_leftSpecimens->isValid())
    {
        return;
    }
    listWidget->clear();
    listWidget->addItem("Centroid sizes");
    listWidget->addItem("Ln of centroid sizes");

    Labels *lbl = m_leftSpecimens->getSpecimenLabels(0);
    QStringList labelNames = lbl->keys();

    for (int i=0;i<labelNames.size();i++)
    {
        listWidget->addItem(labelNames[i]);
    }
}

void PLSNode::FillRightList()
{
    m_rightSpecimens = getInputPortData1();
    if (m_rightSpecimens == NULL || !m_rightSpecimens->isValid())
    {
        return;
    }
    listWidget_2->clear();
    listWidget_2->addItem("Centroid sizes");
    listWidget_2->addItem("Ln of centroid sizes");

    Labels *lbl = m_rightSpecimens->getSpecimenLabels(0);
    QStringList labelNames = lbl->keys();

    for (int i=0;i<labelNames.size();i++)
    {
        listWidget_2->addItem(labelNames[i]);
    }
}

void PLSNode::PrintOutput()
{
    QString fileName = lineEdit->text();
    QFile file(fileName);

    if (m_singularValues->GetCols()==0)
    {
        return;
    }

    ofstream outfile(fileName.toStdString().c_str());

    if (outfile.fail())
    {
        Logger::getInstance()->log("[PLS Node] Error opening output file: " + fileName, Logger::RUN_ERROR);
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
    outfile << "Partial Least Squares Analysis (PLS)" << endl;

    outfile << endl;

    outfile << "Date:\t" << dateString.toStdString() << endl;
    outfile << "Time:\t" << timeString.toStdString() << endl;

    outfile << endl;

    QVector<double> cov;

    double sum = 0.0;
    for(size_t i=0u;i<m_singularValues->GetRows();i++)
    {
        sum += (*m_singularValues)[i][0] * (*m_singularValues)[i][0];
    }

    for(size_t i=0u;i<m_singularValues->GetRows();i++)
    {
        cov.append( (*m_singularValues)[i][0] * (*m_singularValues)[i][0] / sum);
    }

    outfile << "Singular Values\t% of total covariance\tPairwise Correlations" << endl;
    for(size_t i=0u; i<m_singularValues->GetRows(); i++)
    {
        outfile << (*m_singularValues)[i][0] << "\t" << cov[i]<< "\t" <<(*m_correlation)[i][0] << endl;
    }

    outfile << endl;

    outfile << "Left Singular Warp scores" << endl;
    outfile << (*m_leftScores) << endl;

    outfile << endl;

    outfile << "Right Singular Warp scores" << endl;
    outfile << (*m_rightScores) << endl;

    outfile << endl;

    outfile << "Left loadings" << endl;
    outfile << (*m_leftLoadings) << endl;

    outfile << endl;

    outfile << "Right loadings" << endl;
    outfile << (*m_rightLoadings) << endl;

    outfile << endl;
    outfile << "RV coefficient:\t" << m_rv << endl;

    Logger::getInstance()->log("[PLS Node] Output written to file: " + fileName);
}

void PLSNode::browseOutputFile()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = QFileDialog::getSaveFileName(this, tr("Select a file for PLS output"),
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
