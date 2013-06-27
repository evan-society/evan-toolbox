#include "Volume.h"

#include <osg/ImageUtils>
#include <Utilities/MyRayTracedTechnique.h>
#include <Utilities/MyReaderWriterDICOM.cpp>
#include <osg/CopyOp>

extern std::ofstream dbgfile; //RP 29/12/08 Debug file for use throughout toolbox

void Volumes::clear()
{
   m_ap.release();
   m_sp.release();
   m_volume.release();
   m_tile.release();
   m_layer.release();
   m_image.release();
   m_transferFunction.release();
   m_osgTransform=new osgManipulator::Selection;
}

bool Volumes::importVolume(const QString& volumePath, bool dicom)
{
#ifdef USE_DCMTK
    if (volumePath.isEmpty())
        return false;

    if (!dicom)
        m_image=osgDB::readImageFile(volumePath.toStdString());
    else
    {
        MyReaderWriterDICOM* reader = new MyReaderWriterDICOM;
        osgDB::ReaderWriter::ReadResult res = reader->readDicomImage(volumePath.toStdString(),0);
        m_image=res.getImage();
    }

   if (!m_image)
        return false;
    return true;
#else
    return false;
#endif
}

struct ModulateAlphaByLuminanceOperator
{
    ModulateAlphaByLuminanceOperator() {}

    inline void luminance(float&) const {}
    inline void alpha(float&) const {}
    inline void luminance_alpha(float& l,float& a) const { a*= l; }
    inline void rgb(float&,float&,float&) const {}
    inline void rgba(float& r,float& g,float& b,float& a) const { float l = (r+g+b)*0.3333333; a *= l;}
};

struct ModulateAlphaByColourOperator
{
    ModulateAlphaByColourOperator(const osg::Vec4& colour):_colour(colour) { _lum = _colour.length(); }

    osg::Vec4 _colour;
    float _lum;

    inline void luminance(float&) const {}
    inline void alpha(float&) const {}
    inline void luminance_alpha(float& l,float& a) const { a*= l*_lum; }
    inline void rgb(float&,float&,float&) const {}
    inline void rgba(float& r,float& g,float& b,float& a) const { a = (r*_colour.r()+g*_colour.g()+b*_colour.b()+a*_colour.a()); }
};

struct ReplaceAlphaWithLuminanceOperator
{
    ReplaceAlphaWithLuminanceOperator() {}

    inline void luminance(float&) const {}
    inline void alpha(float&) const {}
    inline void luminance_alpha(float& l,float& a) const { a= l; }
    inline void rgb(float&,float&,float&) const { }
    inline void rgba(float& r,float& g,float& b,float& a) const { float l = (r+g+b)*0.3333333; a = l; }
};

osg::Image* Volumes::doColourSpaceConversion(ColourSpaceOperation op, osg::Image* image, osg::Vec4& colour)
{
    switch(op)
    {
        case (MODULATE_ALPHA_BY_LUMINANCE):
        {
            osg::modifyImage(image,ModulateAlphaByLuminanceOperator());
            return image;
        }
        case (MODULATE_ALPHA_BY_COLOUR):
        {
            osg::modifyImage(image,ModulateAlphaByColourOperator(colour));
            return image;
        }
        case (REPLACE_ALPHA_WITH_LUMINANACE):
        {
            osg::modifyImage(image,ReplaceAlphaWithLuminanceOperator());
            return image;
        }
        case (REPLACE_RGB_WITH_LUMINANCE):
        {
            osg::Image* newImage = new osg::Image;
            newImage->allocateImage(image->s(), image->t(), image->r(), GL_LUMINANCE, image->getDataType());
            osg::copyImage(image, 0, 0, 0, image->s(), image->t(), image->r(),
                           newImage, 0, 0, 0, false);
            return newImage;
        }
        default:
            return image;
    }
}

