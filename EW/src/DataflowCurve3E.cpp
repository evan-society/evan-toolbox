// wdkg 2008-2010

#include <string>
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/Curve3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowCurve3.h"
#include "ew/DataflowCurve3E.h"
#include "ew/String.h"

namespace {
  const char ClassName[] = "ew::DataflowCurve3E";
  const char InstanceName[] = "curve3";

  template<typename T>
  inline const T*
  VectorData(const std::vector<T> &v)
  {
    if (v.empty()) {
      return 0;
    } else {
      return &(v[0]);
    }
  }
}

ew::DataflowCurve3E::DataflowCurve3E(
 ew::DataflowNetwork *i_network) :
 ew::DataflowCurve3(i_network, 0, InstanceName, 0),
 dbg(ClassName, ew::DataflowNode::dbg)
{
}

void
ew::DataflowCurve3E::reset()
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "reset",
   ew::DataflowNode::change_cycle);
  inp_data.reset();
  update_cycle_index = 0;
  reset_index();
  update_cycle_tangents = 0;
  reset_tangents();
}

void
ew::DataflowCurve3E::set_data(ew::Curve3 *surdata)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_data",
   ew::DataflowNode::change_cycle);
  inp_data.reset();
  inp_data.swap(*surdata);
  update_cycle_index = 0;
  reset_index();
  update_cycle_tangents = 0;
  reset_tangents();
}

void
ew::DataflowCurve3E::update_helper() const
{
//XXX support file changes/reload on request
  ew::DataflowNode::outp_version = ew::DataflowNode::change_cycle;
  if (ew::DataflowNode::outp_version > ew::DataflowNode::update_cycle) {
    ew::DataflowCurve3::outp_num_points = inp_data.points.size() / 3;
    ew::DataflowCurve3::outp_num_edges = inp_data.edges.size() / 2;
    ew::DataflowCurve3::outp_points = VectorData(inp_data.points);
    ew::DataflowCurve3::outp_edges = VectorData(inp_data.edges);
    ew::DataflowNode::outp_valid = (ew::DataflowCurve3::outp_num_edges != 0);
    if (dbg.on) {
      dbg.dprintf("%s::%s version=%lu", dbg.fn, "update_helper",
       ew::DataflowNode::outp_version);
      dbg.dprintf("%s::%s valid=%s", dbg.fn, "update_helper",
       ew::Debug::to_str(ew::DataflowNode::outp_valid));
    }
  }
}
