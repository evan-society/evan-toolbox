#include "ViewerNode.h"
#include <osg/Light>
#include <osg/Material>
#include <osg/LightSource>
#include <osg/StateAttribute>
#include <osgGA/StateSetManipulator>
#include <osg/Array>
#include <osg/ShapeDrawable>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/LightModel>
#include <osg/ShadeModel>
#include <osgUtil/Optimizer>
#include <osg/LineWidth>
#include <osg/ClipPlane>
#include <osgUtil/SmoothingVisitor>
#include "../../Utilities/Logger.h"
#include <osgText/Text>
#include "DataTypes/LMKSetRenderable.h"
#include <QSettings>

ViewerMainWindow::ViewerMainWindow(ViewerNode* viewer): QMainWindow(), m_viewer(viewer)
{
    std::cout << "ViewerMainWindow::ViewerMainWindow\n"; //husky debug

    QMenuBar* menubar = new QMenuBar();
    QMenu* viewMenu = new QMenu("View");

    QAction* axesAction = new QAction("Show Axes", viewMenu);
    axesAction->setCheckable(true);
    viewMenu->addAction(axesAction);
    connect(axesAction, SIGNAL(toggled(bool)), m_viewer, SLOT(toggleAxes(bool)));

    QAction* focusAction = new QAction("Focus Scene", viewMenu);
    viewMenu->addAction( focusAction );
    connect( focusAction, SIGNAL(triggered()), this, SLOT( focusSceneSlot() ) );

    QAction* bckgnd = new QAction("Background Colour", viewMenu);
    viewMenu->addAction(bckgnd);
    connect(bckgnd, SIGNAL(triggered()), this, SLOT(changeClearColour()));

    QAction* orhtoAction = new QAction("Orthographic Projection", viewMenu);
    orhtoAction->setCheckable(true);
    orhtoAction->setChecked(true); // sync projection state menu/viewer
    viewMenu->addAction(orhtoAction);
    connect(orhtoAction, SIGNAL(toggled(bool)), m_viewer, SLOT(toggleProjection(bool)));
    m_viewer->toggleProjection( true ); // sync projection state menu/viewer

    QAction* lightingAction = new QAction("Lights On/Off", viewMenu);
    lightingAction->setCheckable(true);
    lightingAction->setChecked(true);
    viewMenu->addAction(lightingAction);
    connect(lightingAction, SIGNAL(toggled(bool)), m_viewer, SLOT(toggleLighting(bool)));

    menubar->addMenu(viewMenu);

    m_stereoMenu = new QMenu("Stereo Mode");

    QAction* quad = new QAction("Quad Buffer", m_stereoMenu);
    quad->setCheckable(true);
    m_stereoModes.insert(quad, osg::DisplaySettings::QUAD_BUFFER);
    m_stereoMenu->addAction(quad);

    QAction* anaglyphic = new QAction("Anaglyphic", m_stereoMenu);
    anaglyphic->setCheckable(true);
    m_stereoModes.insert(anaglyphic, osg::DisplaySettings::ANAGLYPHIC);
    m_stereoMenu->addAction(anaglyphic);

    QAction* hSplit = new QAction("Horizontal Split", m_stereoMenu);
    hSplit->setCheckable(true);
    m_stereoModes.insert(hSplit, osg::DisplaySettings::HORIZONTAL_SPLIT);
    m_stereoMenu->addAction(hSplit);

    QAction* vSplit = new QAction("Vertictal Split", m_stereoMenu);
    vSplit->setCheckable(true);
    m_stereoModes.insert(vSplit, osg::DisplaySettings::VERTICAL_SPLIT);
    m_stereoMenu->addAction(vSplit);

    QAction* lEye = new QAction("Left Eye", m_stereoMenu);
    lEye->setCheckable(true);
    m_stereoModes.insert(lEye, osg::DisplaySettings::LEFT_EYE);
    m_stereoMenu->addAction(lEye);

    QAction* rEye = new QAction("Right Eye", m_stereoMenu);
    rEye->setCheckable(true);
    m_stereoModes.insert(rEye, osg::DisplaySettings::RIGHT_EYE);
    m_stereoMenu->addAction(rEye);

    QAction* hInterlace = new QAction("Horizontal Interlace", m_stereoMenu);
    hInterlace->setCheckable(true);
    m_stereoModes.insert(hInterlace, osg::DisplaySettings::HORIZONTAL_INTERLACE);
    m_stereoMenu->addAction(hInterlace);

    QAction* vInterlace = new QAction("Vertictal Interlace", m_stereoMenu);
    vInterlace->setCheckable(true);
    m_stereoModes.insert(vInterlace, osg::DisplaySettings::VERTICAL_INTERLACE);
    m_stereoMenu->addAction(vInterlace);

    QAction* checkerboard = new QAction("Checkerboard", m_stereoMenu);
    checkerboard->setCheckable(true);
    m_stereoModes.insert(checkerboard, osg::DisplaySettings::CHECKERBOARD);
    m_stereoMenu->addAction(checkerboard);

    connect(m_stereoMenu, SIGNAL(triggered(QAction*)), this, SLOT(stereoAction(QAction*)));
    menubar->addMenu(m_stereoMenu);

    QMenu* captureMenu = new QMenu("Capture");
    menubar->addMenu(captureMenu);
    QAction* captureAction = new QAction("Get a screenshot",captureMenu );
    connect(captureAction, SIGNAL(triggered()), this, SLOT(captureScreen()));
    captureMenu->addAction(captureAction);

    QMenu* clipMenu = new QMenu("Add Clipping Plane");
    menubar->addMenu(clipMenu);

    QAction* clipAction1 = new QAction("X-Y plane",clipMenu );
    connect(clipAction1, SIGNAL(triggered()), m_viewer, SLOT(clipXYplane()));
    clipMenu->addAction(clipAction1);

    QAction* clipAction2 = new QAction("X-Z plane",clipMenu );
    connect(clipAction2, SIGNAL(triggered()), m_viewer, SLOT(clipXZplane()));
    clipMenu->addAction(clipAction2);

    QAction* clipAction3 = new QAction("Y-Z plane",clipMenu );
    connect(clipAction3, SIGNAL(triggered()), m_viewer, SLOT(clipYZplane()));
    clipMenu->addAction(clipAction3);

    QWidget* centralwidget = new QWidget(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout(centralwidget);
    QSplitter* page = new QSplitter();
    horizontalLayout->addWidget(page);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    viewer->setSizePolicy(sizePolicy);
    viewer->setMinimumSize(QSize(10, 0));
    page->addWidget(viewer);

    m_renderTable = new RenderableTree;
    m_renderTable->setColumnCount(1);
    m_renderTable->setHeaderLabels(QStringList() << tr("Renderables"));
    QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_renderTable->setSizePolicy(sizePolicy2);
    m_renderTable->setMinimumSize(QSize(10, 0));

    connect(m_renderTable, SIGNAL(renderableReset(IRenderable*)),
            m_viewer, SLOT(resetRenderable(IRenderable*)));
    connect(m_renderTable, SIGNAL(renderableToggled(bool,osg::ref_ptr<osg::Node>)),
           m_viewer, SLOT(toggleRenderable(bool,osg::ref_ptr<osg::Node>)));
    connect(m_renderTable, SIGNAL(transManipClicked(osg::ref_ptr<osg::Node>)),
            m_viewer, SLOT(toggleTransManip(osg::ref_ptr<osg::Node>)));
    connect(m_renderTable, SIGNAL(rotateManipClicked(osg::ref_ptr<osg::Node>)),
            m_viewer, SLOT(toggleRotateManip(osg::ref_ptr<osg::Node>)));
    connect(m_renderTable, SIGNAL(scaleManipClicked(osg::ref_ptr<osg::Node>)),
            m_viewer, SLOT(toggleScaleManip(osg::ref_ptr<osg::Node>)));
    connect(m_renderTable, SIGNAL(renderableChanged(RenderableTreeItem*,RenderableTreeItem*)),
            m_viewer, SLOT(assignManipulators(RenderableTreeItem*,RenderableTreeItem*)));
//    connect(m_renderTable, SIGNAL(signalFocusCamera(osg::Vec3,float,osg::Matrixd)),
//            m_viewer, SLOT(focusCamera(osg::Vec3,float,osg::Matrixd)));
    connect(m_renderTable, SIGNAL(signalFocusObjectCamera( const osg::Vec3 &, const float, const osg::Matrixd *const )),
            m_viewer, SLOT(focusObjectCamera( const osg::Vec3 &, const float, const osg::Matrixd *const )));
    connect(m_renderTable, SIGNAL(addClipPlane(osg::ClipPlane*,bool)),m_viewer,SLOT(addClipPlane(osg::ClipPlane*,bool)));
    connect(m_renderTable, SIGNAL(addMyClipPlane(MyClipPlane*)),m_viewer,SLOT(addMyClipPlane(MyClipPlane*)));
    connect(m_renderTable, SIGNAL(removeManipulator(RenderableTreeItem*)),m_viewer,SLOT(removeManipulator(RenderableTreeItem*)));

    page->addWidget(m_renderTable);
    resize(800,600);

    setMenuBar(menubar);
    setCentralWidget(centralwidget);
    setWindowTitle("3D Viewer");
}

void ViewerMainWindow::show()
{
    QMainWindow::show();
    m_viewer->startTimer();

#ifdef __APPLE__
	// On OS X we need to set the projection again once the window is shown the first time
	if(!osx_loaded) {
		osx_loaded = true;
	    m_viewer->toggleProjection(1);
	}
#endif

}

void ViewerMainWindow::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
    m_renderTable->updateName();
}

