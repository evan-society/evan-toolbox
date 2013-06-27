#ifndef VOLUME_H
#define VOLUME_H

#include "IRenderable.h"
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgVolume/Volume>
#include <osgVolume/VolumeTile>
#include <osg/TransferFunction>
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <iostream>
#include <fstream>

class Volumes : public IRenderable
{
public:
    enum ShadingModel
    {
        Standard,
    //    Light,
        Isosurface,
        MaximumIntensityProjection
    };

    enum ColourSpaceOperation
    {
        NO_COLOUR_SPACE_OPERATION,
        MODULATE_ALPHA_BY_LUMINANCE,
        MODULATE_ALPHA_BY_COLOUR,
        REPLACE_ALPHA_WITH_LUMINANACE,
        REPLACE_RGB_WITH_LUMINANCE
    };

private:
    osg::ref_ptr<osgVolume::AlphaFuncProperty> m_ap;
    osg::ref_ptr<osgVolume::SwitchProperty> m_sp;
    osg::ref_ptr<osgVolume::VolumeTile> m_tile;
    osg::ref_ptr<osgVolume::Volume> m_volume;
    osg::ref_ptr<osgVolume::Layer> m_layer;
    osg::ref_ptr<osg::Image> m_image;
    ShadingModel m_shadingModel;
    ColourSpaceOperation m_colourSpaceOperation;
    osg::ref_ptr<osg::TransferFunction1D> m_transferFunction;

public:
    Volumes() : IRenderable(), m_ap(NULL), m_sp(NULL), m_tile(NULL), m_volume(NULL), m_layer(NULL), m_image(NULL),
    m_shadingModel(Standard), m_colourSpaceOperation(NO_COLOUR_SPACE_OPERATION), m_transferFunction(NULL)
    {
       addType(VOLUME_T);
    }

    ~Volumes()
    {clear();}

    void clear();

    void setAlphaValue(float value);
    float getAlphaValue() {return m_ap->getValue();}
    bool importVolume(const QString& volumePath, bool dicom=false);
    void setShadingModel(ShadingModel shadingModel)                             {m_shadingModel=shadingModel;}
    void setColourSpaceConversion(ColourSpaceOperation colourSpaceOperation)    {m_colourSpaceOperation=colourSpaceOperation;}
    void setTransferFunction(float v,osg::Vec4 color);
    void setTransferFunction(const std::string& tranferFunctionFile);

    osg::ref_ptr<osgVolume::Volume> getVolume() const {return m_volume;}
    QString getRenderableName()                 const {return QString::fromStdString(m_volume->getName());}
    void setRenderableName(const QString& n)          {m_volume->setName(n.toStdString());}
    osg::Image* doColourSpaceConversion(ColourSpaceOperation op, osg::Image* image, osg::Vec4& colour);
    void setOrthographicView(bool ortho);
    bool initialize(const QString&);

    void setSectionning(bool b, int style=0, osg::Vec3 plane1 = osg::Vec3(0.0,0.0,0.0), float clipdValue=0.0, osg::Vec3 plane2 = osg::Vec3(0.0,0.0,0.0));

    void update(){}
    bool isValid() const {return true;}
};

#endif



