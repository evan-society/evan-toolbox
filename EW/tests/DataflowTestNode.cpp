// wdkg 2010

#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/Debug.h"
#include "DataflowTestNode.h"

namespace {
  const char ClassName[] = "DataflowTestNode";
  const char InstanceName[] = "test";
  const char *const DepNames[] = {"one", "two"};
}

ewtst::DataflowTestNode::DataflowTestNode(ew::DataflowNetwork *i_network) :
 ew::DataflowNode(i_network, 2, InstanceName, DepNames),
 dbg(ClassName, ew::DataflowNode::dbg),
 inp_n(0)
{
}

void
ewtst::DataflowTestNode::reset()
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "reset",
   ew::DataflowNode::change_cycle);
  inp_n = 0;
  set_dep1(0);
  set_dep2(0);
}

void
ewtst::DataflowTestNode::set_dep1(const ewtst::DataflowTestNode *dep)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_dep1",
   ew::DataflowNode::change_cycle);
  ew::DataflowNode::set_dependency(0, dep);
}

void
ewtst::DataflowTestNode::set_dep2(const ewtst::DataflowTestNode *dep)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_dep2",
   ew::DataflowNode::change_cycle);
  ew::DataflowNode::set_dependency(1, dep);
}

void
ewtst::DataflowTestNode::set_n(int i)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_n",
   ew::DataflowNode::change_cycle);
  inp_n = i;
}

void
ewtst::DataflowTestNode::update_helper() const
{
  ew::DataflowNode::outp_version = ew::DataflowNode::change_cycle;
  if (get_dep1() != 0) {
    unsigned long v = get_dep1()->ew::DataflowNode::get_version();
    if (ew::DataflowNode::outp_version < v) {
      ew::DataflowNode::outp_version = v;
    }
  }
  if (get_dep2() != 0) {
    unsigned long v = get_dep2()->ew::DataflowNode::get_version();
    if (ew::DataflowNode::outp_version < v) {
      ew::DataflowNode::outp_version = v;
    }
  }
  if (ew::DataflowNode::outp_version > ew::DataflowNode::update_cycle) {
    ew::DataflowNode::outp_valid =
     (inp_n >= 0 && (get_dep1() == 0 || get_dep1()->get_valid()) &&
     (get_dep2() == 0 || get_dep2()->get_valid()));
    if (ew::DataflowNode::outp_valid) {
      outp_sum = inp_n;
      if (get_dep1() != 0 && get_dep1()->get_valid()) {
        outp_sum += get_dep1()->get_sum();
      }
      if (get_dep2() != 0 && get_dep2()->get_valid()) {
        outp_sum += get_dep2()->get_sum();
      }
    }
    if (dbg.on) {
      dbg.dprintf("%s::%s UPDATE version=%lu", dbg.fn, "update_helper",
       ew::DataflowNode::outp_version);
      dbg.dprintf("%s::%s UPDATE valid=%s", dbg.fn, "update_helper",
       ew::Debug::to_str(ew::DataflowNode::outp_valid));
    }
  }
}
