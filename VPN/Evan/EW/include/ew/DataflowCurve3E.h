#ifndef EW_DATAFLOWCURVE3E_H_INCLUDED
#define EW_DATAFLOWCURVE3E_H_INCLUDED

// wdkg 2008-2010

//XXX ? uncache

#include <string>
#include "ew/DebugHelper.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowCurve3.h"
#include "ew/Curve3.h"

namespace ew {
/// @brief Explicit Curve Node
///
/// @headerfile ew/DataflowCurve3E.h "ew/DataflowCurve3E.h"
/// ew::DataflowCurve3E is a node containing an explicitly specified
/// curve.
///
/// ew::DataflowCurve3E objects may not be copied or compared.
/// There are private member variables.
///
/// Nodes of this class do not depend on other nodes, but other nodes can
/// depend on them.
/// They are valid if they have some faces.
///
/// Initially and when reset, the node is empty.
  class DataflowCurve3E : public ew::DataflowCurve3 {
  public:
/// This creates an explicit curve node.
/// @param i_network
///   The network this node should belong to.
    explicit DataflowCurve3E(ew::DataflowNetwork *i_network);
    virtual void reset();
/// This transfers the curve data from the ew::Curve3 object to the node.
/// Afterwards, the ew::Curve3 object is reset.
/// @pre This node must not be a cached node.
/// @param data
///   The object containing the curve data.
    void set_data(ew::Curve3 *data);
/// @return
///   A pointer to the internal curve data.
    inline const ew::Curve3 *get_data() const;
    const ew::DebugHelper dbg;
  private:
    virtual void update_helper() const;
    ew::Curve3 inp_data;
  };

}

inline const ew::Curve3 *
ew::DataflowCurve3E::get_data() const
{
  return &inp_data;
}

#endif
