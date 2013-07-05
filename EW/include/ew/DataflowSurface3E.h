#ifndef EW_DATAFLOWSURFACE3E_H_INCLUDED
#define EW_DATAFLOWSURFACE3E_H_INCLUDED

// wdkg 2008-2010

//XXX ? uncache

#include <string>
#include "ew/DebugHelper.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowSurface3.h"
#include "ew/Surface3.h"

namespace ew {

/// @brief Explicit Surface Node
///
/// @headerfile ew/DataflowSurface3E.h "ew/DataflowSurface3E.h"
/// ew::DataflowSurface3E is a node containing an explicitly specified
/// surface.
///
/// ew::DataflowSurface3E objects may not be copied or compared.
/// There are private member variables.
///
/// Nodes of this class do not depend on other nodes, but other nodes can
/// depend on them.
/// They are valid if they have some faces.
///
/// Initially and when reset, the node is empty.

  class DataflowSurface3E : public ew::DataflowSurface3 {
  public:
    explicit DataflowSurface3E(ew::DataflowNetwork *i_network);
    virtual void reset();
    void set_data(ew::Surface3 *data);
    inline const ew::Surface3 *get_data() const;
    const ew::DebugHelper dbg;
  private:
    virtual void update_helper() const;
    ew::Surface3 inp_data;
  };

}

/// @fn ew::DataflowSurface3E::DataflowSurface3E(
///  ew::DataflowNetwork *i_network)
/// This creates an explicit surface node.
/// @param i_network
///   The network this node should belong to.

/// @fn void ew::DataflowSurface3E::set_data(ew::Surface3 *data)
/// This transfers the surface data from the ew::Surface3 object to the node.
/// Afterwards, the ew::Surface3 object is reset.
/// @pre This node must not be a cached node.
/// @param data
///   The object containing the surface data.

/// @fn inline const ew::Surface3 *ew::DataflowSurface3E::get_data(
///  ) const
/// @return
///   A pointer to the internal surface data.

inline const ew::Surface3 *
ew::DataflowSurface3E::get_data() const
{
  return &inp_data;
}

#endif
