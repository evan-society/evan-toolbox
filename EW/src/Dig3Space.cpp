// wdkg 2008-2010

#include <string>
#include <vector>
#include "ew/Bbox3.h"
#include "ew/DataflowCurve3.h"
#include "ew/DataflowCurve3E.h"
#include "ew/DataflowForm3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowSurface3.h"
#include "ew/DataflowSurface3E.h"
#include "ew/DebugHelper.h"
#include "ew/Debug.h"
#include "ew/Dig3.h"
#include "ew/Dig3Space.h"
#include "ew/Dig3View.h"
#include "ew/Form3.h"
#include "ew/Form3Curve.h"
#include "ew/Form3Embedding.h"
#include "ew/Form3Pointset.h"
#include "ew/Form3Surface.h"
#include "ew/String.h"
#include "ew/View3Curve.h"
#include "ew/View3Surface.h"
#include "ew/View3Landmarks.h"
#include "ew/View3Widget.h"

namespace {
  const char ClassName[] = "ew::Dig3Space";

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

//XXX form_node must be an auto_node, if we are to initialize it in the list
ew::Dig3Space::Dig3Space(ew::Dig3 *d3, int i) :
 network(d3->network),
 dig3(d3),
 index(i),
 dbg(ClassName, ew::String::ssprintf("%d", i)),
 form_node(new ew::DataflowForm3(d3->network))
{
  if (dbg.on) {
    dbg.dprintf("%s::%s(..., %d) {", dbg.fn, "CTOR", i);
    dbg.dprintf("%s::%s }", dbg.fn, "CTOR");
  }
}

ew::Dig3Space::~Dig3Space()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "DTOR");
  for (int i = dig3->get_n_views() - 1; i >= 0; i -= 1) {
    ew::Dig3View *v = dig3->get_views()[i];
    if (index == v->get_space()) {
      v->set_space(-1);
    }
  }
  form_node->ew::DataflowNode::decr_ref_count();
  for (int i = 0; i < VectorSize(curve_nodes); i += 1) {
    if (curve_nodes[i] != 0) {
      curve_nodes[i]->ew::DataflowNode::decr_ref_count();
    }
  }
  for (int i = 0; i < VectorSize(surface_nodes); i += 1) {
    if (surface_nodes[i] != 0) {
      surface_nodes[i]->ew::DataflowNode::decr_ref_count();
    }
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "DTOR");
}

/// This sets the space to an empty
/// form.

void
ew::Dig3Space::reset_form()
{
  if (dbg.on) {
    dbg.dprintf("%s::%s() {", dbg.fn, "reset_form");
    dig3->dbg_check(__FILE__, __LINE__);
  }
  for (int i = VectorSize(curve_nodes) - 1; i >= 0; i -= 1) {
    remove_form_curve(i);
  }
  for (int i = VectorSize(surface_nodes) - 1; i >= 0; i -= 1) {
    remove_form_surface(i);
  }
  for (int i = VectorSize(form_node->get_data()->pointsets) - 1;
   i >= 0; i -= 1) {
    form_node->remove_pointset(i);
  }
//XXX reset the uncontrolled elements.
//  form_node->reset();
  if (dbg.on) {
    dig3->dbg_check(__FILE__, __LINE__);
    dbg.dprintf("%s::%s }", dbg.fn, "reset_form");
  }
}

/// This sets the space to a new form.
/// @param form_data
///   The form data to copy.