void ViewerMainWindow::close()
{
    m_viewer->stopTimer();
    QMainWindow::close();
}

void ViewerMainWindow::stereoAction(QAction* clickedItem)
{
    bool toggled = clickedItem->isChecked();
    osg::DisplaySettings::StereoMode mode;
    if(toggled)
    {
        QMapIterator<QAction*, osg::DisplaySettings::StereoMode> i(m_stereoModes);
        while (i.hasNext())
        {
            i.next();
            if( i.key() == clickedItem)
                mode = i.value();
            else
                i.key()->setChecked(false);
        }
    }
    m_viewer->setStereo(toggled, mode);
}

void ViewerMainWindow::focusSceneSlot()
{
    m_viewer->focusScene();
}

void ViewerMainWindow::changeClearColour()
{
    QColor result;
    osg::Vec4 clearColour = m_viewer->getCamera()->getClearColor();
    result.setRgbF(clearColour.x(),clearColour.y(),clearColour.z(),clearColour.w());
    result = QColorDialog::getColor(result.rgba());
    if(result.isValid())
    {
        qreal r,g,b,a;
        result.getRgbF(&r, &g, &b, &a);
        m_viewer->getCamera()->setClearColor( osg::Vec4( r, g, b, a) );
    }
}

void ViewerMainWindow::captureScreen()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

   QString fileName = QFileDialog::getSaveFileName(this, tr("Save screenshot"), fsLastDir,
                tr("Portable Network Graphics (*.png)\nJoint Photographic Experts Group (*.jpg)\nWindows Bitmap (*.bmp)"));
    if(fileName != "")
    {

        QImage image = m_viewer->grabFrameBuffer();
        image.save(fileName);

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ViewerNode::ViewerNode( QWidget * parent , const char * name ,
                                        const QGLWidget * shareWidget, WindowFlags f ) :
                                        AdapterWidget(parent, name, shareWidget, f), INode()
{
    m_renderInput = NULL;

    m_settings = new osg::DisplaySettings();
    m_settings->setStereo(false);
    setDisplaySettings(m_settings);

    m_root = new osg::Group;
    m_rootSwitch = new osg::Switch;

    m_root->addChild(m_rootSwitch.get());
    m_tranDragger = new osgManipulator::TranslateAxisDragger;
    m_tranDragger->setupDefaultGeometry();
    m_rotDragger = new osgManipulator::TrackballDragger;
    m_rotDragger->setupDefaultGeometry();
    m_scaleDragger = new osgManipulator::Scale2DDragger;
    m_scaleDragger->setupDefaultGeometry();
    m_scale2Dragger = new osgManipulator::Scale2DDragger;
    m_scale2Dragger->setupDefaultGeometry();
    m_manipCmdMgr = new osgManipulator::CommandManager;

	getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
    getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    getCamera()->setGraphicsContext(getGraphicsWindow());

    const double aspectRatio = double(width())/double(height());
    const double fov = 50.0;
    const double y = fov/90.0;
    const double x = y * aspectRatio;
    getCamera()->setProjectionMatrixAsOrtho( -x, x, -y, y, 0.1f, 10000.0f );
    //getCamera()->setProjectionMatrixAsPerspective(fov, aspectRatio, 0.1f, 10000.0f);

    connect(&m_refreshTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
    addEventHandler(new osgViewer::StatsHandler());
    addEventHandler(new DraggerHandler());
    addEventHandler(new osgGA::StateSetManipulator(getCamera()->getOrCreateStateSet()) );
    m_cameraManipulator = new HybridTrackballManipulator(getCamera());
    setCameraManipulator(m_cameraManipulator);
    addEventHandler(new PickHandler());

    // Allow back material when no shaders
    osg::LightModel* lm = new osg::LightModel();
    lm->setTwoSided(true);
    m_root->getOrCreateStateSet()->setAttribute(lm);
    // Allow back material with shaders
    m_root->getOrCreateStateSet()->setMode(GL_VERTEX_PROGRAM_TWO_SIDE, osg::StateAttribute::ON);

    m_loadedViewMatrix.makeIdentity();
    m_window = new ViewerMainWindow(this);
    setupAxes();
    updateGL();
}

ViewerNode::~ViewerNode()
{
    m_root->removeChildren(0, m_root->getNumChildren());
    m_window->close();
}

void ViewerNode::setupAxes()
{
    m_axes = new osg::Switch;
    osg::Geode* axesNode = new osg::Geode;
    osg::Geometry* geom = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(6);
    osg::Vec3 corner(0.0, 0.0, 0.0);
    (*coords)[0] = corner;
    (*coords)[1] = corner+osg::Vec3(2, 0.0, 0.0);
    (*coords)[2] = corner;
    (*coords)[3] = corner+osg::Vec3(0.0, 2, 0.0);
    (*coords)[4] = corner;
    (*coords)[5] = corner+osg::Vec3(0.0, 0.0, 2);

    osg::Box* c1 = new osg::Box((*coords)[1],0.05);
    osg::Box* c2 = new osg::Box((*coords)[3],0.05);
    osg::Box* c3 = new osg::Box((*coords)[5],0.05);
    osg::ShapeDrawable* s1 = new osg::ShapeDrawable(c1);
    osg::ShapeDrawable* s2 = new osg::ShapeDrawable(c2);
    osg::ShapeDrawable* s3 = new osg::ShapeDrawable(c3);

    geom->setVertexArray(coords);

    osg::Vec4 x_color(0.0f,1.0f,0.0f,1.0f);
    osg::Vec4 y_color(0.0f,0.0f,1.0f,1.0f);
    osg::Vec4 z_color(1.0f,0.0f,0.0f,1.0f);

    osg::Vec4Array* color = new osg::Vec4Array(6);
    (*color)[0] = x_color;
    (*color)[1] = x_color;
    (*color)[2] = y_color;
    (*color)[3] = y_color;
    (*color)[4] = z_color;
    (*color)[5] = z_color;

    s1->setColor(x_color);
    s2->setColor(y_color);
    s3->setColor(z_color);

    geom->setColorArray(color);
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6));

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(2.0f);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    geom->setStateSet(stateset);

    axesNode->addDrawable(geom);
    axesNode->addDrawable(s1);
    axesNode->addDrawable(s2);
    axesNode->addDrawable(s3);
    m_axes->addChild(axesNode);
    m_axes->setValue(0, false);
    m_axesScale = new osg::	MatrixTransform();
    m_axesScale->addChild(m_axes.get());

    m_root->addChild(m_axesScale);
}

