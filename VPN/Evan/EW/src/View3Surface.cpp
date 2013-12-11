// wdkg 2008-2010

#include <cmath>
#include <stdexcept>
#include <vector>
#include <GL/gl.h>
#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowSurface3.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/ErrorLogic.h"
#include "ew/ErrorRuntime.h"
#include "ew/View3Item.h"
#include "ew/View3Surface.h"
#include "ew/View3Widget.h"

//XXX try accellerating large surfaces:
//  1. split into subsurfaces
//  2. try to express as a triangle strip

namespace {
  const char ClassName[] = "ew::View3Surface";
  const char InstanceName[] = "surface";
}

const char *const ew::View3Surface::mode_names[] = {
  "smooth",
  "silhouette",
  "wireframe",
  0
};

/// This creates an item.
/// @param v
///   The view widget the item should belong to.
/// @param type_i
///   The initializer for ew::View3Item::type.

ew::View3Surface::View3Surface(ew::View3Widget *v, int i_type) :
 ew::View3Item(v, i_type, InstanceName),
 dbg(ClassName, ew::View3Item::dbg),
 surface(0),
 surface_checked_version(0),
 render_mode(MODE_SMOOTH),
 wireframe_linewidth(1.0),
 sur_dlist(0),
 dlist(0)
{
  front_color[0] = front_color[1] = front_color[2] = 128;
  back_color[0] = back_color[1] = back_color[2] = 128;
}

// The gl context must be active when the surface item is destroyed, unless
// care is taken to unset the surface beforehand.
//XXX add a callback to activate context

ew::View3Surface::~View3Surface()
{
  if (surface != 0) {
    if (sur_dlist) {
      surface->dlist_refs -= 1;
      if (surface->dlist_refs == 0) {
        surface->dlist = 0;
        surface->dlist_cycle = 0;
        glDeleteLists(sur_dlist, 1);
      }
    }
    surface->ew::DataflowNode::decr_ref_count();
  }
  if (dlist) {
    glDeleteLists(dlist, 1);
  }
}

/// @param s
///   A pointer to the new surface node to display, or 0.

void
ew::View3Surface::set_surface(const ew::DataflowSurface3 *s)
{
  dbg.on && dbg.dprintf("%s::%s(...) {", dbg.fn, "set_surface");
  if (s == surface) {
    return;
  }
  dbg.on && dbg.dprintf("%s::%s %s->%s", dbg.fn, "set_surface",
   ew::DataflowNode::to_str(surface), ew::DataflowNode::to_str(s));
  if (s != 0) {
    if (s->network != network) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    s->ew::DataflowNode::incr_ref_count();
  }
  if (surface != 0) {
    if (sur_dlist) {
      surface->dlist_refs -= 1;
      if (surface->dlist_refs == 0) {
        surface->dlist = 0;
        surface->dlist_cycle = 0;
        glDeleteLists(sur_dlist, 1);
      }
      sur_dlist = 0;
    }
    surface->ew::DataflowNode::decr_ref_count();
  }
  surface = s;
  surface_checked_version = 0;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "set_surface");
}

const ew::Bbox3 *
ew::View3Surface::get_bbox()
{
  if (surface != 0) {
    return surface->get_bbox();
  } else {
    return &ew::Bbox3::empty_box;
  }
}

