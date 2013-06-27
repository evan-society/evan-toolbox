#ifndef OSGVOLUME_MYRAYTRACEDTECHNIQUE
#define OSGVOLUME_MYRAYTRACEDTECHNIQUE 1

#include <osgVolume/VolumeTechnique>

class MyRayTracedTechnique : public osgVolume::VolumeTechnique
{
    public:

        MyRayTracedTechnique();

        MyRayTracedTechnique(const MyRayTracedTechnique&,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, MyRayTracedTechnique);

        virtual void init();

        virtual void update(osgUtil::UpdateVisitor* nv);

        virtual void cull(osgUtil::CullVisitor* nv);

        /** Clean scene graph from any terrain technique specific nodes.*/
        virtual void cleanSceneGraph();

        /** Traverse the terrain subgraph.*/
        virtual void traverse(osg::NodeVisitor& nv);

    protected:

        virtual ~MyRayTracedTechnique();

        osg::ref_ptr<osg::Geode> _geode;
        osg::ref_ptr<osg::Geometry> _geometry;
};

#endif
