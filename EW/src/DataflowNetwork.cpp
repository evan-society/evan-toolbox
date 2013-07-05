// wdkg 2008-2010

#include <climits>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/Curve3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNotifier.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowCurve3E.h"
#include "ew/DataflowSurface3E.h"
#include "ew/ErrorRuntime.h"
#include "ew/String.h"
#include "ew/Surface3.h"

// It is perfectly safe to create more than one network, simultaneously or
// serially..
// In normal use, however, only one will be created in an application.
// It is not worth the visual distraction to have debug messages containing
// ew::DataflowNetwork object numbers.

namespace {
  const char ClassName[] = "ew::DataflowNetwork";

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

ew::DataflowNetwork::DataflowNetwork() :
 dbg(ClassName),
 dbg_last_node_n(0),
 dbg_last_widget_n(0),
 first_notifier(0),
 last_notifier(0),
 cycle(1),
 is_updating(false)
{
  if (dbg.on) {
    dbg.dprintf("%s::%s() {", dbg.fn, "CTOR");
    dbg.dprintf("%s::%s   cycle=%lu updating=%s", dbg.fn, "CTOR", cycle,
     ew::Debug::to_str(is_updating));
    dbg.dprintf("%s::%s }", dbg.fn, "CTOR");
  }
}

ew::DataflowNetwork::~DataflowNetwork()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "DTOR");
  for (int i = 0; i < VectorSize(cached_surfaces); i += 1) {
    cached_surfaces[i]->decr_ref_count();
  }
  for (int i = 0; i < VectorSize(cached_curves); i += 1) {
    cached_curves[i]->decr_ref_count();
  }
// Mark handlers as unregistered, so they don't try to unregister themselves
// later.
  for (ew::DataflowNotifier *n = first_notifier; n != 0;
   n = n->next_notifier) {
    n->is_registered = false;
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "DTOR");
}

// This will typically be the first step in making a change to a node.
// The notifier code is optimized to make this fast.

void
ew::DataflowNetwork::start_changing_helper()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "start_changing_helper");
  if (is_updating) {
    cycle += 1;
    if (cycle == 0) {
// unlikely
      cycle -= 1;
//XXX document this exception
      throw ew::ErrorRuntime(
       "A program limitation has been exceeded"
       " (the number of dataflow network cycles).");
    }
    is_updating = false;
  }
// Handlers are not allowed to re-register immediately, to allow a more
// efficient implementation.
  while (first_notifier != 0) {
// first_notifier and last_notifier are only pretending to be
// ew::Dataflow_notifier *'s.
    ew::DataflowNotifier *n = first_notifier;
    n->notifier_handle_cb();
    n->is_registered = false;
    first_notifier = n->next_notifier;
  }
  last_notifier = 0;
  if (dbg.on) {
    dbg.dprintf("%s::%s   cycle=%lu updating=%s", dbg.fn,
     "start_changing_helper", cycle, ew::Debug::to_str(is_updating));
    dbg.dprintf("%s::%s }", dbg.fn, "start_changing_helper");
  }
}

// This is mainly just to dprintf.

void
ew::DataflowNetwork::start_updating_helper()
{
  is_updating = true;
  if (dbg.on) {
    dbg.dprintf("%s::%s() {", dbg.fn, "start_updating_helper");
    dbg.dprintf("%s::%s   cycle=%lu updating=%s", dbg.fn,
     "start_updating_helper", cycle, ew::Debug::to_str(is_updating));
    dbg.dprintf("%s::%s }", dbg.fn, "start_updating_helper");
  }
}

const ew::DataflowCurve3E *
ew::DataflowNetwork::cached_curve(const char *filename)
{
  dbg.on && dbg.dprintf("%s::%s(\"%s\") {", dbg.fn, "cached_curve", filename);
  ew::DataflowCurve3E *cur = 0;
  for (int i = 0; i < VectorSize(cached_curves); i += 1) {
    if (std::strcmp(cached_curve_names[i].c_str(), filename) == 0) {
      cur = cached_curves[i];
// Don't break so we get the latest, if at some point we support reloading
// files when they change.
    }
  }
  if (cur != 0) {
    cur->ew::DataflowNode::incr_ref_count();
  } else {
    dbg.on && dbg.dprintf("%s::%s   reading-file", dbg.fn, "cached_curve");
    ew::Curve3 curdata;
// This will fail if curve is empty.
    curdata.read_file(filename);
    cur = new ew::DataflowCurve3E(this);
//XXX use auto_node for safety
    cur->set_data(&curdata);
    cached_curves.push_back(cur);
    cached_curve_names.push_back(filename);
// This makes 2, one reference returned, one for the cache.
    cur->ew::DataflowNode::incr_ref_count();
  }
  dbg.on && dbg.dprintf("%s::%s } (%s)", dbg.fn, "cached_curve",
   cur->ew::DataflowNode::dbg.in);
  return cur;
}

const ew::DataflowSurface3E *
ew::DataflowNetwork::cached_surface(const char *filename)
{
  dbg.on && dbg.dprintf("%s::%s(\"%s\") {", dbg.fn, "cached_surface",
   filename);
  ew::DataflowSurface3E *sur = 0;
  for (int i = 0; i < VectorSize(cached_surfaces); i += 1) {
    if (std::strcmp(cached_surface_names[i].c_str(), filename) == 0) {
      sur = cached_surfaces[i];
// Don't break so we get the latest, for when, at some point, we support
// reloading files when they change.
    }
  }
  if (sur != 0) {
    sur->ew::DataflowNode::incr_ref_count();
  } else {
    dbg.on && dbg.dprintf("%s::%s   reading-file", dbg.fn, "cached_surface");
    ew::Surface3 surdata;
    surdata.read_file(filename);
    sur = new ew::DataflowSurface3E(this);
//XXX use auto_node for safety
    sur->set_data(&surdata);
    cached_surfaces.push_back(sur);
    cached_surface_names.push_back(filename);
// This makes 2, one reference returned, one for the cache.
    sur->ew::DataflowNode::incr_ref_count();
  }
  dbg.on && dbg.dprintf("%s::%s } (%s)", dbg.fn, "cached_surface",
   sur->ew::DataflowNode::dbg.in);
  return sur;
}

#ifdef XXX
// currently unused
void
ew::DataflowNetwork::uncache_surface(ew::DataflowSurface3E *sur)
{
  for (int i = 0; i < VectorSize(cached_surfaces); i += 1) {
    if (cached_surfaces[i] == sur) {
      VectorErase(cached_surfaces, i);
      VectorErase(cached_surface_names, i);
      break;
    }
  }
}
#endif
