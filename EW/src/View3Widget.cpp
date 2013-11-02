// wdkg 2008-2010

//XXX ? double render on test_dig3 start
//XXX ? break large surfaces up by bbox for efficiency in picking
//XXX reimplement depth offset, make sure it earlier later items visible
//XXX also depth mode so that later drawn items are visible

#include <climits>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <GL/gl.h>
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/ErrorLogic.h"
#include "ew/String.h"
#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNotifier.h"
#include "ew/ErrorRuntime.h"
#include "ew/Transform3.h"
#include "ew/String.h"
#include "ew/View3Item.h"
#include "ew/View3Pick.h"
#include "ew/View3Widget.h"
#include "AutoArray.h"

namespace {
  const char ClassName[] = "ew::View3Widget";

  template<typename T>
  inline int
  VectorSize(const std::vector<T> &v)
  {
    return static_cast<int>(v.size());
  }

  template<typename T>
  inline void
  VectorErase(std::vector<T> &v, int n)
  {
    v.erase(v.begin() + n);
  }

  template<typename T>
  inline void
  VectorInsert(std::vector<T> &v, int n, const T &val)
  {
    v.insert(v.begin() + n, val);
  }
}

/// Create an empty view widget.
/// @param[in] i_network a pointer to a network to associate this view with

ew::View3Widget::View3Widget(ew::DataflowNetwork *i_network) :
 ew::DataflowNotifier(i_network),
 dbg_last_item_n(0),
 network(i_network),
 dbg_n(next_dbg_n()),
 dbg(ClassName, ew::String::ssprintf("%d", dbg_n)),
 use_depth(true),
 currently_rendered(false),
 idle_pending(false),
 redraw_required(false),
 destruction_pending(false),
 dataflow_check_cycle(0),
 clip_ratio(1.0),
 highlight_item(0),
 view_mapping(ew::Transform3::identity_transform)
{
  background_color[0] = 0;
  background_color[1] = 0;
  background_color[2] = 0;
  highlight_color[0] = 255;
  highlight_color[1] = 0;
  highlight_color[2] = 0;
  if (dbg.on) {
    dbg.dprintf("%s::%s(...) {", dbg.fn, "CTOR");
    dbg.dprintf("%s::%s }", dbg.fn, "CTOR");
  }
}

// The derived class is responsible for unscheduling any scheduled rendering.

/// Destroy the widget and all its items.
/// The OpenGL context should be active when this is called.

ew::View3Widget::~View3Widget()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "DTOR");
  while (items.size() != 0) {
    delete items[items.size() - 1];
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "DTOR");
}

int
ew::View3Widget::next_dbg_n()
{
  if (network->dbg_last_widget_n == INT_MAX) {
// unlikely
// tolerate but indicate overflow
    return 0;
  }
  network->dbg_last_widget_n += 1;
  return network->dbg_last_widget_n;
}

/// Delete all items in the widget and reset it to its initial
/// state.

void
ew::View3Widget::reset()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "reset");
  while (items.size() != 0) {
    delete items[items.size() - 1];
  }
  clip_ratio = 1.0;
  view_mapping.set_to_identity();
  background_color[0] = 0;
  background_color[1] = 0;
  background_color[2] = 0;
  highlight_color[0] = 255;
  highlight_color[1] = 0;
  highlight_color[2] = 0;
  redraw_later();
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "reset");
}

/// Perform initialization related to the OpenGL context.
/// This should be called after construction, once the context is active.

