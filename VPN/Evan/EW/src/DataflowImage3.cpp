// wdkg 2008-2010

#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/DataflowArray3.h"
#include "ew/DataflowData3.h"
#include "ew/DataflowImage3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/String.h"

namespace {
  const char ClassName[] = "ew::DataflowImage3";
  const char InstanceName[] = "image3";
  enum {DEP_ARRAY, DEP_DATA};
  const char *const DepNames[] = {"array", "data"};
}

ew::DataflowImage3::DataflowImage3(ew::DataflowNetwork *i_network) :
 ew::DataflowNode(i_network, 2, InstanceName, DepNames),
 dbg(ClassName, ew::DataflowNode::dbg)
{
}

void
ew::DataflowImage3::reset()
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "reset",
   ew::DataflowNode::change_cycle);
  ew::DataflowNode::set_dependency(DEP_ARRAY, 0);
  ew::DataflowNode::set_dependency(DEP_DATA, 0);
}

void
ew::DataflowImage3::set_array_node(const ew::DataflowArray3 *arr)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_array_node",
   ew::DataflowNode::change_cycle);
  ew::DataflowNode::set_dependency(DEP_ARRAY, arr);
}

void
ew::DataflowImage3::set_data_node(const ew::DataflowData3 *dat)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_data_node",
   ew::DataflowNode::change_cycle);
  ew::DataflowNode::set_dependency(DEP_DATA, dat);
}

void
ew::DataflowImage3::update_helper() const
{
  ew::DataflowNode::outp_version = ew::DataflowNode::change_cycle;
  if (get_array_node() != 0) {
    unsigned long v = get_array_node()->ew::DataflowNode::get_version();
    if (ew::DataflowNode::outp_version < v) {
      ew::DataflowNode::outp_version = v;
    }
  }
  if (get_data_node() != 0) {
    unsigned long v = get_data_node()->ew::DataflowNode::get_version();
    if (ew::DataflowNode::outp_version < v) {
      ew::DataflowNode::outp_version = v;
    }
  }
  if (ew::DataflowNode::outp_version > ew::DataflowNode::update_cycle) {
    if (get_array_node() == 0 ||
     !get_array_node()->ew::DataflowNode::get_valid()) {
//XXX get_data_node() == 0 ||
//XXX !get_data_node()->ew::DataflowNode::get_valid() ||
      ew::DataflowNode::outp_valid = false;
    } else {
      const int *as = get_array_node()->get_size();
      ew::DataflowNode::outp_valid = (
       as[0] > 1 &&
       as[1] > 1);
//XXX get_data_node()->get_size()[0] > 1 &&
//XXX get_data_node()->get_size()[1] > 1 &&
//XXX (as[2] > 1) == (get_data_node()->get_size()[2] > 1) &&
    }
    if (dbg.on) {
      dbg.dprintf("%s::%s UPDATE version=%lu", dbg.fn, "update_helper",
       ew::DataflowNode::outp_version);
      dbg.dprintf("%s::%s UPDATE valid=%s", dbg.fn, "update_helper",
       ew::Debug::to_str(ew::DataflowNode::outp_valid));
    }
  }
}
