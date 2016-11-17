#include "MyRayTracedTechnique.h"
#include <osgVolume/VolumeTile>

#include <osg/Geometry>
#include <osg/io_utils>

#include <osg/Program>
#include <osg/TexGen>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/TransferFunction>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <iostream>
#include <fstream>
#include <QString>

#ifdef USE_UNIX_LOCATIONS
extern QString libpath;
#endif

MyRayTracedTechnique::MyRayTracedTechnique()
{
}

MyRayTracedTechnique::MyRayTracedTechnique(const MyRayTracedTechnique& fft,const osg::CopyOp& copyop):
    VolumeTechnique(fft,copyop)
{
}

MyRayTracedTechnique::~MyRayTracedTechnique()
{
}

enum ShadingModel
{
    Standard,
    Light,
    Isosurface,
    MaximumIntensityProjection
};

void MyRayTracedTechnique::init()
{
    osg::notify(osg::NOTICE)<<"RayTracedTechnique::init()"<<std::endl;

     if (!_volumeTile) return;

     ShadingModel shadingModel = Isosurface;
     float alphaFuncValue = 0.1;

    _geode = new osg::Geode;

    osg::Image* image_3d = 0;
    osg::TransferFunction1D* tf = 0;
    osgVolume::Locator* masterLocator = _volumeTile->getLocator();

    image_3d = _volumeTile->getLayer()->getImage();

    osgVolume::CollectPropertiesVisitor cpv;
    if (_volumeTile->getLayer()->getProperty())
    {
        _volumeTile->getLayer()->getProperty()->accept(cpv);
    }

    if (cpv._isoProperty.valid())
    {
        shadingModel = Isosurface;
        alphaFuncValue = cpv._isoProperty->getValue();
    }
    else if (cpv._mipProperty.valid())
    {
        shadingModel = MaximumIntensityProjection;
    }
    else if (cpv._lightingProperty.valid())
    {
        shadingModel = Light;
    }
    else
    {
        shadingModel = Standard;
    }

    if (cpv._tfProperty.valid())
    {
        tf = dynamic_cast<osg::TransferFunction1D*>(cpv._tfProperty->getTransferFunction());
    }

    if (cpv._afProperty.valid())
    {
        alphaFuncValue = cpv._afProperty->getValue();
    }


    if (_volumeTile->getLayer() && !masterLocator)
    {
        masterLocator = _volumeTile->getLayer()->getLocator();
    }

    osg::Matrix matrix;
    if (masterLocator)
    {
        matrix = masterLocator->getTransform();
    }

    osg::Texture::InternalFormatMode internalFormatMode = osg::Texture::USE_IMAGE_DATA_FORMAT;

    {

        osg::Texture::FilterMode minFilter = osg::Texture::LINEAR;
        osg::Texture::FilterMode magFilter = osg::Texture::LINEAR;

        osg::StateSet* stateset = _geode->getOrCreateStateSet();

        stateset->setMode(GL_ALPHA_TEST,osg::StateAttribute::ON);

        osg::Program* program = new osg::Program;
        stateset->setAttribute(program);
//
        osgDB::FilePathList filepath = osgDB::Registry::instance()->getDataFilePathList();
#ifdef USE_UNIX_LOCATIONS
        filepath.push_back((libpath + "Shaders/").toUtf8().data());
#else
        filepath.push_back("Shaders/");
#endif
        osgDB::Registry::instance()->setDataFilePathList(filepath);
        // get shaders from source
        if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile("volume_vert.vert"))))
        {
            return;
        }
        {
            // set up the 3d texture itself,
            // note, well set the filtering up so that mip mapping is disabled,
            // gluBuild3DMipsmaps doesn't do a very good job of handled the
            // imbalanced dimensions of the 256x256x4 texture.
            osg::Texture3D* texture3D = new osg::Texture3D;
            texture3D->setResizeNonPowerOfTwoHint(false);
            texture3D->setFilter(osg::Texture3D::MIN_FILTER,minFilter);
            texture3D->setFilter(osg::Texture3D::MAG_FILTER, magFilter);
            texture3D->setWrap(osg::Texture3D::WRAP_R,osg::Texture3D::CLAMP_TO_BORDER);
            texture3D->setWrap(osg::Texture3D::WRAP_S,osg::Texture3D::CLAMP_TO_BORDER);
            texture3D->setWrap(osg::Texture3D::WRAP_T,osg::Texture3D::CLAMP_TO_BORDER);
            if (image_3d->getPixelFormat()==GL_ALPHA ||
                image_3d->getPixelFormat()==GL_LUMINANCE)
            {
                texture3D->setInternalFormatMode(osg::Texture3D::USE_USER_DEFINED_FORMAT);
                texture3D->setInternalFormat(GL_INTENSITY);
            }
            else
            {
                texture3D->setInternalFormatMode(internalFormatMode);
            }
            texture3D->setImage(image_3d);

            stateset->setTextureAttributeAndModes(0,texture3D,osg::StateAttribute::ON);

            osg::Uniform* baseTextureSampler = new osg::Uniform("baseTexture",0);
            stateset->addUniform(baseTextureSampler);
        }


        if (shadingModel==MaximumIntensityProjection)
        {
            if (tf)
            {
                osg::Texture1D* texture1D = new osg::Texture1D;
                texture1D->setImage(tf->getImage());
                stateset->setTextureAttributeAndModes(1,texture1D,osg::StateAttribute::ON);

#ifdef USE_UNIX_LOCATIONS
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile((libpath + "Shaders//volume_tf_mip_frag.frag").toUtf8().data()))))
#else
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("Shaders//volume_tf_mip_frag.frag"))))
#endif
                    return;


                osg::Uniform* tfTextureSampler = new osg::Uniform("tfTexture",1);
                stateset->addUniform(tfTextureSampler);

            }
            else
            {
#ifdef USE_UNIX_LOCATIONS
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile((libpath + "Shaders//volume_mip_frag.frag").toUtf8().data()))))
#else
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("Shaders//volume_mip_frag.frag"))))
#endif
                    return;

            }
        }
        else if (shadingModel==Isosurface)
        {

            stateset->addUniform(cpv._isoProperty->getUniform());

            if (tf)
            {
                osg::Texture1D* texture1D = new osg::Texture1D;
                texture1D->setImage(tf->getImage());
                texture1D->setResizeNonPowerOfTwoHint(false);
                texture1D->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
                texture1D->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
                texture1D->setWrap(osg::Texture::WRAP_R,osg::Texture::CLAMP_TO_EDGE);
                stateset->setTextureAttributeAndModes(1,texture1D,osg::StateAttribute::ON);

                osg::Uniform* tfTextureSampler = new osg::Uniform("tfTexture",1);
                stateset->addUniform(tfTextureSampler);

#ifdef USE_UNIX_LOCATIONS
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile((libpath + "Shaders//volume_tf_iso_frag.frag").toUtf8().data()))))
#else
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("Shaders//volume_tf_iso_frag.frag"))))
#endif
                    return;

            }
            else
            {
#ifdef USE_UNIX_LOCATIONS
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile((libpath + "Shaders//volume_iso_frag.frag").toUtf8().data()))))
#else
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("Shaders//volume_iso_frag.frag"))))
#endif
                    return;

            }
        }
        else if (shadingModel==Light)
        {
            if (tf)
            {
                osg::Texture1D* texture1D = new osg::Texture1D;
                texture1D->setImage(tf->getImage());
                texture1D->setResizeNonPowerOfTwoHint(false);
                texture1D->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
                texture1D->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
                texture1D->setWrap(osg::Texture::WRAP_R,osg::Texture::CLAMP_TO_EDGE);
                stateset->setTextureAttributeAndModes(1,texture1D,osg::StateAttribute::ON);

                osg::Uniform* tfTextureSampler = new osg::Uniform("tfTexture",1);
                stateset->addUniform(tfTextureSampler);

#ifdef USE_UNIX_LOCATIONS
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile((libpath + "Shaders//volume_lit_tf_frag.frag").toUtf8().data()))))
#else
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("Shaders//volume_lit_tf_frag.frag"))))
#endif
                    return;

            }
            else
            {
#ifdef USE_UNIX_LOCATIONS
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile((libpath + "Shaders//volume_lit_frag.frag").toUtf8().data()))))
#else
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("Shaders//volume_lit_frag.frag"))))
#endif
                    return;
            }
        }
        else
        {
            if (tf)
            {
                osg::notify(osg::NOTICE)<<"Setting up TF path"<<std::endl;

                osg::Texture1D* texture1D = new osg::Texture1D;
                texture1D->setImage(tf->getImage());
                texture1D->setResizeNonPowerOfTwoHint(false);
                texture1D->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
                texture1D->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
                texture1D->setWrap(osg::Texture::WRAP_R,osg::Texture::CLAMP_TO_EDGE);
                stateset->setTextureAttributeAndModes(1,texture1D,osg::StateAttribute::ON);

                osg::Uniform* tfTextureSampler = new osg::Uniform("tfTexture",1);
                stateset->addUniform(tfTextureSampler);

                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("volume_tf_frag.frag"))))
                    return;
            }
            else
            {
                if (!program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("volume_frag.frag"))))
                    return;

            }
        }

        if (cpv._sampleDensityProperty.valid())
            stateset->addUniform(cpv._sampleDensityProperty->getUniform());
        else
            stateset->addUniform(new osg::Uniform("SampleDensityValue",0.0005f));


        if (cpv._transparencyProperty.valid())
            stateset->addUniform(cpv._transparencyProperty->getUniform());
        else
            stateset->addUniform(new osg::Uniform("TransparencyValue",1.0f));


        if (cpv._afProperty.valid())
            stateset->addUniform(cpv._afProperty->getUniform());
        else
            stateset->addUniform(new osg::Uniform("AlphaFuncValue",alphaFuncValue));


        stateset->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

        osg::TexGen* texgen = new osg::TexGen;
        texgen->setMode(osg::TexGen::OBJECT_LINEAR);
        texgen->setPlanesFromMatrix(osg::Matrix::inverse(matrix));

        stateset->setTextureAttributeAndModes(0, texgen, osg::StateAttribute::ON);

    }

    {
        _geometry = new osg::Geometry;

        osg::Vec3Array* coords = new osg::Vec3Array(8);
        (*coords)[0] = osg::Vec3d(0.0,0.0,0.0) * matrix;
        (*coords)[1] = osg::Vec3d(1.0,0.0,0.0) * matrix;
        (*coords)[2] = osg::Vec3d(1.0,1.0,0.0) * matrix;
        (*coords)[3] = osg::Vec3d(0.0,1.0,0.0) * matrix;
        (*coords)[4] = osg::Vec3d(0.0,0.0,1.0) * matrix;
        (*coords)[5] = osg::Vec3d(1.0,0.0,1.0) * matrix;
        (*coords)[6] = osg::Vec3d(1.0,1.0,1.0) * matrix;
        (*coords)[7] = osg::Vec3d(0.0,1.0,1.0) * matrix;
        _geometry->setVertexArray(coords);

        osg::Vec4Array* colours = new osg::Vec4Array(1);
        (*colours)[0].set(1.0f,1.0f,1.0,1.0f);
        _geometry->setColorArray(colours);
        _geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

        osg::DrawElementsUShort* drawElements = new osg::DrawElementsUShort(GL_QUADS);
        // bottom
        drawElements->push_back(0);
        drawElements->push_back(1);
        drawElements->push_back(2);
        drawElements->push_back(3);

        // bottom
        drawElements->push_back(3);
        drawElements->push_back(2);
        drawElements->push_back(6);
        drawElements->push_back(7);

        // left
        drawElements->push_back(0);
        drawElements->push_back(3);
        drawElements->push_back(7);
        drawElements->push_back(4);

        // right
        drawElements->push_back(5);
        drawElements->push_back(6);
        drawElements->push_back(2);
        drawElements->push_back(1);

        // front
        drawElements->push_back(1);
        drawElements->push_back(0);
        drawElements->push_back(4);
        drawElements->push_back(5);

        // top
        drawElements->push_back(7);
        drawElements->push_back(6);
        drawElements->push_back(5);
        drawElements->push_back(4);

        _geometry->addPrimitiveSet(drawElements);

        _geode->addDrawable(_geometry);
    }

}

