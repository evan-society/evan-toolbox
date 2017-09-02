#include "SurfaceVector.h"
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include "../Utilities/Logger.h"
#include "../Utilities/GeodeFinder.h"
#include <osg/LineWidth>

#ifdef OSG_LIBRARY_STATIC
    USE_OSGPLUGIN(stl)
    USE_OSGPLUGIN(obj)
#endif

extern ofstream dbgfile;// debug

bool Surface::setFromContainer(DataTypePtr& surfacesPtr, unsigned int partIndex)
{
    if(surfacesPtr.isValid())
    {
        if(!surfacesPtr->isType(SURFACE_VECTOR_T))
            return false;
        SurfaceVector* surfaces = dynamic_cast<SurfaceVector *>( surfacesPtr.getPtr() );
        //if(partIndex >= surfaces->getSize() || surfaces < 0)
		if( surfaces == NULL)
			return false;
		if( partIndex >= surfaces->getSize() )
            return false;
        initialize(surfaces->getSurface(partIndex), surfaces->getSurface(partIndex)->getRenderableName());
    }
    return true;
}

void Surface::clear()
{
    if(m_vertices)
    {
        delete m_vertices;
        m_vertices = NULL;
    }
    if(m_indeces)
    {
        delete m_indeces;
        m_indeces = NULL;
    }
    m_bRaw = true;
    m_bTextured = false;
    m_hasTexture = false;
    if(m_osgGeometry.get())
    {
        m_osgGeometry->removePrimitiveSet(0, m_osgGeometry->getNumPrimitiveSets() );
        m_osgGeometry->releaseGLObjects();
    }
    if(m_osgNode.get())
    {
        m_osgNode->removeDrawables(0, m_osgNode->getNumDrawables() );
        m_osgNode->releaseGLObjects();
    }
}

bool Surface::initialize(const QString& n)
{
    m_osgNode = new osg::Geode;
    if(m_bTextured) assignTexture();

    setRenderableName(n);
    osg::ref_ptr<osg::Vec3Array> myVertices = new osg::Vec3Array;
    //osg::ref_ptr<osg::Geometry> osgGeometry = new osg::Geometry;

    for (unsigned int i = 0; i< m_vertices->GetRows();i++)
    {
        myVertices->push_back(osg::Vec3(m_vertices->get(i,0),m_vertices->get(i,1),m_vertices->get(i,2)));
    }
    m_osgGeometry->setVertexArray(myVertices.get()); // attach the vertices array to the geometry


    // Creation of the Primitive array
    osg::ref_ptr<osg::DrawElementsUInt> face;

    if (m_indeces->GetCols() == 2)
    {
        for (unsigned int i = 0; i< m_indeces->GetRows();i++)
        {
           face = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP, 0);
           face->push_back(m_indeces->get(i,0));
           face->push_back(m_indeces->get(i,1));
           face->push_back(m_indeces->get(i,0));
           face->push_back(m_indeces->get(i,1));
           m_osgGeometry->addPrimitiveSet(face.get());
        }
        osg::LineWidth *lineWidth = new osg::LineWidth;
        m_osgGeometry->getOrCreateStateSet()->setAttribute(lineWidth, osg::StateAttribute::ON);
        m_osgGeometry->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        m_lineWidth=1;
        lineWidth->setWidth(m_lineWidth);
    }
    else if (m_indeces->GetCols() == 3)
    {
        for (unsigned int i = 0; i< m_indeces->GetRows();i++)
        {
           face = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
           face->push_back(m_indeces->get(i,0));
           face->push_back(m_indeces->get(i,1));
           face->push_back(m_indeces->get(i,2));
           m_osgGeometry->addPrimitiveSet(face.get());
        }
    }
    else if (m_indeces->GetCols() == 4) // VD - grid
    {
        for (unsigned int i = 0; i< m_indeces->GetRows();i++)
        {
           face = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP, 0);
           face->push_back(m_indeces->get(i,0));
           face->push_back(m_indeces->get(i,1));
           face->push_back(m_indeces->get(i,2));
           face->push_back(m_indeces->get(i,3));
           m_osgGeometry->addPrimitiveSet(face.get());
        }
        osg::LineWidth *lineWidth = new osg::LineWidth;
        m_osgGeometry->getOrCreateStateSet()->setAttribute(lineWidth, osg::StateAttribute::ON);
        m_osgGeometry->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        m_lineWidth=1;
        lineWidth->setWidth(m_lineWidth);
    }

    m_osgNode->addDrawable(m_osgGeometry); //attach the surface to the Geode
    m_bRaw = true;
    return true;
}

bool Surface::initialize(const QString& nodeName, const QString& fileName)
{
    osg::Node* modelNode = osgDB::readNodeFile(fileName.toStdString());
    if(!modelNode)
        return false;
    GeodeFinder finder;
    modelNode->accept(finder);
    QVector<osg::ref_ptr<osg::Geode> > foundGeodes = finder.getFoundGeodes();
    if(!foundGeodes.size())
        return false;
    m_osgNode = foundGeodes[0];
    if(m_bTextured) assignTexture();
    if(!m_osgNode->getNumDrawables())
        return false;

    //FIXME: Works only with 1 drawable (maybe no more is needed)
    m_osgGeometry = m_osgNode->getDrawable(0)->asGeometry();
    if(!m_osgGeometry.get())
        return false;

    osg::Vec3Array* verts = (osg::Vec3Array*)m_osgGeometry->getVertexArray();

    if(!verts)
        return false;

    //clear();

    //Fill Vertex Matrix from OSG Geometry
    m_vertices = new Matrix<double>(verts->getNumElements(), verts->getDataSize());
    for(unsigned int i=0; i<verts->getNumElements(); ++i)
    {
        osg::Vec3f vertex = verts->at(i);
        m_vertices->set(i,0, vertex.x());
        m_vertices->set(i,1, vertex.y());
        m_vertices->set(i,2, vertex.z());
    }


    setRenderableName(nodeName);
    m_bRaw = false;
    return true;
}