void ViewerNode::loadSavedRenderable(IRenderable* renderable, const RenderableInfo& rInfo, const QColor& parentColor)
{
    if(renderable->getRenderableName() == rInfo.rName)
    {
        renderable->setFrontMaterial(rInfo.rFrontColor, rInfo.rFrontColor!=parentColor);
        renderable->setBackMaterial(rInfo.rBackColor, rInfo.rFrontColor!=parentColor);
        renderable->getOsgTransform()->setMatrix(rInfo.rTransform);
        for(unsigned int i=0; i<renderable->getChildrenNum(); ++i)
            if((int)i <rInfo.childrenInfo.count())
                loadSavedRenderable(renderable->getChild(i), rInfo.childrenInfo[i], rInfo.rFrontColor);
    }
}



void ViewerNode::process()
{
	m_renderInput = getInputPortData0();
	if ( m_renderInput == NULL || !m_renderInput->isValid() )
        return;
    //Check for new renderables
    bool newRenderable = false;
	for(unsigned int i=0; i<m_renderInput->getSize(); ++i)
	{
        IRenderable* renderable = m_renderInput->getRenderable(i);
        if(!renderable)
            continue;
        if(!renderable->getOsgNode().valid())
        {
            if (renderable->isType(IDataType::VOLUME_T)) // renderabe is a Volumes
            {
               // RenderableTreeItem* item = new RenderableTreeItem(m_window->getRendrableTree());
                Volumes* volume = (Volumes*)renderable;
                newRenderable = !(m_currentVolumeScene.contains(volume));
                if (newRenderable)
                {
                    Logger::getInstance()->log("[Viewer Node] New Volume Renderable");
                    addVolumeNode(volume);
                    VolumeRenderableTreeItem* item = new VolumeRenderableTreeItem(m_window->getRendrableTree());
                    item->setItem(volume->getVolume(), volume);
                    item->setText(0, "Volume");
                    item->setCheckState(0, Qt::Checked);
                }
                else
                {
                    if (m_currentVolumeScene.value(volume) == volume->getVolume())
                        continue;
                    updateVolumeNode(volume);
                    Logger::getInstance()->log("[Viewer Node] Updated Volume Renderable");
                }
            }
            continue;
        }
        renderable->update();
        newRenderable = !(m_currentScene.contains(renderable));
        if(newRenderable)
        {
            if((int)i<m_loadedInfo.count())
                loadSavedRenderable(renderable, m_loadedInfo[i], Qt::transparent);

            RenderableTreeItem* item = new RenderableTreeItem(m_window->getRendrableTree());
            addOsgNode(renderable, m_rootSwitch, item);
            item->setText(0, renderable->getRenderableName());
            item->setData(0, Qt::DecorationRole, renderable->getFrontMaterial());
            item->setCheckState(0, Qt::Checked);
            Logger::getInstance()->log("[Viewer Node] New Renderable : '"+renderable->getRenderableName()+"'");

            //positionManipulators( renderable->getOsgNode() ); //husky
        }
        else if(m_currentScene.value(renderable) != renderable->getOsgNode().get()) {
            updateOsgNode(renderable, m_currentScene.value(renderable));

            //positionManipulators( renderable->getOsgNode() ); //husky
        }
	}

    if(newRenderable)
    {
        // run optimization over the scene graph
        osgUtil::Optimizer optimzer;
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS);
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::REMOVE_REDUNDANT_NODES );
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::REMOVE_LOADED_PROXY_NODES );
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::COMBINE_ADJACENT_LODS );
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::MERGE_GEOMETRY );
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::CHECK_GEOMETRY );
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::SPATIALIZE_GROUPS );
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::TRISTRIP_GEOMETRY );
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::TESSELLATE_GEOMETRY );
        optimzer.optimize(m_root.get(),osgUtil::Optimizer::MERGE_GEODES );

