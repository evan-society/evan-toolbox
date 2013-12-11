// wdkg 2008-2010

#include <string>
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowSurface3.h"
#include "ew/DataflowSurface3E.h"
#include "ew/String.h"
#include "ew/Surface3.h"

namespace {
  const char ClassName[] = "ew::DataflowSurface3E";
  const char InstanceName[] = "surface3";

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

ew::DataflowSurface3E::DataflowSurface3E(
 ew::DataflowNetwork *i_network) :
 ew::DataflowSurface3(i_network, 0, InstanceName, 0),
 dbg(ClassName, ew::DataflowNode::dbg)
{
}

void
ew::DataflowSurface3E::reset()
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "reset",
   ew::DataflowNode::change_cycle);
  inp_data.reset();
  update_cycle_index = 0;
  reset_index();
  update_cycle_normals = 0;
  reset_normals();
}

void
ew::DataflowSurface3E::set_data(ew::Surface3 *surdata)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_data",
   ew::DataflowNode::change_cycle);
  inp_data.reset();
  inp_data.swap(*surdata);
  update_cycle_index = 0;
  reset_index();
  update_cycle_normals = 0;
  reset_normals();
}

void
ew::DataflowSurface3E::update_helper() const
{
//XXX support file changes/reload on request
  ew::DataflowNode::outp_version = ew::DataflowNode::change_cycle;
  if (ew::DataflowNode::outp_version > ew::DataflowNode::update_cycle) {
    ew::DataflowSurface3::outp_num_points = inp_data.points.size() / 3;
    ew::DataflowSurface3::outp_num_faces = inp_data.faces.size() / 3;
    ew::DataflowSurface3::outp_points = VectorData(inp_data.points);
    ew::DataflowSurface3::outp_faces = VectorData(inp_data.faces);
    ew::DataflowNode::outp_valid =
     (ew::DataflowSurface3::outp_num_faces != 0);
    if (dbg.on) {
      dbg.dprintf("%s::%s version=%lu", dbg.fn, "update_helper",
       ew::DataflowNode::outp_version);
      dbg.dprintf("%s::%s valid=%s", dbg.fn, "update_helper",
       ew::Debug::to_str(ew::DataflowNode::outp_valid));
    }
  }
}
