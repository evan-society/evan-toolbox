#ifndef EW_DATAFLOWSPLINE3_H_INCLUDED
#define EW_DATAFLOWSPLINE3_H_INCLUDED

// wdkg 2008-2010

#include <vector>
#include "ew/DebugHelper.h"
#include "ew/DataflowForm3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"

//XXX export/changelog get_version_association ??

namespace ew {
  class DataflowSpline3 : public ew::DataflowNode {
  public:
    explicit DataflowSpline3(ew::DataflowNetwork *i_network);
    virtual void reset();
    void set_template(const ew::DataflowForm3 *f);
    inline const ew::DataflowForm3 *get_template() const;
    inline unsigned long get_change_cycle_template() const;
    void set_specimen(const ew::DataflowForm3 *f);
    inline const ew::DataflowForm3 *get_specimen() const;
    inline unsigned long get_change_cycle_specimen() const;
    inline int get_n_lmks() const;
    inline int get_f_size() const;
    inline int lmk_index(int side, int point_set, int i) const;
    inline int lmk_pointset(int side, int lmk_index) const;
    inline int lmk_pointset_i(int side, int lmk_index) const;
    inline bool get_nonsingular() const;
    inline double get_energy() const;
    inline const double *get_optimized_lmk_images() const;
    void warp_points(double *outp, const double *inp, int l) const;
    const ew::DebugHelper dbg;
  protected:
    mutable unsigned long update_cycle_association;
    mutable unsigned long update_cycle_interaction;
    mutable unsigned long update_cycle_factorization;
    mutable unsigned long update_cycle_spline;
  private:
    virtual void update_helper() const;
    inline unsigned long get_version_association() const;
    inline unsigned long get_version_interaction() const;
    inline unsigned long get_version_factorization() const;
    inline unsigned long get_version_spline() const;
    mutable unsigned long outp_version_association;
    mutable unsigned long outp_version_interaction;
    mutable unsigned long outp_version_factorization;
    mutable unsigned long outp_version_spline;
// phase 1
    inline void make_association() const;
    void update_association() const;
    void reset_association() const;
    mutable int outp_n_lmks;
    mutable std::vector<int> outp_spline_to_ps[2];
    mutable std::vector<int> outp_spline_to_ps_i[2];
    mutable std::vector<int> outp_ps_to_spline[2];
// phase 2
    inline void make_interaction() const;
    void update_interaction() const;
    void reset_interaction() const;
    mutable std::vector<double> outp_lmks;
    mutable std::vector<double> outp_interaction;
// phase 3
    inline void make_factorization() const;
    void update_factorization() const;
    void reset_factorization() const;
    mutable int outp_f_size;
    mutable bool outp_is_mixed;
    mutable bool outp_is_reduced;
    mutable bool outp_nonsingular;
    mutable double outp_nonsingularity;
    mutable std::vector<int> outp_relax_dims;
    mutable std::vector<double> outp_relax_params;
    mutable std::vector<double> outp_factorization;
    mutable std::vector<int> outp_pivots;
// phase 4
    inline void make_spline() const;
    void update_spline() const;
    void reset_spline() const;
    mutable double outp_energy;
    mutable std::vector<double> outp_lmk_images;
    mutable std::vector<double> outp_spline;
    mutable std::vector<double> outp_relax_lmk_images;
  };
}

/// @class ew::DataflowSpline3
/// @headerfile ew/DataflowSpline3.h "ew/DataflowSpline3.h"
/// @brief 3D Spline Node
///
/// ew::DataflowSpline3 is a node that manages a thin-plate spline.
///
/// ew::DataflowSpline3 is a class without assignment or comparison.
/// There are private member variables.
///
/// Nodes of this class depend on 2 nodes, the template and specimen
/// ew::DataflowForm3 nodes.
/// Nodes representing warped geometric objects will depend on a
/// ew::DataflowSpline3 node.
/// A ew::Dataflow_Spline3 is considered valid if it's template
/// and specimen dependencies have been set and if there are 4 or more
/// associated landmarks.
///
/// Initially and when reset, the node has neither template or specimen set.

/// @return
///   A pointer to the template node.

inline const ew::DataflowForm3 *
ew::DataflowSpline3::get_template() const
{
  return static_cast<const ew::DataflowForm3 *>(
   ew::DataflowNode::inp_dependencies[0]);
}

/// @return
///   A pointer to the specimen node.

inline const ew::DataflowForm3 *
ew::DataflowSpline3::get_specimen() const
{
  return static_cast<const ew::DataflowForm3 *>(
   ew::DataflowNode::inp_dependencies[1]);
}

/// @return
///   The last cycle the template pointer was changed.

inline unsigned long
ew::DataflowSpline3::get_change_cycle_template() const
{
  return ew::DataflowNode::change_cycle_dependencies[0];
}

/// @return
///   The last cycle the specimen pointer was changed.

inline unsigned long
ew::DataflowSpline3::get_change_cycle_specimen() const
{
  return ew::DataflowNode::change_cycle_dependencies[1];
}

