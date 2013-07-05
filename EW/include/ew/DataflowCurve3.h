#ifndef EW_DATAFLOWCURVE3_H_INCLUDED
#define EW_DATAFLOWCURVE3_H_INCLUDED

// wdkg 2008-2010

//XXX separate dlist for edges for use by warped curve
//XXX revert to generating dlist here as a update_ method (cf notes_nodes)
//XXX mechanism for optionally responding to file changes
//XXX   check for file modification when load, function for global reload
//XXX check dlist freed when no items using it

#include <memory>
#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DebugHelper.h"
#include "ew/FriendVariable.h"

//XXX
// index is a pointer just to avoid having to define ew::Index3 in the
// exported header files.

namespace ew {
// XXX forward declaration w/o include
  class Index3;
// XXX forward declaration w/o include
  class View3Curve;
/// @brief Curve Node Base Class
///
/// @headerfile ew/DataflowCurve3.h "ew/DataflowCurve3.h"
/// ew::DataflowCurve3 is the base class for nodes containing piecewise-linear
/// curves in @f$\mathbb{R}^3@f$.
///
/// ew::DataflowCurve3 is a class without assignment or comparison.
/// There are private member variables.
///
/// For efficient rendering of the same curve in multiple windows, an OpenGL
/// display list is constructed containing the basic curve geometry commands.
/// All windows displaying a given curve must share display lists.
  class DataflowCurve3 : public ew::DataflowNode {
  public:
/// This will force the network into an updating phase.
/// @return
///   The number of points used in the curve.
    inline int get_num_points() const;
/// This will force the network into an updating phase.
/// @return
///   The number of triangular edges in the curve.
    inline int get_num_edges() const;
/// This will force the network into an updating phase.
/// The pointer is valid until the node or a dependency of the node is changed.
/// @return
///   A pointer to the coordinates of the points used in the curve.
    inline const float *get_points() const;
/// This will force the network into an updating phase.
/// The pointer is valid until the node or a dependency of the node is changed.
/// @return
///   A pointer to the indices of the points used in the curve.
    inline const int *get_edges() const;
/// The pointer is valid until the node or a dependency of the node is changed.
/// @return
///   A pointer to the bounding box of the curve.
    inline const ew::Bbox3 *get_bbox() const;
/// This makes the curve spatial index if it has not already been made since
/// the curve was last changed.
/// The spatial index is used by #project.
    inline void make_index() const;
/// @return
///   @c true if the curve spatial index is up to date.
    inline bool index_is_made() const;
/// This finds the nearest point on the curve to a given point.
/// The curve spatial index will be made if it has not already been made.
/// @param[out] edge
///   The index of the edge of the curve containing the nearest point.
/// @param[out] coeffs
///   The coefficients of the nearest point when expressed as a linear
///    combination of the vertices of the edge.
/// @param[out] tangent
///   The interpolated tangent at the nearest point.
///   If a sensible tangent cannot be calculated, an arbitrary unit vector
///   is returned.
/// @param[out] proj
///   The coordinates of the nearest point on the curve.
/// @param[in] inp
///   The coordinates of the original point.
/// @return
///   The distance from the original point to the projected point.
    double project(int *edge, double *coeffs, double *tangent, double *proj,
     const double *inp) const;
// private to library
    mutable ew::FriendVariable<unsigned int, ew::View3Curve> dlist;
    mutable ew::FriendVariable<unsigned long, ew::View3Curve> dlist_cycle;
    mutable ew::FriendVariable<int, ew::View3Curve> dlist_refs;
    const ew::DebugHelper dbg;
  protected:
    DataflowCurve3(ew::DataflowNetwork *i_network, int ndeps,
     const char *dbg_final_name,
     const char *const *const i_dbg_dependency_names);
    ~DataflowCurve3();
    mutable int outp_num_points;
    mutable int outp_num_edges;
    mutable const float *outp_points;
    mutable const int *outp_edges;
    void reset_tangents() const;
    void reset_index() const;
//XXX make these private (needed for reset)
    mutable unsigned long update_cycle_index;
    mutable unsigned long update_cycle_tangents;
  private:
    void update_bbox() const;
    inline const float *get_tangents() const;
    void update_tangents() const;
    void update_index() const;
    mutable ew::Bbox3 outp_bbox;
    mutable std::vector<float> outp_tangents;
    const std::auto_ptr<ew::Index3> outp_index;
    mutable unsigned long update_cycle_bbox;
  };

}

inline int
ew::DataflowCurve3::get_num_points() const
{
  ew::DataflowNode::update();
  return outp_num_points;
}

inline int
ew::DataflowCurve3::get_num_edges() const
{
  ew::DataflowNode::update();
  return outp_num_edges;
}

inline const float *
ew::DataflowCurve3::get_points() const
{
  ew::DataflowNode::update();
  return outp_points;
}

inline const int *
ew::DataflowCurve3::get_edges() const
{
  ew::DataflowNode::update();
  return outp_edges;
}

inline const ew::Bbox3 *
ew::DataflowCurve3::get_bbox() const
{
  unsigned long v = ew::DataflowNode::get_version();
  if (update_cycle_bbox < v) {
    update_bbox();
    update_cycle_bbox = network->get_cycle();
  }
  return &outp_bbox;
}

inline const float *
ew::DataflowCurve3::get_tangents() const
{
  unsigned long v = ew::DataflowNode::get_version();
  if (update_cycle_tangents < v) {
    update_tangents();
    update_cycle_tangents = network->get_cycle();
  }
  if (outp_tangents.empty()) {
    return 0;
  } else {
    return &(outp_tangents[0]);
  }
}

inline void
ew::DataflowCurve3::make_index() const
{
  unsigned long v = ew::DataflowNode::get_version();
  if (update_cycle_index < v) {
    update_index();
    update_cycle_index = network->get_cycle();
  }
}

inline bool
ew::DataflowCurve3::index_is_made() const
{
  return (update_cycle_index >= ew::DataflowNode::get_version());
}

#endif