void
ew::View3Widget::init_gl_context()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "init_gl_context");
  if (glGetError() != 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  GLboolean rb;
  glGetBooleanv(GL_DOUBLEBUFFER, &rb);
  has_double_buffer = (rb != GL_FALSE);
  GLint ri;
  glGetIntegerv(GL_DEPTH_BITS, &ri);
  if (ri <= 0) {
    throw ew::ErrorRuntime(
     "A program requirement has not been satisfied"
     " (the presence of an OpenGL depth buffer).");
  }
  glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glHint(GL_FOG_HINT, GL_FASTEST);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glDepthFunc(GL_LEQUAL);
//XXX old  glEnable(GL_DEPTH_TEST);
  int err = glGetError();
  if (err != 0) {
    if (err == GL_OUT_OF_MEMORY) {
      throw std::bad_alloc();
    } else {
      dbg.on && dbg.dprintf("%s::%s glError=0x%x", dbg.fn, "init_gl_context",
       err);
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "init_gl_context");
}

/// Performs initialization related to the window size.
/// This should be called after construction and whenever the window is
/// resized.
/// @param w, h the new window width and height

void
ew::View3Widget::set_window_size(int w, int h)
{
  dbg.on && dbg.dprintf("%s::%s(%d,%d) {}", dbg.fn, "set_window_size", w, h);
  if (w < 1) {
    w = 1;
  }
  if (h < 1) {
    h = 1;
  }
  winw = w;
  winh = h;
//XXX int min = (w < h ? w : h);
  int min = std::min(w, h);
  for (int i = 0; i < 15; i += 1) {
    projmatrix[i] = 0.0;
  }
  projmatrix[0] = min * 1.0 / w;
  projmatrix[5] = min * 1.0 / h;
  projmatrix[15] = 1.0;
  window_mapping.set_to_identity();
  window_mapping.translate[0] = w / 2.0;
  window_mapping.translate[1] = h / 2.0;
  window_mapping.scale = min / 2.0;
  window_mapping.orthog[1][1] = -1.0;
  window_mapping.orthog[2][2] = -1.0;
}

void
ew::View3Widget::dataflow_check()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "dataflow_check");
  for (int n = 0; n < VectorSize(items); n += 1) {
    ew::View3Item *it = items[n];
    if (it->get_state()) {
      bool b = it->check_dataflow();
      if (b) {
        redraw_required = true;
      }
      dbg.on && dbg.dprintf("%s::%s  item=%s changed=%s", dbg.fn,
       "dataflow_check", it->dbg.in, ew::Debug::to_str(b));
    }
  }
  dataflow_check_cycle = network->get_cycle();
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "dataflow_check");
}

//XXX later implement a back buffer selection so can get per-pixel picking

//XXX takes longer than rendering? maybe because can't ignore stuff behind
//XXX change to a mask or range on cmpt, so can pick lmks, relax, etc)
//XXX DEPRECATED

/// Pick an item fragment rendered in a sub-window of the current view.
/// Fragments of the lowest dimension visible are picked.
/// With a depth buffer, the nearest eligible item fragment of this dimensions
/// is picked, provided it is not behind, with a tolerance, any surface
/// fragment.
/// This is to allow picking of, for example, points in an inclined surface
/// where otherwise, some part of the surface would always be closer than the
/// point.
/// Without a depth buffer, the latest eligible item fragment of this dimension
/// in the rendering order is picked, provided it is not before any surface
/// fragment.
/// The meaning of an item component depends on the item.
/// The @a pick_z result is in pixels, with 0 at the center of the viewing
/// volume, and positive further away.
/// It is suitable for use in #get_pointer_location.
/// @param x, y the pixel window pointer coordinates
/// @param sz the side of the square, in pixels, centered on @a x and @a y
/// within which rendered items are eligible for picking.
/// @param burrow the tolerance in the depth buffer, for an eligible item
/// to be picked behind a surface.
/// @param[in] constrain_it if non-zero, only fragments rendered by this item
/// will be eligible to be picked
/// @param constrain_cmpt if non-negative, only fragments rendered as part of
/// this component in the item will be eligible for picking
/// @param[in] constrain_dim if non-negative, only fragments of this dimension
/// (0, 1 or 2) will be eligible for picking
/// @param[out] pick_it the location where a pointer to the picked item should
/// be stored, if non-zero and if the pick was successful
/// @param[out] pick_cmpt the location where the component of the picked item
/// should be stored, if non-zero and if the pick was successful
/// @param[out] pick_dim the location where the dimension of the picked item
/// should be stored, if non-zero and if the pick was successful
/// @param[out] pick_z the location where the z coordinate of the nearest part
/// of the picked item should be stored, if non-zero and if the pick was
/// successful


