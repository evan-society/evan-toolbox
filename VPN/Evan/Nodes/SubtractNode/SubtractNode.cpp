#include "SubtractNode.h"
#include "../../Utilities/Logger.h"

#include <cmath> // for log()

SubtractNode::SubtractNode() :  INode()
{
	m_baseInput = NULL;
	m_targetInput = NULL;
	m_diffVecOutput = new Loadings();
	m_specimensOutput = new Specimens();
	setupUi(this);
	m_parameterA = 1.0;
	m_parameterB = 1.0;
	m_textA = "1.0";
	m_textB = "1.0";
	lineEdit->setText(m_textA);
	lineEdit_2->setText(m_textB);
	checkBox->setChecked(false);
}

SubtractNode::~SubtractNode()
{}

void SubtractNode::process()
{
    m_baseInput = getInputPortData0();
    m_targetInput = getInputPortData1();

    if(m_baseInput == NULL || m_targetInput == NULL)
        return;
    if(!m_baseInput->isValid() || !m_targetInput->isValid())
        return;

    for (unsigned int i=0;i<m_targetInput->getSize() ;i++)
    {
        if (m_baseInput->GetRows() != m_targetInput->getLandmarkSet(i)->GetRows() ||
            m_baseInput->GetCols() != m_targetInput->getLandmarkSet(i)->GetCols() )
        {
            Logger::getInstance()->log("[Subtract Node] Source and Target do not match", Logger::RUN_ERROR);
            return;
        }
    }

    Matrix<double> X = m_parameterB * (*m_baseInput);
    Matrix<double> Y;

    Logger::getInstance()->log("[Subtract Node] Processing started");

    if(checkBox->isChecked())
    {
        m_diffVecOutput->ReSize(m_targetInput->getLandmarkCount()*m_targetInput->getLandmarkDimensions()+1,m_targetInput->getSize());
    }
    else
    {
        m_diffVecOutput->ReSize(m_targetInput->getLandmarkCount()*m_targetInput->getLandmarkDimensions(),m_targetInput->getSize());
    }

	m_specimensOutput->clear();
    for (unsigned int i = 0; i<m_targetInput->getSize() ;i++)
    {
        Y = m_parameterA * (*m_targetInput->getLandmarkSet(i));
        LandmarkSet *lm = new LandmarkSet(m_targetInput->getLandmarkSet(i)->GetRows(), m_targetInput->getLandmarkSet(i)->GetCols());

        for (unsigned int k = 0; k < Y.GetRows(); k++)
        {
            for (unsigned int l = 0; l < Y.GetCols(); l++)
            {
                (*lm)[k][l] = Y[k][l] - X[k][l];
                (*m_diffVecOutput)[k*Y.GetCols()+l][i] = Y[k][l] - X[k][l];
            }
        }
        m_specimensOutput->addLandmarkSet(lm);
    }

    if(checkBox->isChecked())
    {
        for (unsigned int i = 0; i<m_targetInput->getSize() ;i++)
        {
            (*m_diffVecOutput)[m_targetInput->getLandmarkCount()*m_targetInput->getLandmarkDimensions()][i] =
            m_parameterB * log(m_targetInput->getCentroidSize(i)) - m_parameterA * log(m_baseInput->getCentroidSize());
        }
    }

    m_specimensOutput->setSpecimenLabels(m_targetInput->getSpecimenLabels());
    m_specimensOutput->setLandmarkLabels(m_targetInput->getLandmarkLabels());
    m_specimensOutput->setLandmarkPairs(m_targetInput->getLandmarkPairs());


    // Add default group
	SpecimenGroup* defaultGroup = new SpecimenGroup;
    for (unsigned int i = 0; i<m_specimensOutput->getSize() ;i++)
    {
        defaultGroup->addMember(m_specimensOutput->getLandmarkSet(i), m_specimensOutput->getSpecimenLabelValue(i, "ID"));
    }
    m_specimensOutput->addGroup("Ungrouped", defaultGroup);
    m_specimensOutput->initialize("Specimens Difference");

    setOutputPort0(m_diffVecOutput);
    setOutputPort1(m_specimensOutput);
    Logger::getInstance()->log("[Subtract Node] Processing finished");
}

void SubtractNode::accept()
{
    bool Aok,Bok;
    double A = lineEdit->text().toDouble(&Aok);
    double B = lineEdit_2->text().toDouble(&Bok);
    if (Aok && Bok)
    {
        m_parameterA = A;
        m_parameterB = B;
        m_textA = lineEdit->text();
        m_textB = lineEdit_2->text();

        process();
        emit dataReady();
    }
    else
    {
        lineEdit->setText(m_textA);
        lineEdit->setText(m_textB);
        Logger::getInstance()->log("[Subtract Node]: Bad value for scaling parameters",Logger::WARNING);
    }
    QDialog::accept();
}

void SubtractNode::reject()
{
    lineEdit->setText(m_textA);
    lineEdit_2->setText(m_textB);
    QDialog::reject();
}

QString SubtractNode::toString() const
{
    QString nodeData = "";
    nodeData += lineEdit->text();
    nodeData += "$$";
    nodeData += lineEdit_2->text();
    nodeData += "$$";
    if(checkBox->isChecked())
        nodeData += "1";
    else
        nodeData += "0";
    return  nodeData;
}

void SubtractNode::fromString(const QString& params)
{
    QStringList val = params.split("$$");
    if (val.count() > 1)
    {
        lineEdit->setText(val[0]);
        lineEdit_2->setText(val[1]);
        m_parameterA = val[0].toDouble();
        m_parameterB = val[1].toDouble();
        if (val.count() == 3)
        {
            checkBox->setChecked(val[2]=="1");
        }
    }
}
