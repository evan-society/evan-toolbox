#include <cstdlib>
#include <cmath>
#include <vector>
#include <QGLWidget>
#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QMouseEvent>
#include "ew/Debug.h"
#include "ew/Transform3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowSurface3.h"
#include "ew/String.h"
#include "ew/View3Widget.h"
#include "ew/View3Item.h"
#include "ew/View3Surface.h"
#include "test_surface3.h"
#include "testlowercase.h"

namespace {
  const char ClassNameV[] = "ViewWidget";
  const char ClassNameS[] = "TestSurface3";
  const int M = 50;
}

ewtst::TestSurface3::TestSurface3(ew::DataflowNetwork *nw) :
 ew::DataflowSurface3(nw, 0, "test_surface3", 0),
 dbg(ClassNameS, ew::DataflowNode::dbg),
 inp_aspect(0.1),
 vpoints(M * 6),
 vfaces(M * 6)
{
  for (int i = 0; i < M; i += 1) {
    vfaces[3 * i] = i;
    vfaces[3 * i + 2] = (i + 1) % M;
    vfaces[3 * i + 1] = i + M;
    vfaces[3 * M + 3 * i] = (i + 1) % M;
    vfaces[3 * M + 3 * i + 1] = i + M;
    vfaces[3 * M + 3 * i + 2] = (i + 1) % M + M;
    double a = 3.1416 * (i / 1.0 / M);
// These deliberately not done until update_helper() is called.
//    vpoints[3 * i] = -inp_aspect;
//    vpoints[3 * M + 3 * i] = inp_aspect;
    vpoints[3 * i + 1] = vpoints[3 * M + 3 * i + 1] = 0.5 * std::sin(a);
    vpoints[3 * i + 2] = vpoints[3 * M + 3 * i + 2] = 0.5 * std::cos(a);
  }
}

void
ewtst::TestSurface3::reset()
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "reset",
   ew::DataflowNode::change_cycle);
  inp_aspect = 0.1;
}

void
ewtst::TestSurface3::set_aspect(double a)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_aspect",
   ew::DataflowNode::change_cycle);
  inp_aspect = a;
}

void
ewtst::TestSurface3::update_helper() const
{
  ew::DataflowNode::outp_version = ew::DataflowNode::change_cycle;
  if (ew::DataflowNode::outp_version > ew::DataflowNode::update_cycle) {
    ew::DataflowNode::outp_valid = true;
    for (int i = 0; i < M; i += 1) {
      vpoints[i * 3] = -inp_aspect;
      vpoints[M * 3 + i * 3] = inp_aspect;
    }
    outp_num_points = M * 2;
    outp_points = &vpoints[0];
    outp_num_faces = M * 2;
    outp_faces = &vfaces[0];
    if (dbg.on) {
      dbg.dprintf("%s::%s UPDATE version=%lu", dbg.fn, "update_helper",
       ew::DataflowNode::outp_version);
      dbg.dprintf("%s::%s UPDATE valid=%s", dbg.fn, "update_helper",
       ew::Debug::to_str(ew::DataflowNode::outp_valid));
    }
  }
}

void
ViewWidget::reset()
{
  set_view_mapping(&tr0);
  unsigned char bc[3];
  bc[0] = bc[1] = 0;
  bc[2] = 64;
  ew::View3Widget::set_background_color(bc);
  sitem->set_state(1);
  bc[0] = bc[1] = bc[2] = 127;
  sitem->set_back_color(bc);
  sitem->set_state(true);
}

ViewWidget::ViewWidget(ew::DataflowNetwork *nw, QWidget *par) :
 QGLWidget(par),
 ew::View3Widget(nw),
 sitem(new ew::View3Surface(this)),
 dbg(ClassNameV, ew::View3Widget::dbg)
{
  tr0.set_to_identity();
  tr0.orthog[0][0] = 0.1;
  tr0.orthog[0][1] = 0.2;
  tr0.orthog[0][2] = 0.3;
  tr0.orthog[1][0] = 0.4;
  tr0.orthog[1][1] = 0.5;
  tr0.orthog[1][2] = 0.6;
  tr0.orthog[2][0] = 0.7;
  tr0.orthog[2][1] = 0.8;
  tr0.orthog[2][2] = 0.9;
  tr0.set_to_normalization(&tr0);
  tr1.set_to_identity();
  reset();
  pointerdown = false;
}

ViewWidget::~ViewWidget()
{
// destruction_in_progress();
}

void
ViewWidget::schedule_idle_handler_cb()
{
  dbg.on && dbg.dprintf("%s::%s", dbg.fn, "schedule_idle_handler_cb");
  idle_timer = startTimer(0);
  if (idle_timer == 0) {
    throw std::runtime_error("could not start qt idle timer");
  }
}

