// wdkg 2008-2010

#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/DataflowArray3.h"
#include "ew/DataflowArray3E.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/String.h"
#include "ew/Transform3.h"

namespace {
  const char ClassName[] = "ew::DataflowArray3E";
  const char InstanceName[] = "array3";
}

ew::DataflowArray3E::DataflowArray3E(
 ew::DataflowNetwork *i_network) :
 ew::DataflowArray3(i_network, 0, InstanceName, 0),
 dbg(ClassName, ew::DataflowNode::dbg),
 inp_mapping(ew::Transform3::identity_transform)
{
  inp_size[0] = 1;
  inp_size[1] = 1;
  inp_size[2] = 1;
//XXX  ew::DataflowArray3::scale = 1.0;
  inp_stretch[0] = 1.0;
  inp_stretch[1] = 1.0;
  inp_stretch[2] = 1.0;
}

void
ew::DataflowArray3E::reset()
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "reset",
   ew::DataflowNode::change_cycle);
  inp_stretch[0] = 1.0;
  inp_stretch[1] = 1.0;
  inp_stretch[2] = 1.0;
  inp_size[0] = 1;
  inp_size[1] = 1;
  inp_size[2] = 1;
  inp_mapping.set_to_identity();
}

// Precondition that sizes >= 1.
//XXX ? >= 221

void
ew::DataflowArray3E::set_explicit_size(const int *n)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_explicit_size",
   ew::DataflowNode::change_cycle);
  inp_size[0] = n[0];
  inp_size[1] = n[1];
  inp_size[2] = n[2];
}

void
ew::DataflowArray3E::set_mapping(const ew::Transform3 *tr)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_mapping",
   ew::DataflowNode::change_cycle);
  inp_mapping = *tr;
//XXX ew::DataflowArray3::scale =
}

void
ew::DataflowArray3E::set_stretch(const double *str)
{
  ew::DataflowNode::record_change();
  dbg.on && dbg.dprintf("%s::%s CHANGE=%lu", dbg.fn, "set_stretch",
   ew::DataflowNode::change_cycle);
//XXX check good parameters
//XXX ew::DataflowArray3::scale =
  inp_stretch[0] = str[0];
  inp_stretch[1] = str[1];
  inp_stretch[2] = str[2];
}

void
ew::DataflowArray3E::update_helper() const
{
  ew::DataflowNode::outp_version = ew::DataflowNode::change_cycle;
  if (ew::DataflowNode::outp_version > ew::DataflowNode::update_cycle) {
    ew::DataflowNode::outp_valid = true;
    ew::DataflowArray3::outp_size = inp_size;
    if (dbg.on) {
      dbg.dprintf("%s::%s UPDATE version=%lu", dbg.fn, "update_helper",
       ew::DataflowNode::outp_version);
      dbg.dprintf("%s::%s UPDATE valid=%s", dbg.fn, "update_helper",
       ew::Debug::to_str(ew::DataflowNode::outp_valid));
    }
  }
}
