// wdkg 2008-2010

#include "ew/Bbox3.h"
#include "ew/DataflowForm3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/ErrorLogic.h"
#include "ew/Form3.h"
#include "ew/Form3Curve.h"
#include "ew/Form3Embedding.h"
#include "ew/Form3Pointset.h"
#include "ew/Form3Reflection.h"
#include "ew/Form3Surface.h"
#include "ew/Form3Volume.h"
#include "ew/String.h"

namespace {
  const char ClassName[] = "ew::DataflowForm3";
  const char InstanceName[] = "form3";

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

ew::DataflowForm3::DataflowForm3(ew::DataflowNetwork *i_network) :
 ew::DataflowNode(i_network, 0, InstanceName, 0),
 dbg(ClassName, ew::DataflowNode::dbg),
 change_cycle_coords(i_network->get_cycle()),
 change_cycle_relax(i_network->get_cycle()),
 change_cycle_association(i_network->get_cycle()),
 update_cycle_bbox(0)
{
}

void
ew::DataflowForm3::reset()
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "reset",
   ew::DataflowNode::change_cycle);
  change_cycle_coords = change_cycle_relax = change_cycle_association =
   ew::DataflowNode::change_cycle;
  inp_data.reset();
}

//XXX also set_data_check to manually check for changes
//XXX or make change_cycle* accessible so can save them with forms in a tableau
//     and supply as optional arg to set_data()
void
ew::DataflowForm3::set_data(const ew::Form3 *d)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_data",
   ew::DataflowNode::change_cycle);
  change_cycle_coords = change_cycle_relax = change_cycle_association =
   ew::DataflowNode::change_cycle;
  inp_data = *d;
}

int
ew::DataflowForm3::set_volume(bool *replaced, const ew::Form3Volume *vl)
{
  int n;
  bool repl = inp_data.search_volume(&n, vl->id.c_str());
  if (!repl || inp_data.volumes[n] != *vl) {
    ew::DataflowNode::record_change();
    dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_volume",
     ew::DataflowNode::change_cycle);
    if (!repl) {
      VectorInsert(inp_data.volumes, n, *vl);
    } else {
      inp_data.volumes[n] = *vl;
    }
  }
  *replaced = repl;
  return n;
}

void
ew::DataflowForm3::remove_volume(int n)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "remove_volume",
   ew::DataflowNode::change_cycle);
  if (n < 0 || n >= VectorSize(inp_data.volumes)) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  VectorErase(inp_data.volumes, n);
}

int
ew::DataflowForm3::set_surface(bool *replaced, const ew::Form3Surface *sr)
{
  int n;
  bool repl = inp_data.search_surface(&n, sr->id.c_str());
  if (!repl || inp_data.surfaces[n] != *sr) {
    ew::DataflowNode::record_change();
    dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_surface",
     ew::DataflowNode::change_cycle);
    if (!repl) {
      VectorInsert(inp_data.surfaces, n, *sr);
    } else {
      inp_data.surfaces[n] = *sr;
    }
  }
  *replaced = repl;
  return n;
}

void
ew::DataflowForm3::remove_surface(int n)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "remove_surface",
   ew::DataflowNode::change_cycle);
  if (n < 0 || n >= VectorSize(inp_data.surfaces)) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  VectorErase(inp_data.surfaces, n);
}

int
ew::DataflowForm3::set_curve(bool *replaced, const ew::Form3Curve *cr)
{
  int n;
  bool repl = inp_data.search_curve(&n, cr->id.c_str());
  if (!repl || inp_data.curves[n] != *cr) {
    ew::DataflowNode::record_change();
    dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_curve",
     ew::DataflowNode::change_cycle);
    if (!repl) {
      VectorInsert(inp_data.curves, n, *cr);
    } else {
      inp_data.curves[n] = *cr;
    }
  }
  *replaced = repl;
  return n;
}

void
ew::DataflowForm3::remove_curve(int n)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "remove_curve",
   ew::DataflowNode::change_cycle);
  if (n < 0 || n >= VectorSize(inp_data.curves)) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  VectorErase(inp_data.curves, n);
}

//XXX ?check how ps differs
//XXX ?check for no change
//XXX check id is valid (doesn't clash, non-empty)
int
ew::DataflowForm3::set_pointset(bool *replaced, const ew::Form3PointSet *ps)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_pointset",
   ew::DataflowNode::change_cycle);
  int n;
  bool repl = inp_data.search_pointset(&n, ps->id.c_str());
  if (repl) {
    if (inp_data.pointsets[n].type <= ew::Form3::TYPE_SEMI_LANDMARK ||
     ps->type <= ew::Form3::TYPE_SEMI_LANDMARK) {
      change_cycle_association = change_cycle_coords = change_cycle_relax =
       ew::DataflowNode::change_cycle;
    }
    inp_data.pointsets[n] = *ps;
  } else {
    if (ps->type <= ew::Form3::TYPE_SEMI_LANDMARK) {
      change_cycle_association = change_cycle_coords = change_cycle_relax =
       ew::DataflowNode::change_cycle;
    }
    VectorInsert(inp_data.pointsets, n, *ps);
  }
  *replaced = repl;
  return n;
}