void
ew::View3Surface::set_render_mode(int m)
{
  if (render_mode == m) {
    return;
  }
  if (m < 0 || m >= N_MODES) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  render_mode = m;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

/// @param[in] rgb
///   This points to an array of size 3 containing the new colour for the front
///   of the surface.

void
ew::View3Surface::set_front_color(const unsigned char *rgb)
{
  if (front_color[0] == rgb[0] &&
   front_color[1] == rgb[1] &&
   front_color[2] == rgb[2]) {
    return;
  }
  front_color[0] = rgb[0];
  front_color[1] = rgb[1];
  front_color[2] = rgb[2];
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

/// @param[in] rgb
///   This points to an array of size 3 containing the new colour for the back
///   of the surface.

void
ew::View3Surface::set_back_color(const unsigned char *rgb)
{
  if (back_color[0] == rgb[0] &&
   back_color[1] == rgb[1] &&
   back_color[2] == rgb[2]) {
    return;
  }
  back_color[0] = rgb[0];
  back_color[1] = rgb[1];
  back_color[2] = rgb[2];
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

void
ew::View3Surface::set_wireframe_linewidth(double w)
{
  if (wireframe_linewidth == w) {
    return;
  }
  if (w <= 0.0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  wireframe_linewidth = w;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

bool
ew::View3Surface::check_dataflow()
{
  if (surface != 0) {
    unsigned long v = surface->ew::DataflowNode::get_version();
    if (surface_checked_version < v) {
      surface_checked_version = v;
      ew::View3Item::prepared = false;
      return true;
    }
  }
  return false;
}

void
ew::View3Surface::clear_highlight()
{
}

void
ew::View3Surface::prepare()
{
  if (dlist == 0) {
    dlist = glGenLists(1);
    if (dlist == 0) {
      throw ew::ErrorRuntime(
       "A program limitation has been exceeded"
       " (the number of OpenGL display lists).");
    }
  }
  if (surface != 0 && surface->ew::DataflowNode::get_valid()) {
    if (sur_dlist == 0) {
      sur_dlist = surface->dlist;
      if (sur_dlist != 0) {
        surface->dlist_refs += 1;
      } else {
        sur_dlist = glGenLists(1);
        if (sur_dlist == 0) {
          throw ew::ErrorRuntime(
           "An OpenGL rendering resource has been exhausted.");
        }
        surface->dlist = sur_dlist;
        surface->dlist_cycle = 0;
        surface->dlist_refs += 1;
      }
    }
    if (surface->dlist_cycle < surface->ew::DataflowNode::get_version()) {
      dbg.on && dbg.dprintf("%s::%s compiling-sur_dlist", dbg.fn,
       "set_surface");
      glNewList(sur_dlist, GL_COMPILE);
      int len = surface->get_num_faces();
      const int *tri = surface->get_faces();
      const float *pts = surface->get_points();
      const float *normals = surface->get_normals();
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_NORMAL_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, pts);
      glNormalPointer(GL_FLOAT, 0, normals);
      glDrawElements(GL_TRIANGLES, 3 * len, GL_UNSIGNED_INT, tri);
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);
      glEndList();
      surface->dlist_cycle = network->get_cycle();
    }
    glNewList(dlist, GL_COMPILE);
    GLfloat fc[3], bc[3];
    fc[0] = front_color[0] / 255.0;
    fc[1] = front_color[1] / 255.0;
    fc[2] = front_color[2] / 255.0;
    bc[0] = back_color[0] / 255.0;
    bc[1] = back_color[1] / 255.0;
    bc[2] = back_color[2] / 255.0;
    switch (render_mode) {
    case MODE_SMOOTH:
//XXX ? revert to widget wide
      glLoadName(2U);
      glShadeModel(GL_SMOOTH);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
      glEnable(GL_NORMALIZE);
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fc);
      glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, bc);
      glCallList(sur_dlist);
      glDisable(GL_NORMALIZE);
      glDisable(GL_LIGHTING);
      glDisable(GL_LIGHT0);
      break;
    case MODE_SILHOUETTE:
      glLoadName(2U);
      glShadeModel(GL_FLAT);
      glColor3fv(fc);
      glCallList(sur_dlist);
      break;
    case MODE_WIREFRAME:
      glLoadName(1U);
      glShadeModel(GL_FLAT);
      glColor3fv(fc);
      glLineWidth(wireframe_linewidth);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glCallList(sur_dlist);
      break;
    }
//XXX restore attributes
    glEndList();
  } else {
    glNewList(dlist, GL_COMPILE);
    glEndList();
  }
  ew::View3Item::prepared = true;
}

void
ew::View3Surface::render()
{
  glCallList(dlist);
}
