// wdkg 2008-2010

#include "ew/Bbox3.h"
#include "ew/DataflowArray3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"

namespace {
  const char ClassName[] = "ew::DataflowArray3";
}

ew::DataflowArray3::DataflowArray3(ew::DataflowNetwork *i_network,
 int ndeps, const char *dbg_final_name,
 const char *const *const i_dbg_dependency_names) :
 ew::DataflowNode(i_network, ndeps, dbg_final_name, i_dbg_dependency_names),
 dbg(ClassName, ew::DataflowNode::dbg),
 update_cycle_bbox(0)
{
	outp_size = NULL;
}

void
ew::DataflowArray3::update_bbox() const
{
  dbg.on && dbg.dprintf("%s::%s UPDATE", dbg.fn, "update_bbox");
  outp_bbox = ew::Bbox3::empty_box;
//XXX not yet implemented
// for (int i = 0; i < vector_size(pointsets); i += 1) {
// for (j = 0; j < vector_size(pointsets[i].locations); j += 3) {
// outp_bbox.add(&pointsets[i].locations[j]);
// }
// }
}
