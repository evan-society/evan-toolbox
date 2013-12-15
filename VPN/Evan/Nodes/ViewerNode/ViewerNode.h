#ifndef VIEWERNODE_H
#define VIEWERNODE_H

#include "../INode.h"
#include "../../DataTypes/Renderables.h"
#include "../../Utilities/QOSGWidget.h"
#include "RenderableTree.h"
#include <QTimer>
#include <osgManipulator/CommandManager>
#include <osgManipulator/TranslateAxisDragger>

extern ofstream dbgfile; //RP 3/1/09 Toolbox debug file

class ViewerNode;

class ViewerMainWindow : public QMainWindow
{
Q_OBJECT

private:
    RenderableTree* m_renderTable;
    ViewerNode* m_viewer;

    QMenu* m_stereoMenu;
    QMap<QAction*, osg::DisplaySettings::StereoMode> m_stereoModes;

#ifdef __APPLE__
	bool osx_loaded;
#endif

protected:
    virtual void enterEvent(QEvent* event);
    //void calculateSceneBound();

public:
    ViewerMainWindow(ViewerNode* viewer);
    virtual ~ViewerMainWindow() {} // class has virtual method(s) but doesn't have virtual destructor
    RenderableTree* getRendrableTree() const {return m_renderTable;}
    void  closeEvent(QCloseEvent * event) {close(); event->accept();}

public slots:
    void stereoAction(QAction*);
    void changeClearColour();
    void captureScreen();
    void show();
    void close();
    void focusSceneSlot(); //husky
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderableInfo
{
    QString rName;
    QColor rFrontColor;
    QColor rBackColor;
    osg::Matrixd rTransform;
    QVector<RenderableInfo> childrenInfo;
};

class ViewerNode : public AdapterWidget, public osgViewer::Viewer, public INode
{
Q_OBJECT

private:
    osg::ref_ptr<osg::DisplaySettings> m_settings;

    osg::ref_ptr<osg::Group> m_root;
    osg::ref_ptr<osg::Switch> m_axes;
    osg::ref_ptr<osg::MatrixTransform> m_axesScale;
    osg::ref_ptr<osg::Switch> m_rootSwitch;
    HybridTrackballManipulator* m_cameraManipulator;
    osg::ref_ptr<osgManipulator::TranslateAxisDragger> m_tranDragger;
    osg::ref_ptr<osgManipulator::TrackballDragger> m_rotDragger;
    osg::ref_ptr<osgManipulator::Scale2DDragger> m_scaleDragger;
    osg::ref_ptr<osgManipulator::Scale2DDragger> m_scale2Dragger;
    osg::ref_ptr<osgManipulator::CommandManager> m_manipCmdMgr;

	Renderables* m_renderInput;
	QTimer m_refreshTimer;

	ViewerMainWindow* m_window;
    QMap<IRenderable*, osg::Node*> m_currentScene;
    QMap<Volumes*, osgVolume::Volume*> m_currentVolumeScene;

    QVector<RenderableInfo> m_loadedInfo;
    osg::Matrixd m_loadedViewMatrix;
    double m_loadedCameraDist;
    double m_loadedZNear;
    double m_loadedZFar;
    bool m_projectionMode;

    void fillRInfo(const QString&, RenderableInfo*);
    void loadSavedRenderable(IRenderable*, const RenderableInfo&, const QColor&);
    void setupAxes();
    void removeFromParents(osg::Node*);
    void addOsgNode(IRenderable*, osg::ref_ptr<osg::Switch>, RenderableTreeItem*);
    void addVolumeNode(Volumes*);
    void updateVolumeNode(Volumes*);
    void updateOsgNode(IRenderable*,  osg::ref_ptr<osg::Node>);
    void positionManipulators(osg::ref_ptr<osg::Node> node);
    virtual void wheelEvent(QWheelEvent *ev)    {clip(ev);}
    void clip(QWheelEvent* ev);

    osgManipulator::Selection* getParentSelection(osg::Node* node)
    {
        osgManipulator::Selection* result = 0;
        for(unsigned int i=0; i< node->getNumParents(); ++i)
        {
            result = dynamic_cast<osgManipulator::Selection*>(node->getParent(i));
            if(result) break;
        }
       return result;
    }


protected:
	Renderables* getInputPortData0()const { return dynamic_cast< Renderables* >( getInputData(0) ); }

public:
	ViewerNode( QWidget * parent = 0, const char * name = 0,
                        const QGLWidget * shareWidget = 0, WindowFlags f = 0);
	~ViewerNode();
    ViewerMainWindow* getMainViewerWindow() { return m_window;}
    virtual void paintGL() {frame();}
	void showGUI() {m_window->activateWindow();m_window->show();}
	void process();
	bool isUpdated() const { return false;}
	void fromString(const QString& params);
    QString toString() const;
    void stopTimer()   {if (m_refreshTimer.isActive()) m_refreshTimer.stop();}
    void startTimer()  {if (!m_refreshTimer.isActive()) m_refreshTimer.start(10);}
    void focusScene();

public slots:
    void setStereo(bool on, osg::DisplaySettings::StereoMode mode)
    {
        m_settings->setStereo(on);
        if(on) m_settings->setStereoMode(mode);
    }
    void toggleAxes(bool flag) {
        m_root->dirtyBound();
        float scale=m_root->getBound().radius();
        m_axesScale->setMatrix(osg::Matrixd::scale(scale,scale,scale));
        m_axes->setValue(0, flag);
    }
    void resetRenderable(IRenderable*);
    void toggleRenderable(bool, osg::ref_ptr<osg::Node>);
    void toggleTransManip(osg::ref_ptr<osg::Node>);
    void toggleRotateManip(osg::ref_ptr<osg::Node>);
    void toggleScaleManip(osg::ref_ptr<osg::Node>);
    void removeManipulator(RenderableTreeItem*);
    void assignManipulators(RenderableTreeItem*,RenderableTreeItem*);
    void toggleProjection(bool);
    void toggleLighting(bool);
    void focusCamera(osg::Vec3,float,osg::Matrixd); // the old one - seems to focus only on Warpgrid
    void focusObjectCamera(osg::Vec3,float,osg::Matrixd);
    void addClipPlane(osg::ClipPlane*,bool);
    void clipXYplane();
    void clipXZplane();
    void clipYZplane();
    void addMyClipPlane(MyClipPlane*);
};

class PickHandler : public osgGA::GUIEventHandler {

public:
    PickHandler() {}
    ~PickHandler() {}
    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
    virtual void pick(osgViewer::Viewer* view, const osgGA::GUIEventAdapter& ea);
};
#endif
