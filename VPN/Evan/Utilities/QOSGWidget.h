#pragma once
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#ifdef _WIN32
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#endif
#include <osgGA/TrackballManipulator>
//#include <osgGA/SphericalManipulator>
#include <osgDB/ReadFile>
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/TabBoxDragger>
#include <QtGui>
#include <QGLWidget>
#if defined(_MACOS) || defined(__APPLE__)
#include <osgViewer/api/Cocoa/GraphicsWindowCocoa>
#else
#ifndef _WIN32 // This include must be last.
#include <osgViewer/api/X11/GraphicsWindowX11>
#endif
#endif

using Qt::WindowFlags;

class AdapterWidget : public QGLWidget
{
    public:

        AdapterWidget( QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0 );
        virtual ~AdapterWidget() {}
        osgViewer::GraphicsWindow* getGraphicsWindow() { return _gw.get(); }
        const osgViewer::GraphicsWindow* getGraphicsWindow() const { return _gw.get(); }

    protected:
        void init();
        virtual void resizeGL( int width, int height );
        virtual void keyPressEvent( QKeyEvent* event );
        virtual void keyReleaseEvent( QKeyEvent* event );
        virtual void mousePressEvent( QMouseEvent* event );
        virtual void mouseReleaseEvent( QMouseEvent* event );
        virtual void mouseMoveEvent( QMouseEvent* event );
        osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;
};

class HybridTrackballManipulator : public osgGA::TrackballManipulator
//class HybridTrackballManipulator : public osgGA::SphericalManipulator
{
    protected:
        osg::ref_ptr<osg::Camera> m_viewerCamera;
        bool m_ortho;
        bool calcMovement();
    public:
		HybridTrackballManipulator(osg::Camera* cam) : osgGA::TrackballManipulator(), m_viewerCamera(cam), m_ortho(true)
        //HybridTrackballManipulator(osg::Camera* cam) : osgGA::SphericalManipulator(), m_viewerCamera(cam), m_ortho(true)
        {}
        bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
        void home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
        void setOrtho(bool flag) {m_ortho = flag;}
        void applyOrtho(double,double,double);
};

class DraggerHandler : public osgGA::GUIEventHandler
{
    private:
        osgManipulator::Dragger* _activeDragger;
        osgManipulator::PointerInfo _pointer;
    public:
        DraggerHandler():
            _activeDragger(0)
        {
        }

        bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                    osg::Object*, osg::NodeVisitor*);
};