bool Surface::loadTexture(const QString& fileName)
{
    if(!m_osgNode.valid())
        return false;
    // Load texture
    osg::ref_ptr<osg::Image> txtImage (osgDB::readImageFile(fileName.toStdString()));
    if (txtImage.get() == 0)
        return false;

    m_texture = new osg::Texture2D;
    m_texture->setImage (txtImage.get());
    assignTexture();
    m_hasTexture = true;
    m_bTextured = true;
    return true;
}

bool Surface::initialize(const Surface* originalSurface, const QString& nodeName)
{
    m_bRaw = originalSurface->isRaw();
    m_hasTexture = originalSurface->hasTexture();
    m_bTextured = originalSurface->isTextured();
    m_texture = originalSurface->getTexture();
    //clear();

    setVertices(new Matrix<double>(*originalSurface->getVertices()));
    if(m_bRaw)
    {
        setIndices(new Matrix<unsigned int>(*originalSurface->getIndeces()));
        initialize(nodeName);
    }
    else
    {
        m_osgNode = new osg::Geode;// (*(originalSurface->getOsgNode().get()), osg::CopyOp::DEEP_COPY_ALL);
        m_osgGeometry = new osg::Geometry(*(originalSurface->getOsgGeometry().get()), osg::CopyOp::DEEP_COPY_ALL);
        m_osgNode->addDrawable(m_osgGeometry);
        setRenderableName(nodeName);
        if(m_bTextured) assignTexture();
    }
    return true;
}

void Surface::refreshGeometry()
{
    osg::ref_ptr<osg::Vec3Array> myVertices = new osg::Vec3Array;
    m_osgNode->removeDrawables(0,m_osgNode->getNumDrawables());
    for (unsigned int i = 0; i< m_vertices->GetRows();i++)
        myVertices->push_back(osg::Vec3(m_vertices->get(i,0),m_vertices->get(i,1),m_vertices->get(i,2)));

    m_osgGeometry->setVertexArray(myVertices.get()); // attach the vertices array to the geometry

    if(m_bRaw)
    {
        m_osgGeometry->removePrimitiveSet(0, m_osgGeometry->getNumPrimitiveSets() );
        m_osgGeometry->releaseGLObjects();
        osg::ref_ptr<osg::DrawElementsUInt> face;

        if (m_indeces->GetCols() == 2)
        {
            for (unsigned int i = 0; i< m_indeces->GetRows();i++)
            {
                face = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP, 0);
                face->push_back(m_indeces->get(i,0));
                face->push_back(m_indeces->get(i,1));
                m_osgGeometry->addPrimitiveSet(face.get());
            }
        }
        else if (m_indeces->GetCols() == 3)
        {
            for (unsigned int i = 0; i< m_indeces->GetRows();i++)
            {
                face = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
                face->push_back(m_indeces->get(i,0));
                face->push_back(m_indeces->get(i,1));
                face->push_back(m_indeces->get(i,2));
                m_osgGeometry->addPrimitiveSet(face.get());
            }
        }
        else if (m_indeces->GetCols() == 4) // VD - grid
        {
            for (unsigned int i = 0; i<m_indeces->GetRows();i++)
            {
               face = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP, 0);
               face->push_back(m_indeces->get(i,0));
               face->push_back(m_indeces->get(i,1));
               face->push_back(m_indeces->get(i,2));
               face->push_back(m_indeces->get(i,3));
               m_osgGeometry->addPrimitiveSet(face.get());
            }
            osg::LineWidth *lineWidth = new osg::LineWidth;
            m_osgGeometry->getOrCreateStateSet()->setAttribute(lineWidth, osg::StateAttribute::ON);
            m_osgGeometry->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
            lineWidth->setWidth(m_lineWidth);
        }
    }

    m_osgNode->addDrawable(m_osgGeometry);
}

bool Surface::saveToFile(const QString& fileName)
{
    osg::Node * node;
    node = (osg::Node*)m_osgNode;

    return osgDB::writeNodeFile(*node, fileName.toStdString());
}

Matrix<double>* osgMatrixToEvanMatrix(const osg::Matrixd& mosg)
{
    Matrix<double>* mat = new Matrix<double>(4,4);
    for (unsigned int i=0;i<4;i++)
    {
        for (unsigned int j=0;j<4;j++)
            mat->set(i,j, mosg(j,i));
    }
    return mat;
}

void Surface::assignTexture()
{
    osg::ref_ptr<osg::StateSet> stateSet (m_osgGeometry->getOrCreateStateSet());
    stateSet->setTextureAttributeAndModes (0,   // unit
                                          m_texture.get(),
                                          osg::StateAttribute::ON);
    if(getShaderUniform("textureUsed")!=NULL)
        getShaderUniform("textureUsed")->set(1);
    m_bTextured=true;
}

void Surface::removeTexture()
{
    osg::ref_ptr<osg::StateSet> stateSet (m_osgGeometry->getOrCreateStateSet());
    stateSet->removeTextureMode (0, osg::StateAttribute::OFF);
    stateSet->removeTextureAttribute (0, m_texture.get());
    if(getShaderUniform("textureUsed")!=NULL)
        getShaderUniform("textureUsed")->set(0);
    m_bTextured=false;
}
