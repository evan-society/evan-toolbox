#include "ConcatNode.h"
#include "../../Utilities/Logger.h"

ConcatNode::ConcatNode() :  INode()
{
	m_specimensInput0 = NULL;
	m_specimensInput1 = NULL;
	m_specimensOutput = new Specimens;
}

ConcatNode::~ConcatNode()
{}

void ConcatNode::process()
{
	m_specimensInput0 = getInputPortData0();
    m_specimensInput1 = getInputPortData1();

    if(m_specimensInput0 == NULL || m_specimensInput1 == NULL)
        return;
    if(!m_specimensInput0->isValid() || !m_specimensInput1->isValid())
        return;

    if(m_specimensInput0->getLandmarkSet(0)->GetCols() != m_specimensInput1->getLandmarkSet(0)->GetCols())
    {
        Logger::getInstance()->log("[Concat Node] Columns of input specimens do not match!", Logger::RUN_ERROR);
        return;
    }

    if(m_specimensInput0->getSpecimenLabels(0)->keys() != m_specimensInput1->getSpecimenLabels(0)->keys())
    {
        Logger::getInstance()->log("[Concat Node] Labels of input specimens do not match!", Logger::RUN_ERROR);
        return;
    }

    Logger::getInstance()->log("[Concat Node] Processing started");

	m_specimensOutput->clear();

	SpecimenGroup* defaultGroup = new SpecimenGroup;
    for (unsigned int i = 0; i<m_specimensInput0->getSize() ;i++)
    {
        LandmarkSet* spec0Set = new LandmarkSet(*m_specimensInput0->getLandmarkSet(i));
        m_specimensOutput->addLandmarkSet(spec0Set);
        m_specimensOutput->addSpecimenLabels(m_specimensInput0->getSpecimenLabels(i));
        defaultGroup->addMember(spec0Set, m_specimensInput0->getSpecimenLabelValue(i, "ID"));
    }
    for (unsigned int i = 0; i<m_specimensInput1->getSize() ;i++)
    {
        LandmarkSet* spec1Set = new LandmarkSet(*m_specimensInput1->getLandmarkSet(i));
        m_specimensOutput->addLandmarkSet(spec1Set);
        m_specimensOutput->addSpecimenLabels(m_specimensInput1->getSpecimenLabels(i));
        defaultGroup->addMember(spec1Set, m_specimensInput1->getSpecimenLabelValue(i, "ID"));
    }
    m_specimensOutput->addGroup("Ungrouped", defaultGroup);
    m_specimensOutput->initialize("Concatenated Specimens");
    setOutputPort0(m_specimensOutput);

    Logger::getInstance()->log("[Concat Node] Processing finished");
}