void
ew::Dig3Space::set_form_data(const ew::Form3 *form_data)
{
  if (dbg.on) {
    dbg.dprintf("%s::%s(...) {", dbg.fn, "set_form");
    dig3->dbg_check(__FILE__, __LINE__);
  }
  const ew::Form3 *form0 = form_node->get_data();
//XXX more intelligent, or do check below, or check by category
  if (*form_data == *form0) {
    dbg.on && dbg.dprintf("%s::%s  unchanged", dbg.fn, "set_form");
  } else {
    bool repl;
    for (int i = 0; i < VectorSize(form_data->curves); i += 1) {
      set_form_curve(&repl, &form_data->curves[i]);
    }
    for (int i = 0; i < VectorSize(form0->curves); i += 1) {
      if (i >= VectorSize(form_data->curves) ||
       form_data->curves[i].id != form0->curves[i].id) {
        remove_form_curve(i);
        i -= 1;
      }
    }
    for (int i = 0; i < VectorSize(form_data->surfaces); i += 1) {
      set_form_surface(&repl, &form_data->surfaces[i]);
    }
    for (int i = 0; i < VectorSize(form0->surfaces); i += 1) {
      if (i >= VectorSize(form_data->surfaces) ||
       form_data->surfaces[i].id != form0->surfaces[i].id) {
        remove_form_surface(i);
        i -= 1;
      }
    }
    for (int i = 0; i < VectorSize(form_data->pointsets); i += 1) {
      form_node->set_pointset(&repl, &form_data->pointsets[i]);
    }
    for (int i = 0; i < VectorSize(form0->pointsets); i += 1) {
      if (i >= VectorSize(form_data->pointsets) ||
       form_data->pointsets[i].id != form0->pointsets[i].id) {
        form_node->remove_pointset(i);
        i -= 1;
      }
    }
    for (int i = 0; i < VectorSize(form_data->embeddings); i += 1) {
      form_node->set_embedding(&repl, &form_data->embeddings[i]);
    }
    for (int i = 0; i < VectorSize(form0->embeddings); i += 1) {
      if (i >= VectorSize(form_data->embeddings) ||
       form_data->embeddings[i].subset_id != form0->embeddings[i].subset_id ||
       form_data->embeddings[i].superset_id !=
       form0->embeddings[i].superset_id) {
        form_node->remove_embedding(i);
        i -= 1;
      }
    }
//XXX copy the uncontrolled elements
  }
  if (dbg.on) {
    dig3->dbg_check(__FILE__, __LINE__);
    dbg.dprintf("%s::%s }", dbg.fn, "set_form");
  }
}

// We switch cur/sur's instead of switching their files, because we want to
// share dlist's which are kept by the cur/sur.

//XXX exception safety

/// This adds or replaces a curve in the space.
/// @param[out] replaced
///   @c true if the curve replaced an existing curve.
/// @param cr
///   A pointer to the curve data to copy.
/// @return
///   The index of the curve.

int
ew::Dig3Space::set_form_curve(bool *replaced, const ew::Form3Curve *cr)
{
  if (dbg.on) {
    dbg.dprintf("%s::%s (\"%s\") {", dbg.fn, "set_form_curve",
     cr->id.c_str());
    dig3->dbg_check(__FILE__, __LINE__);
  }
  const ew::DataflowCurve3E *cur;
  if (cr->file.length() != 0) {
    cur = network->cached_curve(cr->file.c_str());
  } else {
    cur = 0;
  };
  int n = form_node->set_curve(replaced, cr);
  if (*replaced) {
    if (curve_nodes[n]) {
      curve_nodes[n]->ew::DataflowNode::decr_ref_count();
      curve_nodes[n] = cur;
    }
    for (int i = 0; i < dig3->get_n_views(); i += 1) {
      ew::Dig3View *v = dig3->get_views()[i];
      if (index == v->get_space()) {
        v->get_curve_items()[n]->set_curve(cur);
      }
    }
  } else {
    VectorInsert(curve_nodes, n, cur);
    for (int i = 0; i < dig3->get_n_views(); i += 1) {
      ew::Dig3View *v = dig3->get_views()[i];
      if (index == v->get_space()) {
//XXX is this safe
        ew::View3Curve *curi = new ew::View3Curve(v,
         ew::Dig3View::ITEM_CURVE);
        VectorInsert(v->curve_item_index, n, curi);
        curi->set_curve(cur);
// Keep landmarks at end of rendering order.
//XXX ?keep curve after surfaces
        v->ew::View3Widget::move_item(v->get_landmarks_item(),
         v->ew::View3Widget::get_n_items() - 1);
      }
    }
  }
  if (dbg.on) {
    dig3->dbg_check(__FILE__, __LINE__);
    dbg.dprintf("%s::%s } (n=%d, replaced=%s)", dbg.fn, "set_form_curve",
     n, ew::Debug::to_str(*replaced));
  }
  return n;
}

/// This deletes a curve from the space.
/// @param n
///   The index of the curve to delete.

