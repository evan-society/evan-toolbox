#ifndef RENDERABLES_H
#define RENDERABLES_H

#include "LandmarkSet.h"

class Renderables : public IDataType
{
private:
    QList<DataTypePtr> m_renderables;

public:
    Renderables() : IDataType()
    {
        addType(RENDERABLES_T);
        addContainsRelation(IRENDERABLE_T);
        addContainsRelation(LANDMARKSET_T);
    }

    void addRenderable(IRenderable* r)
    {
        if(r)
        {
            int i = m_renderables.indexOf(DataTypePtr(r));
            if(i >= 0 ) setRenderable(i, r);
            else        m_renderables.push_back(DataTypePtr(r));
        }
    }
    void setRenderable(int i, IRenderable* r)     {m_renderables[i] = DataTypePtr(r);}
    void removeRenderable(IRenderable* r)
    {
        if(r)
        {
            int i = m_renderables.indexOf(DataTypePtr(r));
            if(i >= 0 )
                m_renderables.removeAt(i);
        }
    }
    IRenderable* getRenderable(int i)       const {return (IRenderable*)m_renderables[i].getPtr();}
    unsigned int getSize()                  const {return m_renderables.size();}
    bool containsRenderable(IRenderable* r) const {return m_renderables.contains(DataTypePtr(r));}

    void clear()
    {
        for(int i=0; i<m_renderables.size(); ++i)
            m_renderables[i].release();
        m_renderables.clear();
    }

    void addPart(DataTypePtr& part, const QString& desc)
    {
        if(part->isValid())
        {
            if(part->isType(IRENDERABLE_T))
            {
                IRenderable* renderable = dynamic_cast<IRenderable*>(part.getPtr());
                addRenderable(renderable);
            }
            else if(part->isType(LANDMARKSET_T))
            {
                LandmarkSet* lmkSet = (LandmarkSet*)part.getPtr();
                addRenderable(lmkSet->getRenderable());
            }
        }
    }
    void removePart(DataTypePtr& part)
    {
        if(part->isValid())
        {
            if(part->isType(IRENDERABLE_T))
            {
                IRenderable* renderable = dynamic_cast<IRenderable*>(part.getPtr());
                removeRenderable(renderable);
            }
            else if(part->isType(LANDMARKSET_T))
            {
                LandmarkSet* lmkSet = (LandmarkSet*)part.getPtr();
                removeRenderable(lmkSet->getRenderable());
            }
        }
    }
    bool isValid() const {return m_renderables.size() > 0;}
};

#endif
