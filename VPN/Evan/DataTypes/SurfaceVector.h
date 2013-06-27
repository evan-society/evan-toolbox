#ifndef SURFACEVECTOR_H
#define SURFACEVECTOR_H

#include "Surface.h"
#include "DataTypePtr.h"

class SurfaceVector : public IRenderable
{
private:
    QVector<DataTypePtr> m_surfaces;

public:
    SurfaceVector() : IRenderable()
    {
        addType(SURFACE_VECTOR_T);
        addContainsRelation(SURFACE_T);
    }
    ~SurfaceVector()
    {clear();}

    void addSurface(Surface* s)         {m_surfaces.push_back(DataTypePtr(s));}
    void setSurface(int i, Surface* s)  {m_surfaces[i] = DataTypePtr(s);}

    Surface* getSurface(int i)      const {return (Surface*)m_surfaces[i].getPtr();}

    void clear()
    {
        for(int i=0; i<m_surfaces.size(); ++i)
            m_surfaces[i].release();
        m_surfaces.clear();
    }
    unsigned int getSize()              const {return m_surfaces.size();}

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////// Virtual Functions ///////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int getChildrenNum()        const{return getSize();}
    IRenderable* getChild(unsigned int i)const{return getSurface(i);}

    void addPart(DataTypePtr& part, const QString& desc)
    {
        int i = m_surfaces.indexOf(part);
        if(i >= 0 ) m_surfaces[i] = part;
        else        m_surfaces.push_back(part);
    }
    void removePart(DataTypePtr& part)
    {
        int i = m_surfaces.indexOf(part);
        if(i >= 0 )
        {
            m_surfaces.remove(i);
            //part.release();
        }
    }

    bool initialize(const QString& n) //Surfaces should be initialized separately before added to the vector
    {
        m_osgNode = new osg::Geode;
        setRenderableName(n);
        return true;
    }
    void update()
    {}
    bool isValid() const {return getSize() > 0;}
};

#endif


