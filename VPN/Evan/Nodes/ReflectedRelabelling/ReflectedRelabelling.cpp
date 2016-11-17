#include "ReflectedRelabelling.h"
#include "../../Utilities/Logger.h"


ReflectedRelabelling::ReflectedRelabelling(QWidget* parent) : QDialog(parent),  INode()
{
	m_specimensInput = NULL;
	m_specimensOutput = new Specimens();
    m_updated = true;
}

ReflectedRelabelling::~ReflectedRelabelling()
{}

void ReflectedRelabelling::process()
{
    m_specimensInput = getInputPortData0();
    if (m_specimensInput == NULL || !m_specimensInput->isValid())
        return;

    Logger::getInstance()->log("[Reflected Relabelling Node] Processing started");

    QVector<QPair<int,int> > pairs = m_specimensInput->getLandmarkPairs();
    if (pairs.empty())
    {
        Logger::getInstance()->log("[Reflected Relabelling Node] No landmarks paired together. Set Landmark Pairs file in Import Node", Logger::RUN_ERROR);
        m_updated = false;
        return;
    }

    m_specimensOutput->clear();

    for(unsigned int i = 0;i<m_specimensInput->getSize();i++)
    {
        LandmarkSet *lm = m_specimensInput->getLandmarkSet(i);
        LandmarkSet *lm2 = new LandmarkSet(lm->GetRows(),lm->GetCols());
        for(unsigned int j = 0;j<lm->GetRows();j++)
        {
            for(unsigned int k = 0;k<lm->GetCols();k++)
            {
                (*lm2)[j][k] = (*lm)[j][k];
            }
        }
        m_specimensOutput->addLandmarkSet(lm2);
    }

    if(m_specimensInput->getLandmarkDimensions() == 3)
    {
        for(unsigned int i = 0;i<m_specimensInput->getSize();i++)
        {
            LandmarkSet *lm = m_specimensInput->getLandmarkSet(i);
            LandmarkSet *lm2 = new LandmarkSet(lm->GetRows(),lm->GetCols());
            // reflect
            for(unsigned int j = 0;j<lm->GetRows();j++)
            {
                (*lm2)[j][0] = (*lm)[j][0];
                (*lm2)[j][1] = (*lm)[j][1];
                (*lm2)[j][2] = -((*lm)[j][2]);
            }
            // relabel
            for (int j=0;j<pairs.size();j++)
            {
                lm2->SwapRows(pairs[j].first, pairs[j].second);
            }
            m_specimensOutput->addLandmarkSet(lm2);
        }
    }
    else if(m_specimensInput->getLandmarkDimensions() == 2)
    {
        for(unsigned int i = 0;i<m_specimensInput->getSize();i++)
        {
            LandmarkSet *lm = m_specimensInput->getLandmarkSet(i);
            LandmarkSet *lm2 = new LandmarkSet(lm->GetRows(),lm->GetCols());
            // reflect
            for(unsigned int j = 0;j<lm->GetRows();j++)
            {
                (*lm2)[j][0] = (*lm)[j][0];
                (*lm2)[j][1] = -((*lm)[j][1]);
            }
            // relabel
            for (int j=0;j<pairs.size();j++)
            {
                lm2->SwapRows(pairs[j].first,pairs[j].second);
            }
            m_specimensOutput->addLandmarkSet(lm2);
        }
    }

    m_specimensOutput->setSpecimenLabels(m_specimensInput->getSpecimenLabels());
    m_specimensOutput->setLandmarkLabels(m_specimensInput->getLandmarkLabels());
    m_specimensOutput->setLandmarkPairs(m_specimensInput->getLandmarkPairs());

    // Change ID of relected specimens
    for (unsigned int i=0;i<m_specimensInput->getSize();i++)
    {
        Labels *lbl = m_specimensInput->getSpecimenLabels(i);
        m_specimensOutput->addSpecimenLabels(lbl);
        QString value = m_specimensInput->getSpecimenLabelValue(i,"ID") + " Reflected";
        m_specimensOutput->setLabelValue(m_specimensInput->getSize()+i, "ID", value);
    }

    // Add groups
	SpecimenGroup* originalGroup = new SpecimenGroup;
	SpecimenGroup* reflectedGroup = new SpecimenGroup;
    for (unsigned int i = 0; i<m_specimensInput->getSize() ;i++)
    {
        int j = m_specimensInput->getSize() + i;
        originalGroup->addMember(m_specimensOutput->getLandmarkSet(i), m_specimensOutput->getSpecimenLabelValue(i, "ID"));
        reflectedGroup->addMember(m_specimensOutput->getLandmarkSet(j), m_specimensOutput->getSpecimenLabelValue(j, "ID"));
    }
    m_specimensOutput->addGroup("Original Specimens", originalGroup);
    m_specimensOutput->addGroup("Reflected Specimens", reflectedGroup);

    m_specimensOutput->initialize("Reflected and Relabelled Specimens");
    setOutputPort0(m_specimensOutput);
    m_updated = false;
    Logger::getInstance()->log("[Reflected Relabelling Node] Processing finished");
}

bool ReflectedRelabelling::isUpdated() const
{
    return m_updated;
}

