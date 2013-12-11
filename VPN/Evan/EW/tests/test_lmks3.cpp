#include <cstdlib>
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
#include "ew/DataflowForm3.h"
#include "ew/View3Widget.h"
#include "ew/View3Item.h"
#include "ew/View3Landmarks.h"
#include "test_lmks3.h"
#include "testlowercase.h"

namespace {
  const char ClassName[] = "ViewWidget";
}

void
ViewWidget::reset()
{
  ew::Transform3 tr1;
  tr1.set_to_identity();
  set_view_mapping(&tr1);
  unsigned char bc[3];
  bc[0] = bc[1] = 0;
  bc[2] = 64;
  ew::View3Widget::set_background_color(bc);
  bc[0] = 255;
  bc[1] = bc[2] = 0;
  litem->set_color(bc);
}

ViewWidget::ViewWidget(ew::DataflowNetwork *nw, QWidget *par) :
 QGLWidget(par),
 ew::View3Widget(nw),
 litem(new ew::View3Landmarks(this)),
 dbg(ClassName, ew::View3Widget::dbg)
{
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

void ViewWidget::timerEvent(QTimerEvent *ev)
{
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
  } catch(std::exception &e) {
    Error("ew::View3Widget::render exception %s", e.what());
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
  "9 fixed",
  "8 fixed and 1 moving",
  "9 fixed: 2 fixed, reset, empty form, no form",
  "9 fixed: off, circle, dot, green"
};

static const char *const Help =
 "drag button 1               -> tumble\n"
 "shift drag button 1         -> twist\n"
 "control drag button 1       -> translate\n"
 "shift control drag button 1 -> push\n"
 "alt drag button 1           -> scale";

MainWindow::MainWindow(ew::DataflowNetwork *nw, ew::DataflowForm3 *f)
 : QWidget(0)
{
  frm = f;
  ew::Form3 data;
  data.pointsets.resize(3);
  data.pointsets[0].n = 1;
  data.pointsets[0].type = ew::Form3::TYPE_PLANE;
  data.pointsets[0].locations.resize(3);
  data.pointsets[0].locations[0] = 0.0;
  data.pointsets[0].locations[1] = 1.0;
  data.pointsets[0].locations[2] = 0.0;
  data.pointsets[1].n = 1;
  data.pointsets[1].type = ew::Form3::TYPE_LANDMARK;
  data.pointsets[1].locations.resize(3);
  data.pointsets[1].locations[0] = 0.0;
  data.pointsets[1].locations[1] = 0.0;
  data.pointsets[1].locations[2] = 0.0;
  data.pointsets[2].n = 8;
  data.pointsets[2].type = ew::Form3::TYPE_SEMI_LANDMARK;
  data.pointsets[2].locations.resize(24);
  for (int i = 0; i < 2; i += 1) {
    for (int j = 0; j < 2; j += 1) {
      for (int k = 0; k < 2; k += 1) {
        data.pointsets[2].locations[i * 12 + j * 6 + k * 3] = 0.5 - i * 1.0;
        data.pointsets[2].locations[i * 12 + j * 6 + k * 3 + 1] =
         0.5 - j * 1.0;
        data.pointsets[2].locations[i * 12 + j * 6 + k * 3 + 2] =
         0.5 - k * 1.0;
      }
    }
  }
  frm->set_data(&data);
  setWhatsThis(tr(Help));
  ok = false;
  view = new ViewWidget(nw, 0);
  view->litem->set_form(frm);
  pass = new QPushButton(tr("Pa&ss"));
  fail = new QPushButton(tr("&Fail"));
  exit = new QPushButton(tr("E&xit"));
  lab = new QLabel(tr("Press Shift-F1 for help"));
  exp = new QLabel(tr(Exps[TestN]));
  connect(pass, SIGNAL(clicked()), this, SLOT(dopass()));
  connect(fail, SIGNAL(clicked()), this, SLOT(dofail()));
  connect(exit, SIGNAL(clicked()), this, SLOT(doexit()));
  grid = new QGridLayout;
  grid->addWidget(view, 0, 0, 1, 4);
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
    cnt += 1;
    try {
      if (TestN == 2) {
        ew::Form3 data = *frm->get_data();
        data.pointsets[1].locations[0] = (cnt % 100) / 100.0 * 0.5;
        frm->set_data(&data);
      }
      if (TestN == 3) {
        static ew::Form3 Data0;
        if (cnt == 1) {
          Data0 = *frm->get_data();
        }
        if (cnt % 160 == 20) {
          ew::Form3 data1;
          data1.pointsets.resize(1);
          data1.pointsets[0].n = 2;
          data1.pointsets[0].locations.resize(6);
          data1.pointsets[0].locations[0] = 0.5;
          data1.pointsets[0].locations[1] = 0.0;
          data1.pointsets[0].locations[2] = 0.0;
          data1.pointsets[0].locations[3] = -0.5;
          data1.pointsets[0].locations[4] = 0.0;
          data1.pointsets[0].locations[5] = 0.0;
          frm->set_data(&data1);
        } else if (cnt % 160 == 40) {
          frm->set_data(&Data0);
        } else if (cnt % 160 == 60) {
          frm->reset();
        } else if (cnt % 160 == 80) {
          frm->set_data(&Data0);
        } else if (cnt % 160 == 100) {
          ew::Form3 fm;
          frm->set_data(&fm);
        } else if (cnt % 160 == 120) {
          frm->set_data(&Data0);
        } else if (cnt % 160 == 140) {
          view->litem->set_form(0);
        } else if (cnt % 160 == 0) {
          view->litem->set_form(frm);
        }
      }
      if (TestN == 4) {
        if (cnt % 200 == 25) {
          view->litem->set_state(false);
        } else if (cnt % 200 == 50) {
          view->litem->set_state(true);
        } else if (cnt % 200 == 75) {
          view->litem->set_symbol(ew::View3Landmarks::SYMBOL_CIRCLE);
        } else if (cnt % 200 == 100) {
          view->litem->set_symbol(ew::View3Landmarks::SYMBOL_CROSS);
        } else if (cnt % 200 == 125) {
          view->litem->set_symbol(ew::View3Landmarks::SYMBOL_DOT);
        } else if (cnt % 200 == 150) {
          view->litem->set_symbol(ew::View3Landmarks::SYMBOL_CROSS);
        } else if (cnt % 200 == 175) {
          unsigned char c[3];
          c[0] = c[2] = 0;
          c[1] = 255;
          view->litem->set_color(c);
        } else if (cnt % 200 == 0) {
          unsigned char c[3];
          c[1] = c[2] = 0;
          c[0] = 255;
          view->litem->set_color(c);
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
  ew::DataflowForm3 *fm = new ew::DataflowForm3(&nw);
  struct auto_fm {
    ew::DataflowForm3 *v;
    ~auto_fm() {v->decr_ref_count();};
  } afm;
  afm.v = fm;
  int ac = 0;
  char **av = 0;
  QApplication app(ac, av);
  if (!QGLFormat::hasOpenGL()) {
    Error("no OpenGL support");
    return;
  }
  MainWindow top(&nw, fm);
  QObject::connect(&top, SIGNAL(done()), &app, SLOT(quit()));
  top.resize(800, 800);
  top.show();
  int r = app.exec();
  if (r != 0) {
    Error("app failed");
  }
}

static test_t Tests[] = {
  {"constant", &Testxx},
  {"moving", &Testxx},
  {"changing", &Testxx},
  {"settings", &Testxx},
  {0, 0}
};

int
main(int ac, char **av)
{
  return ProcessCmdline("test_lmks3",
   "Exercise the landmarks item of the view widget.\n", 0, Tests, ac, av);
}