//        Volumes* test = new Volumes();
//        test->importVolume(QString("D:\\CardiacCT\\CardiacCT.dicom"));
//        test->setShadingModel(Volumes::MaximumIntensityProjection);
//        test->initialize("volume");
//        test->setSectionning(true, 2, osg::Vec3(1.0,0.0,0.0), 0.5);
//        test->setAlphaValue(0.07);


       // m_root->addChild(test->getVolume());
        setSceneData(m_root.get());
//        if(!m_refreshTimer.isActive())
//            m_refreshTimer.start(10);

        if(!m_loadedViewMatrix.isIdentity())
        {
            double left=0, right=0, bottom=0, up=0, zNear=0, zFar=0;
            getCamera()->getProjectionMatrixAsOrtho(left, right, bottom, up, zNear, zFar);
            double aspectRatio = up ? right/up : 0;
            m_cameraManipulator->setDistance(m_loadedCameraDist);

            m_cameraManipulator->applyOrtho(aspectRatio,m_loadedZNear,m_loadedZFar);
            m_cameraManipulator->setByMatrix(m_loadedViewMatrix);
            m_loadedViewMatrix.makeIdentity();
        }
    }

    focusScene();
}

void ViewerNode::focusSceneObject( const osg::Vec3 &center, const float radius, const osg::Matrixd *const matrix )
{
   // now set viewer matrix
   osg::Matrixd Vmat;
   if ( matrix != NULL ) {
       Vmat = *matrix;
   } else {
       // no matter which matrix is passed into this method, use the current camera manipulator matrix
       Vmat = m_cameraManipulator->getMatrix();
   }


    // osg::Matrix::operator() ( int row, int column )
    osg::Vec3 upDir = osg::Vec3( Vmat( 1, 0 ), Vmat( 1, 1 ), Vmat( 1, 2 ) );
    osg::Vec3 viewDir = osg::Vec3( Vmat( 2, 0 ), Vmat( 2, 1 ), Vmat( 2, 2 ) );

    float distance = radius * 2.0f;


//    Logger::getInstance()->log(     QString( "[ViewerNode] process() viewDir: " ) +
//                                    QString( "( " ) +   QString::number( viewDir.x() ) + QString( ", " ) +
//                                                        QString::number( viewDir.y() ) + QString( ", " ) +
//                                                        QString::number( viewDir.z() ) + QString( " ), " ) );

//osg::Matrix::lookAt (const Vec3f &eye, const Vec3f &center, const Vec3f &up)

    m_loadedCameraDist = distance; // seems to not be necessary

    m_cameraManipulator->setTransformation ( viewDir * distance + center, center, upDir );

    m_loadedViewMatrix = m_cameraManipulator->getMatrix();

    m_cameraManipulator->setDistance( distance );

    toggleProjection( m_projectionMode );

}

void ViewerNode::focusScene()
{
    QMap< IRenderable*, osg::Node* >::const_iterator it;
    QMap< IRenderable*, osg::Node* >::const_iterator itEnd = m_currentScene.end();

    QMap< Volumes*, osgVolume::Volume* >::const_iterator itVol;
    QMap< Volumes*, osgVolume::Volume* >::const_iterator itVolEnd = m_currentVolumeScene.end();

//    osg::Vec3 minAABB( +FLT_MAX, +FLT_MAX, +FLT_MAX );
//    osg::Vec3 maxAABB( -FLT_MAX, -FLT_MAX, -FLT_MAX );
//
//    // manually determine AABB
//    for ( it = m_currentScene.begin() ; it != itEnd; ++it ) {
//        float r = it.value()->getBound().radius();
//        osg::Vec3 rVec = osg::Vec3( r, r, r );
//        osg::Vec3 c = it.value()->getBound().center();
//        osg::Vec3 minC = c - rVec;
//        osg::Vec3 maxC = c + rVec;
//
//        if ( minC.x() < minAABB.x() ) {
//            minAABB.x() = minC.x();
//        }
//        if ( minC.y() < minAABB.y() ) {
//            minAABB.y() = minC.y();
//        }
//        if ( minC.z() < minAABB.z() ) {
//            minAABB.z() = minC.z();
//        }
//
//        if ( maxC.x() > maxAABB.x() ) {
//            maxAABB.x() = maxC.x();
//        }
//        if ( maxC.y() > maxAABB.y() ) {
//            maxAABB.y() = maxC.y();
//        }
//        if ( maxC.z() > maxAABB.z() ) {
//            maxAABB.z() = maxC.z();
//        }
//    }
//    Logger::getInstance()->log(     QString( "[ViewerNode] process() AABB manually: " ) +
//                                    QString( "( " ) +   QString::number( minAABB.x() ) + QString( ", " ) +
//                                                        QString::number( minAABB.y() ) + QString( ", " ) +
//                                                        QString::number( minAABB.z() ) + QString( " ), " ) +
//                                    QString( "( " ) +   QString::number( maxAABB.x() ) + QString( ", " ) +
//                                                        QString::number( maxAABB.y() ) + QString( ", " ) +
//                                                        QString::number( maxAABB.z() ) + QString( " )" )  );


    // let OSG perform the heavy lifting
    osg::Group *groupRootAABB = new osg::Group();

    for ( it = m_currentScene.begin() ; it != itEnd; ++it ) {
            groupRootAABB->addChild( it.value() );
    }
    for ( itVol = m_currentVolumeScene.begin() ; itVol != itVolEnd; ++itVol ) {
            groupRootAABB->addChild( itVol.value() );
    }

    osg::Vec3 groupC = groupRootAABB->getBound().center();
    float groupR = groupRootAABB->getBound().radius();

//    Logger::getInstance()->log(     QString( "[ViewerNode] process() AABB osg: " ) +
//                                    QString( "( " ) +   QString::number( groupC.x() ) + QString( ", " ) +
//                                                        QString::number( groupC.y() ) + QString( ", " ) +
//                                                        QString::number( groupC.z() ) + QString( " ), " ) +
//                                    QString( "radius = " ) + QString::number( groupR ) );


    //delete groupRootAABB;
    // delete does not work => ref counted
    for ( it = m_currentScene.begin() ; it != itEnd; ++it ) {
            groupRootAABB->removeChild( it.value() );
    }
    for ( itVol = m_currentVolumeScene.begin() ; itVol != itVolEnd; ++itVol ) {
            groupRootAABB->removeChild( itVol.value() );
    }

    groupRootAABB->unref();

    focusSceneObject( groupC, groupR, &m_loadedViewMatrix );
}