bool Volumes::initialize(const QString& name)
{
    if (!m_image)
        return false;

    if (m_colourSpaceOperation!=NO_COLOUR_SPACE_OPERATION)
    {
        osg::Vec4 colourModulate(0.25f,0.25f,0.25f,0.25f);
        doColourSpaceConversion(m_colourSpaceOperation, m_image, colourModulate);
    }

    m_volume = new osgVolume::Volume;
    m_tile = new osgVolume::VolumeTile;
    m_volume->addChild(m_tile);

    setRenderableName(name);

    m_layer = new osgVolume::ImageLayer(m_image);

    osg::ref_ptr<osg::RefMatrix> matrix = dynamic_cast<osg::RefMatrix*>(m_image->getUserData());

    if (!matrix)
    {

        int image_s = m_image->s();
        int image_t = m_image->t();
        int image_r = m_image->r();
        float xSize=0.0f, ySize=0.0f, zSize=0.0f;

        if (xSize==0.0) xSize = static_cast<float>(image_s);
        if (ySize==0.0) ySize = static_cast<float>(image_t);
        if (zSize==0.0) zSize = static_cast<float>(image_r);

        matrix = new osg::RefMatrix(1.0, 0.0,   0.0,   0.0,
                                    0.0,   1, 0.0,   0.0,
                                    0.0,   0.0,   1, 0.0,
                                    0.0,   0.0,   0.0,   1.0);
    }

    osgVolume::Locator* locator = new osgVolume::Locator(*matrix);

    m_layer->setLocator(locator);
    m_tile->setLocator(locator);

    m_tile->setLayer(m_layer);

    m_tile->setEventCallback(new osgVolume::PropertyAdjustmentCallback());

    m_sp = new osgVolume::SwitchProperty;
    m_sp->setActiveProperty(0);

    osg::Vec4 minValue(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
    osg::Vec4 maxValue(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

    bool computeMinMax = false;
    {
        osg::Vec4 localMinValue, localMaxValue;
        if (osg::computeMinMax(m_image, localMinValue, localMaxValue))
        {
            if (localMinValue.r()<minValue.r()) minValue.r() = localMinValue.r();
            if (localMinValue.g()<minValue.g()) minValue.g() = localMinValue.g();
            if (localMinValue.b()<minValue.b()) minValue.b() = localMinValue.b();
            if (localMinValue.a()<minValue.a()) minValue.a() = localMinValue.a();

            if (localMaxValue.r()>maxValue.r()) maxValue.r() = localMaxValue.r();
            if (localMaxValue.g()>maxValue.g()) maxValue.g() = localMaxValue.g();
            if (localMaxValue.b()>maxValue.b()) maxValue.b() = localMaxValue.b();
            if (localMaxValue.a()>maxValue.a()) maxValue.a() = localMaxValue.a();

            computeMinMax = true;
        }
    }

    if (computeMinMax)
    {
        float minComponent = minValue[0];
        minComponent = osg::minimum(minComponent,minValue[1]);
        minComponent = osg::minimum(minComponent,minValue[2]);
        minComponent = osg::minimum(minComponent,minValue[3]);

        float maxComponent = maxValue[0];
        maxComponent = osg::maximum(maxComponent,maxValue[1]);
        maxComponent = osg::maximum(maxComponent,maxValue[2]);
        maxComponent = osg::maximum(maxComponent,maxValue[3]);

        float scale = 0.99f/(maxComponent-minComponent);
        float offset = -minComponent * scale;

        osg::offsetAndScaleImage(m_image, osg::Vec4(offset, offset, offset, offset), osg::Vec4(scale, scale, scale, scale));
        float radius = m_volume->getBound().radius()*2;
        osg::Matrixd mat = osg::Matrixd::scale(1.0/radius,1.0/radius,1.0/radius);

        osg::Vec3f center = -m_volume->getBound().center();

        mat.preMultTranslate(center);

        getOsgTransform()->setMatrix(mat);
    }

    m_ap = new osgVolume::AlphaFuncProperty(0.005);
    osgVolume::SampleDensityProperty* sd = new osgVolume::SampleDensityProperty(0.005);
    osgVolume::TransparencyProperty* tp = new osgVolume::TransparencyProperty(1.0);
    osgVolume::TransferFunctionProperty* tfp = m_transferFunction.valid() ? new osgVolume::TransferFunctionProperty(m_transferFunction) : 0;

    {
        // Standard
        osgVolume::CompositeProperty* cp = new osgVolume::CompositeProperty;
        cp->addProperty(m_ap);
        cp->addProperty(sd);
        cp->addProperty(tp);
       if (tfp) cp->addProperty(tfp);

        m_sp->addProperty(cp);
    }

    {
        // Isosurface
        osgVolume::CompositeProperty* cp = new osgVolume::CompositeProperty;
        cp->addProperty(sd);
        cp->addProperty(tp);
        cp->addProperty(new osgVolume::IsoSurfaceProperty(m_ap->getValue()));
        if (tfp) cp->addProperty(tfp);

        m_sp->addProperty(cp);
    }

    {
        // MaximumIntensityProjection
        osgVolume::CompositeProperty* cp = new osgVolume::CompositeProperty;
        cp->addProperty(m_ap);
        cp->addProperty(sd);
        cp->addProperty(tp);
        cp->addProperty(new osgVolume::MaximumIntensityProjectionProperty);
        if (tfp) cp->addProperty(tfp);

        m_sp->addProperty(cp);
    }

    switch(m_shadingModel)
    {
        case(Standard):                     m_sp->setActiveProperty(0); break;
        case(Isosurface):                   m_sp->setActiveProperty(1); break;
        case(MaximumIntensityProjection):   m_sp->setActiveProperty(2); break;
    }
    m_layer->addProperty(m_sp);

    MyRayTracedTechnique* technique = new MyRayTracedTechnique;

    m_tile->setVolumeTechnique(technique);

    osg::Uniform* clipStyle = new osg::Uniform("clipStyle", 0);
    m_volume->getOrCreateStateSet()->addUniform(clipStyle);
    osg::Uniform* ortho = new osg::Uniform("isOrtho",1);
    m_volume->getOrCreateStateSet()->addUniform(ortho);

    return true;
}

void Volumes::setAlphaValue(float value)
{
    m_ap->setValue(value);
    if (m_sp->getActiveProperty()==1)
    {
        osgVolume::CompositeProperty* compProp = (osgVolume::CompositeProperty*)m_sp->getProperty(1);
        compProp->setProperty(2,new osgVolume::IsoSurfaceProperty(m_ap->getValue()));
    }
    m_tile->setVolumeTechnique(new MyRayTracedTechnique);
}

void Volumes::setTransferFunction(float v,osg::Vec4 color)
{
    if (!m_transferFunction)
        m_transferFunction = new osg::TransferFunction1D();

    m_transferFunction->setColor(v, color);
}

/**
 * set the sectionning/clipping for the volume.
 *@param b if true, then sectionning will be applied if false no sectionning at all
 *@param style determine the style of sectionning/clipping for the volume 1 is positve clipping, 2 is negative clipping, 3 is a slice, 4 is sectionning
 *@param plane1 is the equation of the clipping plane or the negative value for the sectionning
 *@param clipValue is the height (d) of the plane as in the equation of the plane P(x,y,z) = ax + by +cz + d
 *@param plane2 is the positive value for the sectionning
 **/
void Volumes::setSectionning(bool b, int style, osg::Vec3 plane1, float clipdValue, osg::Vec3 plane2)
{
    if (!b || style > 5 || style <= 0)
        m_volume->getOrCreateStateSet()->getUniform("clipStyle")->set(0);
    else
    {
        m_volume->getOrCreateStateSet()->getUniform("clipStyle")->set(style);
        osg::Uniform* clipabc = new osg::Uniform("clipabc",plane1);
        m_volume->getOrCreateStateSet()->addUniform(clipabc);

        if (style == 4)
        {
            osg::Uniform* clipSup = new osg::Uniform("clipSup",plane2);
            m_volume->getOrCreateStateSet()->addUniform(clipSup);
        }
        else
        {
            osg::Uniform* clipd = new osg::Uniform("clipd",clipdValue);
            m_volume->getOrCreateStateSet()->addUniform(clipd);
        }
    }
}

void Volumes::setOrthographicView(bool ortho)
{
    if (ortho)
        m_volume->getOrCreateStateSet()->getUniform("isOrtho")->set(1);
    else
        m_volume->getOrCreateStateSet()->getUniform("isOrtho")->set(0);
}

void Volumes::setTransferFunction(const std::string& filename)
{
    std::string foundFile = osgDB::findDataFile(filename);
    if (foundFile.empty())
    {
        return ;
    }

    osg::TransferFunction1D::ColorMap valueMap;
    osgDB::ifstream fin(foundFile.c_str());
    while(fin)
    {
        float value, red, green, blue, alpha;
        fin >> value >> red >> green >> blue >> alpha;
        if (fin)
        {
            valueMap[value] = osg::Vec4(red,green,blue,alpha);
        }
    }

    if (valueMap.empty())
    {
        return ;
    }

    m_transferFunction = new osg::TransferFunction1D;
    m_transferFunction->assign(valueMap);

    return;
}

