// wdkg 2008-2010

#include <string>
#include <vector>
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/ErrorLogic.h"
#include "ew/String.h"
#include "ew/DataflowArray3.h"
#include "ew/DataflowArray3E.h"
#include "ew/DataflowCurve3.h"
#include "ew/DataflowCurve3E.h"
#include "ew/DataflowForm3.h"
#include "ew/DataflowImage3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowSurface3.h"
#include "ew/DataflowSurface3E.h"
#include "ew/Dig3.h"
#include "ew/Dig3Space.h"
#include "ew/Dig3View.h"
#include "ew/Form3.h"
#include "ew/Transform3.h"
#include "ew/View3Curve.h"
#include "ew/View3Image.h"
#include "ew/View3Landmarks.h"
#include "ew/View3Surface.h"
#include "ew/View3Widget.h"

namespace {
  const char ClassName[] = "ew::Dig3View";

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
}

ew::Dig3View::Dig3View(ew::Dig3 *d3, int sp, int sm) :
 ew::View3Widget(d3->network),
 dig3(d3),
 dbg(ClassName, ew::View3Widget::dbg),
 index(dig3->get_n_views()),
 space(-1),
 slice_mode(-1),
 view_link(0),
 slice_array_node(0),
 slice_image_node(0)
{
  dbg.on && dbg.dprintf("%s::%s(...,%d,%d) {", dbg.fn, "CTOR", sp, sm);
//XXX ?auto ptr
  landmarks_item = new ew::View3Landmarks(this, ITEM_LANDMARKS);
  if (sm == 1) {
    ew::View3Widget::set_clip_ratio(0.01);
  }
  dig3->views.push_back(this);
  for (int i = 0; i < dig3->get_n_views() - 1; i += 1) {
    ew::Dig3View *vw = dig3->get_views()[i];
    vw->slice_item_index.push_back(0);
  }
  for (int i = 0; i < dig3->get_n_views(); i += 1) {
    slice_item_index.push_back(0);
  }
  set_space(sp);
  set_slice_mode(sm);
  if (dbg.on) {
    dig3->dbg_check(__FILE__, __LINE__);
    dbg.dprintf("%s::%s }", dbg.fn, "CTOR");
  }
}

ew::Dig3View::~Dig3View()
{
  if (dbg.on) {
    dbg.dprintf("%s::%s() {", dbg.fn, "DTOR");
    dig3->dbg_check(__FILE__, __LINE__);
  }
  ew::View3Widget::destruction_in_progress();
  set_slice_mode(-1);
  set_space(-1);
  for (int i = 0; i < dig3->get_n_views(); i += 1) {
    ew::Dig3View *v = dig3->get_views()[i];
    VectorErase(v->slice_item_index, index);
    //!? apart from the style warning, this causes a mem leak, doesn't it?!?
    //!? if (v->view_link = this) {
    if ( ( v->view_link = this ) != NULL ) {
      v->view_link = 0;
    }
  }
  ew::View3Widget::reset();
  VectorErase(dig3->views, index);
  for (int i = index; i < dig3->get_n_views(); i += 1) {
    dig3->get_views()[i]->index = i;
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "DTOR");
}

void
ew::Dig3View::set_slice_mode(int sm)
{
  if (dbg.on) {
    dbg.dprintf("%s::%s(%d) {", dbg.fn, "set_slice_mode", sm);
    dig3->dbg_check(__FILE__, __LINE__);
  }
  if (space == -1 && sm != -1) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  if (sm == slice_mode) {
    dbg.on && dbg.dprintf("%s::%s    unchanged", dbg.fn, "set_slice_mode");
  } else {
//XXX
    if ((slice_image_node != 0) != (slice_mode == 1)) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    if (slice_mode == 1) {
      slice_image_node->ew::DataflowNode::decr_ref_count();
      slice_array_node->ew::DataflowNode::decr_ref_count();
      slice_image_node = 0;
      slice_array_node = 0;
    }
    for (int i = 0; i < dig3->get_n_views(); i += 1) {
      ew::Dig3View *v = dig3->get_views()[i];
      if (v->slice_item_index[index] != 0) {
        v->slice_item_index[index]->destroy();
        v->slice_item_index[index] = 0;
      }
    }
    for (int i = 0; i < dig3->get_n_views(); i += 1) {
      if (slice_item_index[i] != 0) {
        slice_item_index[i]->destroy();
        slice_item_index[i] = 0;
      }
    }
    if (sm == 1) {
//XXX ?auto ptr
      slice_array_node = new ew::DataflowArray3E(network);
// Stretch [0, 1]^2 to [-1, 1]^2.
      double str[3] = {2.0, 2.0, 1.0};
      slice_array_node->set_stretch(str);
      int n[3] = {2, 2, 1};
      slice_array_node->set_explicit_size(n);
//XXX ?auto ptr
      slice_image_node = new ew::DataflowImage3(network);
      slice_image_node->set_array_node(slice_array_node);
      for (int i = 0; i < dig3->get_n_views(); i += 1) {
        ew::Dig3View *v = dig3->get_views()[i];
        if (i == index ||
         (v->get_slice_mode() == 0 && v->get_space() == space)) {
//XXX ?auto ptr
          ew::View3Image *imi = new ew::View3Image(v, ITEM_SLICE);
//XXX method to arrange order
// Keep images at start of rendering order.
          v->ew::View3Widget::move_item(imi, 0);
          imi->set_image(slice_image_node);
          v->slice_item_index[index] = imi;
        }
      }
//XXX ??also tr, bg
      ew::View3Widget::set_clip_ratio(0.01);
      ew::View3Widget::set_use_depth(false);
    }
    if (sm == 0) {
      for (int i = 0; i < dig3->get_n_views(); i += 1) {
        ew::Dig3View *v = dig3->get_views()[i];
        if (i != index && v->get_slice_mode() == 1 &&
         v->get_space() == space) {
          ew::View3Image *imi = new ew::View3Image(this, ITEM_SLICE);
// Keep images at start of rendering order.
          ew::View3Widget::move_item(imi, 0);
          imi->set_image(v->slice_image_node);
          slice_item_index[i] = imi;
        }
      }
//XXX ??also tr, bg
      ew::View3Widget::set_clip_ratio(1.0);
      ew::View3Widget::set_use_depth(true);
    }
    slice_mode = sm;
  }
  if (dbg.on) {
    dig3->dbg_check(__FILE__, __LINE__);
    dbg.dprintf("%s::%s }", dbg.fn, "set_slice_mode");
  }
}