void
ew::Dig3Space::remove_form_curve(int n)
{
  if (dbg.on) {
    dbg.dprintf("%s::%s(%d) {", dbg.fn, "remove_curve", n);
    dig3->dbg_check(__FILE__, __LINE__);
  }
  form_node->remove_curve(n);
  for (int i = 0; i < dig3->get_n_views(); i += 1) {
    ew::Dig3View *v = dig3->get_views()[i];
    if (index == v->get_space()) {
      v->curve_item_index[n]->destroy();
      VectorErase(v->curve_item_index, n);
    }
  }
  if (curve_nodes[n] != 0) {
    curve_nodes[n]->ew::DataflowNode::decr_ref_count();
  }
  VectorErase(curve_nodes, n);
  if (dbg.on) {
    dig3->dbg_check(__FILE__, __LINE__);
    dbg.dprintf("%s::%s }", dbg.fn, "remove_curve");
  }
}

//XXX cache, inline

/// This finds the curve a pointset is embedded in if any.
/// @param ps
///   The index of the pointset.
/// @return
///   The index of the curve, or -1.

//XXX merge with Form3::search_superset
int
ew::Dig3Space::get_curve_of_pointset(int ps) const
{
  dbg.on && dbg.dprintf("%s::%s (%d) {", dbg.fn, "get_curve_of_pointset", ps);
  const ew::Form3 *frm = form_node->get_data();
  const std::string &psname = frm->pointsets[ps].id;
  int r = -1;
  for (int i = 0; i < VectorSize(frm->embeddings); i += 1) {
//XXX use binary search using ordering
    if (frm->embeddings[i].subset_id != psname) {
      continue;
    }
    const std::string &surname = frm->embeddings[i].superset_id;
//XXX use binary search using ordering
    for (int j = 0; j < VectorSize(frm->curves); j += 1) {
      if (surname == frm->curves[j].id) {
        r = j;
        break;
      }
    }
    if (r >= 0) {
      break;
    }
  }
  dbg.on && dbg.dprintf("%s::%s } (%d)", dbg.fn, "get_curve_of_pointset", r);
  return r;
}

//XXX exception safety

/// This adds or replaces a surface in the space.
/// @param[out] replaced
///   @c true if the surface replaced an existing surface.
/// @param sr
///   A pointer to the surface data to copy.
/// @return
///   The index of the surface.

int
ew::Dig3Space::set_form_surface(bool *replaced, const ew::Form3Surface *sr)
{
  if (dbg.on) {
    dbg.dprintf("%s::%s (\"%s\") {", dbg.fn, "set_form_surface",
     sr->id.c_str());
    dig3->dbg_check(__FILE__, __LINE__);
  }
  const ew::DataflowSurface3E *sur;
  if (sr->file.length() != 0) {
    sur = network->cached_surface(sr->file.c_str());
  } else {
    sur = 0;
  };
  int n = form_node->set_surface(replaced, sr);
  if (*replaced) {
    if (surface_nodes[n]) {
      surface_nodes[n]->ew::DataflowNode::decr_ref_count();
      surface_nodes[n] = sur;
    }
    for (int i = 0; i < dig3->get_n_views(); i += 1) {
      ew::Dig3View *v = dig3->get_views()[i];
      if (index == v->get_space()) {
        v->get_surface_items()[n]->set_surface(sur);
      }
    }
  } else {
    VectorInsert(surface_nodes, n, sur);
    for (int i = 0; i < dig3->get_n_views(); i += 1) {
      ew::Dig3View *v = dig3->get_views()[i];
      if (index == v->get_space()) {
//XXX is this safe
        ew::View3Surface *suri = new ew::View3Surface(v,
         ew::Dig3View::ITEM_SURFACE);
        VectorInsert(v->surface_item_index, n, suri);
        suri->set_surface(sur);
// Keep landmarks at end of rendering order.
        v->ew::View3Widget::move_item(v->get_landmarks_item(),
         v->ew::View3Widget::get_n_items() - 1);
      }
    }
  }
  if (dbg.on) {
    dig3->dbg_check(__FILE__, __LINE__);
    dbg.dprintf("%s::%s } (n=%d, replaced=%s)", dbg.fn, "set_form_surface",
     n, ew::Debug::to_str(*replaced));
  }
  return n;
}

/// This deletes a surface from the space.
/// @param n
///   The index of the surface to delete.

