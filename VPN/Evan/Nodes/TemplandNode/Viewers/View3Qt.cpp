#include "View3Qt.h"
#include "../IdDialog.h"
#include "ew/View3Landmarks.h"
#include <QBitmap>
#include <QMessageBox>
#include <cmath>

#include "../ViewTree/LandmarkItem.h"
#include "../ViewTree/SemiLandmarksTopItem.h"

#if TEMPLAND_TOOLKIT_BUILD
#include "../../Utilities/Logger.h"
#endif


View3Qt::View3Qt(ew::DataflowNetwork *nw, QWidget *par, QGLWidget *share, QGLWidget *share2, ew::Dig3 *dig3, int space, int slicemode) : QGLWidget(par, share2 ? share2 : share ),ew::Dig3View(dig3,space,slicemode),
                                                                            m_dig3Cursor(QBitmap(":Images/xhf.bmp"),QBitmap(":Images/xhbg.bmp")), m_lmkTopItem(0), m_tarLmkTopItem(0)
{
    m_isTemplate = false;
    m_semitype = -1;
    m_restrictX = false;
    m_restrictY = false;
    m_twist = false;
    m_moved = false;
    m_rotation_time = 200;
    m_rotation_updates = 5;
    m_updates_counter = 0;
    m_rotation_timer = new QTimer(this);
    connect(m_rotation_timer, SIGNAL(timeout()), this, SLOT(interpolateRotation()));

    _pointtype = ew::Form3::TYPE_LANDMARK;
}

View3Qt::~View3Qt()
{
    destruction_in_progress();
}

void View3Qt::schedule_idle_handler_cb()
{
    m_idle_timer = startTimer(0);
    if (m_idle_timer == 0)
    {
      throw std::runtime_error("could not start qt idle timer");
    }
}

void View3Qt::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == m_idle_timer)
    {
        killTimer(m_idle_timer);
        idle_handler();
    }
}

void View3Qt::initializeGL()
{
    init_gl_context();
}

void View3Qt::resizeGL(int w, int h)
{
    set_window_size(w, h);
}

void View3Qt::paintGL()
{
    ew::View3Widget::render();
    glFlush();
}

void View3Qt::hideEvent(QHideEvent *)
{
    set_currently_unrendered();
}

void View3Qt::redraw_cb()
{
    glDraw();
}

void View3Qt::enterEvent(QEvent* event)
{
    setFocus();
    setCursor(m_dig3Cursor);
}

void View3Qt::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton && !m_isTemplate )
    {
		if(m_tarLmkTopItem && m_tarLmkTopItem->isMoving())
		{
			int n = 0;
			int i = 0;
			m_tarLmkTopItem->getMovingItem(n,i);

			if (n >= 0)
			{
				double loc[3];
				double x=(double)mapFromGlobal(QCursor::pos()).x(), y=(double)mapFromGlobal(QCursor::pos()).y(), z = 0.0;
				if (get_use_depth())
					ew::View3Widget::pick(x, y, 0.00001, 0.0, 0, -1, 2, 0, 0, 0, &z);

				get_pointer_location(loc, x * 1.0, y * 1.0, z);
				ew::Dig3Space *sp = dig3->get_spaces()[get_space()];

				if(i>=0)
				{
					sp->set_form_pointset_location(n, i, loc);
				}
				else
				{
					sp->set_form_pointset_location(n, 0, loc);
				}

				// now change the state of pointset to SET
				ew::Form3PointSet ps = sp->get_form_data()->pointsets[n];
				ps.state = ew::Form3::STATE_SET;
				bool replaced = false;
				sp->set_form_pointset( &replaced, &ps );

				// inform that we moved a landmark
				emit landmarkMoved( ps.id.c_str(), get_space() );
			}
			m_tarLmkTopItem->setMoving(false);
		}
		else
		{

        int x = ev->x();
        int y = ev->y();
        double z = 0.0;

        try
        {
        double loc[3];
        if(get_use_depth())
            ew::View3Widget::pick(x, y, 0.00001, 0.0, 0, -1, 2, 0, 0, 0, &z);
        get_pointer_location(loc, x * 1.0, y * 1.0, z);
        placePoint( loc[0], loc[1], loc[2] );

        emit adjustLandmarkState();
        }
        catch(...)
        {
            QMessageBox::information( parentWidget(), "Error", "Failed placing landmark on surface." );
        }

		}
    }
}