void
ew::Dig3View::set_space(int sp)
{
  if (dbg.on) {
    dbg.dprintf("%s::%s(%d) {", dbg.fn, "set_space", sp);
    dig3->dbg_check(__FILE__, __LINE__);
  }
  if (sp == space) {
    dbg.on && dbg.dprintf("%s::%s    unchanged", dbg.fn, "set_space");
  } else {
    int sm = slice_mode;
    set_slice_mode(-1);
//XXX here
// curve_item_index.size() ==
//  (space == -1 ? 0 : dig3->spaces[space]->get_n_curve_nodes())
// surface_item_index.size() ==
//  (space == -1 ? 0 : dig3->spaces[space]->get_n_surface_nodes())
    for (int i = 0; i < VectorSize(curve_item_index); i += 1) {
//XXX here curve_item_index[i] != 0
      curve_item_index[i]->destroy();
    }
    for (int i = 0; i < VectorSize(surface_item_index); i += 1) {
//XXX here surface_item_index[i] != 0
      surface_item_index[i]->destroy();
    }
    if (sp == -1) {
      landmarks_item->set_form(0);
    } else {
      landmarks_item->set_form(dig3->get_spaces()[sp]->get_form_node());
    }
    curve_item_index.clear();
    surface_item_index.clear();
    if (sp != -1) {
      for (int i = 0; i < dig3->get_spaces()[sp]->get_n_curve_nodes(); i += 1) {
        ew::View3Curve *curi = new ew::View3Curve(this, ITEM_CURVE);
        curve_item_index.push_back(curi);
        curi->set_curve(dig3->get_spaces()[sp]->get_curve_nodes()[i]);
      }
      for (int i = 0; i < dig3->get_spaces()[sp]->get_n_surface_nodes();
       i += 1) {
        ew::View3Surface *suri = new ew::View3Surface(this, ITEM_SURFACE);
        surface_item_index.push_back(suri);
        suri->set_surface(dig3->get_spaces()[sp]->get_surface_nodes()[i]);
      }
    }
    space = sp;
    if (space != -1) {
      set_slice_mode(sm);
    }
// Keep landmarks at end of rendering order.
    ew::View3Widget::move_item(landmarks_item,
     ew::View3Widget::get_n_items() - 1);
  }
  if (dbg.on) {
    dig3->dbg_check(__FILE__, __LINE__);
    dbg.dprintf("%s::%s }", dbg.fn, "set_space");
  }
}

void
ew::Dig3View::set_link(ew::Dig3View *v, const ew::Transform3 *tr)
{
  if (dbg.on) {
    const char *vstr;
    if (v == 0) {
      vstr = "NULL";
    } else {
      vstr = v->dbg.fn;
    }
    dbg.dprintf("%s::%s(%s,...) {", dbg.fn, "set_link", vstr);
  }
  view_link = v;
  if (v != 0) {
    view_link_tr = *tr;
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "set_link");
}

//XXX provisonal implementation supporting cycles and linear
// support independent cyles + oneway warped view (max 1) + oneway warped
// slice (max 1) + oneway slice fixd in view mode (max 1 per main view)
void
ew::Dig3View::sync_view()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "sync_view");
  ew::Dig3View *v0 = this;
  ew::Dig3View *v = this;
  while (1) {
    dbg.on && dbg.dprintf("%s::%s   handling(%s)", dbg.fn, "sync_view",
     v->dbg.fn);
    const ew::Transform3 *tr = v->ew::View3Widget::get_view_mapping();
    if (v->get_slice_mode() == 1) {
      ew::Transform3 tr2;
      tr2.set_to_inverse(tr);
      v->slice_array_node->set_mapping(&tr2);
    }
    ew::Transform3 *vt = &v->view_link_tr;
    v = v->view_link;
    if (v == 0 || v == v0) {
      break;
    }
    ew::Transform3 tr3;
    tr3.set_to_composition(vt, tr);
    v->ew::View3Widget::set_view_mapping(&tr3);
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "sync_view");
}

void
ew::Dig3View::set_view_mapping(const ew::Transform3 *tr)
{
  dbg.on && dbg.dprintf("%s::%s(...) {", dbg.fn, "set_view_mapping");
  ew::View3Widget::set_view_mapping(tr);
  sync_view();
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "set_view_mapping");
}

void
ew::Dig3View::view_post_compose(const ew::Transform3 *tr)
{
  dbg.on && dbg.dprintf("%s::%s(...) {", dbg.fn, "view_post_compose");
  ew::View3Widget::view_post_compose(tr);
  sync_view();
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "view_post_compose");
}
