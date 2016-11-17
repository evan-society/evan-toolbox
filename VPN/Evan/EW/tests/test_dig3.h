#ifndef EWTST_TEST_DIG3_H_INCLUDED
#define EWTST_TEST_DIG3_H_INCLUDED

// wdkg 2009-2010

#include <QGLWidget>
#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QMouseEvent>
#include "ew/DebugHelper.h"
#include "ew/Dig3.h"
#include "ew/Dig3View.h"
#include "ew/Form3.h"
#include "ew/View3Widget.h"

// XXX forward declaration w/o include
class MainWindow;
//XXX separate headers for each class

class ViewWidget : public QGLWidget, public ew::Dig3View {
  Q_OBJECT
public:
  ViewWidget(QWidget *par, ew::Dig3 *dig3, int space, int slice_mode);
  ~ViewWidget();
  const ew::DebugHelper dbg;
protected:
  bool pointerdown;
  int pointerx;
  int pointery;
  Qt::KeyboardModifiers pointermods;
private:
  virtual void schedule_idle_handler_cb();
  virtual void redraw_cb();
  virtual void hideEvent(QHideEvent *event);
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void timerEvent(QTimerEvent *);
  virtual void keyPressEvent(QKeyEvent *ev);
  virtual void mousePressEvent(QMouseEvent *ev);
  virtual void mouseReleaseEvent(QMouseEvent *ev);
  virtual void mouseMoveEvent(QMouseEvent *ev);
  int idle_timer;
  static QGLWidget *share;
};

class MainWindow : public QWidget {
  Q_OBJECT
public:
  MainWindow(ew::DataflowNetwork *nw);
  ~MainWindow();
  ew::Dig3 dig3;
  ViewWidget *view[4];
  QPushButton *pass;
  QPushButton *fail;
  QPushButton *exit;
  QLabel *lab;
  QLabel *exp;
  QPushButton *mode;
  QPushButton *save;
  QPushButton *load;
  QPushButton *reset;
  QCheckBox *play;
  QGridLayout *grid;
  bool ok;
  int mode_n;
  std::vector<ew::Dig3Tableau> saves;
  int save_pos;
  bool playing;
  int playtimer;
  int playlastms;
  QTime playtime;
  int playlastframe;
  std::vector<ew::Dig3Tableau> playsaves;
  std::vector<ew::Form3> playforms1;
  std::vector<ew::Form3> playforms2;
  ew::DataflowNetwork *const nw;
private:
  virtual void timerEvent(QTimerEvent *);
signals:
  void done();
private slots:
  void dopass();
  void dofail();
  void doexit();
  void domode();
  void dosave();
  void doload();
  void doreset();
  void doplay(int state);
};

#endif