QString View3Qt::createAlphaNumeric( unsigned int idnum )
{
    QString num; num = num.setNum( idnum );
    QString base = "LM";
    //if( idnum < 99999  && idnum <= 9999)
	if( idnum < 99999  && idnum >= 10000 )
        base += "0";
    else
    {
        base += num;
        return base;
    }
    //if( idnum < 9999  && idnum <= 999)
	if( idnum < 9999  && idnum >= 1000 )
        base += "0";
    else
    {
        base += num;
        return base;
    }
    //if( idnum < 999  && idnum <= 99)
	if( idnum < 999  && idnum >= 100 )
        base += "0";
    else
    {
        base += num;
        return base;
    }
    //if( idnum < 99  && idnum <= 9)
	if( idnum < 99  && idnum >= 10)
        base += "0";
    else
    {
        base += num;
        return base;
    }

    base += num;
    return base;

}

void View3Qt::placePoint( double x, double y, double z, bool promptID, bool fixed, QString id, int state )
{
    QString lmkID = id;
    ew::Dig3Space *sp = dig3->get_spaces()[get_space()];

    if( lmkID == "" && m_lmkTopItem)
    {
        lmkID = m_lmkTopItem->getSelectedLandmarkID();

        for( int i = 0; i < m_tarLmkTopItem->childCount(); ++i )
        {
            QString cmp = "";
            //ViewTreeItem* vti = (ViewTreeItem*)m_tarLmkTopItem->child(i);
			ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_tarLmkTopItem->child(i) );
            if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
            {
                LandmarkItem* lti = dynamic_cast< LandmarkItem* >( vti );
                cmp = lti->getLmkID();
                if(lmkID == cmp )
                {
                    QMessageBox::information( parentWidget(), "Error", "The Landmark already exists. Please select another landmark." );
                    return;
                }
            }
            else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
            {
                SemiLandmarksTopItem* sli = dynamic_cast< SemiLandmarksTopItem* >( vti );
                cmp = sli->getLmkID();
            }
        }
    }

    if( lmkID.isEmpty() )
    {
        unsigned int lmkCount = 0;
        for( unsigned int i = 0 ; i < sp->get_form_data()->pointsets.size(); ++i )
            if( sp->get_form_data()->pointsets[i].type == ew::Form3::TYPE_LANDMARK )
                ++lmkCount;
        lmkCount++;
        lmkID = createAlphaNumeric(lmkCount);
        int ind;
        while(sp->get_form_data()->search_pointset(&ind, lmkID.toStdString().c_str()))
        {
            lmkCount++;
            lmkID = createAlphaNumeric(lmkCount);
        }
    }
    IdDialog* detailsDialog = new IdDialog(0, lmkID, lmkID);
    detailsDialog->hideLabel();
    bool exec = false;
    if( !promptID || !m_addlandmark.isEmpty() )
        exec = true;
    else
    {
        exec = detailsDialog->exec();
        lmkID = detailsDialog->getID();
    }

    if( promptID && m_addlandmark.isEmpty() )
    {
        // make sure the id is unique
        bool unique = true;
        while( exec && m_tarLmkTopItem )
        {
            for( int i = 0; i < m_tarLmkTopItem->childCount() && unique; ++i )
            {
                QString cmp = "";
                ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_tarLmkTopItem->child(i) );
                if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
                {
                    LandmarkItem* lti = dynamic_cast< LandmarkItem* >( vti );
                    cmp = lti->getLmkID();
                }
                else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
                {
                    SemiLandmarksTopItem* sli = dynamic_cast< SemiLandmarksTopItem* >( vti );
                    cmp = sli->getLmkID();
                }

                unique = lmkID != cmp;
            }

            if( !unique )
            {
                QMessageBox::information( parentWidget(), "Error", "The Landmark name already exists. Please enter another name." );
                exec = detailsDialog->exec();

                lmkID = detailsDialog->getID();

                unique = true;
            }
            else
                break;
        }
    }

    if( exec )
    {
        ew::Form3PointSet ps;
        ps.id = lmkID.toStdString();
        ps.n = 1;
        ps.locations.resize(3);
        ps.locations[0] = x;
        ps.locations[1] = y;
        ps.locations[2] = z;
        ps.type = m_addlandmark.isEmpty() ? ew::Form3::TYPE_LANDMARK : /*ew::Form3::TYPE_SEMI_LANDMARK;/*/ _pointtype;
        ps.state = fixed ? ew::Form3::STATE_SET : state;
        bool b;
        if( m_addlandmark.isEmpty() )
        {
            sp->set_form_pointset(&b, &ps);
        }
        else
        {
            bool found = false;
            for( unsigned int i = 0; i < sp->get_form_data()->pointsets.size() && !found; ++i )
            {
                if( sp->get_form_data()->pointsets[i].id == m_addlandmark.toStdString() )
                {
                    ew::Form3PointSet sps = sp->get_form_data()->pointsets[i];
                    sps.n++;
                    sps.locations.push_back( x );
                    sps.locations.push_back( y );
                    sps.locations.push_back( z );
                    sps.type = ew::Form3::TYPE_SEMI_LANDMARK;
                    if( sps.relax_dims.size() == sps.locations.size() / 3 - 1 )
                        sps.relax_dims.push_back( 0);//2 );
                    //else
                    //    sps.relax_dims.push_back( 1 );
                    //sps.relax_params.push_back( 0 );
                    //sps.relax_params.push_back( 0 );
                    //sps.relax_params.push_back( 0 );
                    // now replace it
                    sp->set_form_pointset(&b, &sps);
                    found = true;
                }
            }
            if( !found )
            {
                // add in new semi landmark pointset
                ps.id = m_addlandmark.toStdString();
                ps.type = ew::Form3::TYPE_SEMI_LANDMARK;
                //if( m_semitype == 0 )
                    ps.relax_dims.push_back( 0);//2 );
                //else
                //    ps.relax_dims.push_back( 1 );
                //ps.relax_params.push_back( 0 );
                //ps.relax_params.push_back( 0 );
                //ps.relax_params.push_back( 0 );
                sp->set_form_pointset(&b, &ps);
            }
        }
        emit landmarkAdded(lmkID);
    }
    delete detailsDialog;
}

