#ifndef EW_FORM3_H_INCLUDED
#define EW_FORM3_H_INCLUDED

// wdkg 2008-2010

//XXX doc well-formedness and consequences of ill-formedness
//XXX cannot have more than 2G of anything or of bytes: doc, check and throw
//XXX ? don't order, use linear search (spline association slow)
//XXX ? maintain order of pointsets in ew::DataflowForm3
//XXX check and canonicalize method:
//XXX   ordered id's
//XXX   id's non-empty and distinct
//XXX   embedding and reflection not hanging and appropriate dims/reflexivity
//XXX   export order a permutation
//XXX check and canonicalize on read
//XXX transform/stretch in volume
//XXX export order (landmarks only?) (maintain on insert/delete)

#include <stdexcept>
#include <vector>
#include "ew/Form3Curve.h"
#include "ew/Form3Embedding.h"
#include "ew/Form3Pointset.h"
#include "ew/Form3Reflection.h"
#include "ew/Form3Surface.h"
#include "ew/Form3Volume.h"

namespace ew {

/// @brief Morphometric Form
///
/// @headerfile ew/Form3.h "ew/Form3.h"
/// ew::Form3 represents morphometric forms in @f$\mathbb{R}^3@f$.
/// These are landmark configurations, possibly including semi-landmarks,
/// generalized to include curve, surface and volume data.
///
/// ew::Form3 is a data structure that supports default construction, copy
/// construction, assignment and equality comparison.
///
/// ew::Form3 data is represented in form files read by #read_file and written
/// by #write_file.
/// These are example files:
/// - @ref simple_landmarks.form
/// - @ref mandible_template.form
/// - @ref mandible_case.form
/// .
/// The representation is exact except that filenames in the form files are
/// abbreviated to relative filenames in common situations were possible, but
/// in ew::Form3 they are always in canonical form.
///
/// All elements in the form have an id.
/// These id's must be unique in the from.

  class Form3 {
  public:
    enum state_t {
      STATE_SET,
      STATE_UNSET,
      STATE_PROVISIONAL,
      STATE_WARPED,
      STATE_PROJECTED,
      STATE_OPTIMIZED
    };
    static const int N_STATES = 6;
    static const char *const state_names[];
    enum point_t {
      TYPE_LANDMARK,
      TYPE_SEMI_LANDMARK,
      TYPE_POINT,
      TYPE_LINE,
      TYPE_PLANE,
      TYPE_FRAME
    };
    static const int N_TYPES = 6;
    static const char *const type_names[];
    std::vector<ew::Form3Volume> volumes;
    std::vector<ew::Form3Surface> surfaces;
    std::vector<ew::Form3Curve> curves;
    std::vector<ew::Form3PointSet> pointsets;
//XXX ? replace with attribute
    std::vector<ew::Form3Embedding> embeddings;
//XXX ? replace with attribute
    std::vector<ew::Form3Reflection> reflections;
    void reset();
    void read_file(const char *file);
    void write_file(const char *file, bool compress) const;
    bool search_volume(int *position, const char *id) const;
    bool search_surface(int *position, const char *id) const;
    bool search_curve(int *position, const char *id) const;
    bool search_pointset(int *position, const char *id) const;
//XXX not useful, want to search for id1 or id2 not both
    bool search_embedding(int *position, const char *id1,
     const char *id2) const;
//XXX not useful, want to search for id1 or id2 not both
    bool search_reflection(int *position, const char *id1,
     const char *id2) const;
    const char *search_superset(const char *id) const;
    void set_superset(const char *subset_id, const char *superset_id);
    inline void swap(ew::Form3 &f);
    bool operator==(const ew::Form3 &a) const;
    inline bool operator!=(const ew::Form3 &a) const;
  };

}

/// @enum ew::Form3::state_t
/// Some elements can have a state associated with them.
/// These are the possible values.
/// Not all states are appropriate for all element types.
/// The default is #STATE_SET.

