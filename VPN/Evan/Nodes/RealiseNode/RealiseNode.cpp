#include "RealiseNode.h"
#include "../../Utilities/Logger.h"

RealiseNode::RealiseNode() :  INode()
{
	m_specimensInput = NULL;
	m_loadings = NULL;
	m_warpValues = NULL;
	m_specimensOutput = new LandmarkSet;
}

RealiseNode::~RealiseNode()
{}

void RealiseNode::process()
{

	m_specimensInput = getInputPortData0();
    m_loadings = getInputPortData1();
    m_warpValues = getInputPortData2();

    if(m_specimensInput == NULL || !m_specimensInput->isValid())
        return;

    if(m_loadings == NULL || !m_loadings->isValid())
        return;

    if(m_warpValues == NULL || !m_warpValues->isValid())
        return;

    Logger::getInstance()->log("[Realise Node] Processing started");

    int landmarks = m_specimensInput->GetRows();
    int dimensions = m_specimensInput->GetCols();
    m_specimensOutput->ReSize(landmarks,dimensions);

    for (unsigned int i = 0; i<m_specimensInput->GetRows() ;i++)
    {
        for (unsigned int j = 0; j<m_specimensInput->GetCols() ;j++)
        {
            (*m_specimensOutput)[i][j] = (*m_specimensInput)[i][j];
            for (unsigned int k = 0; k<m_warpValues->getSize() ;k++)
            {
                double factor = m_warpValues->getFloat(k);
                (*m_specimensOutput)[i][j] += factor * (*m_loadings)[i*dimensions+j][k];
            }
        }
    }

    double cs = m_specimensInput->getCentroidSize();
    m_specimensOutput->setCentroidSize(cs);
    m_specimensOutput->initialize("Realised specimen");
    setOutputPort0(m_specimensOutput);
    Logger::getInstance()->log("[Realise Node] Processing finished");

}