void ViewerNode::removeFromParents(osg::Node* node)
{
    if(m_rootSwitch.get() == node)
        return;
    for(unsigned int i=0; i< node->getNumParents(); ++i)
    {
        osg::Group* parent = node->getParent(i);
        parent->removeChild(node);
        removeFromParents(parent);
    }
}

void ViewerNode::updateOsgNode(IRenderable* renderable,  osg::ref_ptr<osg::Node> oldNode)
{
    removeFromParents(oldNode);

    RenderableTreeItem* updatedItem = m_window->getRendrableTree()->getItem(oldNode.get());
    if (updatedItem != NULL)
    {
        renderable->setFrontMaterial(renderable->getFrontMaterial(), true);
        renderable->setBackMaterial(renderable->getBackMaterial(), true);
        updatedItem->setData(0, Qt::DecorationRole, renderable->getFrontMaterial());
        updatedItem->setCheckState(0, Qt::Checked);
        foreach(QTreeWidgetItem* child, updatedItem->takeChildren())
            delete child;
    }
    addOsgNode(renderable, m_rootSwitch, updatedItem);
    assignManipulators(updatedItem, NULL);
    Logger::getInstance()->log("[Viewer Node] Updated Renderable : '"+renderable->getRenderableName()+"'");
}

void ViewerNode::updateVolumeNode(Volumes* volume)
{
    osgVolume::Volume* oldVolume = m_currentVolumeScene.value(volume);
    VolumeRenderableTreeItem* updatedItem = m_window->getRendrableTree()->getVolumeItem(oldVolume);
    updatedItem->setItem(volume->getVolume(),volume);
    volume->getOsgTransform()->addChild(volume->getVolume());
    osg::ref_ptr<osgManipulator::Selection> oldNodeSelect = (osgManipulator::Selection*)oldVolume->getParent(0);
    m_rootSwitch->replaceChild(oldNodeSelect.get(), volume->getOsgTransform().get());
    m_currentVolumeScene.remove(volume);
    m_currentVolumeScene.insert(volume, volume->getVolume());
    Logger::getInstance()->log("[Viewer Node] Updated Volume Renderable");
}

void ViewerNode::addOsgNode(IRenderable* renderable,  osg::ref_ptr<osg::Switch> parent,
                                                        RenderableTreeItem* parentTreeItem)
{
    if(!renderable || !parentTreeItem)
        return;

    osg::ref_ptr<osgUtil::SmoothingVisitor> normalizer = new osgUtil::SmoothingVisitor;
    osg::ref_ptr<osg::Geode> node = renderable->getOsgNode();
    osg::ref_ptr<osgManipulator::Selection> nodeSelect = renderable->getOsgTransform();
    if(renderable->isRaw() && node.valid()) node->accept(*normalizer);

    if(renderable->getChildrenNum() > 0)
    {
        osg::ref_ptr<osg::Switch> newParent(new osg::Switch);
        nodeSelect->addChild(newParent.get());
        parent->addChild(nodeSelect.get());
        parentTreeItem->setItem(newParent, renderable);

        m_currentScene.insert(renderable, newParent);
        newParent->setName(node->getName());
        renderable->setOsgNode(newParent.get());
        renderable->setFrontMaterial(renderable->getFrontMaterial(), true);

        for(unsigned int i=0; i<renderable->getChildrenNum(); ++i)
        {
            IRenderable* childRenderable = renderable->getChild(i);
            if(childRenderable && childRenderable->getOsgNode().valid())
            {
                RenderableTreeItem* newItem = new RenderableTreeItem(parentTreeItem);
                newItem->setText(0,childRenderable->getRenderableName());
                newItem->setData(0, Qt::DecorationRole, childRenderable->getFrontMaterial());
                newItem->setCheckState(0, Qt::Checked);
                addOsgNode(childRenderable, newParent, newItem);
            }
        }
    }
    else
    {
        nodeSelect->addChild(node.get());
        parent->addChild(nodeSelect.get());
        parentTreeItem->setItem(node, renderable);
        m_currentScene.insert(renderable, node);
        parentTreeItem->setCheckState(0, Qt::Checked);
        //renderable->setMatColor(renderable->getMatColor(), true);
    }
}

void ViewerNode::addVolumeNode(Volumes* volume)
{
    if(!volume)
        return;

    if(volume->getVolume())
    {
        osg::ref_ptr<osgManipulator::Selection> nodeSelect = volume->getOsgTransform();
        nodeSelect->addChild(volume->getVolume());
        m_rootSwitch->addChild(nodeSelect);
        m_currentVolumeScene.insert(volume, volume->getVolume());
    }
}

void ViewerNode::resetRenderable(IRenderable* renderable)
{
    renderable->resetTransform();
}


void ViewerNode::toggleRenderable(bool flag, osg::ref_ptr<osg::Node> osgNode)
{
    for(unsigned int i=0; i< osgNode->getNumParents(); ++i)
    {
        osg::ref_ptr<osg::Group> osgParent =osgNode->getParent(i);
        if(osgParent.valid())
        {
            osg::ref_ptr<osg::Switch> osgSwitch = osgParent->asSwitch();
            if(osgSwitch.valid())
            {
                osgSwitch->setChildValue(osgNode.get(), flag);
                if(!flag)
                {
                    osgSwitch->setChildValue(m_tranDragger.get(), flag);
                    osgSwitch->setChildValue(m_rotDragger.get(), flag);
                    osgSwitch->setChildValue(m_scaleDragger.get(), flag);
                    osgSwitch->setChildValue(m_scale2Dragger.get(), flag);
                }
                break;
            }
            else toggleRenderable(flag, osgParent);
        }
    }
}

