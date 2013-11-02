#ifndef IRENDERABLE_H
#define IRENDERABLE_H

#include "IDataType.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <osgManipulator/Selection>
#include <QStringList>
#include <QColor>

#include <cassert>

class IRenderable : public IDataType
{
protected:
    osg::ref_ptr<osg::Geode> m_osgNode;
    osg::ref_ptr<osg::Geometry> m_osgGeometry;
    osg::ref_ptr<osg::Program> m_osgShader;
    osg::ref_ptr<osgManipulator::Selection> m_osgTransform;

    QColor m_fMatColor;
    QColor m_bMatColor;

public:
    IRenderable() : IDataType(), m_osgGeometry(new osg::Geometry), m_osgTransform(new osgManipulator::Selection), m_fMatColor(Qt::lightGray), m_bMatColor(Qt::black)
    {addType(IRENDERABLE_T);}

    virtual ~IRenderable()
    {}

    bool attachShaders(const QString&);
    void bindVertexAttributes(const QStringList&, const QList<unsigned int>&, const QList<osg::ref_ptr<osg::Vec3Array> >&);
    void toggleShaders(bool on);
    void addShaderUniform(osg::Uniform::Type type, const QString &name, int numElements=1)
    {m_osgGeometry->getOrCreateStateSet()->addUniform(new osg::Uniform(type, name.toStdString(), numElements));}
    osg::Uniform* getShaderUniform(const QString& name)
    {return m_osgGeometry->getOrCreateStateSet()->getUniform(name.toStdString());}

    void setOsgNode(osg::Node* n)                     { assert(n !=NULL); m_osgNode = (osg::Geode*)n;}
    void setFrontMaterial(const QColor&, bool apply=false);
    void setBackMaterial(const QColor&, bool apply=false);
    const QColor& getFrontMaterial()            const {return m_fMatColor;}
    const QColor& getBackMaterial()             const {return m_bMatColor;}

    osg::ref_ptr<osg::Geode> getOsgNode()       const {return m_osgNode;}
    osg::ref_ptr<osg::Geometry> getOsgGeometry()const {return m_osgGeometry;}
    QString getRenderableName()                 const { /*assert(m_osgNode != NULL);*/ return ( m_osgNode != NULL ) ? QString( m_osgNode->getName().c_str() ) : QString("?"); }
    osg::ref_ptr<osgManipulator::Selection> getOsgTransform()const {return m_osgTransform;}

    virtual QString toString() const;
    virtual unsigned int getChildrenNum()       const {return 0;}
    virtual void resetTransform()                     {m_osgTransform->setMatrix(osg::Matrix::identity());}
    virtual IRenderable* getChild(unsigned int i)const{return NULL;}
    void setRenderableName(const QString& n)          {m_osgNode->setName(n.toStdString());}
    virtual bool isRaw()                        const {return true;}
    virtual bool isValid()                      const {return m_osgNode.valid();}

    virtual bool initialize(const QString&) = 0;
    virtual void update()                   = 0;
};

#endif