void
ew::Dig3Space::remove_form_surface(int n)
{
  if (dbg.on) {
    dbg.dprintf("%s::%s(%d) {", dbg.fn, "remove_surface", n);
    dig3->dbg_check(__FILE__, __LINE__);
  }
  form_node->remove_surface(n);
  for (int i = 0; i < dig3->get_n_views(); i += 1) {
    ew::Dig3View *v = dig3->get_views()[i];
    if (index == v->get_space()) {
      v->surface_item_index[n]->destroy();
      VectorErase(v->surface_item_index, n);
    }
  }
  if (surface_nodes[n] != 0) {
    surface_nodes[n]->ew::DataflowNode::decr_ref_count();
  }
  VectorErase(surface_nodes, n);
  if (dbg.on) {
    dig3->dbg_check(__FILE__, __LINE__);
    dbg.dprintf("%s::%s }", dbg.fn, "remove_surface");
  }
}

//XXX cache, inline

/// This finds the surface a pointset is embedded in if any.
/// @param ps
///   The index of the pointset.
/// @return
///   The index of the surface, or -1.

int
ew::Dig3Space::get_surface_of_pointset(int ps) const
{
  dbg.on && dbg.dprintf("%s::%s (%d) {", dbg.fn, "get_surface_of_pointset",
   ps);
  const ew::Form3 *frm = form_node->get_data();
  const std::string &psname = frm->pointsets[ps].id;
  int r = -1;
  for (int i = 0; i < VectorSize(frm->embeddings); i += 1) {
//XXX use binary search using ordering
    if (frm->embeddings[i].subset_id != psname) {
      continue;
    }
    const std::string &surname = frm->embeddings[i].superset_id;
//XXX use binary search using ordering
    for (int j = 0; j < VectorSize(frm->surfaces); j += 1) {
      if (surname == frm->surfaces[j].id) {
        r = j;
        break;
      }
    }
    if (r >= 0) {
      break;
    }
  }
  dbg.on && dbg.dprintf("%s::%s } (%d)", dbg.fn, "get_surface_of_pointset", r);
  return r;
}

//XXX when points other than lmks and slmks are supported, this will need
// changing

/// This calculates the bounding box of all elements of the space's form.
/// @param[out] b
///   Where to store the bounding box.

void
ew::Dig3Space::get_bbox(ew::Bbox3 *b) const
{
  dbg.on && dbg.dprintf("%s::%s (...) {", dbg.fn, "get_bbox");
  *b = *form_node->get_bbox();
  for (int i = 0; i < VectorSize(curve_nodes); i += 1) {
    b->set_to_union(b, curve_nodes[i]->get_bbox());
  }
  for (int i = 0; i < VectorSize(surface_nodes); i += 1) {
    b->set_to_union(b, surface_nodes[i]->get_bbox());
  }
  dbg.on && dbg.dprintf("%s::%s } (...)", dbg.fn, "get_bbox");
}

/// This projects a point onto either a curve or surface.
/// @param[out] rdim
///   Where to store the relax_dim of the projection.
/// @param[out] rparam
///   Where to store the relax_params of the projection.
///   This should point to an array of size 3.
/// @param[out] proj
///   Where to store the coordinates of the projected point.
///   This should point to an array of size 3.
/// @param[in] p
///   The coordinates of the original point.
///   This should point to an array of size 3.
/// @param[in] id
///   The id of the surface or curve to project onto.
/// @return
///   Whether the operation could be performed.
///   This is false if there is no surface or curve with the given id, or if
///   this surface or curve is a placeholder (has no filename).

bool
ew::Dig3Space::project(int *rdim, double *rparam, double *proj,
 const double *p, const char *id) const
{
  const ew::Form3 *frm = get_form_data();
  int index;
  if (frm->search_surface(&index, id)) {
    const ew::DataflowSurface3 *sur = get_surface_nodes()[index];
    if (sur == 0) {
      return false;
    }
    int face;
    double coeffs[3];
    sur->project(&face, coeffs, rparam, proj, p);
    *rdim = 1;
    return true;
  } else if (frm->search_curve(&index, id)) {
    const ew::DataflowCurve3 *cur = get_curve_nodes()[index];
    if (cur == 0) {
      return false;
    }
    int edge;
    double coeffs[2];
    cur->project(&edge, coeffs, rparam, proj, p);
    *rdim = 2;
    return true;
  } else {
    return false;
  }
}
