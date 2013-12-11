// wdkg 2009-2010
//XXX allow gui to supply bitmap
//XXX use point specific get_bbox

#include <stdexcept>
#include <GL/gl.h>
#include "ew/Bbox3.h"
#include "ew/DataflowForm3.h"
#include "ew/DataflowNetwork.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/ErrorLogic.h"
#include "ew/ErrorRuntime.h"
#include "ew/Form3.h"
#include "ew/View3Item.h"
#include "ew/View3Landmarks.h"
#include "ew/View3Widget.h"

namespace {
  const char ClassName[] = "ew::View3Landmarks";
  const char InstanceName[] = "landmarks";
  const GLsizei MarkersN[] = {9, 7, 5};
  const GLfloat MarkersO[] = {4.0, 3.0, 2.0};
  const GLubyte MarkersD[][18] = {
    {
      0x08, 0x00,
      0x08, 0x00,
      0x08, 0x00,
      0x08, 0x00,
      0xff, 0x80,
      0x08, 0x00,
      0x08, 0x00,
      0x08, 0x00,
      0x08, 0x00
    },
    {
      0x38,
      0x44,
      0x82,
      0x82,
      0x82,
      0x44,
      0x38,
    },
    {
      0x70,
      0xf8,
      0xf8,
      0xf8,
      0x70
    }
  };

  template<typename T>
  inline int
  VectorSize(const std::vector<T> &v)
  {
    return static_cast<int>(v.size());
  }
}

/// The names of the #symbol_t
/// choices.

const char *const ew::View3Landmarks::symbol_names[] = {
  "cross",
  "circle",
  "dot",
  0
};

/// This creates an item.
/// @param view_i
///   The view widget the item should belong to.
/// @param type_i
///   The initializer for ew::View3Item::type.

ew::View3Landmarks::View3Landmarks(ew::View3Widget *i_view, int i_type) :
 ew::View3Item(i_view, i_type, InstanceName),
 dbg(ClassName, ew::View3Item::dbg),
 form(0),
 form_checked_version(0),
 symbol(SYMBOL_CROSS),
 dlist(0),
 highlight_ps(-1),
 highlight_i(-1)
{
  color[0] = color[2] = 0;
  color[1] = 255;
}

ew::View3Landmarks::~View3Landmarks()
{
  if (form != 0) {
    form->ew::DataflowNode::decr_ref_count();
  }
//XXX need to ensure ctxt active
  if (dlist) {
    glDeleteLists(dlist, 1);
  }
}

/// @param frm
///   A pointer to the new form node to display, or 0.

void
ew::View3Landmarks::set_form(const ew::DataflowForm3 *frm)
{
  if (frm == form) {
    return;
  }
  if (frm != 0) {
    if (frm->network != network) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    frm->ew::DataflowNode::incr_ref_count();
  }
  if (form != 0) {
    form->ew::DataflowNode::decr_ref_count();
  }
  form = frm;
  form_checked_version = 0;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

const ew::Bbox3 *
ew::View3Landmarks::get_bbox()
{
  if (form != 0) {
    return form->get_bbox();
  } else {
    return &ew::Bbox3::empty_box;
  }
}

/// @param[in] rgb
///   This points to an array of size 3 containing the new colour for the
///   landmark symbols.

void
ew::View3Landmarks::set_color(const unsigned char *rgb)
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

/// @param sym
///   The new landmark symbol.

void
ew::View3Landmarks::set_symbol(int sym)
{
  if (sym == symbol) {
    return;
  }
  if (sym < 0 || sym >= N_SYMBOLS) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  symbol = sym;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

//XXX finer grain versions
bool
ew::View3Landmarks::check_dataflow()
{
  if (form != 0) {
    unsigned long v = form->ew::DataflowNode::get_version();
    if (form_checked_version < v) {
      form_checked_version = v;
      ew::View3Item::prepared = false;
      return true;
    }
  }
  return false;
}

void
ew::View3Landmarks::prepare()
{
  if (dlist == 0) {
    dlist = glGenLists(1);
    if (dlist == 0) {
      throw ew::ErrorRuntime(
       "A program limitation has been exceeded"
       " (the number of OpenGL display lists).");
    }
  }
  if (form != 0 && form->ew::DataflowNode::get_valid()) {
    glNewList(dlist, GL_COMPILE);
    glShadeModel(GL_FLAT);
    const ew::Form3 *data = form->get_data();
    GLfloat c[3];
    c[0] = color[0] / 255.0;
    c[1] = color[1] / 255.0;
    c[2] = color[2] / 255.0;
    int sy = symbol;
    if (sy < 0 || sy > N_SYMBOLS) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    glColor3fv(c);
    unsigned int u = 0;
    for (int ps = 0; ps < VectorSize(data->pointsets); ps += 1) {
      const ew::Form3PointSet *fps = &data->pointsets[ps];
      if (fps->type == ew::Form3::TYPE_LANDMARK ||
       fps->type == ew::Form3::TYPE_SEMI_LANDMARK) {
        for (int i = 0; i < VectorSize(fps->locations); i += 3) {
          if (ps == highlight_ps && i == highlight_i) {
            const unsigned char *hc =
             ew::View3Item::view->get_highlight_color();
            GLfloat ch[3];
            ch[0] = hc[0] / 255.0;
            ch[1] = hc[1] / 255.0;
            ch[2] = hc[2] / 255.0;
            glColor3fv(ch);
          }
          glLoadName(u << 2);
          glRasterPos3dv(&fps->locations[i]);
          glBitmap(MarkersN[sy], MarkersN[sy], MarkersO[sy], MarkersO[sy], 0.0,
           0.0, MarkersD[sy]);
          u += 1;
          if (ps == highlight_ps && i == highlight_i) {
            glColor3fv(c);
          }
        }
      }
    }
    glEndList();
  } else {
    glNewList(dlist, GL_COMPILE);
    glEndList();
  }
  ew::View3Item::prepared = true;
}

void
ew::View3Landmarks::render()
{
  glCallList(dlist);
}

/// @param ps
///   The pointset of the landmark to highlight.
/// @param i
///   The index of the landmark within the pointset of the landmark to
///   highlight.

void
ew::View3Landmarks::set_highlight(int ps, int i)
{
  if (highlight_ps == ps && highlight_i == i) {
    return;
  }
  ew::View3Item::set_this_highlight_item();
  highlight_ps = ps;
  highlight_i = i;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

void
ew::View3Landmarks::clear_highlight()
{
  if (highlight_ps == -1 && highlight_i == -1) {
    return;
  }
  highlight_ps = -1;
  highlight_i = -1;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}