void
ew::DataflowForm3::set_pointset_location(int n, int i, const double *loc)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_pointset_location",
   ew::DataflowNode::change_cycle);
  ew::Form3PointSet *ps = &inp_data.pointsets[n];
  if (ps->type <= ew::Form3::TYPE_SEMI_LANDMARK) {
    change_cycle_coords = ew::DataflowNode::change_cycle;
  }
  if (ps->locations.size() == 0) {
    ps->locations.resize(ps->n * 3, 0.0);
  }
  ps->locations[3 * i] = loc[0];
  ps->locations[3 * i + 1] = loc[1];
  ps->locations[3 * i + 2] = loc[2];
}

void
ew::DataflowForm3::set_pointset_relax(int n, int i, int rdim,
 const double *rparam)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_pointset_relax",
   ew::DataflowNode::change_cycle);
  ew::Form3PointSet *ps = &inp_data.pointsets[n];
  if (ps->type <= ew::Form3::TYPE_SEMI_LANDMARK) {
    change_cycle_relax = ew::DataflowNode::change_cycle;
  }
  if (ps->relax_dims.size() == 0 && rdim != 0) {
    ps->relax_dims.resize(ps->n, 0);
  }
  if (ps->relax_params.size() == 0 && (rdim != 0 && rdim != 3)) {
    ps->relax_params.resize(ps->n * 3, 0.0);
  }
  if (ps->relax_dims.size() != 0) {
    ps->relax_dims[i] = rdim;
  }
  if (ps->relax_params.size() != 0) {
    if (rdim != 0 && rdim != 3) {
      ps->relax_params[3 * i] = rparam[0];
      ps->relax_params[3 * i + 1] = rparam[1];
      ps->relax_params[3 * i + 2] = rparam[2];
    } else {
      ps->relax_params[3 * i] = 0.0;
      ps->relax_params[3 * i + 1] = 0.0;
      ps->relax_params[3 * i + 2] = 0.0;
    }
  }
}

void
ew::DataflowForm3::remove_pointset(int n)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "remove_pointset",
   ew::DataflowNode::change_cycle);
  if (n < 0 || n >= VectorSize(inp_data.pointsets)) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  ew::Form3PointSet *ps = &inp_data.pointsets[n];
  if (ps->type <= ew::Form3::TYPE_SEMI_LANDMARK) {
    change_cycle_coords = change_cycle_relax = change_cycle_association =
     ew::DataflowNode::change_cycle;
  }
  VectorErase(inp_data.pointsets, n);
}

int
ew::DataflowForm3::set_embedding(bool *replaced,
 const ew::Form3Embedding *em)
{
  int n;
  bool repl = inp_data.search_embedding(&n, em->subset_id.c_str(),
   em->superset_id.c_str());
  if (!repl || inp_data.embeddings[n] != *em) {
    ew::DataflowNode::record_change();
    dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_embedding",
     ew::DataflowNode::change_cycle);
    if (!repl) {
      VectorInsert(inp_data.embeddings, n, *em);
    } else {
      inp_data.embeddings[n] = *em;
    }
  }
  *replaced = repl;
  return n;
}

void
ew::DataflowForm3::remove_embedding(int n)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "remove_reflection",
   ew::DataflowNode::change_cycle);
  if (n < 0 || n >= VectorSize(inp_data.embeddings)) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  VectorErase(inp_data.embeddings, n);
}

int
ew::DataflowForm3::set_reflection(bool *replaced,
 const ew::Form3Reflection *rf)
{
  int n;
  bool repl = inp_data.search_reflection(&n, rf->left_id.c_str(),
   rf->right_id.c_str());
  if (!repl || inp_data.reflections[n] != *rf) {
    ew::DataflowNode::record_change();
    dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_reflection",
     ew::DataflowNode::change_cycle);
    if (!repl) {
      VectorInsert(inp_data.reflections, n, *rf);
    } else {
      inp_data.reflections[n] = *rf;
    }
  }
  *replaced = repl;
  return n;
}

void
ew::DataflowForm3::remove_reflection(int n)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "remove_reflection",
   ew::DataflowNode::change_cycle);
  if (n < 0 || n >= VectorSize(inp_data.reflections)) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  VectorErase(inp_data.reflections, n);
}

void
ew::DataflowForm3::update_helper() const
{
  ew::DataflowNode::outp_version = ew::DataflowNode::change_cycle;
  if (ew::DataflowNode::outp_version > ew::DataflowNode::update_cycle) {
    ew::DataflowNode::outp_valid = true;
    if (dbg.on) {
      dbg.dprintf("%s::%s UPDATE version=%lu", dbg.fn, "update_helper",
       ew::DataflowNode::outp_version);
      dbg.dprintf("%s::%s UPDATE valid=%s", dbg.fn, "update_helper",
       ew::Debug::to_str(ew::DataflowNode::outp_valid));
    }
  }
}

void
ew::DataflowForm3::update_bbox() const
{
  dbg.on && dbg.dprintf("%s::%s UPDATE", dbg.fn, "update_bbox");
  outp_bbox = ew::Bbox3::empty_box;
  for (int i = 0; i < VectorSize(inp_data.pointsets); i += 1) {
    for (int j = 0; j < VectorSize(inp_data.pointsets[i].locations);
     j += 3) {
      outp_bbox.add(&inp_data.pointsets[i].locations[j]);
    }
  }
}
