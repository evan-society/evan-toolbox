#include "IRenderable.h"
#include <QString>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#ifdef USE_UNIX_LOCATIONS
extern QString libpath;
#endif

bool IRenderable::attachShaders(const QString& shaderName)
{
    m_osgShader = new osg::Program;

#ifdef USE_UNIX_LOCATIONS
    QString vertexFile = libpath + "Shaders/" + shaderName + ".vert";
    QString fragmentFile = libpath + "Shaders/" + shaderName + ".frag";
#else
    QString vertexFile = QString("Shaders/") + shaderName + ".vert";
    QString fragmentFile = QString("Shaders/") + shaderName + ".frag";
#endif
    if(!m_osgShader->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile(vertexFile.toStdString()))))
        return false;
    if(!m_osgShader->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile(fragmentFile.toStdString()))))
        return false;

    //m_osgGeometry->getOrCreateStateSet()->setAttributeAndModes( m_osgShader.get(), osg::StateAttribute::ON);
    return true;
}

void IRenderable::bindVertexAttributes(const QStringList& bindNames, const QList<unsigned int>& bindLocations,
                                        const QList<osg::ref_ptr<osg::Vec3Array> >& bindValues)
{
    if(!m_osgShader.valid())
        return;
    for (int i=0;i<bindValues.size();++i)
    {
        m_osgShader->addBindAttribLocation(bindNames[i].toStdString(), bindLocations[i]);
        m_osgGeometry->setVertexAttribArray(bindLocations[i], bindValues[i].get());
        m_osgGeometry->setVertexAttribBinding(bindLocations[i], osg::Geometry::BIND_PER_VERTEX);
    }
}

void IRenderable::toggleShaders(bool on)
{
    if(!m_osgShader.valid())
        return;
    m_osgGeometry->getOrCreateStateSet()->setAttributeAndModes( m_osgShader.get(),
                                                        on ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
}

void IRenderable::setFrontMaterial(const QColor& matColor, bool apply)
{
    m_fMatColor = matColor;
    if(!m_osgNode.valid() || !apply)
        return;
    osg::ref_ptr<osg::StateSet> nodess = m_osgNode->getOrCreateStateSet();
    osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>
                                            (nodess->getAttribute(osg::StateAttribute::MATERIAL));
    QColor result;
    if(!material.valid())
        material = new osg::Material;

    qreal r,g,b,a;
    m_fMatColor.getRgbF(&r, &g, &b, &a);
    material->setDiffuse(osg::Material::FRONT,osg::Vec4(r,g,b,a));
    material->setAlpha(osg::Material::FRONT_AND_BACK, a);
    if (a<1)
    {
        nodess->setMode(GL_BLEND, osg::StateAttribute::ON);
        nodess->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        // Conversely, disable writing to depth buffer so that
        // a transparent polygon will allow polygons behind it to shine thru.
        // OSG renders transparent polygons after opaque ones.
        osg::Depth* depth = new osg::Depth;
        depth->setWriteMask( false );
        nodess->setAttributeAndModes( depth, osg::StateAttribute::ON );
    }
    else
    {
        nodess->setMode(GL_BLEND, osg::StateAttribute::OFF);
        nodess->setRenderingHint(osg::StateSet::OPAQUE_BIN);

        // Conversely, enable writing to depth buffer so that
        // a transparent polygon will allow polygons behind it to shine thru.
        // OSG renders transparent polygons after opaque ones.
        osg::Depth* depth = new osg::Depth;
        depth->setWriteMask( true );
        nodess->setAttributeAndModes( depth, osg::StateAttribute::ON );
    }
    osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
    nodess->setAttributeAndModes(bf);
    nodess->setAttributeAndModes(material.get());
}

void IRenderable::setBackMaterial(const QColor& matColor, bool apply)
{
    m_bMatColor = matColor;
    if(!m_osgNode.valid() || !apply)
        return;
    osg::ref_ptr<osg::StateSet> nodess = m_osgNode->getOrCreateStateSet();
    osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>
                                            (nodess->getAttribute(osg::StateAttribute::MATERIAL));
    QColor result;
    if(!material.valid())
        material = new osg::Material;

    //Update the material square on the tree
    qreal r,g,b,a;
    m_bMatColor.getRgbF(&r, &g, &b, &a);
    material->setDiffuse(osg::Material::BACK,osg::Vec4(r,g,b,a));

    nodess->setAttributeAndModes(material.get());
}

QString IRenderable::toString() const
{
    if(!m_osgNode.valid())
        return "";
    QString result = "";

    result +=   getRenderableName()  +"|";

    if (m_osgNode->getStateSet())
        result +=   QString().setNum(m_fMatColor.red())  +"|"+
                    QString().setNum(m_fMatColor.green())+"|"+
                    QString().setNum(m_fMatColor.blue())+"|"+
                    QString().setNum(m_fMatColor.alpha())+"|";
    else
        result += "0|0|0|0"; // VD - bug fix

    osg::Matrixd rTransform = m_osgTransform->getMatrix();
    result +=   QString().setNum(rTransform.getScale().x())+"|"+
                QString().setNum(rTransform.getScale().y())+"|"+
                QString().setNum(rTransform.getScale().z())+"|"+

                QString().setNum(rTransform.getRotate().x())+"|"+
                QString().setNum(rTransform.getRotate().y())+"|"+
                QString().setNum(rTransform.getRotate().z())+"|"+
                QString().setNum(rTransform.getRotate().w())+"|"+

                QString().setNum(rTransform.getTrans().x())+"|"+
                QString().setNum(rTransform.getTrans().y())+"|"+
                QString().setNum(rTransform.getTrans().z())+"|";

    result +=   QString().setNum(m_bMatColor.red())  +"|"+
                QString().setNum(m_bMatColor.green())+"|"+
                QString().setNum(m_bMatColor.blue());

    if(getChildrenNum())
        result += "(";

    for(unsigned int i=0; i<getChildrenNum(); ++i)
    {
        result += getChild(i)->toString();
        if(getChild(i)->getChildrenNum() == 0)
            result += ":";
    }

    if(getChildrenNum())
        result += ")";

    return result;
}
