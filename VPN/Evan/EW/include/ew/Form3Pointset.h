#ifndef EW_FORM3POINTSET_H_INCLUDED
#define EW_FORM3POINTSET_H_INCLUDED

// wdkg 2008-2010

// This class is implemented entirely in this header.

#include <string>
#include <vector>

namespace ew {
  class Form3PointSet {
  public:
    std::string id;
    int type;
    int state;
    int n;
    std::vector<double> locations;
    std::vector<int> relax_dims;
    std::vector<double> relax_params;
    std::vector<double> orientations;
    std::vector<double> sizes;
    inline Form3PointSet();
    inline bool operator==(const ew::Form3PointSet &a) const;
    inline bool operator!=(const ew::Form3PointSet &a) const;
  };
}

/// @class ew::Form3PointSet
/// @headerfile ew/Form3Pointset.h "ew/Form3Pointset.h"
/// @brief Point Set Element
///
/// ew::Form3PointSet contains data for a point set element of ew::Form3.
/// This could be, for example, a single named landmark, a set of
/// semi-landmarks or a set of landmarks which are not individually named.
/// Also supported are features not intended to be interpreted as landmarks,
/// such as non-landmark points or planes and frames which are represented as a
/// point with a size and orientation.
///
/// ew::Form3PointSet is a data structure that supports default construction,
/// copy construction, assignment and equality comparison.
///
/// It is intended that landmarks be represented one to a point set.
/// The use of the term "point set" in this context might be confusing.
/// The "set" part of this is only really useful for semi-landmarks.
/// Note that all elements of the point set share the same id, type and
/// state.

/// @var std::string ew::Form3PointSet::id
/// The id of this element used as a key to refer to this element and as a
/// label.

/// @var int ew::Form3PointSet::type
/// A code indicating the type of point set.
/// Its value is a ew::Form3::point_t.
/// The default is 0.

/// @var int ew::Form3PointSet::state
/// A code indicating the digitizing state of the point set.
/// Its value is a ew::Form3::state_t.
/// The default is 0.

/// @var int ew::Form3PointSet::n
/// The number of points in the set.
/// It must be non-negative.

/// @var std::vector<double> ew::Form3PointSet::locations
/// The coordinates of the points.
/// If all the coordinates are 0.0, this vector can have size 0.
/// Otherwise it must have size #n * 3.

/// @var std::vector<int> ew::Form3PointSet::relax_dims
/// The relax dimensions of the points in the sense of ew::Tps3.
/// These are only relevant for points of type "landmark" or "semi-landmark".
/// They indicate the dimension of the linear space that the landmark or
/// semi-landmark can slide along.
/// A value of 0 indicates that landmark or semi-landmark should not slide.
/// A value of 3 indicates that landmark or semi-landmark slide in the whole
/// of @f$\mathbb{R}^3@f$.
/// In this case, the landmark or semi-landmark does contribute to the spline
/// and its optimized image is just the image of the landmark or semi-landmark
/// under the spline.
/// Values of 1 and 2 are only applicable to semi-landmarks.
/// A value of 1 indicates that the semi-landmark slides along a line.
/// This is appropraite for a curve semi-landmark.
/// A value of 2 indicates that the semi-landmark slides along a plane.
/// This is appropraite for a surface emi-landmark.
/// If all the relax dimensions are 0, this vector can have size 0.
/// Otherwise it must have size #n.

/// @var std::vector<double> ew::Form3PointSet::relax_params
/// The relax parameters of the points in the sense of ew::Tps3.
/// These are only relevant for points of type "semi-landmark".
/// They encode the direction or directions in which the semi-landmark is
/// currently being slid.
/// If all the relax dimensions are 0 or 3, this vector can have size 0.
/// Otherwise it must have size #n * 3.

/// @var std::vector<double> ew::Form3PointSet::orientations
/// The orientations of the points, orthogonal matrices like
/// ew::Transform3::orthog.
/// These are only relevant for points of type "line", "plane" or "frame".
/// If all the matrices are identities, this vector can have size 0.
/// Otherwise it must have size #n * 9.

/// @var std::vector<double> ew::Form3PointSet::sizes
/// The sizes of the points, like ew::Transform3::scale.
/// These are only relevant for points of type "line", "plane" or "frame".
/// If all the scales are 1.0, this vector must can size 0.
/// Otherwise it must have size #n.

/// This sets intrinsic member variables to their default
/// values.

inline
ew::Form3PointSet::Form3PointSet() :
 type(0),
 state(0),
 n(0)
{
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3PointSet::operator==(const ew::Form3PointSet &a) const
{
  return
   id == a.id &&
   type == a.type &&
   state == a.state &&
   n == a.n &&
   locations == a.locations &&
   relax_params == a.relax_params &&
   relax_dims == a.relax_dims &&
   orientations == a.orientations &&
   sizes == a.sizes;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3PointSet::operator!=(const ew::Form3PointSet &a) const
{
  return !operator==(a);
}

#endif
