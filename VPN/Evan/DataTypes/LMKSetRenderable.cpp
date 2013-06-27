#include "LMKSetRenderable.h"
#include <osg/Point>

bool LMKSetRenderable::initialize(const QString& n)
{
    if(!m_data)
        return false;
    m_osgNode = new osg::Geode;
    setRenderableName(n);

    osg::ref_ptr<osg::Vec3Array> myVertices = new osg::Vec3Array;

    for (unsigned int i=0; i<m_rows; ++i)
        myVertices->push_back(osg::Vec3(m_data[i][0] , m_data[i][1], m_data[i][2]));

    myVertices->dirty();
    m_osgGeometry->setVertexArray(myVertices.get());
    m_osgGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, m_rows));
    m_osgGeometry->getOrCreateStateSet()->setAttribute( new osg::Point(m_landmarkSize), osg::StateAttribute::ON );
    m_osgGeometry->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
    m_osgNode->addDrawable(m_osgGeometry);
    return true;
}

void LMKSetRenderable::update()
{
    if(m_osgNode.valid())
    {
        osg::ref_ptr<osg::Vec3Array> myVertices = (osg::Vec3Array*)m_osgGeometry->getVertexArray();
        myVertices->clear();
        for (unsigned int i=0; i<m_rows; ++i)
            myVertices->push_back(osg::Vec3(m_data[i][0] , m_data[i][1], m_data[i][2]));

        myVertices->dirty();
        m_osgGeometry->setVertexArray(myVertices.get());

        osg::ref_ptr<osg::DrawArrays> indeces = (osg::DrawArrays*)m_osgGeometry->getPrimitiveSet(0);
        indeces->setCount(m_rows);
    }
}
