#ifdef _WIN32
#include <time.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <stdexcept>
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
#include "ew/Debug.h"
#include "ew/Transform3.h"
#include "ew/DataflowSurface3.h"
#include "ew/DataflowSurface3E.h"
#include "ew/Dig3View.h"
#include "ew/Form3.h"
#include "ew/Form3Surface.h"
#include "ew/View3Surface.h"
#include "ew/View3Widget.h"
#include "ew/Dig3.h"
#include "ew/Dig3View.h"
#include "ew/Dig3Space.h"
#include "ew/Dig3Tableau.h"
#include "test_dig3.h"
#include "testlowercase.h"

namespace {
  const char ClassName[] = "ViewWidget";
}

QGLWidget *ViewWidget::share = 0;
int sharen = 0;

ViewWidget::ViewWidget(QWidget *par, ew::Dig3 *dig3, int space,
 int slice_mode) :
 QGLWidget(par, share),
 ew::Dig3View(dig3, space, slice_mode),
 dbg(ClassName, ew::View3Widget::dbg)
{
  pointerdown = false;
  setFocusPolicy(Qt::ClickFocus);
  if (share == 0) {
    share = this;
  }
  sharen += 1;
}

ViewWidget::~ViewWidget()
{
  destruction_in_progress();
  sharen -= 1;
  if (sharen == 0) {
    share = 0;
  }
}

void
ViewWidget::schedule_idle_handler_cb()
{
  dbg.on && dbg.dprintf("%s::%s idle_timer start", dbg.fn,
   "schedule_idle_handler_cb");
  idle_timer = startTimer(0);
  if (idle_timer == 0) {
    throw std::runtime_error("could not start qt idle timer");
  }
}

void
ViewWidget::timerEvent(QTimerEvent *ev) {
  if (ev->timerId() == idle_timer) {
    dbg.on && dbg.dprintf("%s::%s idle_timer fire", dbg.fn,
     "schedule_idle_handler_cb");
    killTimer(idle_timer);
    idle_handler();
  }
}

void
ViewWidget::initializeGL()
{
  init_gl_context();
}

void
ViewWidget::resizeGL(int w, int h)
{
  set_window_size(w, h);
}

void
ViewWidget::paintGL()
{
  ew::View3Widget::render();
  glFlush();
}

void
ViewWidget::hideEvent(QHideEvent *)
{
  set_currently_unrendered();
}

void
ViewWidget::redraw_cb()
{
  glDraw();
}