void View3Qt::mousePressEvent(QMouseEvent *ev)
{
    m_pointerButton = ev->button();
    m_pointerX = ev->x();
    m_pointerY = ev->y();
//    if (m_pointerButton == Qt::LeftButton)
//        setFocus();
}

void View3Qt::applyTransform(QInputEvent *e)
{
//    if(!m_dig3view)
//        return;
    double factor = (e->modifiers()==Qt::ControlModifier || e->modifiers()==(Qt::AltModifier | Qt::ControlModifier)) ? 10.0 : 1.0;
    ew::Transform3 tr;
    tr.set_to_identity();
    m_rotation_transform.set_to_identity();

    if(e->type() == QEvent::Wheel) //Push
    {
        QWheelEvent* zoomEvent = (QWheelEvent*)e;
        double numDegrees = (zoomEvent->delta() / 8) / factor;
        get_pointer_translate_tr(&tr, 0, 0, numDegrees);
    }
    else if(e->type() == QEvent::MouseMove) //tumble, twist, and scale
    {
        QMouseEvent* ev = (QMouseEvent*)e;

        double dx = (ev->x()-m_pointerX)/factor;
        double dy = (ev->y()-m_pointerY)/factor;
        if ( m_pointerButton == Qt::LeftButton)
        {
            if(m_twist || e->modifiers()==Qt::AltModifier || e->modifiers()==(Qt::AltModifier | Qt::ControlModifier))
                get_pointer_twist_tr(&tr, m_pointerX, m_pointerY, ev->x(), ev->y(), 1.0/factor);
            //else if(e->modifiers()==(Qt::AltModifier | Qt::ControlModifier))
            //    get_pointer_twist_tr(&tr, m_pointerX, m_pointerY, ev->x(), ev->y(), 0.1);
            else
                get_pointer_roll_tr(&tr, m_restrictX ? 0:dx, m_restrictY ? 0:dy);
        }
        else if( m_pointerButton == Qt::MidButton)
            get_pointer_translate_tr(&tr, m_restrictX ? 0:dx, m_restrictY ? 0:dy, 0);
        else
            get_pointer_scale_tr(&tr, 0, 0, dy, dy, 1.0);

        m_pointerX = ev->x();
        m_pointerY = ev->y();
    }
    else if(e->type() == QEvent::KeyRelease) //90 degree rotations
    {
        QKeyEvent* keyEvent = (QKeyEvent*)e;
        m_rotation_transform.set_to_identity();

        if(keyEvent->key()==Qt::Key_PageUp)
        {
            tr.translate[2] = 0.02/factor;
            //get_pointer_translate_tr(&tr, 0, 0, -10.0/factor);
        }
        else if(keyEvent->key()==Qt::Key_PageDown)
        {
            tr.translate[2] = -0.02/factor;
            //get_pointer_translate_tr(&tr, 0, 0, 10.0/factor);
        }
        else if(keyEvent->key() == Qt::Key_R && e->modifiers()!=Qt::ShiftModifier)
        {
            m_rotation_transform.orthog[0][0] = 0.0;
            m_rotation_transform.orthog[1][1] = 0.0;
            m_rotation_transform.orthog[2][2] = 0.0;
            m_rotation_transform.orthog[0][1] = 1.0;
            m_rotation_transform.orthog[1][2] = 1.0;
            m_rotation_transform.orthog[2][0] = 1.0;
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key() == Qt::Key_R && e->modifiers()==Qt::ShiftModifier)
        {
            m_rotation_transform.orthog[0][0] = 0.0;
            m_rotation_transform.orthog[1][1] = 0.0;
            m_rotation_transform.orthog[2][2] = 0.0;
            m_rotation_transform.orthog[0][2] = 1.0;
            m_rotation_transform.orthog[1][0] = 1.0;
            m_rotation_transform.orthog[2][1] = 1.0;
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key()==Qt::Key_Plus || keyEvent->key()==Qt::Key_Equal)
        {
            m_rotation_transform.scale = sqrt(2.0);
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates*factor));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key()==Qt::Key_Minus || keyEvent->key()==Qt::Key_Underscore)
        {
            m_rotation_transform.scale = 1.0/sqrt(2.0);
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates*factor));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key() == Qt::Key_Up)
        {
            m_rotation_transform.orthog[1][1] = 0.0;
            m_rotation_transform.orthog[1][2] = 1.0;
            m_rotation_transform.orthog[2][1] = -1.0;
            m_rotation_transform.orthog[2][2] = 0.0;
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates*factor));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key() == Qt::Key_Down)
        {
            m_rotation_transform.orthog[1][1] = 0.0;
            m_rotation_transform.orthog[1][2] = -1.0;
            m_rotation_transform.orthog[2][1] = 1.0;
            m_rotation_transform.orthog[2][2] = 0.0;
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates*factor));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key()==Qt::Key_BraceRight || keyEvent->key()==Qt::Key_BracketRight)
        {
            m_rotation_transform.orthog[0][0] = 0.0;
            m_rotation_transform.orthog[1][1] = 0.0;
            m_rotation_transform.orthog[1][0] = -1.0;
            m_rotation_transform.orthog[0][1] = 1.0;
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates*factor));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key() == Qt::Key_BraceLeft || keyEvent->key()==Qt::Key_BracketLeft)
        {
            m_rotation_transform.orthog[0][0] = 0.0;
            m_rotation_transform.orthog[1][1] = 0.0;
            m_rotation_transform.orthog[1][0] = 1.0;
            m_rotation_transform.orthog[0][1] = -1.0;
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates*factor));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key() == Qt::Key_Right)
        {
            m_rotation_transform.orthog[0][0] = 0.0;
            m_rotation_transform.orthog[0][2] = 1.0;
            m_rotation_transform.orthog[2][0] = -1.0;
            m_rotation_transform.orthog[2][2] = 0.0;
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates*factor));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key() == Qt::Key_Left)
        {
            m_rotation_transform.orthog[0][0] = 0.0;
            m_rotation_transform.orthog[0][2] = -1.0;
            m_rotation_transform.orthog[2][0] = 1.0;
            m_rotation_transform.orthog[2][2] = 0.0;
            m_rotation_transform.set_to_interpolation(&tr,&m_rotation_transform, 1.0/double(m_rotation_updates*factor));
            m_rotation_timer->start(m_rotation_time/m_rotation_updates);
        }
        else if(keyEvent->key() == Qt::Key_C)
        {
            double loc[3];
            get_pointer_location(loc, (double)mapFromGlobal(QCursor::pos()).x(), (double)mapFromGlobal(QCursor::pos()).y());
            //get_center_location(loc);
            get_jump_tr(&tr, loc, true);
        }
        else if(keyEvent->key() == Qt::Key_Space)
        {
            ew::Bbox3 b;
            get_bbox(&b);
            if (!get_bbox_tr(&tr, &b, true, true))
                return;
        }
        else if (keyEvent->key() == Qt::Key_M)
        {
            int n = get_landmarks_item()->get_highlight_ps();
            int i = get_landmarks_item()->get_highlight_i() / 3;

            if (n >= 0)
            {
                double loc[3];
                double x=(double)mapFromGlobal(QCursor::pos()).x(), y=(double)mapFromGlobal(QCursor::pos()).y(), z = 0.0;
                if (get_use_depth())
                    ew::View3Widget::pick(x, y, 0.00001, 0.0, 0, -1, 2, 0, 0, 0, &z);

                get_pointer_location(loc, x * 1.0, y * 1.0, z);
                ew::Dig3Space *sp = dig3->get_spaces()[get_space()];

                if(i>=0)
                {
                    sp->set_form_pointset_location(n, i, loc);
                }
                else
                {
                    sp->set_form_pointset_location(n, 0, loc);
                }

                // now change the state of pointset to SET
                ew::Form3PointSet ps = sp->get_form_data()->pointsets[n];
                ps.state = ew::Form3::STATE_SET;
                bool replaced = false;
                sp->set_form_pointset( &replaced, &ps );

                // inform that we moved a landmark
                emit landmarkMoved( ps.id.c_str(), get_space() );
            }
            if(m_tarLmkTopItem)
            	m_tarLmkTopItem->setMoving(false);
        }
        else if (keyEvent->key() == Qt::Key_Escape)
        {
        	if(m_tarLmkTopItem)
        		m_tarLmkTopItem->setMoving(false);
        }
    }
    ew::Transform3 identity;
    identity.set_to_identity();
    //if( !tr.equals( identity ) || !m_rotation_transform.equals( identity ) )
    //    emit viewChanged();
    if(tr != identity || m_rotation_transform != identity)
        emit viewChanged();
    view_post_compose(&tr);
}

