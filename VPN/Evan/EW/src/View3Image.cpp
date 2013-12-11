// wdkg 2008-2010
//XXX cgeck image 2D etc, flat
//XXX rigorate polygon offset

#include <stdexcept>
#include <GL/gl.h>
#include "ew/Bbox3.h"
#include "ew/DataflowArray3.h"
//XXX temporary
#include "ew/DataflowArray3E.h"
#include "ew/DataflowImage3.h"
#include "ew/DataflowNetwork.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/ErrorLogic.h"
#include "ew/ErrorRuntime.h"
#include "ew/View3Image.h"
#include "ew/View3Item.h"
#include "ew/View3Widget.h"

namespace {
  const char ClassName[] = "ew::View3Image";
  const char InstanceName[] = "image";
}

/// This creates an item.
/// @param view
///   The view widget the item should belong to.
/// @param i_type
///   The initializer for ew::View3Item::type.

ew::View3Image::View3Image(ew::View3Widget *view, int i_type) :
 ew::View3Item(view, i_type, InstanceName),
 dbg(ClassName, ew::View3Item::dbg),
 image(0),
 image_checked_version(0),
 dlist(0)
{
  color[0] = color[1] = color[2] = 128;
}

ew::View3Image::~View3Image()
{
  if (image != 0) {
    image->ew::DataflowNode::decr_ref_count();
  }
//XXX need to ensure ctxt active
  if (dlist) {
    glDeleteLists(dlist, 1);
  }
//XXX ?? is depth item count necessary
}

/// @param im
///   A pointer to the new image node to display, or 0.

void
ew::View3Image::set_image(const ew::DataflowImage3 *im)
{
  if (im == image) {
    return;
  }
  if (im != 0) {
    if (im->network != network) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    im->ew::DataflowNode::incr_ref_count();
  }
  if (image != 0) {
    image->ew::DataflowNode::decr_ref_count();
  }
  image = im;
  image_checked_version = 0;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

const ew::Bbox3 *
ew::View3Image::get_bbox()
{
//XXX this is temporary until make array points in node
  if (image != 0 && image->ew::DataflowNode::get_valid()) {
    bbox = ew::Bbox3::empty_box;
    const ew::DataflowArray3E *array =
     static_cast<const ew::DataflowArray3E*>(image->get_array_node());
    const double *str = array->get_stretch();
    const int *sz = array->get_size();
    const ew::Transform3 *tr = array->get_mapping();
    for (int n = 0; n < 4; n += 1) {
      double f[3];
      if (n < 2) {
        f[0] = str[0] * (sz[0] - 1.0) / 2.0;
      } else {
        f[0] = -str[0] * (sz[0] - 1.0) / 2.0;
      }
      if (n % 2) {
        f[1] = str[1] * (sz[1] - 1.0) / 2.0;
      } else {
        f[1] = -str[1] * (sz[1] - 1.0) / 2.0;
      }
      f[2] = 0.0;
      tr->apply(f, f);
      bbox.add(f);
    }
    return &bbox;
//XXX not implemented yet    return image->get_array_node()->get_bbox();
  } else {
    return &ew::Bbox3::empty_box;
  }
}

/// @param[in] rgb
///   This points to an array of size 3 containing the new colour for the
///   image.

void
ew::View3Image::set_color(const unsigned char *rgb)
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
ew::View3Image::clear_highlight()
{
}

bool
ew::View3Image::check_dataflow()
{
  if (image != 0) {
    unsigned long v = image->ew::DataflowNode::get_version();
    if (image_checked_version < v) {
      image_checked_version = v;
      ew::View3Item::prepared = false;
      return true;
    }
  }
  return false;
}

void
ew::View3Image::prepare()
{
  if (dlist == 0) {
    dlist = glGenLists(1);
    if (dlist == 0) {
      throw ew::ErrorRuntime(
       "A program limitation has been exceeded"
       " (the number of OpenGL display lists).");
    }
  }
  if (image != 0 && image->ew::DataflowNode::get_valid()) {
    const ew::DataflowArray3E *array =
     static_cast<const ew::DataflowArray3E*>(image->get_array_node());
    double coords[12];
    const double *str = array->get_stretch();
    const int *sz = array->get_size();
    const ew::Transform3 *tr = array->get_mapping();
    for (int n = 0; n < 4; n += 1) {
      double f[3];
      if (n < 2) {
        f[0] = str[0] * (sz[0] - 1.0) / 2.0;
      } else {
        f[0] = -str[0] * (sz[0] - 1.0) / 2.0;
      }
      if (n % 2) {
        f[1] = str[1] * (sz[1] - 1.0) / 2.0;
      } else {
        f[1] = -str[1] * (sz[1] - 1.0) / 2.0;
      }
      f[2] = 0.0;
      tr->apply(coords + n * 3, f);
    }
    if (dbg.on) {
// check image flat in slice
      const ew::Transform3* trw = ew::View3Item::view->get_view_mapping();
      double f1[3];
      for (int i = 0; i < 4; i += 1) {
        trw->apply(f1, coords + 3 * i);
        dbg.dprintf("%s::%s flat-check window-coords %g %g %g", dbg.fn,
         "prepare", f1[0], f1[1], f1[2]);
      }
    }
    glNewList(dlist, GL_COMPILE);
//XXX figure out exactly what to do with polygon offset
//XXX    glEnable(GL_POLYGON_OFFSET_FILL);
//XXX    glPolygonOffset(2.0, 4.0);
//XXX glShadeModel(GL_FLAT);
//XXX GLfloat fc[3] = {1.0, 0.0, 0.0};
//XXX glColor3fv(fc);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glEnable (GL_TEXTURE_2D);
//XXX  glColor4us(iitem->bg->red, iitem->bg->green, iitem->bg->blue,
//XXX  (short) (65535 * iitem->opaque));
//XXX glColor4us(60000, 0.0, 0.0, 65535);
    int w0 = 256;
    int h0 = 256;
    int w = 256;
    double d = 0.5 / w;
    int h = 256;
    double e = 0.5 / h;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0 + d, w0 * 1.0 / h - e);
    glVertex3dv(coords);
    glTexCoord2f(h0 * 1.0 / w - d, w0 * 1.0 / h - e);
    glVertex3dv(coords + 3);
    glTexCoord2f(h0 * 1.0 / w - d, 0.0 + e);
    glVertex3dv(coords + 9);
    glTexCoord2f(0.0 + d, 0.0 + e);
    glVertex3dv(coords + 6);
    glEnd();
    glDisable(GL_TEXTURE_2D);
//XXX    glDisable(GL_POLYGON_OFFSET_FILL);
    glEndList();
  } else {
    glNewList(dlist, GL_COMPILE);
    glEndList();
  }
  ew::View3Item::prepared = true;
}

void
ew::View3Image::render()
{
  if (image != 0 && image->ew::DataflowNode::get_valid()) {
    char buf[256 * 256 * 3];
    int w = 256;
    int h = 256;
    for (int i = 0; i < w; i += 1) {
      for (int j = 0; j < h; j += 1) {
        buf[3 * (j * w + i) + 0] = color[0];
        buf[3 * (j * w + i) + 1] = color[1];
        buf[3 * (j * w + i) + 2] = color[2];
      }
    }
    glLoadName(2U);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
    glCallList(dlist);
  }
}
//XXX opaque, stipple, mask, frame, cross hairs
//XXX pick, anti alias, data, polygon offset
