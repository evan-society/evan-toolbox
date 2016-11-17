#ifndef EWTST_TEST_LMKS3_H_INCLUDED
#define EWTST_TEST_LMKS3_H_INCLUDED

// wdkg 2009-2010

// Qt needs any class with QOBJECT to be defined in a separate header file.
//XXX reconcile with test_surface3

#include <QGLWidget>
#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QMouseEvent>
#include "ew/DebugHelper.h"
#include "ew/Transform3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowForm3.h"
#include "ew/View3Widget.h"
#include "ew/View3Item.h"
#include "ew/View3Landmarks.h"

class ViewWidget : public QGLWidget, public ew::View3Widget {
  Q_OBJECT
public:
  ViewWidget(ew::DataflowNetwork *nw, QWidget *parent);
  ~ViewWidget();
  void reset();
  ew::View3Landmarks *litem;
  ew::Transform3 tr0;
  ew::Transform3 tr1;
  bool pointerdown;
  const ew::DebugHelper dbg;
private:
  virtual void schedule_idle_handler_cb();
  virtual void redraw_cb();
  virtual void hideEvent(QHideEvent *event);
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void timerEvent(QTimerEvent *);
  virtual void mousePressEvent(QMouseEvent *ev);
  virtual void mouseMoveEvent(QMouseEvent *ev);
  virtual void mouseReleaseEvent(QMouseEvent *ev);
  int idle_timer;
  int pointerx;
  int pointery;
  Qt::KeyboardModifiers pointermods;
};

class MainWindow : public QWidget {
  Q_OBJECT
public:
  MainWindow(ew::DataflowNetwork *nw, ew::DataflowForm3 *f);
  ~MainWindow();
  ViewWidget *view;
  QPushButton *pass;
  QPushButton *fail;
  QPushButton *exit;
  QLabel *lab;
  QLabel *exp;
  QGridLayout *grid;
  bool ok;
  int animate_timer;
  int cnt;
  ew::DataflowForm3 *frm;
private:
  virtual void timerEvent(QTimerEvent *);
signals:
  void done();
private slots:
  void dopass();
  void dofail();
  void doexit();
};

#endif
