#ifndef SPECIMENS_H
#define SPECIMENS_H

#include "Labels.h"
#include "SpecimenGroup.h"

#ifdef USE_R_
#include "RList.h"
#endif

class Specimens : public IRenderable
{
private:
    QVector<DataTypePtr> m_landmarkSets;
    QVector<DataTypePtr> m_specimensLabels;
    QVector<DataTypePtr> m_landmarksLabels;

    QVector<QPair<int,int> > m_landmarkPairs;
    QVector<double> m_centroidSizes;

    QStringList m_groupNames;
    QVector<DataTypePtr> m_groups;

    bool m_tangentProjected;

public:
    Specimens() : IRenderable(), m_landmarkSets()
    {
        addType(SPECIMENS_T);
        addContainsRelation(LANDMARKSET_T);
        addPartOfRelation(RLIST_T);
        m_tangentProjected = false;
    }
    ~Specimens()
    {clear();}

    void addGroup(const QString& groupName, SpecimenGroup* sg) {m_groups.push_back(DataTypePtr(sg)); m_groupNames.push_back(groupName);}
    void addLandmarkSet(LandmarkSet* ls)                {ls->setSpecimenIndex(getSize());
                                                        m_landmarkSets.push_back(DataTypePtr(ls));}
    void setLandmarkSet(int i, LandmarkSet* ls)         {ls->setSpecimenIndex(i);
                                                        m_landmarkSets[i] = DataTypePtr(ls);}
    void setSpecimenLabels(const QVector<DataTypePtr>& sl)      {m_specimensLabels = sl;}
    void setLandmarkLabels(const QVector<DataTypePtr>& ll)      {m_landmarksLabels = ll;}
    void addSpecimenLabels(Labels* l)
    {
        Labels* specimenLabels = new Labels();
        foreach(QString name, l->keys())
            specimenLabels->addLabel(name, l->getLabel(name));
        m_specimensLabels.push_back(DataTypePtr(specimenLabels));
    }
    void addLandmarkLabels(Labels* l)
    {
        Labels* landmarkLabels = new Labels();
        foreach(QString name, l->keys())
            landmarkLabels->addLabel(name, l->getLabel(name));
        m_landmarksLabels.push_back(DataTypePtr(landmarkLabels));
    }
    void setLabelValue(int i, const QString& labelName, const QString& labelValue)
    {
        Labels* specimenLabels = (i>=m_specimensLabels.size()) ? new Labels() : getSpecimenLabels(i);
        specimenLabels->addLabel(labelName, labelValue);
        if(i>=m_specimensLabels.size())
            m_specimensLabels.push_back(DataTypePtr(specimenLabels));
    }
    void setLandmarkLabelValue(int i, const QString& labelName, const QString& labelValue)
    {
        Labels* landmarkLabels = (i>=m_landmarksLabels.size()) ? new Labels() : getLandmarkLabels(i);
        landmarkLabels->addLabel(labelName, labelValue);
        if(i>=m_landmarksLabels.size())
            m_landmarksLabels.push_back(DataTypePtr(landmarkLabels));
    }
    void setCentroidSize(int i, double csize)
    {
        if(i>=m_centroidSizes.size())
            m_centroidSizes.push_back(csize);
        else if (i>=0)
            m_centroidSizes[i] = csize;

        if(i<m_landmarkSets.size() && i>=0)
            ( dynamic_cast<LandmarkSet*>( m_landmarkSets[i].getPtr() ) )->setCentroidSize(csize);
    }
    void addLandmarkPair(QPair<int,int> p)
    {m_landmarkPairs.push_back(p);}
    void setLandmarkPairs(const QVector<QPair<int,int> >& pairs)
    {
        m_landmarkPairs.clear();
        m_landmarkPairs = pairs;
    }
    const QVector<QPair<int,int> >& getLandmarkPairs()
    {
        return m_landmarkPairs;
    }
    void setTangentProjected(bool tangentProjected)
    {
        m_tangentProjected = tangentProjected;
    }

    SpecimenGroup* getGroup(int i)              const { return dynamic_cast<SpecimenGroup*>( m_groups[i].getPtr() ); }
    const QString& getGroupName(int i)          const { return m_groupNames[i]; }
    unsigned int getGroupNumber()               const { return m_groups.size(); }
    LandmarkSet* getLandmarkSet(int i)          const { return dynamic_cast<LandmarkSet*>( m_landmarkSets[i].getPtr() ); }
    Labels* getSpecimenLabels(int i)            const { return dynamic_cast<Labels*>( m_specimensLabels[i].getPtr() ); }
    Labels* getLandmarkLabels(int i)            const { return dynamic_cast<Labels*>( m_landmarksLabels[i].getPtr() ); }
    QString getSpecimenLabelValue(int i, const QString& labelName) const {return getSpecimenLabels(i)->getLabel(labelName);}
    QString getLandmarkLabelValue(int i, const QString& labelName) const {return getLandmarkLabels(i)->getLabel(labelName);}
    const QVector<DataTypePtr>& getSpecimenLabels()   const {return m_specimensLabels;}
    const QVector<DataTypePtr>& getLandmarkLabels()   const {return m_landmarksLabels;}
    const QVector<double>& getCentroidSizes() const {return m_centroidSizes;}
    double getCentroidSize(int i)
    {
        if (i < (int)m_centroidSizes.size())
            return m_centroidSizes[i];
        else
            return 0.0;
    }

