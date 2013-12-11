#ifndef EW_BBOX3_H_INCLUDED
#define EW_BBOX3_H_INCLUDED

// wdkg 2008-2010

namespace ew {
  class Bbox3 {
  public:
    double min[3];
    double max[3];
    inline bool get_empty() const;
    double get_radius_center(double *c) const;
    inline void set_to_empty();
    inline void set_to_point(const float *pt);
    inline void set_to_point(const double *pt);
    inline void set_to_points(const float *pt, int n);
    inline void set_to_points(const double *pt, int n);
    inline void add(const float *pt);
    inline void add(const double *pt);
    inline void add(const float *pt, int n);
    inline void add(const double *pt, int n);
    void set_to_union(const ew::Bbox3 *b1, const ew::Bbox3 *b2);
    bool operator==(const ew::Bbox3 &a) const;
    inline bool operator!=(const ew::Bbox3 &a) const;
    static const ew::Bbox3 empty_box;
  };
}

/// @class ew::Bbox3
/// @headerfile ew/Bbox3.h "ew/Bbox3.h"
/// @brief 3D Bounding Box
///
/// ew::Bbox3 represents bounding boxes in @f$\mathbb{R}^3@f$.
///
/// ew::Bbox3 is a POD type class.

/// @var double ew::Bbox3::min[3]
/// These are the minima in the 3 coordinate directions.
/// Except for the empty box, these should all be finite and no greater than
/// the corresponding maxima.

/// @var double ew::Bbox3::max[3]
/// These are the maxima in the 3 coordinate directions.
/// Except for the empty box, these should all be finite and no less than the
/// corresponding minima.

/// @return @c true if the minima are infinity and the maxima are negative
/// infinity

inline bool
ew::Bbox3::get_empty() const
{
  return (min[0] > max[0]);
}

/// Set this box to be the empty
/// box.

inline void
ew::Bbox3::set_to_empty()
{
  *this = empty_box;
}

/// Set this box to be the bounding box of a point.
/// @param[in] pt the address of the array of size 3 containing the point
/// coordinates

inline void
ew::Bbox3::set_to_point(const float *pt)
{
  for (int i = 0; i < 3; i += 1) {
    min[i] = max[i] = static_cast<double>(pt[i]);
  }
}

/// Set this box to be the bounding box of a point.
/// @param[in] pt the address of the array of size 3 containing the point
/// coordinates

inline void
ew::Bbox3::set_to_point(const double *pt)
{
  for (int i = 0; i < 3; i += 1) {
    min[i] = max[i] = pt[i];
  }
}

/// Set this box to be the bounding box of an array of points.
/// @param[in] pt the address of an array of size 3 * @a n containing the point
/// coordinates, ordered by point
/// @param n the number of points

inline void
ew::Bbox3::set_to_points(const float *pt, int n)
{
  set_to_point(pt);
  add(pt + 3, n - 1);
}

/// Set this box to be the bounding box of an array of points.
/// @param[in] pt the address of an array of size 3 * @a n containing the point
/// coordinates, ordered by point
/// @param n the number of points

inline void
ew::Bbox3::set_to_points(const double *pt, int n)
{
  set_to_point(pt);
  add(pt + 3, n - 1);
}

/// Extend this box, if necessary, to include a point.
/// @param[in] pt the address of the array of size 3 containing the point
/// coordinates

inline void
ew::Bbox3::add(const float *pt)
{
  for (int i = 0; i < 3; i += 1) {
    if (pt[i] < min[i]) {
      min[i] = static_cast<double>(pt[i]);
    }
    if (pt[i] > max[i]) {
      max[i] = static_cast<double>(pt[i]);
    }
  }
}

/// Extend this box, if necessary, to include a point.
/// @param[in] pt the address of the array of size 3 containing the point
/// coordinates

inline void
ew::Bbox3::add(const double *pt)
{
  for (int i = 0; i < 3; i += 1) {
    if (pt[i] < min[i]) {
      min[i] = static_cast<double>(pt[i]);
    }
    if (pt[i] > max[i]) {
      max[i] = static_cast<double>(pt[i]);
    }
  }
}

/// Extend this box, if necessary, to include an array of points.
/// @param[in] pt the address of an array of size 3 * @a n containing the point
/// coordinates, ordered by point
/// @param n the number of points

inline void
ew::Bbox3::add(const float *pt, int n)
{
  for (int i = 0; i < n; i += 1) {
    add(pt + 3 * i);
  }
}

/// Extend this box, if necessary, to include an array of points.
/// @param[in] pt the address of an array of size 3 * @a n containing the point
/// coordinates, ordered by point
/// @param n the number of points

inline void
ew::Bbox3::add(const double *pt, int n)
{
  for (int i = 0; i < n; i += 1) {
    add(pt + 3 * i);
  }
}

/// Compares this box with another, member by member.
/// @param a the other box

inline bool
ew::Bbox3::operator!=(const ew::Bbox3 &a) const
{
  return !operator==(a);
}

#endif
