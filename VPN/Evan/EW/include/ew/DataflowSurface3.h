#ifndef EW_DATAFLOWSURFACE3_H_INCLUDED
#define EW_DATAFLOWSURFACE3_H_INCLUDED

// wdkg 2008-2010

//XXX separate dlist for faces for use by warped surface
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

namespace ew {
// XXX forward declaration w/o include
  class Index3;
// XXX forward declaration w/o include
  class View3Surface;

/// @brief Surface Node Base Class
///
/// @headerfile ew/DataflowSurface3.h "ew/DataflowSurface3.h"
/// ew::DataflowSurface3 is the base class for nodes containing triangulated
/// surfaces in @f$\mathbb{R}^3@f$.
///
/// ew::DataflowSurface3 is a class without assignment or comparison.
/// There are private member variables.
///
/// For efficient rendering of the same surface in multiple windows, an OpenGL
/// display list is constructed containing the basic surface geometry commands.
/// All windows displaying a given surface must share display lists.

  class DataflowSurface3 : public ew::DataflowNode {
  public:
    inline int get_num_points() const;
    inline int get_num_faces() const;
    inline const float *get_points() const;
    inline const int *get_faces() const;
    inline const ew::Bbox3 *get_bbox() const;
    inline const float *get_normals() const;
    inline void make_index() const;
    inline bool index_is_made() const;
    double project(int *face, double *coeffs, double *normal, double *proj,
     const double *inp) const;
// private to library
    mutable ew::FriendVariable<unsigned int, ew::View3Surface> dlist;
    mutable ew::FriendVariable<unsigned long, ew::View3Surface>
     dlist_cycle;
    mutable ew::FriendVariable<int, ew::View3Surface> dlist_refs;
    const ew::DebugHelper dbg;
  protected:
    DataflowSurface3(ew::DataflowNetwork *i_network, int ndeps,
     const char *dbg_final_name,
     const char *const *const i_dbg_dependency_names);
    ~DataflowSurface3();
    mutable int outp_num_points;
    mutable int outp_num_faces;
    mutable const float *outp_points;
    mutable const int *outp_faces;
    void reset_normals() const;
    void reset_index() const;
//XXX make these private (needed for reset)
    mutable unsigned long update_cycle_index;
    mutable unsigned long update_cycle_normals;
  private:
    void update_bbox() const;
    void update_normals() const;
    void update_index() const;
    mutable ew::Bbox3 outp_bbox;
    mutable std::vector<float> outp_normals;
    const std::auto_ptr<ew::Index3> outp_index;
    mutable unsigned long update_cycle_bbox;
  };

}

// index is a pointer just to avoid having to define ew::Index3 in the
// exported header files.

/// @fn int ew::DataflowSurface3::get_num_points() const
/// This will force the network into an updating phase.
/// @return
///   The number of points used in the surface.

/// @fn int ew::DataflowSurface3::get_num_faces() const
/// This will force the network into an updating phase.
/// @return
///   The number of triangular faces in the surface.

/// @fn const float *ew::DataflowSurface3::get_points() const
/// This will force the network into an updating phase.
/// The pointer is valid until the node or a dependency of the node is changed.
/// @return
///   A pointer to the coordinates of the points used in the surface.

/// @fn const int *ew::DataflowSurface3::get_faces() const
/// This will force the network into an updating phase.
/// The pointer is valid until the node or a dependency of the node is changed.
/// @return
///   A pointer to the indices of the points used in the surface.

/// @fn const ew::Bbox3 *ew::DataflowSurface3::get_bbox() const
/// The pointer is valid until the node or a dependency of the node is changed.
/// @return
///   A pointer to the bounding box of the surface.

/// @fn inline void ew::DataflowSurface3::make_index() const
/// This makes the surface spatial index if it has not already been made since
/// the surface was last changed.
/// The spatial index is used by #project.

/// @fn inline bool ew::DataflowSurface3::index_is_made() const;
/// @return
///   @c true if the surface spatial index is up to date.

/// @fn double ew::DataflowSurface3::project(int *face, double *coeffs,
///  double *normal, double *proj, const double *inp) const
/// This finds the nearest point on the surface to a given point.
/// The surface spatial index will be made if it has not already been made.
/// This might be time consuming.
/// The timing of this delay can be controlled by calling #make_index
/// beforehand.
/// @param[out] face
///   The index of the face of the surface containing the nearest point.
/// @param[out] coeffs
///   The coefficients of the nearest point when expressed as a linear
///    combination of the vertices of the face.
/// @param[out] normal
///   The interpolated normal at the nearest point.
///   If a sensible normal cannot be calculated, an arbitrary unit vector
///   is returned.
/// @param[out] proj
///   The coordinates of the nearest point on the surface.
/// @param[in] inp
///   The coordinates of the original point.
/// @return
///   The distance from the original point to the projected point.

inline int
ew::DataflowSurface3::get_num_points() const
{
  ew::DataflowNode::update();
  return outp_num_points;
}

inline int
ew::DataflowSurface3::get_num_faces() const
{
  ew::DataflowNode::update();
  return outp_num_faces;
}

inline const float *
ew::DataflowSurface3::get_points() const
{
  ew::DataflowNode::update();
  return outp_points;
}

inline const int *
ew::DataflowSurface3::get_faces() const
{
  ew::DataflowNode::update();
  return outp_faces;
}

inline const ew::Bbox3 *
ew::DataflowSurface3::get_bbox() const
{
  unsigned long v = ew::DataflowNode::get_version();
  if (update_cycle_bbox < v) {
    update_bbox();
    update_cycle_bbox = network->get_cycle();
  }
  return &outp_bbox;
}

inline const float *
ew::DataflowSurface3::get_normals() const
{
  unsigned long v = ew::DataflowNode::get_version();
  if (update_cycle_normals < v) {
    update_normals();
    update_cycle_normals = network->get_cycle();
  }
  if (outp_normals.empty()) {
    return 0;
  } else {
    return &(outp_normals[0]);
  }
}

inline void
ew::DataflowSurface3::make_index() const
{
  unsigned long v = ew::DataflowNode::get_version();
  if (update_cycle_index < v) {
    update_index();
    update_cycle_index = network->get_cycle();
  }
}

inline bool
ew::DataflowSurface3::index_is_made() const
{
  return (update_cycle_index >= ew::DataflowNode::get_version());
}

#endif