void View3Qt::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton && m_moved == false)
    {
        int x = ev->x();
        int y = ev->y();
        int c = -1;
        ew::View3Widget::pick(x, y, 15.0, 7.5, get_landmarks_item(), -1, -1, 0, &c, 0, 0);
        if(c >= 0)
        {
            // now notify the tab that we have selected a landmark
            const ew::Form3* form = dig3->get_spaces()[ get_space() ]->get_form_data();
            int count = -1;
            int f_c = -1;
            int f_ci = -1;
            for( unsigned int i = 0; i < form->pointsets.size() && (f_c == -1); ++i )
            {
                if( form->pointsets[i].n > 1 )
                {
                    for( int wi = 0; wi < form->pointsets[i].n; ++wi )
                    {
                        ++count;

                        if( c == count )
                        {
                            f_c = (int)i;
                            f_ci = wi;
                            break;
                        }
                    }
                }
                else if( form->pointsets[i].n == 1 )
                {
                    ++count;

                    if( c == count )
                    {
                        f_c = (int)i;
                        f_ci = 0;
                    }
                }
            }
            QString id = form->pointsets[ f_c ].id.c_str();
            emit landmarkSelected( id, f_c, f_ci, get_space() );
        }
    }
    m_moved = false;
}

void View3Qt::interpolateRotation()
{
    view_post_compose(&m_rotation_transform);
    if(++m_updates_counter == m_rotation_updates)
    {
        m_updates_counter = 0;
        m_rotation_timer->stop();
    }
}