// Exceptions in these callbacks result in an immediate abort.  This is a
// Qt mis-feature.

void ViewWidget::timerEvent(QTimerEvent *ev) {
  if (ev->timerId() == idle_timer) {
    dbg.on && dbg.dprintf("%s::%s idle_timer", dbg.fn, "timerEvent");
    try {
      killTimer(idle_timer);
      ew::View3Widget::idle_handler();
    } catch(...) {
      Error("ew::View3Widget::idle_handler exception");
    }
  }
}

void
ViewWidget::initializeGL()
{
  try {
    ew::View3Widget::init_gl_context();
  } catch(...) {
    Error("ew::View3Widget::init_gl_context exception");
  }
}

void
ViewWidget::resizeGL(int w, int h)
{
  try {
    ew::View3Widget::set_window_size(w, h);
  } catch(...) {
    Error("ew::View3Widget::set_window_size exception");
  }
}

void
ViewWidget::paintGL()
{
  try {
    ew::View3Widget::render();
    glFlush();
  } catch(...) {
    Error("ew::View3Widget::render exception");
  }
}

void
ViewWidget::hideEvent(QHideEvent *)
{
  ew::View3Widget::set_currently_unrendered();
}

void
ViewWidget::mousePressEvent(QMouseEvent *ev)
{
  if (ev->button() == Qt::LeftButton) {
    pointerdown = true;
    pointerx = ev->x();
    pointery = ev->y();
    pointermods = ev->modifiers();
  }
}

void
ViewWidget::mouseMoveEvent(QMouseEvent *ev)
{
  if ((ev->buttons() & Qt::LeftButton) && pointerdown) {
    int x = ev->x();
    int y = ev->y();
    if (pointermods == 0) {
      ew::Transform3 tr;
      get_pointer_roll_tr(&tr, (x - pointerx) * 1.0, (y - pointery) * 1.0);
      view_post_compose(&tr);
    }
    if (pointermods == Qt::ControlModifier) {
      ew::Transform3 tr;
      get_pointer_twist_tr(&tr, pointerx * 1.0, pointery * 1.0,
       x * 1.0, y * 1.0, 1.0);
      view_post_compose(&tr);
    }
    if (pointermods == Qt::ShiftModifier) {
      ew::Transform3 tr;
      get_pointer_translate_tr(&tr,
       (x - pointerx) * 1.0, (y - pointery) * 1.0, 0);
      view_post_compose(&tr);
    }
    if (pointermods == (Qt::ControlModifier | Qt::ShiftModifier)) {
      ew::Transform3 tr;
      get_pointer_translate_tr(&tr, 0, 0, (x - pointerx) * 1.0);
      view_post_compose(&tr);
    }
    if (pointermods == Qt::AltModifier) {
      ew::Transform3 tr;
      get_pointer_scale_tr(&tr, pointerx * 1.0, pointery * 1.0,
       x * 1.0, y * 1.0, 1.0);
      view_post_compose(&tr);
    }
    pointerx = x;
    pointery = y;
  }
}

void
ViewWidget::mouseReleaseEvent(QMouseEvent *ev)
{
  if (ev->button() == Qt::LeftButton) {
    pointerdown = false;
  }
}

void
ViewWidget::redraw_cb()
{
  glDraw();
}

static const char *const Exps[] = {
  0,
  "Half cylinder rotating",
  "Half cylinder lengthening and shortening",
  "Half cylinder changing colour",
  "Half cylinder appearing and disappearing",
  "Half cylinder appearing and disappearing"
};

static const char *const Help =
 "drag button 1               -> tumble\n"
 "shift drag button 1         -> twist\n"
 "control drag button 1       -> translate\n"
 "shift control drag button 1 -> push\n"
 "alt drag button 1           -> scale";