/// @var ew::Form3::STATE_SET
/// The element has been explicitly digitized (default).

/// @var ew::Form3::STATE_UNSET
/// The element has not been digitized.

/// @var ew::Form3::STATE_PROVISIONAL
/// The element has been provisionally digitized.

/// @var ew::Form3::STATE_WARPED
/// The element is the image under a warp.

/// @var ew::Form3::STATE_PROJECTED
/// The element has been projected.

/// @var ew::Form3::STATE_OPTIMIZED
/// The element has been optimized.

/// @enum ew::Form3::point_t
/// Point sets can have different types.
/// These are the possible values.
/// The default is #TYPE_LANDMARK.

/// @var ew::Form3::TYPE_LANDMARK
/// The points are landmarks (default).

/// @var ew::Form3::TYPE_SEMI_LANDMARK
/// The points are semi-landmarks.

/// @var ew::Form3::TYPE_POINT
/// The point is a non-landmark point.

/// @var ew::Form3::TYPE_LINE
/// The point represents a line, such as a best-fit axis of symmetry.

/// @var ew::Form3::TYPE_PLANE
/// The point represents a plane, such as a best-fit multi-tangent
/// or best-fit symmetry plane.

/// @var ew::Form3::TYPE_FRAME
/// The point represents a frame, such as a standard view.

/// @var std::vector<ew::Form3Volume> ew::Form3::volumes
/// An arbitrary number of volume elements.
/// The volume data is stored in separate files.
/// The files named are not expected to change during their use.
/// These must be in alphabetical order by ew::Form3Volume::id in the POSIX
/// sorting order.

/// @var std::vector<ew::Form3Surface> ew::Form3::surfaces
/// An arbitrary number of surface elements.
/// The surface data is stored in separate files.
/// The files named are not expected to change during their use.
/// These must be in alphabetical order by ew::Form3Surface::id in the POSIX
/// sorting order.

/// @var std::vector<ew::Form3Curve> ew::Form3::curves
/// An arbitrary number of curve elements.
/// The curve data is stored in separate files.
/// The files named can be created during the digitization, so can change
/// during their use.
/// These must be in alphabetical order by ew::Form3Curve::id in the POSIX
/// sorting order.

/// @var std::vector<ew::Form3PointSet> ew::Form3::pointsets
/// An arbitrary number of point set elements.
/// These must be in alphabetical order by id in the POSIX sorting order.

/// @var std::vector<ew::Form3Embedding> ew::Form3::embeddings
/// An arbitrary number of individual embedding relationships.
/// These must be in alphabetical order by ew::Form3Embedding::subset_id,
/// then ew::Form3Embedding::superset_id in the POSIX sorting order.
/// They must refer to elements of the form of appropriate dimension.
/// This is how the curve or surface a semi-landmark belongs to is indicated.
/// The semi-landmarks must be embedded in a unique curve or surface of the
/// appropriate dimension.
/// Regular landmarks can be embedded in one or more curves or surfaces.

/// @var std::vector<ew::Form3Reflection> ew::Form3::reflections
/// An arbitrary number of individual reflection relationships.
/// These must be in alphabetical order by ew::Form3Reflection::left_id,
/// then ew::Form3Reflection::right_id in the POSIX sorting order.
/// They must refer to elements of the form of appropriate dimension.

/// This swaps the data of 2 forms without any copying.
/// @param that
///   The form to swap with @c this.

inline void
ew::Form3::swap(ew::Form3 &that)
{
  volumes.swap(that.volumes);
  surfaces.swap(that.surfaces);
  curves.swap(that.curves);
  pointsets.swap(that.pointsets);
  embeddings.swap(that.embeddings);
  reflections.swap(that.reflections);
}

/// Compares this form with another, member by member.
/// @param a the other form

inline bool
ew::Form3::operator!=(const ew::Form3 &a) const
{
  return !operator==(a);
}

#endif
