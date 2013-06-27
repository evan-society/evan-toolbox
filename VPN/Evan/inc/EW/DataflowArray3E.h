#ifndef EW_DATAFLOWARRAY3E_H_INCLUDED
#define EW_DATAFLOWARRAY3E_H_INCLUDED

// wdkg 2008-2010

#include "ew/DebugHelper.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowArray3.h"
#include "ew/Transform3.h"

//XXX make points

namespace ew {

// @brief Explicit Point Array Node
//
// @headerfile ew/DataflowArray3E.h "ew/DataflowArray3E.h"
// ew::DataflowArray3E is a node containing an exlicitly specified
// regular array of points.
//
// ew::DataflowArray3E is a class without assignment or comparison.
// There are private member variables.
//
// Initially and when reset, the node has no points.
// In an explicit array the sizes are parameters.
// An explicit array is valid iff it has some points.
//
// Nodes of this class are input only: other nodes may depend on them but they
// never depend on other nodes.

  class DataflowArray3E : public ew::DataflowArray3 {
  public:
    explicit DataflowArray3E(ew::DataflowNetwork *i_network);
    virtual void reset();
    void set_explicit_size(const int *n);
    inline const int *get_explicit_size() const;
    void set_stretch(const double *str);
    inline const double *get_stretch() const;
    void set_mapping(const ew::Transform3 *tr);
    inline const ew::Transform3 *get_mapping() const;
    const ew::DebugHelper dbg;
  private:
    virtual void update_helper() const;
    int inp_size[3];
    double inp_stretch[3];
    ew::Transform3 inp_mapping;
  };

}

// @fn explicit ew::DataflowArray3E::DataflowArray3E(
//  ew::DataflowNetwork *i_network)
// This creates an explicit array node.
// @param i_network
//   The network this node should belong to.

// These are named to disambiguate ew::DataflowArray3::get_size().

// @fn void ew::DataflowArray3E::set_explicit_size(const int *n)
// This sets the size of the array.
// Each component of the size must be positive.
// 2 dimensional arrays are constucted by setting the last dimension to 1.
// Use ew::DataflowNode::reset to set all sizes to 0.
// @param n
//   This points to an array of size 3 containing the new sizes.

// @fn const int *ew::DataflowArray3E::get_explicit_size() const
// @return
//   A pointer to the current array size.

// @fn void ew::DataflowArray3E::set_stretch(const double *str)
// str points to an array of size 3
//XXX interpretation

// @fn const double *ew::DataflowArray3E::get_stretch() const
// @return
//   A pointer to the current stretch parameters.

// @fn void ew::DataflowArray3E::set_mapping(const ew::Transform3 *tr)
//XXX interpretation

// @fn const ew::Transform3 *ew::DataflowArray3E::get_mapping() const
// @return
//   A pointer to the current mapping parameter.

inline const int *
ew::DataflowArray3E::get_explicit_size() const
{
  return inp_size;
}

inline const double *
ew::DataflowArray3E::get_stretch() const
{
  return inp_stretch;
}

inline const ew::Transform3 *
ew::DataflowArray3E::get_mapping() const
{
  return &inp_mapping;
}

#endif
