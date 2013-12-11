/* OpenSceneGraph example, osganimate.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include "QOSGWidget.h"

AdapterWidget::AdapterWidget( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f):
QGLWidget(parent, shareWidget, f)
{
    _gw = new osgViewer::GraphicsWindowEmbedded(0,0,width(),height());
    setFocusPolicy(Qt::ClickFocus);
}

void AdapterWidget::resizeGL( int width, int height )
{
    _gw->getEventQueue()->windowResize(0, 0, width, height );
    _gw->resized(0,0,width,height);
}

void AdapterWidget::keyPressEvent( QKeyEvent* event )
{

 _gw->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

}

void AdapterWidget::keyReleaseEvent( QKeyEvent* event )
{

    _gw->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

}

void AdapterWidget::mousePressEvent( QMouseEvent* event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    _gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}

void AdapterWidget::mouseReleaseEvent( QMouseEvent* event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    _gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
}

void AdapterWidget::mouseMoveEvent( QMouseEvent* event )
{
    _gw->getEventQueue()->mouseMotion(event->x(), event->y());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HybridTrackballManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::FRAME):
            if (_thrown)
            {
                if (calcMovement()) us.requestRedraw();
            }
            return false;
        default:
            break;
    }

    if (ea.getHandled()) return false;

    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::DRAG):
        {
            addMouseEvent(ea);
            if (calcMovement()) us.requestRedraw();
            us.requestContinuousUpdate(false);
            _thrown = false;
            return true;
        }

        default:
			return osgGA::TrackballManipulator::handle(ea,us);
            //return osgGA::SphericalManipulator::handle(ea,us);
    }
}

void HybridTrackballManipulator::applyOrtho(double aspectRatio, double zNear, double zFar)
{
    const double fov = 50.0;
    const double y = _distance * fov/90.0;
    const double x = y * aspectRatio;
    m_viewerCamera->setProjectionMatrixAsOrtho( -x, x, -y, y, zNear, zFar);
}

void HybridTrackballManipulator::home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	osgGA::TrackballManipulator::home(ea,us);
    //osgGA::SphericalManipulator::home(ea,us);
    if(m_ortho)
    {
        double left=0, right=0, bottom=0, up=0, zNear=0, zFar=0;
        m_viewerCamera->getProjectionMatrixAsOrtho(left, right, bottom, up, zNear, zFar);
        double aspectRatio = up ? right/up : 0;
        applyOrtho(aspectRatio,zNear,zFar);
    }
}

bool HybridTrackballManipulator::calcMovement()
{
    // return if less then two events have been added.
    if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

    float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
    float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();

    float distance = sqrtf(dx*dx + dy*dy);
    // return if movement is too fast, indicating an error in event values or change in screen.
    if (distance>0.5)
    {
        return false;
    }

    // return if there is no movement.
    if (distance==0.0f)
    {
        return false;
    }

    unsigned int buttonMask = _ga_t1->getButtonMask();
    if (buttonMask==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON && m_ortho)
    {
        _distance *= 1.0f+dy;
        double left=0, right=0, bottom=0, up=0, zNear=0, zFar=0;
        m_viewerCamera->getProjectionMatrixAsOrtho(left, right, bottom, up, zNear, zFar);
        double aspectRatio = up ? right/up : 0;
        applyOrtho(aspectRatio,zNear,zFar);
        return true;
    }
    else
        //return osgGA::TrackballManipulator::calcMovement(); // only available in 'older' OSG versions!
		return osgGA::TrackballManipulator::performMovement();
		//return osgGA::SphericalManipulator::calcMovement();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool DraggerHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                    osg::Object*, osg::NodeVisitor*)
{
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (!view) return false;

    switch (ea.getEventType())
    {
        case osgGA::GUIEventAdapter::PUSH:
        {
            osgUtil::LineSegmentIntersector::Intersections intersections;

            _pointer.reset();

            if (view->computeIntersections(ea.getX(),ea.getY(),intersections))
            {
                _pointer.setCamera(view->getCamera());
                _pointer.setMousePosition(ea.getX(), ea.getY());

                for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
                    hitr != intersections.end();
                    ++hitr)
                {
                    _pointer.addIntersection(hitr->nodePath, hitr->getLocalIntersectPoint());
                }
                for (osg::NodePath::iterator itr = _pointer._hitList.front().first.begin();
                        itr != _pointer._hitList.front().first.end();
                        ++itr)
                {
                    osgManipulator::Dragger* dragger = dynamic_cast<osgManipulator::Dragger*>(*itr);
                    if (dragger)
                    {

                        dragger->handle(_pointer, ea, aa);
                        _activeDragger = dragger;
                        break;
                    }
                }
            }
        }
        break; //!?
        case osgGA::GUIEventAdapter::DRAG:
        case osgGA::GUIEventAdapter::RELEASE:
        {
            if (_activeDragger)
            {
                _pointer._hitIter = _pointer._hitList.begin();
                _pointer.setCamera(view->getCamera());
                _pointer.setMousePosition(ea.getX(), ea.getY());

                _activeDragger->handle(_pointer, ea, aa);
            }
            break;
        }
    default:
        break;
    }

    if(!_activeDragger) return false;
    if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
    {
        _activeDragger = 0;
        _pointer.reset();
    }

    return true;
}
/*EOF*/