void ViewerNode::toggleTransManip(osg::ref_ptr<osg::Node> node)
{
    osg::ref_ptr<osg::Group> osgParent = m_tranDragger->getParent(0);
    if(osgParent.valid())
    {
        osg::ref_ptr<osg::Switch> osgSwitch = osgParent->asSwitch();
        if(osgSwitch.valid())
        {
            osgSwitch->setChildValue(m_tranDragger.get(), !osgSwitch->getChildValue(m_tranDragger.get()));
            osgSwitch->setChildValue(m_rotDragger.get(), false);
            osgSwitch->setChildValue(m_scaleDragger.get(), false);
            osgSwitch->setChildValue(m_scale2Dragger.get(), false);
            positionManipulators(node);

            QAction* scale =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Scale");
            scale->setChecked(false);
            QAction* rotate =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Rotate");
            rotate->setChecked(false);
            QAction* translate =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Translate");
            translate->setChecked(osgSwitch->getChildValue(m_tranDragger.get()));
        }
    }
}

void ViewerNode::toggleRotateManip(osg::ref_ptr<osg::Node> node)
{
    osg::ref_ptr<osg::Group> osgParent = m_rotDragger->getParent(0);
    if(osgParent.valid())
    {
        osg::ref_ptr<osg::Switch> osgSwitch = osgParent->asSwitch();
        if(osgSwitch.valid())
        {
            osgSwitch->setChildValue(m_rotDragger.get(), !osgSwitch->getChildValue(m_rotDragger.get()));
            osgSwitch->setChildValue(m_tranDragger.get(), false);
            osgSwitch->setChildValue(m_scaleDragger.get(), false);
            osgSwitch->setChildValue(m_scale2Dragger.get(), false);
            positionManipulators(node);

            QAction* scale =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Scale");
            scale->setChecked(false);
            QAction* rotate =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Rotate");
            rotate->setChecked(osgSwitch->getChildValue(m_rotDragger.get()));
            QAction* translate =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Translate");
            translate->setChecked(false);
        }
    }
}

void ViewerNode::toggleScaleManip(osg::ref_ptr<osg::Node> node)
{
    osg::ref_ptr<osg::Group> osgParent = m_scaleDragger->getParent(0);
    if(osgParent.valid())
    {
        osg::ref_ptr<osg::Switch> osgSwitch = osgParent->asSwitch();
        if(osgSwitch.valid())
        {
            osgSwitch->setChildValue(m_scaleDragger.get(), !osgSwitch->getChildValue(m_scaleDragger.get()));
            osgSwitch->setChildValue(m_scale2Dragger.get(), !osgSwitch->getChildValue(m_scale2Dragger.get()));
            osgSwitch->setChildValue(m_tranDragger.get(), false);
            osgSwitch->setChildValue(m_rotDragger.get(), false);
            positionManipulators(node);

            QAction* scale =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Scale");
            scale->setChecked(osgSwitch->getChildValue(m_scaleDragger.get()));
            QAction* rotate =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Rotate");
            rotate->setChecked(false);
            QAction* translate =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Translate");
            translate->setChecked(false);
        }
    }
}

void ViewerNode::removeManipulator(RenderableTreeItem* prevItem)
{
    if(prevItem)
    {
        osg::ref_ptr<osg::Node> prevNode = prevItem->getItemPtr();
        if (!prevNode)
            return;
        osgManipulator::Selection* prevSelect = getParentSelection(prevNode);
        if(prevSelect)
        {
            osg::ref_ptr<osg::Switch> prevSwitch = prevSelect->getParent(0)->asSwitch();

            prevSwitch->removeChild(m_tranDragger.get());
            prevSwitch->removeChild(m_rotDragger.get());
            prevSwitch->removeChild(m_scaleDragger.get());
            prevSwitch->removeChild(m_scale2Dragger.get());
            m_manipCmdMgr->disconnect(*m_tranDragger);
            m_manipCmdMgr->disconnect(*m_rotDragger);
            m_manipCmdMgr->disconnect(*m_scaleDragger);
            m_manipCmdMgr->disconnect(*m_scale2Dragger);
        }
        QAction* scale =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Scale");
        scale->setChecked(false);
        QAction* rotate =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Rotate");
        rotate->setChecked(false);
        QAction* translate =  m_window->getRendrableTree()->getContextMenu()->findChild<QAction *>("Translate");
        translate->setChecked(false);
    }
}

void ViewerNode::assignManipulators(RenderableTreeItem* currentItem, RenderableTreeItem* prevItem)
{
    //Restore the previous renderable
    removeManipulator(prevItem);
    //Add the manipulator to the current renderable
    if (currentItem)
    {
        osg::ref_ptr<osg::Node> currNode = currentItem->getItemPtr();
        osgManipulator::Selection* currSelect = getParentSelection(currNode);
        if(currSelect)
        {
            osg::ref_ptr<osg::Switch> currSwitch = currSelect->getParent(0)->asSwitch();
            currSwitch->addChild(m_tranDragger, false);
            currSwitch->addChild(m_rotDragger, false);
            currSwitch->addChild(m_scaleDragger, false);
            currSwitch->addChild(m_scale2Dragger, false);
            m_manipCmdMgr->connect(*m_tranDragger, *currSelect);
            m_manipCmdMgr->connect(*m_rotDragger, *currSelect);
            m_manipCmdMgr->connect(*m_scaleDragger, *currSelect);
            m_manipCmdMgr->connect(*m_scale2Dragger, *currSelect);
        }
    }
}

void ViewerNode::toggleProjection(bool on)
{
    m_projectionMode = on;

    m_cameraManipulator->setOrtho(on);
    double aspectRatio = double(width())/double(height());
    if(on)
        m_cameraManipulator->applyOrtho(aspectRatio, 0.1, 10000);
    else
        getCamera()->setProjectionMatrixAsPerspective(50.0, aspectRatio, 0.1f, 10000.0f);
    if (!m_currentVolumeScene.empty())
    {
        QMapIterator<Volumes*, osgVolume::Volume*> i(m_currentVolumeScene);
        while (i.hasNext())
        {
            i.next();
            i.key()->setOrthographicView(on);
        }
    }
}

