// wdkg 2008-2010

#include <climits>
#include <stdexcept>
#include <string>
#include <vector>
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/ErrorLogic.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/ErrorRuntime.h"
#include "ew/String.h"

namespace {
  const char ClassName[] = "ew::DataflowNode";

  template<typename T>
  inline int
  VectorSize(const std::vector<T> &v)
  {
    return static_cast<int>(v.size());
  }
}

ew::DataflowNode::DataflowNode(ew::DataflowNetwork *i_network, int ndeps,
 const char *dbg_final_name,
 const char *const *const i_dbg_dependency_names) :
 network(i_network),
 dbg_dependency_names(i_dbg_dependency_names),
 dbg_n(next_dbg_n()),
 dbg(ClassName, ew::String::ssprintf("%s#%d", dbg_final_name, dbg_n)),
 change_cycle(0),
 inp_dependencies(ndeps, static_cast<ew::DataflowNode *>(0)),
 change_cycle_dependencies(ndeps),
 update_cycle(0),
 ref_count(1)
{
  dbg.on && dbg.dprintf("%s::%s(...) {", dbg.fn, "CTOR");
  record_change();
// This must be done after record_change.
  for (int i = 0; i < ndeps; i += 1) {
    change_cycle_dependencies[i] = change_cycle;
  }
  if (dbg.on) {
    dbg.dprintf("%s::%s   CHANGE=%lu", dbg.fn, "CTOR", change_cycle);
    dbg.dprintf("%s::%s }", dbg.fn, "CTOR");
  }
}

// Reference count here is zero, so there is no change to the network.

ew::DataflowNode::~DataflowNode()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "DTOR");
  for (int i = 0; i < VectorSize(inp_dependencies); i += 1) {
    if (inp_dependencies[i] != 0) {
      inp_dependencies[i]->decr_ref_count();
    }
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "DTOR");
}

int
ew::DataflowNode::next_dbg_n()
{
  if (network->dbg_last_node_n == INT_MAX) {
// unlikely
// tolerate but indicate overflow
    return 0;
  }
  network->dbg_last_node_n += 1;
  return network->dbg_last_node_n;
}

void
ew::DataflowNode::incr_ref_count() const
{
  ref_count += 1;
  dbg.on && dbg.dprintf("%s::%s ref_count=%u", dbg.fn, "incr_ref_count",
   ref_count);
}

void
ew::DataflowNode::decr_ref_count() const
{
  ref_count -= 1;
  dbg.on && dbg.dprintf("%s::%s ref_count=%u", dbg.fn, "decr_ref_count",
   ref_count);
  if (ref_count <= 0) {
    delete this;
  }
}

bool
ew::DataflowNode::depends_on(ew::DataflowNode *node) const
{
  if (node == this) {
    return true;
  }
  for (int i = 0; i < VectorSize(inp_dependencies); i += 1) {
    if (inp_dependencies[i] != 0 && inp_dependencies[i]->depends_on(node)) {
      return true;
    }
  }
  return false;
}

// This assumes record_change() has already been called.

void
ew::DataflowNode::set_dependency(int i, const ew::DataflowNode *dep)
{
  dbg.on && dbg.dprintf("%s::%s(%s,...) {", dbg.fn, "set_dependency",
   dbg_dependency_names[i]);
  if (dep != inp_dependencies[i]) {
    dbg.on && dbg.dprintf("%s::%s   %s->%s", dbg.fn, "set_dependency",
     ew::DataflowNode::to_str(dep),
     ew::DataflowNode::to_str(inp_dependencies[i]));
    if (dep != 0) {
      if (dep->network != network) {
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
      if (dep->depends_on(this)) {
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
    }
    change_cycle_dependencies[i] = change_cycle;
    if (dep != 0) {
      dep->incr_ref_count();
    }
    if (inp_dependencies[i] != 0) {
      inp_dependencies[i]->decr_ref_count();
    }
    inp_dependencies[i] = dep;
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "set_dependency");
}

const char *
ew::DataflowNode::to_str(const ew::DataflowNode *n)
{
  if (n == 0) {
    return "NULL";
  } else {
    return n->dbg.in;
  }
}