bool ew::View3Widget::pick(double x, double y, double sz, double burrow,
 ew::View3Item *constrain_it, int constrain_cmpt, int constrain_dim,
 ew::View3Item **pick_it, int *pick_cmpt, int *pick_dim, double *pick_z)
{
  // THE ORIGINAL EW PICKING CODE - based on OpenGL's select buffers

  if (dbg.on) {
    const char *constrain_it_str;
    if (constrain_it == 0) {
      constrain_it_str = "NULL";
    } else {
      constrain_it_str = constrain_it->dbg.in;
    }
    dbg.dprintf("%s::%s(%g,%g,%g,%g,%s,%d,%d) {", dbg.fn, "pick",
     x, y, sz, burrow, constrain_it_str, constrain_cmpt, constrain_dim);
  }
  if (dataflow_check_cycle < network->get_cycle()) {
    dataflow_check();
  }
  if (glGetError() != 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  int bufn = 4096;
  ew::AutoArray<GLuint> buf(bufn);
  int hits;
// Extremely unlikely we'll need a larger buffer.  More likely if we change to
// feedback mode.
  while (1) {
    glSelectBuffer(bufn, &buf[0]);
    glRenderMode(GL_SELECT);
    glViewport(0, 0, winw, winh);
    double cr = clip_ratio;
    if (cr < 0.001) {
      cr = 0.001;
    }
    if (cr > 1000.0) {
      cr = 1000.0;
    }
// sz is in pixels.
    int min = std::min(winw, winh);
    double pixsz = 2.0 / min;
    double projmatrixpick[16];
    for (int i = 0; i < 15; i += 1) {
      projmatrixpick[i] = 0.0;
    }
    projmatrixpick[0] = 2.0 / (sz * pixsz);
    projmatrixpick[5] = 2.0 / (sz * pixsz);
    projmatrixpick[10] = -1.0 / cr;
    projmatrixpick[12] = -projmatrixpick[0] * (x - winw / 2.0) * pixsz;
    projmatrixpick[13] = projmatrixpick[5] * (y - winh / 2.0) * pixsz;
    projmatrixpick[15] = 1.0;
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(projmatrixpick);
    double mm[16];
    view_mapping.get_matrix_gl(mm);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(mm);
    for (int n = 0; n < VectorSize(items); n += 1) {
      ew::View3Item *it = items[n];
      if (it->get_state() && !it->get_prepared()) {
        dbg.on && dbg.dprintf("%s::%s   prepare(%s)", dbg.fn, "pick",
         it->dbg.in);
        it->prepare();
      }
    }
    glInitNames();
    glPushName(0);
    for (unsigned int u = 0; u < items.size(); u += 1) {
      ew::View3Item *it = items[u];
      if (it->get_state()) {
        dbg.on && dbg.dprintf("%s::%s   render(%s)", dbg.fn, "pick",
         it->dbg.in);
        glLoadName(u);
        glPushName(0);
        it->render();
        glPopName();
      }
    }
    glPopName();
    
    //husky - maybe picking-related bugs are due to incorrect CPU/GPU sync
    glFlush();
    glFinish();
    
    hits = glRenderMode(GL_RENDER);
    if (hits >= 0) {
      break;
    }
    bufn *= 2;
    dbg.on && dbg.dprintf("%s::%s   doubling-bufffer", dbg.fn, "pick");
    if (bufn < 0) {
// nearly impossible
      throw ew::ErrorRuntime(
       "A program limitation has been exceeded"
       " (the size of the OpenGL selection buffer).");
    }
    buf.resize(bufn);
  }
// This is the item index, not pointer.
  int pi = -1;
  int pc;
  int pd;
// This is in window (pixel) coords.
  double pz;
  bool hassurz = false;
  double surz;
  for (int i = 0; i < hits; i += 1) {
// hit buffer:
//  5i->2, 5i+1->near, 5i+2->far, 5i+3->item name, 5i+4-> cmpt name
    int dim = (buf[5 * i + 4] & 3);
    int it = buf[5 * i + 3];
    double z = -(1.0 - buf[5 * i + 1] * 2.0 / 0xffffffffU) *
     window_mapping.scale;
    int cmpt = (buf[5 * i + 4] >> 2);
    dbg.on && dbg.dprintf(
     "%s::%s   pick-hit=%d z=%g dim=%d it=%d cmpt=%d zfar=%g", dbg.fn, "pick",
     buf[5 * i], z, dim, it, cmpt,
     -(1.0 - buf[5 * i + 2] * 2.0 / 0xffffffffU) * window_mapping.scale);
    if (dim == 2) {
// surfaces obscure even if not eligible
      if (pi >= 0 && (!use_depth || z < pz - burrow)) {
        dbg.on && dbg.dprintf("%s::%s   obscures", dbg.fn, "pick");
        pi = -1;
      }
      if (use_depth) {
        if (hassurz) {
          if (z < surz) {
            surz = z;
          }
        } else {
          surz = z;
        }
        hassurz = true;
      }
    }
    if (constrain_it != 0 && constrain_it->get_index() != it) {
      continue;
    }
    if (constrain_dim >= 0 && constrain_dim != dim) {
      continue;
    }
    if (constrain_cmpt >= 0 && cmpt != constrain_cmpt) {
      continue;
    }
    if (pi >= 0 && dim > pd) {
      continue;
    }
    if (pi >= 0 && dim == pd) {
      if (use_depth && z > pz) {
        continue;
      }
    }
    if (dim < 2 && hassurz) {
      dbg.on && dbg.dprintf("%s::%s   XXX surz=%g z=%g burrow=%g", dbg.fn,
       "pick", surz, z, burrow);
      if (surz < z - burrow) {
        continue;
      }
    }
    dbg.on && dbg.dprintf("%s::%s  picked", dbg.fn, "pick");
    pi = it;
    pc = cmpt;
    pd = dim;
    pz = z;
  }
  if (pick_it && pi >= 0) {
    *pick_it = items[pi];
  }
  if (pick_cmpt && pi >= 0) {
    *pick_cmpt = pc;
  }
  if (pick_dim && pi >= 0) {
    *pick_dim = pd;
  }
  if (pick_z && pi >= 0) {
    *pick_z = pz;
  }
  int err = glGetError();
  if (err != 0) {
    if (err == GL_OUT_OF_MEMORY) {
      throw std::bad_alloc();
    } else {
      dbg.on && dbg.dprintf("%s::%s glError=0x%x", dbg.fn, "pick", err);
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
  }
  dbg.on && dbg.dprintf("%s::%s } (%s)", dbg.fn, "pick",
   ew::Debug::to_str(pi >= 0));

  return true; //!?
}

bool
ew::View3Widget::MODIFIED_TEST_pick(double x, double y, double sz, double burrow,
 ew::View3Item *constrain_it, int constrain_cmpt, int constrain_dim,
 ew::View3Item **pick_it, int *pick_cmpt, int *pick_dim, double *pick_z)
{
  // THE MODIFIED EW PICKING CODE - based on reading the depth buffer

  glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
  glClear(GL_DEPTH_BUFFER_BIT);

  double cr = clip_ratio;
  if (cr < 0.001) {
    cr = 0.001;
  }
  if (cr > 1000.0) {
    cr = 1000.0;
  }
// sz is in pixels.
  int min = std::min(winw, winh);
  double pixsz = 2.0 / min;
  double projmatrixpick[16];
  for (int i = 0; i < 15; i += 1) {
    projmatrixpick[i] = 0.0;
  }
  projmatrixpick[0] = 2.0 / (sz * pixsz);
  projmatrixpick[5] = 2.0 / (sz * pixsz);
  projmatrixpick[10] = -1.0 / cr;
  projmatrixpick[12] = -projmatrixpick[0] * (x - winw / 2.0) * pixsz;
  projmatrixpick[13] = projmatrixpick[5] * (y - winh / 2.0) * pixsz;
  projmatrixpick[15] = 1.0;
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(projmatrixpick);
  double mm[16];
  view_mapping.get_matrix_gl(mm);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(mm);
  for (int n = 0; n < VectorSize(items); n += 1) {
    ew::View3Item *it = items[n];
    if (it->get_state() && !it->get_prepared()) {
      dbg.on && dbg.dprintf("%s::%s   prepare(%s)", dbg.fn, "pick",
       it->dbg.in);
      it->prepare();
      it->render();
    }
  }

  GLfloat depth;
  glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

  glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
  *pick_z = depth;

  return true;
}

/// Find all item fragment picks, in order of most prominent first.
/// @param[out] outp a reference to the vector to store the output list in
/// @param x, y the pixel window coordinates
/// @param sz the side of the square, in pixels, centered on @a x and @a y
/// within which rendered items are eligible for picking

void
ew::View3Widget::pickv(std::vector<ew::View3Pick> &outp, double x, double y,
 double sz)
{
  dbg.on && dbg.dprintf("%s::%s(...,%g,%g,%g) {", dbg.fn, "pickv", x, y, sz);
  if (dataflow_check_cycle < network->get_cycle()) {
    dataflow_check();
  }
  if (glGetError() != 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  int bufn = 4096;
  ew::AutoArray<GLuint> buf(bufn);
  int hits;
// Extremely unlikely we'll need a larger buffer.  More likely if we change to
// feedback mode.
  while (1) {
    glSelectBuffer(bufn, &buf[0]);
    glRenderMode(GL_SELECT);
    glViewport(0, 0, winw, winh);
    double cr = clip_ratio;
    if (cr < 0.001) {
      cr = 0.001;
    }
    if (cr > 1000.0) {
      cr = 1000.0;
    }
// sz is in pixels.
    int min = std::min(winw, winh);
    double pixsz = 2.0 / min;
    double projmatrixpick[16];
    for (int i = 0; i < 15; i += 1) {
      projmatrixpick[i] = 0.0;
    }
    projmatrixpick[0] = 2.0 / (sz * pixsz);
    projmatrixpick[5] = 2.0 / (sz * pixsz);
    projmatrixpick[10] = -1.0 / cr;
    projmatrixpick[12] = -projmatrixpick[0] * (x - winw / 2.0) * pixsz;
    projmatrixpick[13] = projmatrixpick[5] * (y - winh / 2.0) * pixsz;
    projmatrixpick[15] = 1.0;
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(projmatrixpick);
    double mm[16];
    view_mapping.get_matrix_gl(mm);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(mm);
    for (int n = 0; n < VectorSize(items); n += 1) {
      ew::View3Item *it = items[n];
      if (it->get_state() && !it->get_prepared()) {
        dbg.on && dbg.dprintf("%s::%s   prepare(%s)", dbg.fn, "pickv",
         it->dbg.in);
        it->prepare();
      }
    }
    glInitNames();
    glPushName(0);
    for (unsigned int u = 0; u < items.size(); u += 1) {
      ew::View3Item *it = items[u];
      if (it->get_state()) {
        dbg.on && dbg.dprintf("%s::%s   render(%s)", dbg.fn, "pickv",
         it->dbg.in);
        glLoadName(u);
        glPushName(0);
        it->render();
        glPopName();
      }
    }
    glPopName();
    hits = glRenderMode(GL_RENDER);
    if (hits >= 0) {
      break;
    }
// unlikely
    bufn *= 2;
    dbg.on && dbg.dprintf("%s::%s   doubling-buffer", dbg.fn, "pickv");
    if (bufn < 0) {
// nearly impossible
      throw ew::ErrorRuntime(
       "A program limitation has been exceeded"
       " (the size of the OpenGL selection buffer).");
    }
    buf.resize(bufn);
  }
  int err = glGetError();
  if (err != 0) {
    if (err == GL_OUT_OF_MEMORY) {
      throw std::bad_alloc();
    } else {
      dbg.on && dbg.dprintf("%s::%s   glError=0x%x", dbg.fn, "pickv", err);
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
  }
  outp.resize(hits);
  for (int i = 0; i < hits; i += 1) {
// hit buffer:
//  5i->2, 5i+1->near, 5i+2->far, 5i+3->item name, 5i+4-> cmpt name
    if (buf[5 * i] != 2) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    outp[hits - 1 - i].item = items[buf[5 * i + 3]];
    outp[hits - 1 - i].item_component = (buf[5 * i + 4] >> 2);
    outp[hits - 1 - i].dim = (buf[5 * i + 4] & 3);
    if (use_depth) {
      outp[hits - 1 - i].z_near = -(1.0 - buf[5 * i + 1] * 2.0 / 0xffffffffU) *
       window_mapping.scale;
      outp[hits - 1 - i].z_far = -(1.0 - buf[5 * i + 2] * 2.0 / 0xffffffffU) *
       window_mapping.scale;
    } else {
      outp[hits - 1 - i].z_near = outp[hits - 1 - i].z_far = 0.0;
    }
    outp[hits - 1 - i].hit = i;
  }
  if (use_depth) {
    sort(outp.begin(), outp.end());
  }
  for (int i = 0; i < hits; i += 1) {
    dbg.on && dbg.dprintf(
     "%s::%s   hit=%d item=%s cmpt=%d dim=%d znear=%g zfar=%g",
     dbg.fn, "pickv", outp[i].hit, outp[i].item->dbg.fn,
     outp[i].item_component, outp[i].dim, outp[i].z_near, outp[i].z_far);
  }
  dbg.on && dbg.dprintf("%s::%s } (%d)", dbg.fn, "pickv", hits);
}

/// Render the scene in the current OpenGL context and associated window.
/// If the context has a double buffer, this will render to the back buffer.
/// The buffers must then be swapped.

void
ew::View3Widget::render()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "render");
  ew::DataflowNotifier::notifier_register();
  currently_rendered = true;
  if (dataflow_check_cycle < network->get_cycle()) {
    dataflow_check();
  }
  if (glGetError() != 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  glViewport(0, 0, winw, winh);
  glMatrixMode(GL_PROJECTION);
  double cr = clip_ratio;
  if (cr < 0.001) {
    cr = 0.001;
  }
  if (cr > 1000.0) {
    cr = 1000.0;
  }
  projmatrix[10] = -1.0 / cr;
  glLoadMatrixd(projmatrix);
  glMatrixMode(GL_MODELVIEW);
  double mm[16];
  view_mapping.get_matrix_gl(mm);
  glLoadMatrixd(mm);
  if (has_double_buffer) {
    glDrawBuffer(GL_BACK);
  } else {
    glDrawBuffer(GL_FRONT);
  }
  glClearColor(background_color[0] / 255.0, background_color[1] / 255.0,
   background_color[2] / 255.0, 0.0);
  for (int n = 0; n < VectorSize(items); n += 1) {
    ew::View3Item *it = items[n];
    if (it->get_state() && !it->get_prepared()) {
      dbg.on && dbg.dprintf("%s::%s   prepare(%s)", dbg.fn, "render",
       it->dbg.in);
      it->prepare();
    }
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (use_depth) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
  int err = glGetError();
  if (err != 0) {
    if (err == GL_OUT_OF_MEMORY) {
      throw std::bad_alloc();
    } else {
      dbg.on && dbg.dprintf("%s::%s glError=0x%x", dbg.fn, "render", err);
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
  }
  for (int n = 0; n < VectorSize(items); n += 1) {
    ew::View3Item *it = items[n];
    if (it->get_state()) {
      dbg.on && dbg.dprintf("%s::%s   render(%s)", dbg.fn, "render",
       it->dbg.in);
      it->render();
    }
  }
  err = glGetError();
  if (err != 0) {
    if (err == GL_OUT_OF_MEMORY) {
      throw std::bad_alloc();
    } else {
      dbg.on && dbg.dprintf("%s::%s glError=0x%x", dbg.fn, "render", err);
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "render");
}

/// #schedule_idle_handler_cb should arrange for this function to be called at
/// an appropriate time.

void
ew::View3Widget::idle_handler()
{
  dbg.on && dbg.dprintf("%s::%s", dbg.fn, "idle_handler");
  idle_pending = false;
  if (!redraw_required && dataflow_check_cycle < network->get_cycle()) {
    dataflow_check();
  }
  if (redraw_required) {
    redraw_cb();
  } else {
    ew::DataflowNotifier::notifier_register();
  }
}

// This is the implementation of callback
// ew::DataflowNotifier::notifier_handle_cb.

void
ew::View3Widget::notifier_handle_cb()
{
  dbg.on && dbg.dprintf("%s::%s", dbg.fn, "notifier_handle_cb");
  if (currently_rendered && !idle_pending) {
    schedule_idle_handler_cb();
    redraw_required = false;
    idle_pending = true;
  }
}

/// Set the clip ratio.
/// The viewing volume width and height are determined by the window dimensions
/// as specified in #set_window_size.
/// The clipping ratio is the ratio of the viewing volume depth to the smaller
/// of its width and height.
/// The viewing volume coordinate system is centered in the viewing volume,
/// with axes, in order, pointing right, up and out of the screen, and with
/// homogeneous scale such that the range of coordinates in the direction of
/// the smaller of the width and height is [-1.0, 1.0].
/// The clip ratio is initially 1.0.
/// It must be positive, but also not unreasonably small or large values.
/// @param cr the new clipping ratio

void
ew::View3Widget::set_clip_ratio(double cr)
{
  if (clip_ratio != cr) {
    clip_ratio = cr;
    redraw_later();
  }
}

/// Set the background color.
/// The background is by default [0, 0, 0].
/// @param[in] rgb the address of an array of size 3 containing the new
/// background colour components

void
ew::View3Widget::set_background_color(const unsigned char *rgb)
{
  if (background_color[0] != rgb[0] ||
   background_color[1] != rgb[1] ||
   background_color[2] != rgb[2]) {
    background_color[0] = rgb[0];
    background_color[1] = rgb[1];
    background_color[2] = rgb[2];
    redraw_later();
  }
}

/// Set the highlight color.
/// The highlight colour is by default [255, 0, 0].
/// @param[in] rgb the address of an array of size 3 containing the new
/// highlight colour components

void
ew::View3Widget::set_highlight_color(const unsigned char *rgb)
{
  if (highlight_color[0] != rgb[0] ||
   highlight_color[1] != rgb[1] ||
   highlight_color[2] != rgb[2]) {
    highlight_color[0] = rgb[0];
    highlight_color[1] = rgb[1];
    highlight_color[2] = rgb[2];
    redraw_later();
  }
}

/// Clear the current highlighted item, if
/// any.

void
ew::View3Widget::clear_highlight()
{
  if (highlight_item != 0) {
    highlight_item->clear_highlight();
    highlight_item = 0;
  }
}

// This is called by the item when it set its own highlight.

void
ew::View3Widget::set_highlight_item(ew::View3Item *it)
{
  if (highlight_item == it) {
    return;
  }
  if (highlight_item != 0) {
    highlight_item->clear_highlight();
    highlight_item = 0;
  }
  highlight_item = it;
}

/// Set the view mapping.
/// The view mapping is the transformation from model coordinates to viewing
/// volume coordinates.
/// @param[in] tr the address of a transform containing the new view mapping

void
ew::View3Widget::set_view_mapping(const ew::Transform3 *tr)
{
  dbg.on && dbg.dprintf("%s::%s", dbg.fn, "set_view_mapping");
  view_mapping = *tr;
  redraw_later();
}

/// Composes the view mapping with a transform on the left.
/// This is for incremental changes to the view mapping relative to the viewing
/// volume coordinate system.
/// @param[in] tr the address of a transform containing the view mapping
/// increment

void
ew::View3Widget::view_post_compose(const ew::Transform3 *tr)
{
  dbg.on && dbg.dprintf("%s::%s", dbg.fn, "view_post_compose");
  view_mapping.set_to_composition(tr, &view_mapping);
  redraw_later();
}

/// Calculate the bounding box of all items displayed in the widget.
/// @param[out] b where to store the result

void
ew::View3Widget::get_bbox(ew::Bbox3 *b) const
{
  *b = ew::Bbox3::empty_box;
  for (int n = 0; n < VectorSize(items); n += 1) {
    ew::View3Item *it = items[n];
    b->set_to_union(b, it->get_bbox());
  }
}

/// Calculate the location of the point with given pixel window coordinates.
/// @param[out] loc The address of an array of size 3 to store the location in
/// @param x, y, z  the pixel window coordinates (right from left side, down
/// from top and into window from the midplane of the viewing volume)

void
ew::View3Widget::get_pointer_location(double *loc, double x, double y,
 double z) const
{
  ew::Transform3 tr;
  tr.set_to_inverse(&window_mapping);
  loc[0] = x;
  loc[1] = y;
  loc[2] = z;
  tr.apply(loc, loc);
  tr.set_to_inverse(&view_mapping);
  tr.apply(loc, loc);
}

/// Calculate the location of center of the space currently in view.
/// @param[out] loc the address of an array of size 3 to store the location in

void
ew::View3Widget::get_center_location(double *loc) const
{
  loc[0] = 0.0;
  loc[1] = 0.0;
  loc[2] = 0.0;
  ew::Transform3 tr;
  tr.set_to_inverse(&view_mapping);
  tr.apply(loc, loc);
}

/// Construct a translation suitable for #view_post_compose from window pixel
/// coordinate changes.
/// @param[out] tr where to store the view mapping increment
/// @param x, y, z the coordinate changes

void
ew::View3Widget::get_pointer_translate_tr(ew::Transform3 *tr,
 double x, double y, double z) const
{
  tr->set_to_identity();
  tr->translate[0] = x / window_mapping.scale;
  tr->translate[1] = -y / window_mapping.scale;
  tr->translate[2] = -z / window_mapping.scale;
}

/// Constructs a roll-ball type rotation suitable for #view_post_compose
/// from window pixel coordinate changes.
/// @param[out] tr where to store the view mapping increment
/// @param x, y the coordinate changes

void
ew::View3Widget::get_pointer_roll_tr(ew::Transform3 *tr, double x,
 double y) const
{
  tr->set_to_identity();
  double r = std::sqrt(x * x + y * y);
  if ((x == 0.0 && y == 0.0) || r == 0.0) {
    return;
  }
  double csa = x / r;
  double sna = -y / r;
  double csr = std::cos(r / window_mapping.scale);
  double snr = std::sin(r / window_mapping.scale);
  tr->orthog[0][0] = sna * sna + csa * csa * csr;
  tr->orthog[0][1] = sna * csa * (csr - 1);
  tr->orthog[0][2] = csa * snr;
  tr->orthog[1][0] = sna * csa * (csr - 1);
  tr->orthog[1][1] = csa * csa + sna * sna * csr;
  tr->orthog[1][2] = sna * snr;
  tr->orthog[2][0] = -snr * csa;
  tr->orthog[2][1] = -snr * sna;
  tr->orthog[2][2] = csr;
}

/// Constructs a twist rotation, about the center of the window, suitable
/// for #view_post_compose from window pixel coordinate changes.
/// @param[out] tr where to store the view mapping increment
/// @param x0, y0 the coordinate initial values
/// @param x1, y1 the coordinate final values
/// @param acc an acceleration factor that scales the rotation relative to the
/// rotation indicated by the coordinate changes

void
ew::View3Widget::get_pointer_twist_tr(ew::Transform3 *tr,
 double x0, double y0, double x1, double y1, double acc) const
{
  tr->set_to_identity();
  double dx = (x1 - x0) * acc + x0 - window_mapping.translate[0];
  double dy = (y1 - y0) * acc + y0 - window_mapping.translate[1];
  double ex = x0 - window_mapping.translate[0];
  double ey = y0 - window_mapping.translate[1];
  double dr = std::sqrt(dx * dx + dy * dy);
  double er = std::sqrt(ex * ex + ey * ey);
  if (dr < 0.5 || er < 0.5) {
    return;
  }
  double cs = (dx * ex + dy * ey) / (dr * er);
  double sn = (dx * ey - ex * dy) / (dr * er);
  tr->orthog[0][0] = cs;
  tr->orthog[0][1] = -sn;
  tr->orthog[1][0] = sn;
  tr->orthog[1][1] = cs;
}

/// Constructs a scaling about the center of the window suitable for
/// #view_post_compose from window pixel coordinate changes.
/// @param[out] tr where to store the view mapping increment
/// @param x0, y0 the coordinate initial values
/// @param x1, y1 the coordinate final values
/// @param acc an acceleration factor that scales the scale relative to the
/// scale indicated by the coordinate changes

void
ew::View3Widget::get_pointer_scale_tr(ew::Transform3 *tr,
 double x0, double y0, double x1, double y1, double acc) const
{
  tr->set_to_identity();
  double dx = (x1 - x0) * acc + x0 - window_mapping.translate[0];
  double dy = (y1 - y0) * acc + y0 - window_mapping.translate[1];
  double ex = x0 - window_mapping.translate[0];
  double ey = y0 - window_mapping.translate[1];
  double dr = std::sqrt(dx * dx + dy * dy);
  double er = std::sqrt(ex * ex + ey * ey);
  if (dr < 0.5 || er < 0.5) {
    return;
  }
  double sc = dr / er;
  tr->scale = sc;
}

/// Construct a translation suitable for #view_post_compose that would put the
/// specified point (in model space, not viewing space) at the center of the
/// view.
/// @param[out] tr where to store the view mapping increment
/// @param[in] loc the address of an array of size 3 containing the point
/// coordinates
/// @param do_z whether to translate in the z direction so that the point is at
/// the center of the view in all 3 directions

void
ew::View3Widget::get_jump_tr(ew::Transform3 *tr, const double *loc,
 bool do_z) const
{
  double loc_v[3];
  view_mapping.apply(loc_v, loc);
  tr->set_to_identity();
  tr->translate[0] = -loc_v[0];
  tr->translate[1] = -loc_v[1];
  if (do_z) {
    tr->translate[2] = -loc_v[2];
  }
}

/// Constructs a translation suitable for #view_post_compose that would put the
/// specified bounding box (in model space, not viewing space) at the center of
/// the view.
/// The orientation of the view is not effected.
/// This will return @c false and not otherwise do anything if the bounding box
/// is empty, or if is a single point and @a do_scale is @c true.
/// @param[out] tr where to store the view mapping increment, if successful
/// @param[in] bbox the address of the bounding box
/// @param do_z whether to translate in the z direction
/// @param do_scale whether to scale so that the box fits comfortably in the
/// view
/// @return @c true if successful

bool
ew::View3Widget::get_bbox_tr(ew::Transform3 *tr, const ew::Bbox3 *bbox,
 bool do_z, bool do_scale) const
{
  double loc[3], loc_v[3];
  if (bbox->get_empty()) {
    return false;
  }
  double r = bbox->get_radius_center(loc);
  if (r == 0.0 && do_scale) {
    return false;
  }
  view_mapping.apply(loc_v, loc);
  tr->set_to_identity();
  tr->translate[0] = -loc_v[0];
  tr->translate[1] = -loc_v[1];
  if (do_z) {
    tr->translate[2] = -loc_v[2];
  }
  if (do_scale) {
    r *= view_mapping.scale;
    tr->scale = 1.0 / r;
    tr->translate[0] /= r;
    tr->translate[1] /= r;
    tr->translate[2] /= r;
  }
  return true;
}

/// Turn on or off the use of the depth buffer.
/// It is initially @c true.
/// @param d whether to use the depth buffer

void
ew::View3Widget::set_use_depth(bool d)
{
  if (use_depth != d) {
    use_depth = d;
    redraw_later();
  }
}

/// This should be called when the window associated with this widget is
/// unmapped, to avoid unnecessary activity.
/// A call to #render undoes the effect of this.

void
ew::View3Widget::set_currently_unrendered()
{
  currently_rendered = false;
}

/// This should be called when a derived object is being destroyed, to prevent
/// any callbacks being invoked.

void
ew::View3Widget::destruction_in_progress()
{
  destruction_pending = true;
  if (ew::DataflowNotifier::notifier_is_registered()) {
    ew::DataflowNotifier::notifier_unregister();
  }
}

void
ew::View3Widget::add_item(ew::View3Item *it)
{
  items.push_back(it);
  it->set_index(items.size() - 1);
}

void
ew::View3Widget::del_item(ew::View3Item *it)
{
  if (highlight_item == it) {
    highlight_item = 0;
  }
  VectorErase(items, it->get_index());
  it->set_index(-1);
//XXX only if necessary, i.e not last
  for (int i = 0; i < VectorSize(items); i += 1) {
    items[i]->set_index(i);
  }
}

/// Move an item in the rendering order.
/// @param[in] it a pointer to the item to move
/// @param n the position in the order to move the item to

void
ew::View3Widget::move_item(ew::View3Item *it, int n)
{
  if (it->view != this) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  if (n >= VectorSize(items) || n < 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  } else if (it->get_index() == n) {
    return;
  } else {
    VectorErase(items, it->get_index());
    if (n == VectorSize(items)) {
      items.push_back(it);
    } else {
      VectorInsert(items, n, it);
    }
//XXX just set the 2 changed
    for (int i = 0; i < VectorSize(items); i += 1) {
      items[i]->set_index(i);
    }
    if (it->get_state()) {
      redraw_later();
    }
  }
}
