#ifndef SPECIMENGROUP_H
#define SPECIMENGROUP_H

#include "LMKSetRenderable.h"
#include "LandmarkSet.h"
#include <QColor>

struct GroupMemberData
{
    DataTypePtr memberPtr;
    QString ID;
    QColor viewerColor;
    DataTypePtr viewerPtr;
    float viewerSize;
    int plotSymbol;
    GroupMemberData() : memberPtr(NULL), ID(""),
                        viewerColor(Qt::transparent),
                        viewerPtr(DataTypePtr(new LMKSetRenderable)),
                        viewerSize(3.0f),
                        plotSymbol(0)
    {}
};

class SpecimenGroup : public IRenderable
{
private:
    QColor m_groupColor;
    float m_groupLMKSize;
    int m_groupPSymbol;
    QVector<GroupMemberData> m_memberData;
    bool m_hasMean;

    SpecimenGroup(const SpecimenGroup&){}
    //SpecimenGroup & operator = (const SpecimenGroup& copy) {return *this;}
    SpecimenGroup & operator = (const SpecimenGroup& copy);

public:
    SpecimenGroup();
    ~SpecimenGroup();

    QColor getGroupColor()           const{return m_groupColor;}
    void setGroupColor(const QColor& col) {m_groupColor = col;}
    float getGroupLMKSize()          const{return m_groupLMKSize;}
    void setGroupLMKSize(float s)         {m_groupLMKSize = s;}
    void setGroupPlotSymbol(int ps)       {m_groupPSymbol = ps;}
    int getGroupPlotSymbol()         const{return m_groupPSymbol;}

    void addMean(unsigned int, const QString&, const QColor&, float, int);
    void addMember(LandmarkSet* ls, const QString& ID = "", const QColor& vColor = Qt::transparent, float vSize = -1.0f, int pSymbol = 0)
    {
        GroupMemberData info;
        info.memberPtr = DataTypePtr(ls);
        info.ID = ID;
        info.viewerColor = vColor;
        info.viewerSize = vSize >= 0? vSize : info.viewerSize;
        info.plotSymbol = pSymbol;
        m_memberData.push_back(info);
    }
    unsigned int getSize()                              const {return m_memberData.size();}
    LandmarkSet* getMember(unsigned int i)              const { return dynamic_cast<LandmarkSet*>( m_memberData[i].memberPtr.getPtr() ); }
    const QString& getMemberID(unsigned int i)          const {return m_memberData[i].ID;}
    const QColor& getMemberColor(unsigned int i)        const {return m_memberData[i].viewerColor;}
    LMKSetRenderable* getMemberViewerPtr(unsigned int i)const { return dynamic_cast<LMKSetRenderable*>( m_memberData[i].viewerPtr.getPtr() ); }
    float getMemberViewerSize(unsigned int i)           const {return m_memberData[i].viewerSize;}
    int getMemberPlotSymbol(unsigned int i)             const {return m_memberData[i].plotSymbol;}

    bool contains(QString id)
    {
        for (unsigned int i=0;i<getSize();i++)
        {
            if(getMemberID(i)==id)
            {
                return true;
            }
        }
        return false;
    }

    LandmarkSet* getMean() const
    {
        if (!m_hasMean)
        {
            return 0;
        }
        return dynamic_cast<LandmarkSet*>( m_memberData.last().memberPtr.getPtr() );
    }

    unsigned int getChildrenNum()        const{return getSize();}
    IRenderable* getChild(unsigned int i)const{return getMemberViewerPtr(i);}
    bool initialize(const QString&);
    void update(){}
    bool isValid() const {return getSize() > 0;}
};

#endif
