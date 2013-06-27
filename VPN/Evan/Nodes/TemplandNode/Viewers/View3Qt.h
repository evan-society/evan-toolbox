#ifndef VIEW3QT_H
#define VIEW3QT_H

#include <QGLWidget>
#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QMouseEvent>
#include "ew/Dig3.h"
#include "ew/Form3.h"
#include "ew/View3Widget.h"
#include "ew/Dig3View.h"
#include "ew/Dig3Space.h"

#include "../ViewTree/LandmarksTopItem.h"
#include "../ViewTree/CurvesTopItem.h"

//! Templand View3Qt class
/*!
	This class represents one of the four OpenGL 3D views in a typical Templand window.
*/
class View3Qt : public QGLWidget, public ew::Dig3View
{
	Q_OBJECT
public:
	View3Qt(ew::DataflowNetwork *network, QWidget *parent, QGLWidget *share, QGLWidget* share2 = 0, ew::Dig3* dig3 = 0, int space = 0, int slicemode = 0);
	~View3Qt();

	//! Sets the type of pointset that the view should create.
	/*!
		\param A pointset type either ew::Form3::TYPE_LANDMARK or ew::Form3::TYPE_SEMI_LANDMARK.
	*/
    void pointType( int type ) { _pointtype = type; }

	//! Digitizes a point on a surface.
	/*!
		\param x X coordinate of the pointset.
		\param y Y coordinate of the pointset.
		\param z Z coordinate of the pointset.
		\param promptID Whether the pointset ID Dialog box should show up or not.
		\param fixed If the pointset is fixed or warped from another surface.
		\param id Pointset ID if exists.
		\param state Pointset state if exists. Possible state values are ew::Form3::StateType  in EW library.
	*/
    void placePoint( double x, double y, double z, bool promptID = false, bool fixed = true, QString id = "", int state = ew::Form3::STATE_SET );

	//! Digitizes a point on a curve.
	/*!
		\param x X coordinate of the pointset.
		\param y Y coordinate of the pointset.
		\param z Z coordinate of the pointset.
		\param promptID Whether the pointset ID Dialog box should show up or not.
		\param fixed If the pointset is fixed or warped from another surface.
		\param id Pointset ID if exists.
		\param state Pointset state if exists. Possible state values are ew::Form3::StateType  in EW library.
	*/
    void placePointOnCurve( double x, double y, double z, bool promptID = true, bool fixed = true, QString id = "", int state = ew::Form3::STATE_SET );

	//! Sets a pointer to the Template Landmark top tree view..
	/*!
		\param lti Top tree item in the Template tree view. Normally used to for ID checks.
	*/
    void templateLandmarkTopItem( LandmarksTopItem* lti ) { m_lmkTopItem = lti; }

	//! Sets a pointer to the Sepcimen Landmark top tree view..
	/*!
		\param lti Top tree item in the Specimen tree view. Normally used to for ID checks.
	*/
    void targetLandmarkTopItem( LandmarksTopItem* lti ) { m_tarLmkTopItem = lti; }

	//! Sets a pointer to the Sepcimen Curves top tree view..
	/*!
		\param lti Top tree item in the Curves tree view.
	*/
    void curvesTopItem( CurvesTopItem* cti ) { m_curvesTopItem = cti; }

    //! Sets a property to disable the insertions of pointsets in Template views.
	/*!
		\param istemp Boolean to whether this view is a template view or not.
	*/
    void IsTemplate( bool istemp ) { m_isTemplate = istemp; }


protected:
//    ew::Dig3View* m_dig3view;
    bool m_restrictX;
    bool m_restrictY;
    bool m_twist;
	Qt::MouseButton m_pointerButton;
	int m_pointerX;
	int m_pointerY;
private:
	virtual void schedule_idle_handler_cb();
	virtual void redraw_cb();
	virtual void hideEvent(QHideEvent *event);
	virtual void paintGL();
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
    virtual void enterEvent(QEvent* event);
	virtual void timerEvent(QTimerEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *ev);
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseReleaseEvent(QMouseEvent *ev);
	virtual void mouseMoveEvent(QMouseEvent *ev){applyTransform(ev); m_moved = true;}
    virtual void wheelEvent(QWheelEvent *ev)    {applyTransform(ev);}
    virtual void keyReleaseEvent(QKeyEvent *ev) {applyTransform(ev);}
    void applyTransform(QInputEvent *);
    QString createAlphaNumeric( unsigned int idnum );
//	virtual void mouseReleaseEvent(QMouseEvent *ev);
//	virtual void mouseMoveEvent(QMouseEvent *ev);
	int m_idle_timer;
	QTimer* m_rotation_timer;
	int m_updates_counter;
	int m_rotation_time;        //in milliseconds
	int m_rotation_updates;
	int _pointtype;
	ew::Transform3 m_rotation_transform;
	QCursor m_dig3Cursor;
	QString m_addlandmark;
	int m_semitype;
	LandmarksTopItem* m_lmkTopItem;
	LandmarksTopItem* m_tarLmkTopItem;
	CurvesTopItem* m_curvesTopItem;
	bool m_isTemplate;
	bool m_moved;

public slots:
    void restrictAlongX(bool on){m_restrictX = on;}
    void restrictAlongY(bool on){m_restrictY = on;}
    void toggleTwist(bool on){m_twist = on;}
    void addLandmarkHere( const QString& id, int semitype ) { m_addlandmark = id; m_semitype = semitype; }
private slots:
    void interpolateRotation();
signals:
    void landmarkAdded( const QString& );
    void landmarkMoved( const QString&, int );
    void adjustLandmarkState();
    void landmarkSelected( const QString& id, int index, int indexWithin, int space );
    void viewChanged();
};

#endif
