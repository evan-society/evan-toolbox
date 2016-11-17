#include "GenerateSurfaceNode.h"
#include "../../Utilities/Logger.h"
#include <tps.h>

GenerateSurfaceNode::GenerateSurfaceNode() : INode()
{
	m_sourceLMKInput = NULL;
	m_targetLMKInput = NULL;
	m_surfaceInput = NULL;
	m_surfaceOutput = NULL;
}

GenerateSurfaceNode::~GenerateSurfaceNode()
{}

void GenerateSurfaceNode::process()
{
	m_sourceLMKInput = getInputPortData0();
    m_targetLMKInput = getInputPortData1();
    m_surfaceInput = getInputPortData2();

    if (m_sourceLMKInput == NULL || m_targetLMKInput == NULL || m_surfaceInput == NULL)
        return;
    if ( !m_sourceLMKInput->isValid() || !m_targetLMKInput->isValid() || !m_surfaceInput->isValid())
        return;

    Logger::getInstance()->log("[Generate Surface Node] Processing started");

    TPS tps;
    tps.LoadData(m_sourceLMKInput->GetMatrix(),m_targetLMKInput->GetMatrix(),m_sourceLMKInput->GetRows(),3);
    tps.PerformTPS();

    Matrix<double>* pointOutMat = new Matrix<double>((int) m_surfaceInput->getVertices()->GetRows(),3);
    tps.WarpPoints(m_surfaceInput->getVertices()->GetMatrix(),pointOutMat,m_surfaceInput->getVertices()->GetRows());

    m_surfaceOutput = new Surface();
    m_surfaceOutput->initialize(m_surfaceInput, "Target Surface");
    m_surfaceOutput->setVertices(pointOutMat);
    m_surfaceOutput->refreshGeometry();
    setOutputPort0(m_surfaceOutput);

    Logger::getInstance()->log("[Generate Surface Node] Processing finished");
}