    QSet<unsigned int> matchExpression(const QString& expr)
    {
        QSet<unsigned int> matchedIndices;
        for(int i=0; i<m_specimensLabels.size(); ++i)
            if(getSpecimenLabels(i)->match(expr))
                matchedIndices.insert(i);
        return matchedIndices;
    }
    bool isTangentProjected()
    {
        return m_tangentProjected;
    }

    void clearLabels()
    {
        clearSpecimenLabels();
        clearLandmarkLabels();
    }
    void clearSpecimenLabels()
    {
        for(int i=0; i<m_specimensLabels.size(); ++i)
            m_specimensLabels[i].release();
        m_specimensLabels.clear();
    }
    void clearLandmarkLabels()
    {
        for(int i=0; i<m_landmarksLabels.size(); ++i)
            m_landmarksLabels[i].release();
        m_landmarksLabels.clear();
    }
    void clear()
    {
        for(int i=0; i<m_landmarkSets.size(); ++i)
            m_landmarkSets[i].release();
        m_landmarkSets.clear();

        for(int i=0; i<m_groups.size(); ++i)
            m_groups[i].release();
        m_groups.clear();
        m_groupNames.clear();

        m_landmarkPairs.clear();
        clearLabels();
    }
    unsigned int getSize()              const {return m_landmarkSets.size();}
    unsigned int getLandmarkCount()     const {return getLandmarkSet(0)->GetRows();}
    unsigned int getLandmarkDimensions()const {return getLandmarkSet(0)->GetCols();}

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////// Virtual Functions ///////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int getChildrenNum()        const{return getGroupNumber();}
    IRenderable* getChild(unsigned int i)const{return getGroup(i);}

    void addPart(DataTypePtr& part, const QString& desc)
    {
        int i = m_landmarkSets.indexOf(part);
        if(i >= 0 ) m_landmarkSets[i] = part;
        else
        {
            i = getSize();
            m_landmarkSets.push_back(part);
        }
        LandmarkSet* lmkSet = dynamic_cast<LandmarkSet*>(part.getPtr());
        if(lmkSet)
            setLabelValue(i, "ID", lmkSet->getRenderable()->getRenderableName());
    }
    void removePart(DataTypePtr& part)
    {
        int i = m_landmarkSets.indexOf(part);
        if(i >= 0 )
        {
            m_landmarkSets.remove(i);
            //part.release();
        }
    }

    bool initialize(const QString& n)
    {
        m_osgNode = new osg::Geode;
        setRenderableName(n);
        for(unsigned int i=0; i<getGroupNumber(); ++i)
        {
            SpecimenGroup* group = getGroup(i);
            if(!group->initialize(getGroupName(i))) return false;
        }
        return true;
    }
    void update()
    {
//        for(unsigned int i=0; i<getSize(); ++i)
//        {
//            LandmarkSet* lmkSet = getLandmarkSet(i);
//            lmkSet->update();
//        }
    }
    bool isValid() const {return m_landmarkSets.size() > 0;}

#ifdef USE_R_

    bool setFromContainer(DataTypePtr& rList, unsigned int partIndex)
    {
        if(rList.isValid() && rList->isType(RLIST_T))
        {
            RList* results = dynamic_cast<RList*>( rList.getPtr() );
            if(partIndex >= results->getSize() /* || partIndex < 0 */ ) // checking unsigned var for < 0
                return false;
            DataTypePtr dt = results->getDataType(partIndex);
            if(directMatch(dt.getPtr()))
            {
                Specimens* sp = dynamic_cast<Specimens*>( dt.getPtr() );
                SpecimenGroup* defaultGroup = new SpecimenGroup;
                for (unsigned int i=0;i<sp->getSize();i++)
                {
                    LandmarkSet* ls = sp->getLandmarkSet(i);
                    addLandmarkSet(ls);

                    QString value;
                    value.setNum(i+1);
                    setLabelValue(i, "ID", value);
                    defaultGroup->addMember(ls, value);
                }
                addGroup("Ungrouped", defaultGroup);
                initialize("Specimens from R");
           }
        }
        return true;
    }
#endif

};

#endif