MainWindow::MainWindow(ew::DataflowNetwork *nw, ewtst::TestSurface3 *sur)
 : QWidget(0)
{
  s = sur;
  setWhatsThis(tr(Help));
  ok = false;
  view = new ViewWidget(nw, 0);
  pass = new QPushButton(tr("Pa&ss"));
  fail = new QPushButton(tr("&Fail"));
  exit = new QPushButton(tr("E&xit"));
  pause = new QCheckBox(tr("Pause"));
  lab = new QLabel(tr("Press Shift-F1 for help"));
  exp = new QLabel(tr(Exps[TestN]));
  connect(pass, SIGNAL(clicked()), this, SLOT(dopass()));
  connect(fail, SIGNAL(clicked()), this, SLOT(dofail()));
  connect(exit, SIGNAL(clicked()), this, SLOT(doexit()));
  grid = new QGridLayout;
  grid->addWidget(view, 0, 0, 1, 4);
  grid->addWidget(pause, 1, 0);
  grid->addWidget(pass, 1, 1);
  grid->addWidget(fail, 1, 2);
  grid->addWidget(exit, 1, 3);
  grid->addWidget(exp, 2, 0, 1, 2);
  grid->addWidget(lab, 2, 2, 1, 2, Qt::AlignRight);
  grid->setRowStretch(0, 1);
  setLayout(grid);
  cnt = 0;
  animate_timer = startTimer(20);
  if (animate_timer == 0) {
    throw std::runtime_error("could not start qt animate timer");
  }
}

MainWindow::~MainWindow()
{
  if (!ok) {
    Error("not passed by user");
  }
}

void
MainWindow::timerEvent(QTimerEvent *ev)
{
  if (ev->timerId() == animate_timer) {
    if (pause->isChecked() || (TestN == 1 && view->pointerdown)) {
      return;
    }
    cnt += 1;
    try {
      if (cnt == 1) {
        view->sitem->set_surface(s);
        view->reset();
      }
      if (TestN == 1) {
        ew::Transform3 tr;
        tr.set_to_interpolation(&view->tr0, &view->tr1, cnt / 100.0);
        view->set_view_mapping(&tr);
      }
      if (TestN == 2) {
        const ewtst::TestSurface3 *cts =
         static_cast<const ewtst::TestSurface3 *>(view->sitem->get_surface());
        ewtst::TestSurface3 *ts = const_cast<ewtst::TestSurface3 *>(cts);
        if (cnt % 100 < 50) {
          ts->set_aspect(0.1 + 0.006 * (cnt % 100));
        } else {
          ts->set_aspect(0.1 + 0.006 * (100 - (cnt % 100)));
        }
      }
      if (TestN == 3) {
        if (cnt % 100 < 25) {
          unsigned char c[3];
          c[0] = 127 + (cnt % 100) * 4;
          c[1] = c[2] = 127;
          view->sitem->set_back_color(c);
        } else if (cnt % 100 < 50) {
          unsigned char c[3];
          c[0] = 127 + (50 - cnt % 100) * 4;
          c[1] = c[2] = 127;
          view->sitem->set_back_color(c);
        } else if (cnt % 100 < 75) {
          unsigned char c[3];
          c[0] = 255 - (cnt % 100 - 50) * 8;
          c[1] = c[2] = 255;
          view->sitem->set_front_color(c);
        } else {
          unsigned char c[3];
          c[0] = 255 - (100 - cnt % 100) * 8;
          c[1] = c[2] = 255;
          view->sitem->set_front_color(c);
        }
      }
      if (TestN == 4) {
        if (cnt % 100 < 50) {
          view->sitem->set_state(true);
        } else {
          view->sitem->set_state(false);
        }
      }
      if (TestN == 5) {
        if (cnt % 100 < 50) {
          view->sitem->set_surface(s);
        } else {
          view->sitem->set_surface(0);
        }
      }
    } catch(...) {
      Error("set exception");
    }
  }
}

void MainWindow::doexit()
{
  std::exit(1);
}

void MainWindow::dopass()
{
  ok = true;
  emit done();
}

void MainWindow::dofail()
{
  emit done();
}

static void
Testxx()
{
  ew::DataflowNetwork nw;
  ewtst::TestSurface3 *sv = new ewtst::TestSurface3(&nw);
  struct auto_sur {
    ewtst::TestSurface3 *v;
    ~auto_sur() {v->decr_ref_count();};
  } sur;
  sur.v = sv;
  int ac = 0;
  char **av = 0;
  QApplication app(ac, av);
  if (!QGLFormat::hasOpenGL()) {
    Error("no OpenGL support");
    return;
  }
  MainWindow top(&nw, sur.v);
  QObject::connect(&top, SIGNAL(done()), &app, SLOT(quit()));
  top.resize(800, 800);
  top.show();
  int r = app.exec();
  if (r != 0) {
    Error("app failed");
  }
}

static test_t Tests[] = {
  {"tumble_view", &Testxx},
  {"change_surface", &Testxx},
  {"change_colour", &Testxx},
  {"toggle_state", &Testxx},
  {"toggle_surface", &Testxx},
  {0, 0}
};

int
main(int ac, char **av)
{
  return ProcessCmdline("test_surface3",
   "Exercise the surface item of the view widget.\n", 0, Tests, ac, av);
}