void MyRayTracedTechnique::update(osgUtil::UpdateVisitor* uv)
{
//    osg::notify(osg::NOTICE)<<"RayTracedTechnique:update(osgUtil::UpdateVisitor* nv):"<<std::endl;
}

void MyRayTracedTechnique::cull(osgUtil::CullVisitor* cv)
{
    //osg::notify(osg::NOTICE)<<"RayTracedTechnique::cull(osgUtil::CullVisitor* nv)"<<std::endl;
    if (_geode.valid())
    {
        _geode->accept(*cv);
    }
}

void MyRayTracedTechnique::cleanSceneGraph()
{
    osg::notify(osg::NOTICE)<<"RayTracedTechnique::cleanSceneGraph()"<<std::endl;
}

void MyRayTracedTechnique::traverse(osg::NodeVisitor& nv)
{
    // osg::notify(osg::NOTICE)<<"RayTracedTechnique::traverse(osg::NodeVisitor& nv)"<<std::endl;
    if (!_volumeTile) return;

    // if app traversal update the frame count.
    if (nv.getVisitorType()==osg::NodeVisitor::UPDATE_VISITOR)
    {
        if (_volumeTile->getDirty()) _volumeTile->init();

        osgUtil::UpdateVisitor* uv = dynamic_cast<osgUtil::UpdateVisitor*>(&nv);
        if (uv)
        {
            update(uv);
            return;
        }

    }
    else if (nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR)
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(&nv);
        if (cv)
        {
            cull(cv);
            return;
        }
    }


    if (_volumeTile->getDirty())
    {
        osg::notify(osg::INFO)<<"******* Doing init ***********"<<std::endl;
        _volumeTile->init();
    }
}