void ViewerNode::toggleLighting(bool on)
{
    if(on)
        m_root->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
    else
        m_root->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
}


//void ViewerNode::focusCamera(osg::Vec3 center ,float distance,osg::Matrixd transfo)
//{
//    Logger::getInstance()->log( "[ViewerNode] focusCamera" );
//    //!
//    m_cameraManipulator->setRotation(transfo.getRotate());
//
//    //osg::Quat q;
//    //q.set( transfo.getRotate() );
//    //m_cameraManipulator->setRotation( q );
//
//    m_cameraManipulator->setCenter(center);
//   // m_cameraManipulator->setDistance( distance ); //husky
//}

void ViewerNode::focusObjectCamera( const osg::Vec3 &center, const float distance, const osg::Matrixd *const transfo )
{
    focusSceneObject( center, distance, transfo );
}

QString ViewerNode::toString() const
{
    QString result = "";
    osg::Vec4 clearColour = getCamera()->getClearColor();
    result +=   QString().setNum(clearColour.x()) + "|"+
                QString().setNum(clearColour.y()) + "|"+
                QString().setNum(clearColour.z()) + "|";

    double left=0, right=0, bottom=0, up=0, zNear=0, zFar=0;
    getCamera()->getProjectionMatrixAsOrtho(left, right, bottom, up, zNear, zFar);
    osg::Matrixd viewMatrix = m_cameraManipulator->getMatrix();
    result +=   QString().setNum(m_cameraManipulator->getDistance())+"|"+
                QString().setNum(zNear)+"|"+
                QString().setNum(zFar)+"|"+

                QString().setNum(viewMatrix.getRotate().x())+"|"+
                QString().setNum(viewMatrix.getRotate().y())+"|"+
                QString().setNum(viewMatrix.getRotate().z())+"|"+
                QString().setNum(viewMatrix.getRotate().w())+"|"+

                QString().setNum(viewMatrix.getTrans().x())+"|"+
                QString().setNum(viewMatrix.getTrans().y())+"|"+
                QString().setNum(viewMatrix.getTrans().z())+"|";

    result += QString().setNum(m_axes->getValue(0)) + "$";

	if ( m_renderInput != NULL)
    {
        for(unsigned int i=0; i<m_renderInput->getSize(); ++i)
        {
            IRenderable* renderable = m_renderInput->getRenderable(i);
            if(!renderable)
                continue;
            if(!renderable->getOsgNode().valid())
                continue;

            result += renderable->toString()+"/";
        }
    }
    return result;
}

void ViewerNode::fillRInfo(const QString& rAttrib, RenderableInfo* rInfo)
{
    if(rAttrib == "" || rAttrib.isEmpty())
        return;

    QStringList rInts = rAttrib.split("|");
    if(rInts.count() != 18)
        return;

    rInfo->rName = rInts[0];
    QColor matColor = QColor(   rInts[1].toInt(),
                                rInts[2].toInt(),
                                rInts[3].toInt(),
                                rInts[4].toInt());
    rInfo->rFrontColor = matColor==Qt::transparent ? Qt::lightGray : matColor;
    rInfo->rTransform =  osg::Matrix::scale(rInts[5].toDouble(), rInts[6].toDouble(), rInts[7].toDouble()) *
                        osg::Matrix::rotate(osg::Quat(rInts[8].toDouble(), rInts[9].toDouble(), rInts[10].toDouble(), rInts[11].toDouble())) *
                        osg::Matrix::translate(rInts[12].toDouble(), rInts[13].toDouble(), rInts[14].toDouble());
    rInfo->rBackColor = QColor( rInts[15].toInt(),
                                rInts[16].toInt(),
                                rInts[17].toInt());
}

void ViewerNode::fromString(const QString& params)
{
    m_loadedInfo.clear();
    QStringList viewerParams = params.split("$");
    if(viewerParams.count() != 2)
        return;
    QStringList viewerAttribs = viewerParams[0].split("|");
    if(viewerAttribs.count() == 14)
    {
        getCamera()->setClearColor( osg::Vec4( viewerAttribs[0].toDouble(),
                                               viewerAttribs[1].toDouble(),
                                               viewerAttribs[2].toDouble(),
                                               1.0));
        m_loadedCameraDist = viewerAttribs[3].toDouble();
        //m_loadedCameraDist = 10.0;
        m_loadedZNear = viewerAttribs[4].toDouble();
        m_loadedZFar = viewerAttribs[5].toDouble();
        m_loadedViewMatrix =osg::Matrix::rotate(osg::Quat(viewerAttribs[6].toDouble(), viewerAttribs[7].toDouble(), viewerAttribs[8].toDouble(), viewerAttribs[9].toDouble())) *
                            osg::Matrix::translate(viewerAttribs[10].toDouble(), viewerAttribs[11].toDouble(), viewerAttribs[12].toDouble());
        toggleAxes(viewerAttribs[13].toInt());
    }

    QStringList rParams = viewerParams[1].split("/");
    foreach(QString rParam, rParams)
    {
        if(rParam == "" || rParam.isEmpty())
            continue;

        RenderableInfo rInfo;
        if(rParam.contains("(") && rParam.contains(")"))
        {
            QString parentParams = rParam.left(rParam.indexOf("("));
            fillRInfo(parentParams, &rInfo);
            QString childrenData = rParam.mid(rParam.indexOf("(")+1, rParam.length()-rParam.indexOf("(")-2);
            QStringList childrenParams = childrenData.split(")");
            foreach(QString childParam, childrenParams)
            {
                if(childParam == "" || childParam.isEmpty())
                    continue;

                QStringList childData = childParam.split("(");
                if(childData.count() != 2)
                    continue;

                RenderableInfo childInfo;
                fillRInfo(childData[0], &childInfo);

                QStringList gChildParams = childData[1].split(":");
                foreach(QString gChildParam, gChildParams)
                {
                    if(gChildParam == "" || gChildParam.isEmpty())
                        continue;
                    RenderableInfo gChildInfo;
                    fillRInfo(gChildParam, &gChildInfo);
                    childInfo.childrenInfo.push_back(gChildInfo);
                }

                rInfo.childrenInfo.push_back(childInfo);
            }
        }
        else
            fillRInfo(rParam, &rInfo);
        m_loadedInfo.push_back(rInfo);
    }
}