void
ViewWidget::keyPressEvent(QKeyEvent *ev)
{
  double loc[3];
  ew::Transform3 tr;
  QPoint cp = mapFromGlobal(QCursor::pos());
  int x = cp.x();
  int y = cp.y();
  if (ev->key() == Qt::Key_Backspace) {
    int n = get_landmarks_item()->get_highlight_ps();
    if (n >= 0) {
      for (int i = 0; i < dig3->get_n_views(); i += 1) {
        dig3->get_views()[i]->clear_highlight();
      }
      ew::Dig3Space *sp = dig3->get_spaces()[get_space()];
      sp->remove_form_pointset(n);
    }
  } else if (ev->key() == Qt::Key_Return) {
    double loc[3];
    double z = 0.0;
    if (get_use_depth()) {
      ew::View3Widget::pick(x, y, 0.00001, 0.0, 0, -1, 2, 0, 0, 0, &z);
    }
    get_pointer_location(loc, x * 1.0, y * 1.0, z);
    ew::Dig3Space *sp = dig3->get_spaces()[get_space()];
    double t;
#ifdef _WIN32
    FILETIME tv;
    GetSystemTimeAsFileTime(&tv);
    t = (tv.dwHighDateTime * 4294967296.0 + tv.dwLowDateTime) * 1.0e-7;
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    t = tv.tv_sec * 1.0 + tv.tv_usec * 1.0e-6;
#endif
    char buf[100];
    std::snprintf(buf, 90, "lmk%.0f", t * 100.0);
    ew::Form3PointSet ps;
    ps.id = buf;
    ps.n = 1;
    ps.locations.resize(3);
    ps.locations[0] = loc[0];
    ps.locations[1] = loc[1];
    ps.locations[2] = loc[2];
    bool b;
    int n = sp->set_form_pointset(&b, &ps);
    for (int i = 0; i < dig3->get_n_views(); i += 1) {
      ew::Dig3View *v = dig3->get_views()[i];
      if (v->get_space() == get_space()) {
        v->get_landmarks_item()->set_highlight(n, 0);
      } else {
        v->clear_highlight();
      }
    }
  } else if (ev->key() == Qt::Key_D) {
    int n = get_landmarks_item()->get_highlight_ps();
    if (n >= 0) {
      get_center_location(loc);
      ew::Dig3Space *sp = dig3->get_spaces()[get_space()];
      sp->set_form_pointset_location(n, 0, loc);
    }
  } else if (ev->key() == Qt::Key_M) {
    int n = get_landmarks_item()->get_highlight_ps();
    if (n >= 0) {
      double z = 0.0;
      if (get_use_depth()) {
        ew::View3Widget::pick(x, y, 0.00001, 0.0, 0, -1, 2, 0, 0, 0, &z);
      }
      get_pointer_location(loc, x * 1.0, y * 1.0, z);
      ew::Dig3Space *sp = dig3->get_spaces()[get_space()];
      sp->set_form_pointset_location(n, 0, loc);
    }
  } else if (ev->key() == Qt::Key_C) {
    get_pointer_location(loc, x * 1.0, y * 1.0);
    get_jump_tr(&tr, loc, false);
    view_post_compose(&tr);
  } else if (ev->key() == Qt::Key_S) {
    ew::Dig3Space *sp = dig3->get_spaces()[get_space()];
    if (sp->get_form_data()->surfaces.size() != 0) {
      const ew::Bbox3 *b = get_surface_items()[0]->get_bbox();
      if (get_bbox_tr(&tr, b, true, true)) {
        view_post_compose(&tr);
      }
    }
  } else if (ev->key() == Qt::Key_L) {
    const ew::Bbox3 *b = get_landmarks_item()->get_bbox();
    if (get_bbox_tr(&tr, b, true, true)) {
      view_post_compose(&tr);
    }
  } else if (ev->key() == Qt::Key_I) {
    int i = 0;
    while (get_slice_items()[i] == 0) {
      i += 1;
    }
    const ew::Bbox3 *b = get_slice_items()[i]->get_bbox();
    if (get_bbox_tr(&tr, b, true, true)) {
      view_post_compose(&tr);
    }
  } else if (ev->key() == Qt::Key_V) {
    ew::Bbox3 b;
    get_bbox(&b);
    if (get_bbox_tr(&tr, &b, true, true)) {
      view_post_compose(&tr);
    }
  } else if (ev->key() == Qt::Key_F) {
    ew::Bbox3 b;
    ew::Dig3Space *sp = dig3->get_spaces()[get_space()];
    sp->get_bbox(&b);
    if (get_bbox_tr(&tr, &b, true, true)) {
      view_post_compose(&tr);
    }
  } else if (ev->key() == Qt::Key_X) {
    ew::Dig3Space *sp = dig3->get_spaces()[get_space()];
    sp->get_surface_nodes()[0]->make_index();
  } else {
    QWidget::keyPressEvent(ev);
  }
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
ViewWidget::mouseReleaseEvent(QMouseEvent *ev)
{
  if (ev->button() == Qt::LeftButton) {
    pointerdown = false;
  } else if (ev->button() == Qt::RightButton) {
    int x = ev->x();
    int y = ev->y();
    int c;
    bool r = ew::View3Widget::pick(x, y, 5.0, 2.5,
     get_landmarks_item(), -1, -1, 0, &c, 0, 0);
    for (int i = 0; i < dig3->get_n_views(); i += 1) {
      ew::Dig3View *v = dig3->get_views()[i];
      if (r && v->get_space() == get_space()) {
        v->get_landmarks_item()->set_highlight(c, 0);
      } else {
        v->clear_highlight();
      }
    }
#ifdef XXX
    double loc[3];
    get_pointer_location(loc, x * 1.0, y * 1.0);
    ew::Dig3Space *sp = dig3->get_spaces()[get_space()];
    if (sp->get_form_data()->pointsets.size() != 0) {
      sp->get_form_node()->set_pointset_location(0, 0, loc);
    } else {
      ew::Form3PointSet ps;
      ps.id = "mylmk";
      ps.n = 1;
      ps.locations.resize(3);
      ps.locations[0] = loc[0];
      ps.locations[1] = loc[1];
      ps.locations[2] = loc[2];
      sp->get_form_node()->set_pointset(&ps);
    }
    for (int i = 0; i < dig3->get_n_views(); i += 1) {
      ew::Dig3View *v = dig3->get_views()[i];
      if (v->get_space() == get_space()) {
        v->get_landmarks_item()->set_highlight(0, 0);
      } else {
        v->clear_highlight();
      }
    }
#endif
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

static const char *const Exps1[] = {
  "template(mandible+lmk) main+slice, specimen(mandible+tetra+lmks)",
  "template(mandible+lmk) main+3 linked slices",
  "specimen(mandible+testrahedron+lmks) 3linked mains+slice",
};

static const char *const Exps2 =
 "template(mandible+lmk) main+slice,"
 " specimen(mandible+tetra+lmks) main+slice";

static const char *const Help =
 "drag button 1               -> tumble\n"
 "shift drag button 1         -> twist\n"
 "control drag button 1       -> translate\n"
 "shift control drag button 1 -> push\n"
 "alt drag button 1           -> scale\n"
 "click button 3              -> select/deselect lmk\n"
 "c                           -> center view\n"
 "d                           -> move selected lmk to center\n"
 "m                           -> move selected lmk\n"
 "enter                       -> create lmk\n"
 "backspace                   -> delete selected lmk\n"
 "s                           -> look at surface\n"
 "i                           -> look at slice\n"
 "l                           -> look at landmarks\n"
 "v                           -> look at entire view\n"
 "f                           -> look at form";

MainWindow::MainWindow(ew::DataflowNetwork *n) :
 QWidget(0),
 dig3(n),
 nw(n)
{
  mode_n = 0;
  setWhatsThis(tr(Help));
  ok = false;
  playing = false;
  playtimer = 0;
  view[0] = new ViewWidget(this, &dig3, 0, 0);
  view[1] = new ViewWidget(this, &dig3, 0, 1);
  view[2] = new ViewWidget(this, &dig3, 1, 0);
  view[3] = new ViewWidget(this, &dig3, 1, 1);
  ew::Form3Surface sr;
  sr.id = "Mand";
  sr.file = FullPath("mandible.obj");
  bool repl;
  dig3.get_spaces()[0]->set_form_surface(&repl, &sr);
  dig3.get_spaces()[1]->set_form_surface(&repl, &sr);
  sr.id = "Tetra";
  sr.file = FullPath("tetra.obj");
  dig3.get_spaces()[1]->set_form_surface(&repl, &sr);
  ew::Form3PointSet ps;
  ps.locations.resize(3);
  ps.n = 1;
  ps.id = "lmk 1";
  ps.locations[0] = -0.3;
  ps.locations[1] = 3.4;
  ps.locations[2] = -20.9;
  dig3.get_spaces()[0]->set_form_pointset(&repl, &ps);
  dig3.get_spaces()[1]->set_form_pointset(&repl, &ps);
  ps.id = "lmk 2";
  ps.locations[0] = -2;
  ps.locations[1] = -1;
  ps.locations[2] = -10;
  dig3.get_spaces()[1]->set_form_pointset(&repl, &ps);
  ew::Transform3 trm;
  trm.set_to_identity();
  trm.scale = 0.1;
  trm.translate[2] = 1.5;
  dig3.get_views()[0]->set_view_mapping(&trm);
  dig3.get_views()[1]->set_view_mapping(&trm);
  dig3.get_views()[2]->set_view_mapping(&trm);
  dig3.get_views()[3]->set_view_mapping(&trm);
  unsigned char col[3] = {64, 64, 0};
  dig3.get_views()[0]->get_slice_items()[1]->set_color(col);
  dig3.get_views()[1]->get_slice_items()[1]->set_color(col);
  unsigned char col2[3] = {0, 64, 64};
  dig3.get_views()[2]->get_slice_items()[3]->set_color(col2);
  dig3.get_views()[3]->get_slice_items()[3]->set_color(col2);
  pass = new QPushButton(tr("Pa&ss"));
  fail = new QPushButton(tr("&Fail"));
  exit = new QPushButton(tr("E&xit"));
  if (TestN == 1) {
    mode = new QPushButton(tr("switch view mode"));
  } else {
    load = new QPushButton(tr("load (cycle)"));
    save = new QPushButton(tr("save (append)"));
    reset = new QPushButton(tr("reset"));
    play = new QCheckBox(tr("play"));
  }
  lab = new QLabel(tr("Press Shift-F1 for help"));
  if (TestN == 1) {
    exp = new QLabel(tr(Exps1[mode_n]));
  } else {
    exp = new QLabel(tr(Exps2));
  }
  connect(pass, SIGNAL(clicked()), this, SLOT(dopass()));
  connect(fail, SIGNAL(clicked()), this, SLOT(dofail()));
  connect(exit, SIGNAL(clicked()), this, SLOT(doexit()));
  if (TestN == 1) {
    connect(mode, SIGNAL(clicked()), this, SLOT(domode()));
  } else {
    connect(save, SIGNAL(clicked()), this, SLOT(dosave()));
    connect(load, SIGNAL(clicked()), this, SLOT(doload()));
    connect(reset, SIGNAL(clicked()), this, SLOT(doreset()));
    connect(play, SIGNAL(stateChanged(int)), this, SLOT(doplay(int)));
  }
  grid = new QGridLayout;
  grid->addWidget(view[0], 0, 0, 1, 4);
  grid->addWidget(view[1], 0, 4, 1, 4);
  grid->addWidget(view[2], 1, 0, 1, 4);
  grid->addWidget(view[3], 1, 4, 1, 4);
  if (TestN == 1) {
    grid->addWidget(mode, 2, 0);
  } else {
    grid->addWidget(load, 2, 0);
    grid->addWidget(save, 2, 1);
    grid->addWidget(reset, 2, 2);
    grid->addWidget(play, 2, 3);
  }
  grid->addWidget(pass, 2, 4);
  grid->addWidget(fail, 2, 5);
  grid->addWidget(exit, 2, 6);
  grid->addWidget(exp, 3, 0, 1, 4);
  grid->addWidget(lab, 3, 4, 1, 3, Qt::AlignRight);
  grid->setRowStretch(0, 1);
  grid->setRowStretch(1, 1);
  setLayout(grid);
  if (TestN != 1) {
    dosave();
    save_pos = 0;
  }
}

// Rely on dig3 being a member, so it will be destroyed before base QWidget,
// whose destruction destroys grid and its children.

MainWindow::~MainWindow()
{
  delete dig3.get_views()[1];
  delete dig3.get_views()[0];
  if (!ok) {
    Error("not passed by user");
  }
}

void
MainWindow::doload()
{
  for (int i = 0; i < dig3.get_n_views(); i += 1) {
    dig3.get_views()[i]->clear_highlight();
  }
  std::string fname = FullPath("test_dig3_tab.out");
  ew::Dig3Tableau::read_file(&saves, fname.c_str());
  if (save_pos >= static_cast<int>(saves.size())) {
    save_pos = 0;
  }
  ew::Form3 form1;
  ew::Form3 form2;
  form1.read_file(saves[save_pos].space[0].form_filename.c_str());
  form2.read_file(saves[save_pos].space[1].form_filename.c_str());
  dig3.get_spaces()[0]->set_form_data(&form1);
  dig3.get_spaces()[1]->set_form_data(&form2);
  if (*dig3.get_spaces()[0]->get_form_data() != form1) {
    Error("template form not as set");
  }
  if (*dig3.get_spaces()[1]->get_form_data() != form2) {
    std::string fnamea = FullPath("test_dig3_err_sf1a.out");
    dig3.get_spaces()[1]->get_form_data()->write_file(fnamea.c_str(), 0);
    std::string fnameb = FullPath("test_dig3_err_sf1b.out");
    form2.write_file(fnameb.c_str(), 0);
    Error("specimen form not as set");
  }
  dig3.load_tableau(&saves[save_pos], 0, ew::Dig3::TABLEAU_ALL);
  dig3.load_tableau(&saves[save_pos], 1, ew::Dig3::TABLEAU_ALL);
  dig3.load_tableau(&saves[save_pos], 2, ew::Dig3::TABLEAU_ALL);
  dig3.load_tableau(&saves[save_pos], 3, ew::Dig3::TABLEAU_ALL);
  ew::Dig3Tableau check_save;
  dig3.save_tableau(&check_save, 0, 1, 2, 3);
  check_save.space[0].form_filename =
   saves[save_pos].space[0].form_filename.c_str();
  check_save.space[1].form_filename =
   saves[save_pos].space[1].form_filename.c_str();
  if (saves[save_pos] != check_save) {
    Error("tableau not as set");
  }
  save_pos += 1;
}

void
MainWindow::dosave()
{
  int n = saves.size();
  char name1[30];
  char name2[30];
  std::snprintf(name1, 25, "./test_dig3_tem%03d.out", n);
  std::snprintf(name2, 25, "./test_dig3_tar%03d.out", n);
  std::string fname1 = FullPath(name1);
  std::string fname2 = FullPath(name2);
  dig3.get_spaces()[0]->get_form_data()->write_file(fname1.c_str(), 0);
  dig3.get_spaces()[1]->get_form_data()->write_file(fname2.c_str(), 0);
  ew::Form3 fck;
  fck.read_file(fname1.c_str());
  if (fck != *dig3.get_spaces()[0]->get_form_data()) {
    Error("template form not as written");
  }
  fck.read_file(fname2.c_str());
  if (fck != *dig3.get_spaces()[1]->get_form_data()) {
    Error("specimen form not as written");
  }
  saves.resize(n + 1);
  dig3.save_tableau(&saves[n], 0, 1, 2, 3);
  saves[n].space[0].form_filename = fname1.c_str();
  saves[n].space[1].form_filename = fname2.c_str();
  std::string fname = FullPath("test_dig3_tab.out");
  ew::Dig3Tableau::write_file(fname.c_str(), 0, &saves);
  static std::vector<ew::Dig3Tableau> SavesCheck;
  ew::Dig3Tableau::read_file(&SavesCheck, fname.c_str());
  if (saves == SavesCheck) {
    return;
  }
  Error("save file not as written");
  if (saves.size() != SavesCheck.size()) {
    Error("save file not as written: size %d %d\n", saves.size(),
     SavesCheck.size());
    return;
  }
  for (int i = 0; i < static_cast<int>(saves.size()); i += 1) {
    if (saves[i] != SavesCheck[i]) {
      Error("save file not as written: record %d", i);
      if (saves[i].slice_clip_ratio != SavesCheck[i].slice_clip_ratio) {
        Error("save file not as written: slice_clip_ratio %g %g\n",
         saves[i].slice_clip_ratio, SavesCheck[i].slice_clip_ratio);
      }
      for (int j = 0; j < 3; j += 1) {
        if (saves[i].bg[j] != SavesCheck[i].bg[j]) {
          Error("save file not as written: bg[%d] %d %d\n",
           j, saves[i].bg[j], SavesCheck[i].bg[j]);
        }
      }
      for (int j = 0; j < 2; j += 1) {
        if (saves[i].space[j] == SavesCheck[i].space[j]) {
          continue;
        }
        Error("save file not as written: record %d space %d", i, j);
        if (saves[i].space[j].form_filename !=
         SavesCheck[i].space[j].form_filename) {
          Error("save file not as written: form_filename %s %s",
           saves[i].space[j].form_filename.c_str(),
           SavesCheck[i].space[j].form_filename.c_str());
        }
        if (saves[i].space[j].main_view != SavesCheck[i].space[j].main_view) {
          char buf1[999], buf2[999];
          saves[i].space[j].main_view.format(buf1, 999, 0);
          SavesCheck[i].space[j].main_view.format(buf2, 999, 0);
          Error("save file not as written: mainview %s %s",
           buf1, buf2);
        }
        if (saves[i].space[j].slice_view !=
         SavesCheck[i].space[j].slice_view) {
          char buf1[999], buf2[999];
          saves[i].space[j].slice_view.format(buf1, 999, 0);
          SavesCheck[i].space[j].slice_view.format(buf2, 999, 0);
          Error("save file not as written: slice %s %s",
           buf1, buf2);
        }
        if (saves[i].space[j].show_slice_in_main !=
         SavesCheck[i].space[j].show_slice_in_main) {
          Error("save file not as written: show_slice_in_main %d %d",
           saves[i].space[j].show_slice_in_main,
           SavesCheck[i].space[j].show_slice_in_main);
        }
        if (saves[i].space[j].surface_settings !=
         SavesCheck[i].space[j].surface_settings) {
          Error("save file not as written: surface_settings");
        }
      }
    }
  }
}

void
MainWindow::doreset()
{
  for (int i = 0; i < dig3.get_n_views(); i += 1) {
    dig3.get_views()[i]->clear_highlight();
  }
  dig3.get_spaces()[0]->reset_form();
  dig3.get_spaces()[1]->reset_form();
}

void
MainWindow::domode()
{
  for (int i = 0; i < dig3.get_n_views(); i += 1) {
    dig3.get_views()[i]->clear_highlight();
  }
  mode_n += 1;
  if (mode_n > 2) {
    mode_n = 0;
  }
  exp->setText(tr(Exps1[mode_n]));
  if (mode_n == 0) {
    view[0]->set_space(0);
    view[1]->set_space(0);
    view[2]->set_space(1);
    view[3]->set_space(1);
    view[0]->set_slice_mode(0);
    view[1]->set_slice_mode(1);
    view[2]->set_slice_mode(0);
    view[3]->set_slice_mode(1);
    ew::Transform3 trm;
    trm.set_to_identity();
    view[0]->set_link(0, 0);
    view[1]->set_link(0, 0);
    view[2]->set_link(0, 0);
    view[3]->set_link(0, 0);
    trm.scale = 0.1;
    trm.translate[2] = 1.5;
    dig3.get_views()[0]->set_view_mapping(&trm);
    dig3.get_views()[1]->set_view_mapping(&trm);
    dig3.get_views()[2]->set_view_mapping(&trm);
    unsigned char col[3] = {64, 64, 0};
    dig3.get_views()[0]->get_slice_items()[1]->set_color(col);
    dig3.get_views()[1]->get_slice_items()[1]->set_color(col);
    unsigned char col2[3] = {0, 64, 64};
    dig3.get_views()[2]->get_slice_items()[3]->set_color(col2);
    dig3.get_views()[3]->get_slice_items()[3]->set_color(col2);
  } else if (mode_n == 1) {
    view[0]->set_space(0);
    view[1]->set_space(0);
    view[2]->set_space(0);
    view[3]->set_space(0);
    view[0]->set_slice_mode(0);
    view[1]->set_slice_mode(1);
    view[2]->set_slice_mode(1);
    view[3]->set_slice_mode(1);
    ew::Transform3 trm23;
    trm23.set_to_identity();
    trm23.orthog[0][0] = 0.0;
    trm23.orthog[2][2] = 0.0;
    trm23.orthog[0][2] = -1.0;
    trm23.orthog[2][0] = 1.0;
    ew::Transform3 trm31;
    trm31.set_to_identity();
    trm31.orthog[1][1] = 0.0;
    trm31.orthog[2][2] = 0.0;
    trm31.orthog[1][2] = -1.0;
    trm31.orthog[2][1] = 1.0;
    ew::Transform3 trm12;
    trm12.set_to_composition(&trm31, &trm23);
    trm12.set_to_inverse(&trm12);
    view[0]->set_link(0, 0);
    view[2]->set_link(view[3], &trm23);
    view[3]->set_link(view[1], &trm31);
    view[1]->set_link(view[2], &trm12);
    ew::Transform3 trm;
    trm.set_to_identity();
    trm.scale = 0.1;
    trm.translate[2] = 1.5;
    dig3.get_views()[0]->set_view_mapping(&trm);
    dig3.get_views()[3]->set_view_mapping(&trm);
    unsigned char col[3] = {64, 64, 0};
    dig3.get_views()[0]->get_slice_items()[1]->set_color(col);
    dig3.get_views()[1]->get_slice_items()[1]->set_color(col);
    unsigned char col2[3] = {0, 64, 64};
    dig3.get_views()[0]->get_slice_items()[2]->set_color(col2);
    dig3.get_views()[2]->get_slice_items()[2]->set_color(col2);
    unsigned char col3[3] = {64, 0, 64};
    dig3.get_views()[0]->get_slice_items()[3]->set_color(col3);
    dig3.get_views()[3]->get_slice_items()[3]->set_color(col3);
  } else if (mode_n == 2) {
    view[0]->set_space(1);
    view[1]->set_space(1);
    view[2]->set_space(1);
    view[3]->set_space(1);
    view[0]->set_slice_mode(0);
    view[1]->set_slice_mode(0);
    view[2]->set_slice_mode(0);
    view[3]->set_slice_mode(1);
    ew::Transform3 trm01;
    trm01.set_to_identity();
    trm01.orthog[0][0] = 0.0;
    trm01.orthog[2][2] = 0.0;
    trm01.orthog[0][2] = -1.0;
    trm01.orthog[2][0] = 1.0;
    ew::Transform3 trm20;
    trm20.set_to_identity();
    trm20.orthog[1][1] = 0.0;
    trm20.orthog[2][2] = 0.0;
    trm20.orthog[1][2] = -1.0;
    trm20.orthog[2][1] = 1.0;
    ew::Transform3 trm12;
    trm12.set_to_composition(&trm01, &trm20);
    trm12.set_to_inverse(&trm12);
    view[0]->set_link(view[1], &trm01);
    view[2]->set_link(view[0], &trm20);
    view[1]->set_link(view[2], &trm12);
    view[3]->set_link(0, 0);
    ew::Transform3 trm;
    trm.set_to_identity();
    trm.scale = 0.1;
    trm.translate[2] = 1.5;
    dig3.get_views()[0]->set_view_mapping(&trm);
    dig3.get_views()[3]->set_view_mapping(&trm);
    unsigned char col[3] = {64, 64, 0};
    dig3.get_views()[0]->get_slice_items()[3]->set_color(col);
    dig3.get_views()[1]->get_slice_items()[3]->set_color(col);
    dig3.get_views()[2]->get_slice_items()[3]->set_color(col);
    dig3.get_views()[3]->get_slice_items()[3]->set_color(col);
  }
}

void
MainWindow::doexit()
{
  std::exit(1);
}

void
MainWindow::dopass()
{
  ok = true;
  emit done();
}

void
MainWindow::dofail()
{
  emit done();
}

void
MainWindow::timerEvent(QTimerEvent *ev) {
  if (ev->timerId() != playtimer) {
    return;
  }
  int ms = playtime.elapsed();
  if (ew::Debug::instance().get_active("MainWindow")) {
    ew::Debug::instance().dprintf("%s::%s fire playtimer ms=%d del=%d",
     "MainWindow", "timerEvent", ms, ms - playlastms);
  }
  playlastms = ms;
  int n = playsaves.size();
  int msperframe = 1000;
  int frame0 = (ms / msperframe) % n;
  int frame1 = (frame0 + 1) % n;
  double msf = (ms % msperframe) * 0.001;
  int framenear;
  if (msf < 0.5) {
    framenear = frame0;
  } else {
    framenear = frame1;
  }
  if (framenear != playlastframe) {
    for (int i = 0; i < dig3.get_n_views(); i += 1) {
      dig3.get_views()[i]->clear_highlight();
    }
    if (playlastframe == -1 ||
     playforms1[framenear] != playforms1[playlastframe]) {
      dig3.get_spaces()[0]->set_form_data(&playforms1[framenear]);
    }
    if (playlastframe == -1 ||
     playforms2[framenear] != playforms2[playlastframe]) {
      dig3.get_spaces()[1]->set_form_data(&playforms2[framenear]);
    }
    dig3.load_tableau(&playsaves[framenear], 0, ew::Dig3::TABLEAU_SETTINGS);
    dig3.load_tableau(&playsaves[framenear], 1, ew::Dig3::TABLEAU_SETTINGS);
    dig3.load_tableau(&playsaves[framenear], 2, ew::Dig3::TABLEAU_SETTINGS);
    dig3.load_tableau(&playsaves[framenear], 3, ew::Dig3::TABLEAU_SETTINGS);
    playlastframe = framenear;
  }
  dig3.interpolate_tableau(&playsaves[frame0], &playsaves[frame1], msf, 0);
  dig3.interpolate_tableau(&playsaves[frame0], &playsaves[frame1], msf, 1);
  dig3.interpolate_tableau(&playsaves[frame0], &playsaves[frame1], msf, 2);
  dig3.interpolate_tableau(&playsaves[frame0], &playsaves[frame1], msf, 3);
//XXX might not need to kill it
  killTimer(playtimer);
//XXX might need to have a small interval
  playtimer = startTimer(0);
  if (playtimer == 0) {
    throw std::runtime_error("could not start qt play timer");
  }
}

void
MainWindow::doplay(int state)
{
  for (int i = 0; i < dig3.get_n_views(); i += 1) {
    dig3.get_views()[i]->clear_highlight();
  }
  if (playing) {
    playing = false;
    if (playtimer != 0) {
      killTimer(playtimer);
    }
  }
  if (!state) {
    return;
  }
  std::string fname = FullPath("test_dig3_tab.out");
  ew::Dig3Tableau::read_file(&playsaves, fname.c_str());
  int n = playsaves.size();
  if (n < 2) {
    return;
  }
  playforms1.resize(n);
  playforms2.resize(n);
  for (int i = 0; i < n; i += 1) {
    playforms1[i].read_file(playsaves[i].space[0].form_filename.c_str());
    playforms2[i].read_file(playsaves[i].space[1].form_filename.c_str());
  }
  playing = true;
  playtime.start();
  playlastms = 0;
  playlastframe = -1;
  if (ew::Debug::instance().get_active("MainWindow")) {
    ew::Debug::instance().dprintf("%s::%s start playtimer", "MainWindow",
     "doplay");
  }
  playtimer = startTimer(0);
  if (playtimer == 0) {
    throw std::runtime_error("could not start qt play timer");
  }
}

static const char Name01[] = "navigation";

static void
Test01()
{
  ew::DataflowNetwork nw;
  int ac = 0;
  char **av = 0;
  QApplication app(ac, av);
  if (!QGLFormat::hasOpenGL()) {
    Error("no OpenGL support");
    return;
  }
  MainWindow top(&nw);
  QObject::connect(&top, SIGNAL(done()), &app, SLOT(quit()));
  top.resize(800, 800);
  top.show();
  int r = app.exec();
  if (r != 0) {
    Error("app failed");
  }
}

static const char Name02[] = "save";

static void
Test02()
{
  ew::DataflowNetwork nw;
  int ac = 0;
  char **av = 0;
  QApplication app(ac, av);
  if (!QGLFormat::hasOpenGL()) {
    Error("no OpenGL support");
    return;
  }
  MainWindow top(&nw);
  QObject::connect(&top, SIGNAL(done()), &app, SLOT(quit()));
  top.resize(800, 800);
  top.show();
  int r = app.exec();
  if (r != 0) {
    Error("app failed");
  }
}

static test_t Tests[] = {
  {Name01, &Test01},
  {Name02, &Test02},
  {0, 0}
};

int
main(int ac, char **av)
{
  return ProcessCmdline("test_dig3",
   "Exercise the dig3 main and slice widgets.\n", 0, Tests, ac, av);
}