/// @return
///   The last cycle the XXX was changed.

inline unsigned long
ew::DataflowSpline3::get_version_association() const
{
  ew::DataflowNode::update();
  return outp_version_association;
}

/// @return
///   The last cycle the XXX was changed.

inline unsigned long
ew::DataflowSpline3::get_version_interaction() const
{
  ew::DataflowNode::update();
  return outp_version_interaction;
}

/// @return
///   The last cycle the XXX was changed.

inline unsigned long
ew::DataflowSpline3::get_version_factorization() const
{
  ew::DataflowNode::update();
  return outp_version_factorization;
}

/// @return
///   The last cycle the XXX was changed.

inline unsigned long
ew::DataflowSpline3::get_version_spline() const
{
  ew::DataflowNode::update();
  return outp_version_spline;
}

/// @return
///   The number of landmarks and semi-landmarks matched between template and
///   specimen.

inline int
ew::DataflowSpline3::get_n_lmks() const
{
  make_association();
  return outp_n_lmks;
}

/// @return
///   The algebraic dimension of the spline, or -1.

inline int
ew::DataflowSpline3::get_f_size() const
{
  make_association();
  return outp_f_size;
}

/// To construct the spline, landmarks in the template and specimen are matched
/// by id and by position within the point_set.
/// This calculates the index in the spline of a point in a point_set.
/// @param side
///   0 if the point is in the template, 1 if it is in the specimen.
/// @param point_set
///   The index of the point_set.
/// @param i
///   The position of the point within the point_set.
/// @return
///   The index, if the point is matched, otherwise -1.

inline int
ew::DataflowSpline3::lmk_index(int side, int point_set, int i) const
{
  make_association();
  int r = outp_ps_to_spline[side][point_set];
  if (r < 0) {
    return -1;
  } else {
    return r + i;
  }
}

/// This is the inverse of #lmk_index.
/// @param side
///   0 for the pointset in the template, 1 for the pointset in the specimen.
/// @param index
///   The index as returned by #lmk_index.
/// @return
///   The pointset of the specified landmark in the spline.

inline int
ew::DataflowSpline3::lmk_pointset(int side, int index) const
{
  make_association();
  if (index >= outp_n_lmks) {
    return -1;
  }
  return outp_spline_to_ps[side][index];
}

/// This is the inverse of #lmk_index.
/// @param side
///   0 for the pointset in the template, 1 for the pointset in the specimen.
/// @param index
///   The index as returned by #lmk_index.
/// @return
///   The index of the landmark in the pointset of the specified landmark in
///   the spline.

inline int
ew::DataflowSpline3::lmk_pointset_i(int side, int index) const
{
  make_association();
  if (index >= outp_n_lmks) {
    return -1;
  }
  return outp_spline_to_ps_i[side][index];
}

/// @return
///   @c true if the spline is non-singular.

inline bool
ew::DataflowSpline3::get_nonsingular() const
{
  make_factorization();
  return outp_nonsingular;
}

/// @return
///   The bending energy of the spline.
/// @exception
///   If the spline is singular, a std::runtime_error is thrown.

inline double
ew::DataflowSpline3::get_energy() const
{
  make_spline();
  return outp_energy;
}

/// This returns the positions of the landmarks after being allowed to slide
/// in their relaxation spaces to the positions that minimize bending energy.
/// The order of the landmarks is as defined by #lmk_index.
/// If the spline is singular, a std::runtime_error is thrown.
/// The pointer is valid until the node or a dependency of the node is
/// changed.
/// @return
///   A pointer to an array of coordinates.

inline const double *
ew::DataflowSpline3::get_optimized_lmk_images() const
{
  make_spline();
  if (outp_relax_lmk_images.empty()) {
    return 0;
  } else {
    return &outp_relax_lmk_images[0];
  }
}

inline void
ew::DataflowSpline3::make_association() const
{
  unsigned long v = get_version_association();
  if (update_cycle_association < v) {
    update_association();
    update_cycle_association = network->get_cycle();
  }
}

inline void
ew::DataflowSpline3::make_interaction() const
{
  unsigned long v = get_version_interaction();
  if (update_cycle_interaction < v) {
    make_association();
    update_interaction();
    update_cycle_interaction = network->get_cycle();
  }
}

inline void
ew::DataflowSpline3::make_factorization() const
{
  unsigned long v = get_version_factorization();
  if (update_cycle_factorization < v) {
//XXX probably unnecessary
    make_association();
//XXX probably unnecessary
    make_interaction();
    update_factorization();
    update_cycle_factorization = network->get_cycle();
  }
}

inline void
ew::DataflowSpline3::make_spline() const
{
  unsigned long v = get_version_spline();
  if (update_cycle_spline < v) {
//XXX probably unnecessary
    make_association();
//XXX probably unnecessary
    make_interaction();
//XXX probably unnecessary
    make_factorization();
    update_spline();
    update_cycle_spline = network->get_cycle();
  }
}

#endif
