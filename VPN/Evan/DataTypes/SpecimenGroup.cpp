#include "SpecimenGroup.h"
#include "Specimens.h"

SpecimenGroup::SpecimenGroup() :
    IRenderable(),
    m_groupColor(Qt::lightGray),
    m_groupLMKSize(3.0f),
    m_groupPSymbol(0),
    m_hasMean(false)
{}

SpecimenGroup::~SpecimenGroup()
{}

SpecimenGroup & SpecimenGroup::operator = (const SpecimenGroup& copy)
{
    // http://courses.cms.caltech.edu/cs11/material/cpp/donnie/cpp-ops.html
    if (this == &rhs) {     // Same object?
        return *this;        // Yes, so skip assignment, and just return *this.
    }
    // perform actual copy of values
    m_groupLMKSize = copy.m_groupLMKSize;
    m_groupPSymbol = copy.m_groupPSymbol;
    m_hasMean = copy.m_hasMean;
    return *this;

}

void SpecimenGroup::addMean(unsigned int index, const QString& ID, const QColor& vColor, float vSize, int pSymbol)
{
    if(getSize() == 0)
        return;
    LandmarkSet* meanSpecimen = new LandmarkSet(*getMember(0)); //initialize mean to first specimen
    for(unsigned int i=1; i<getSize(); ++i)
        *meanSpecimen += *getMember(i);
    *meanSpecimen /= (double)getSize();
    meanSpecimen->setSpecimenIndex(index);
    addMember(meanSpecimen, ID, vColor, vSize, pSymbol);
    m_hasMean = true;
}

bool SpecimenGroup::initialize(const QString& name)
{
    m_osgNode = new osg::Geode;
    setRenderableName(name);
    setFrontMaterial(m_groupColor);

    for(unsigned int i=0; i<getSize(); ++i)
    {
        getMemberViewerPtr(i)->setData(getMember(i)->GetRows(), getMember(i)->GetCols(), getMember(i)->GetMatrix());
        getMemberViewerPtr(i)->setLandmarkSize(m_memberData[i].viewerSize);
        if(!getMemberViewerPtr(i)->initialize(m_memberData[i].ID))
            return false;
        if(m_memberData[i].viewerColor != Qt::transparent)
            getMemberViewerPtr(i)->setFrontMaterial(m_memberData[i].viewerColor, true);
        else
            getMemberViewerPtr(i)->setFrontMaterial(m_groupColor);
    }
    return true;
}
