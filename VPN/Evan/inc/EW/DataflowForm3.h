#ifndef EW_DATAFLOWFORM3_H_INCLUDED
#define EW_DATAFLOWFORM3_H_INCLUDED

// wdkg 2008-2010

#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DebugHelper.h"
#include "ew/Form3.h"
#include "ew/Form3Curve.h"
#include "ew/Form3Embedding.h"
#include "ew/Form3Pointset.h"
#include "ew/Form3Reflection.h"
#include "ew/Form3Surface.h"
#include "ew/Form3Volume.h"

namespace ew {
/// @brief 3D Form Node
///
/// @headerfile ew/DataflowForm3.h "ew/DataflowForm3.h"
/// ew::DataflowForm3 is a node that manages an ew::Form3.
///
/// ew::DataflowForm3 is a class without assignment or comparison.
/// There are private member variables.
///
/// Nodes of this class do not depend on other nodes, but other nodes can
/// depend on them.
/// They are always considered valid.
///
/// Initially and when reset, the node is empty.
  class DataflowForm3 : public ew::DataflowNode {
  public:
/// This creates a form3 node.
/// @param i_network
///   The network this node should belong to.
    explicit DataflowForm3(ew::DataflowNetwork *i_network);
    virtual void reset();
/// This sets the contents of the node.
/// @param data
///   A pointer to the ew::Form3 data to copy.
    void set_data(const ew::Form3 *data);
/// @return
///   A pointer to the current ew::Form3 data contained in the node.
    inline const ew::Form3 *get_data() const;
/// This adds or replaces a volume in the ew::Form3 data managed by the node.
/// @param[out] replaced
///   @c true if the volume replaced an existing volume.
/// @param vl
///   A pointer to the volume data to copy.
/// @return
///   The index of the new volume in ew::Form3::volumes.
    int set_volume(bool *replaced, const ew::Form3Volume *vl);
/// This deletes a volume in the ew::Form3 managed by the node.
/// @param n
///   The index of the volume to delete.
    void remove_volume(int n);
/// This adds or replaces a surface in the ew::Form3 data managed by the node.
/// @param[out] replaced
///   @c true if the surface replaced an existing surface.
/// @param sr
///   A pointer to the surface data to copy.
/// @return
///   The index of the new surface in ew::Form3::surfaces.
    int set_surface(bool *replaced, const ew::Form3Surface *sr);
/// This deletes a surface in the ew::Form3 managed by the node.
/// @param n
///   The index of the surface to delete.
    void remove_surface(int n);
/// This adds or replaces a curve in the ew::Form3 data managed by the node.
/// @param[out] replaced
///   @c true if the curve replaced an existing curve.
/// @param cr
///   A pointer to the curve data to copy.
/// @return
///   The index of the new curve in ew::Form3::curves.
    int set_curve(bool *replaced, const ew::Form3Curve *cr);
/// This deletes a curve in the ew::Form3 managed by the node.
/// @param n
///   The index of the curve to delete.
    void remove_curve(int n);
/// This adds or replaces a point set in the ew::Form3 data managed by the
/// node.
/// @param[out] replaced
///   @c true if the point set replaced an existing point set.
/// @param ps
///   A pointer to the point set data to copy.
/// @return
///   The index of the new or replaced pointset.
    int set_pointset(bool *replaced, const ew::Form3PointSet *ps);
/// This changes the coordinates of an element of the point set.
/// @param n
///   The index of the point set.
/// @param i
///   The index of the element in the point set.
/// @param[in] loc
///   A pointer to the new coordinates of this entry.
    void set_pointset_location(int n, int i, const double *loc);
/// This changes the relaxation parameters of an element of the point set.
/// @param n
///   The index of the point set.
/// @param i
///   The index of the element in the point set.
/// @param[in] rdim
///   The new relax_dims of this entry.
/// @param[in] rparam
///   A pointer to the new relax_params of this entry.
    void set_pointset_relax(int n, int i, int rdim, const double *rparam);
/// This deletes a point set in the ew::Form3 managed by the node.
/// @param n
///   The index of the point set to delete.
    void remove_pointset(int n);
/// This adds or replaces a embedding in the ew::Form3 data managed by the
/// node.
/// @param[out] replaced
///   @c true if the embedding replaced an existing embedding.
/// @param em
///   A pointer to the embedding data to copy.
/// @return
///   The index of the new embedding in ew::Form3::embeddings.
    int set_embedding(bool *replaced, const ew::Form3Embedding *em);
/// This makes one element of the form the unique superset of another element.
/// @param subset_id
///   The id of the element that should have a unique superset.
/// @param superset_id
///   The id of the element that be the unique superset.
    void set_superset(const char *subset_id, const char *superset_id);
/// This deletes a embedding in the ew::Form3 managed by the node.
/// @param n
///   The index of the embedding to delete.
    void remove_embedding(int n);
/// This adds or replaces a reflection in the ew::Form3 data managed by the
/// node.
/// @param[out] replaced
///   @c true if the reflection replaced an existing reflection.
/// @param rf
///   A pointer to the reflection data to copy.
/// @return
///   The index of the new reflection in ew::Form3::reflections.
    int set_reflection(bool *replaced, const ew::Form3Reflection *rf);
/// This deletes a reflection in the ew::Form3 managed by the node.
/// @param n
///   The index of the reflection to delete.
    void remove_reflection(int n);
/// @return
///   The last cycle the form was changed in a way that effects landmark
///   matching.
    inline unsigned long get_change_cycle_association() const;
/// @return
///   The last cycle that any location of a landmark was changed.
    inline unsigned long get_change_cycle_coords() const;
/// @return
///   The last cycle that any relaxation of a landmark was changed.
    inline unsigned long get_change_cycle_relax() const;
/// The pointer until the node or a dependency of the node is changed.
/// @return
///   A pointer to the bounding box of the points in the form.
    inline const ew::Bbox3 *get_bbox() const;
    const ew::DebugHelper dbg;
  private:
    virtual void update_helper() const;
    void update_bbox() const;
    ew::Form3 inp_data;
    unsigned long change_cycle_coords;
    unsigned long change_cycle_relax;
    unsigned long change_cycle_association;
    mutable ew::Bbox3 outp_bbox;
    mutable unsigned long update_cycle_bbox;
  };
}

inline const ew::Form3 *
ew::DataflowForm3::get_data() const
{
  return &inp_data;
}

inline unsigned long
ew::DataflowForm3::get_change_cycle_association() const
{
  return change_cycle_association;
}

inline unsigned long
ew::DataflowForm3::get_change_cycle_coords() const
{
  return change_cycle_coords;
}

inline unsigned long
ew::DataflowForm3::get_change_cycle_relax() const
{
  return change_cycle_relax;
}

inline const ew::Bbox3 *
ew::DataflowForm3::get_bbox() const
{
  unsigned long v = ew::DataflowNode::get_version();
  if (update_cycle_bbox < v) {
    update_bbox();
    update_cycle_bbox = network->get_cycle();
  }
  return &outp_bbox;
}

#endif