void ViewerNode::positionManipulators(osg::ref_ptr<osg::Node> node)
{
    osg::ref_ptr<osgManipulator::Selection> select = getParentSelection(node);
    if(select.valid())
    {

        float scale = node->getBound().radius() * 1.3;
        osg::Matrixd transform = osg::Matrix::scale(scale, scale, scale) *
                                 osg::Matrix::translate(node->getBound().center()) *
                                select->getMatrix();
        m_tranDragger->setMatrix(transform);
        m_rotDragger->setMatrix(transform);
        m_scaleDragger->setMatrix(transform);
        transform = osg::Matrix::scale(scale*1.1, scale*1.1, scale*1.1) *
                    osg::Matrix::rotate(osg::Vec3f(0.0,1.0,0.0),osg::Vec3f(0.0,0.0,1.0)) *
                                 osg::Matrix::translate(node->getBound().center()) *
                                select->getMatrix();
        m_scale2Dragger->setMatrix(transform);
    }
}

void ViewerNode::clip(QWheelEvent* ev)
{
    double numDegrees = (ev->delta() / 8) ;
    if (ev->modifiers()==Qt::ControlModifier)
        numDegrees*=0.1;
    double left=0, right=0, bottom=0, up=0, zNear=0, zFar=0;
    getCamera()->getProjectionMatrixAsOrtho(left, right, bottom, up, zNear, zFar);
    zNear += 0.1* numDegrees / 15.0;
    zFar += 0.1* numDegrees / 15.0;
    getCamera()->setProjectionMatrixAsOrtho(left, right, bottom, up, zNear, zFar);
}

void ViewerNode::addClipPlane(osg::ClipPlane* cp,bool show)
{
    for (unsigned int i=0;i<m_renderInput->getSize();i++)
        if (m_renderInput->getRenderable(i)->isType(IDataType::IRENDERABLE_T) && !m_renderInput->getRenderable(i)->isType(IDataType::GRID_T))
        {
            osg::ref_ptr<osg::Geode> node = m_renderInput->getRenderable(i)->getOsgNode();
            if (node)
               node->getOrCreateStateSet()->setAttributeAndModes(cp, (show? osg::StateAttribute::ON : osg::StateAttribute::OFF));
        }
}

void ViewerNode::clipXYplane()
{
    MyClipPlane* clip = new MyClipPlane;
    clip->setSize(1);
    clip->initializeGrid("X-Y plane");
    clip->setGridTransform(osg::Matrixd::identity());
    RenderableTreeItem* item = new RenderableTreeItem(m_window->getRendrableTree());
    addOsgNode(clip, m_rootSwitch, item);
    item->setText(0, clip->getRenderableName());
    item->setData(0, Qt::DecorationRole, clip->getFrontMaterial());
    item->setCheckState(0, Qt::Checked);
    clip->update();
}

void ViewerNode::clipXZplane()
{
    MyClipPlane* clip = new MyClipPlane;
    clip->setSize(1);
    clip->initializeGrid("X-Z plane");
    clip->setGridTransform(osg::Matrixd::rotate(osg::Vec3(0.0,0.0,1.0),osg::Vec3(0.0,1.0,0.0)));
    RenderableTreeItem* item = new RenderableTreeItem(m_window->getRendrableTree());
    addOsgNode(clip, m_rootSwitch, item);
    item->setText(0, clip->getRenderableName());
    item->setData(0, Qt::DecorationRole, clip->getFrontMaterial());
    item->setCheckState(0, Qt::Checked);
    clip->update();
}

void ViewerNode::clipYZplane()
{
    MyClipPlane* clip = new MyClipPlane;
    clip->setSize(1);
    clip->initializeGrid("Y-Z plane");
    clip->setGridTransform(osg::Matrixd::rotate(osg::Vec3(0.0,0.0,1.0),osg::Vec3(1.0,0.0,0.0)));
    RenderableTreeItem* item = new RenderableTreeItem(m_window->getRendrableTree());
    addOsgNode(clip, m_rootSwitch, item);
    item->setText(0, clip->getRenderableName());
    item->setData(0, Qt::DecorationRole, clip->getFrontMaterial());
    item->setCheckState(0, Qt::Checked);
    clip->update();
}

void ViewerNode::addMyClipPlane(MyClipPlane* newClip){
    RenderableTreeItem* item = new RenderableTreeItem(m_window->getRendrableTree());
    addOsgNode(newClip, m_rootSwitch, item);
    item->setText(0, newClip->getRenderableName());
    item->setData(0, Qt::DecorationRole, newClip->getFrontMaterial());
    item->setCheckState(0, Qt::Checked);
    newClip->update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::PUSH):
        {
            osgViewer::Viewer* view = dynamic_cast<osgViewer::Viewer*>(&aa);
            if (view) pick(view,ea);
            return false;
        }
        default:
            return false;
    }
}

void PickHandler::pick(osgViewer::Viewer* view, const osgGA::GUIEventAdapter& ea)
{
    osgUtil::PolytopeIntersector::Intersections intersections;

    float x = ea.getX();
    float y = ea.getY();

    double w(5), h(5);
    osgUtil::PolytopeIntersector* picker =
            new osgUtil::PolytopeIntersector(
                osgUtil::Intersector::WINDOW,
                x-w, y-h, x+w, y+h );

    ViewerNode* vn = dynamic_cast<ViewerNode*>(view);
    if (!view)
        return;

    osgUtil::IntersectionVisitor iv(picker);
    view->getCamera()->accept(iv);

    if (picker->containsIntersections())
    {
        osg::NodePath nodePath = picker->getFirstIntersection().nodePath;
        unsigned int idx = nodePath.size();
        idx--;

        osg::Node* node = nodePath[idx];

        if (node)
        {
            RenderableTreeItem* rti = vn->getMainViewerWindow()->getRendrableTree()->getItem(node);
            if (rti)
            {
                QTreeWidgetItem* parent = rti->parent();
                while (parent)
                {
                    parent->setExpanded(true);
                    parent = parent->parent();
                }
                vn->getMainViewerWindow()->getRendrableTree()->clearSelection();
                rti->setSelected(true);
            }
        }
    }
}
