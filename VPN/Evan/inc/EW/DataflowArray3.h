#ifndef EW_DATAFLOWARRAY3_H_INCLUDED
#define EW_DATAFLOWARRAY3_H_INCLUDED

// wdkg 2008-2010

#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DebugHelper.h"

namespace ew {

// @brief Point Array Node Base Class
//
// @headerfile ew/DataflowArray3.h "ew/DataflowArray3.h"
// ew::DataflowArray3 is the base class for nodes containing a 3 dimensional
// array of points in @f$\mathbb{R}^3@f$.
//
// ew::DataflowArray3 is a class without assignment or comparison.
// There are private member variables.

  class DataflowArray3 : public ew::DataflowNode {
  public:
    inline const int *get_size() const;
    inline const ew::Bbox3 *get_bbox() const;
    const ew::DebugHelper dbg;
//XXX  inline double get_scale() const;
  protected:
    DataflowArray3(ew::DataflowNetwork *i_network, int ndeps,
     const char *dbg_final_name,
     const char *const *const i_dbg_dependency_names);
    mutable const int *outp_size;
//XXX  mutable double outp_scale;
  private:
    void update_bbox() const;
    mutable ew::Bbox3 outp_bbox;
    mutable unsigned long update_cycle_bbox;
  };

}

// @fn const int *ew::DataflowArray3::get_size()
// This will force the network into an updating phase.
// The pointer is valid until the node or a dependency of the node is changed.
// @return
//   A pointer to an integer array of length 3 containing the dimensions of
//   the point array.

// @fn const ew::Bbox3 *ew::DataflowArray3::get_bbox()
// The pointer is valid until the node or a dependency of the node is changed.
// @return
//   A pointer to the bounding box of the points in the array.

inline const int *
ew::DataflowArray3::get_size() const
{
  ew::DataflowNode::update();
  return outp_size;
}

#ifdef XXX
inline double
ew::DataflowArray3::get_scale() const
{
  ew::DataflowNode::update();
  return outp_scale;
}
#endif

inline const ew::Bbox3 *
ew::DataflowArray3::get_bbox() const
{
  unsigned long v = ew::DataflowNode::get_version();
  if (update_cycle_bbox < v) {
    update_bbox();
    update_cycle_bbox = network->get_cycle();
  }
  return &outp_bbox;
}

#endif
