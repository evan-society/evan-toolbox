// wdkg 2008-2010

#include <cmath>
#include <stdexcept>
#include <vector>
#include <GL/gl.h>
#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowCurve3.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/ErrorLogic.h"
#include "ew/ErrorRuntime.h"
#include "ew/View3Item.h"
#include "ew/View3Curve.h"
#include "ew/View3Widget.h"

namespace {
  const char ClassName[] = "ew::View3Curve";
  const char InstanceName[] = "curve";
  const char ErrorOutOfDisplayLists[] =
   "A program limitation has been exceeded"
   " (the number of OpenGL display lists).";
}

/// This creates an item.
/// @param view
///   The view widget the item should belong to.
/// @param i_type
///   The initializer for ew::View3Item::type.

ew::View3Curve::View3Curve(ew::View3Widget *view, int i_type) :
 ew::View3Item(view, i_type, InstanceName),
 dbg(ClassName, ew::View3Item::dbg),
 curve(0),
 curve_checked_version(0),
 linewidth(1.0),
 cur_dlist(0),
 dlist(0)
{
  color[0] = color[1] = color[2] = 128;
}

// The gl context must be active when the curve item is destroyed, unless
// care is taken to unset the curve beforehand.
//XXX add a callback to activate context

ew::View3Curve::~View3Curve()
{
  if (curve != 0) {
    if (cur_dlist) {
      curve->dlist_refs -= 1;
      if (curve->dlist_refs == 0) {
        curve->dlist = 0;
        curve->dlist_cycle = 0;
        glDeleteLists(cur_dlist, 1);
      }
    }
    curve->ew::DataflowNode::decr_ref_count();
  }
  if (dlist) {
    glDeleteLists(dlist, 1);
  }
}

/// @param cur
///   A pointer to the new curve node to display, or 0.

void
ew::View3Curve::set_curve(const ew::DataflowCurve3 *cur)
{
  dbg.on && dbg.dprintf("%s::%s(...) {", dbg.fn, "set_curve");
  if (cur == curve) {
    return;
  }
  dbg.on && dbg.dprintf("%s::%s %s->%s", dbg.fn, "set_curve",
   ew::DataflowNode::to_str(curve), ew::DataflowNode::to_str(cur));
  if (cur != 0) {
    if (cur->network != network) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    cur->ew::DataflowNode::incr_ref_count();
  }
  if (curve != 0) {
    if (cur_dlist) {
      curve->dlist_refs -= 1;
      if (curve->dlist_refs == 0) {
        curve->dlist = 0;
        curve->dlist_cycle = 0;
        glDeleteLists(cur_dlist, 1);
      }
      cur_dlist = 0;
    }
    curve->ew::DataflowNode::decr_ref_count();
  }
  curve = cur;
  curve_checked_version = 0;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "set_curve");
}

const ew::Bbox3 *
ew::View3Curve::get_bbox()
{
  if (curve != 0) {
    return curve->get_bbox();
  } else {
    return &ew::Bbox3::empty_box;
  }
}

/// @param[in] rgb
///   This points to an array of size 3 containing the new colour for the
///   curve.

void
ew::View3Curve::set_color(const unsigned char *rgb)
{
  if (color[0] == rgb[0] &&
   color[1] == rgb[1] &&
   color[2] == rgb[2]) {
    return;
  }
  color[0] = rgb[0];
  color[1] = rgb[1];
  color[2] = rgb[2];
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

void
ew::View3Curve::set_linewidth(double w)
{
  if (linewidth == w) {
    return;
  }
  if (w <= 0.0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  linewidth = w;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

bool
ew::View3Curve::check_dataflow()
{
  if (curve != 0) {
    unsigned long v = curve->ew::DataflowNode::get_version();
    if (curve_checked_version < v) {
      curve_checked_version = v;
      ew::View3Item::prepared = false;
      return true;
    }
  }
  return false;
}

void
ew::View3Curve::clear_highlight()
{
}

void
ew::View3Curve::prepare()
{
  if (dlist == 0) {
    dlist = glGenLists(1);
    if (dlist == 0) {
      throw ew::ErrorRuntime(ErrorOutOfDisplayLists);
    }
  }
  if (curve != 0 && curve->ew::DataflowNode::get_valid()) {
    if (cur_dlist == 0) {
      cur_dlist = curve->dlist;
      if (cur_dlist != 0) {
        curve->dlist_refs += 1;
      } else {
        cur_dlist = glGenLists(1);
        if (cur_dlist == 0) {
          throw ew::ErrorRuntime(ErrorOutOfDisplayLists);
        }
        curve->dlist = cur_dlist;
        curve->dlist_cycle = 0;
        curve->dlist_refs += 1;
      }
    }
    if (curve->dlist_cycle < curve->ew::DataflowNode::get_version()) {
      dbg.on && dbg.dprintf("%s::%s compiling-cur_dlist", dbg.fn, "set_curve");
      glNewList(cur_dlist, GL_COMPILE);
      int len = curve->get_num_edges();
      const int *edg = curve->get_edges();
      const float *pts = curve->get_points();
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, pts);
      glDrawElements(GL_LINES, 2 * len, GL_UNSIGNED_INT, edg);
      glDisableClientState(GL_VERTEX_ARRAY);
      glEndList();
      curve->dlist_cycle = network->get_cycle();
    }
    glNewList(dlist, GL_COMPILE);
    GLfloat c[3];
    c[0] = color[0] / 255.0;
    c[1] = color[1] / 255.0;
    c[2] = color[2] / 255.0;
    glLoadName(1U);
    glColor3fv(c);
    glLineWidth(linewidth);
    glCallList(cur_dlist);
//XXX restore attributes
    glEndList();
  } else {
    glNewList(dlist, GL_COMPILE);
    glEndList();
  }
  ew::View3Item::prepared = true;
}

void
ew::View3Curve::render()
{
  glCallList(dlist);
}
