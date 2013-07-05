// wdkg 2009-2010

#include <cmath>
#include <algorithm>
#include <limits>
#include "ew/Bbox3.h"
#include "ew/ErrorLogic.h"

/// This is an empty
/// box.

const ew::Bbox3 ew::Bbox3::empty_box = {
  {
    std::numeric_limits<double>::infinity(),
    std::numeric_limits<double>::infinity(),
    std::numeric_limits<double>::infinity()
  }, {
    -std::numeric_limits<double>::infinity(),
    -std::numeric_limits<double>::infinity(),
    -std::numeric_limits<double>::infinity()
  }
};

/// Set the bounding box to the union of the specified bounding boxes.
/// @param b1, b2 the boxes to combine

void
ew::Bbox3::set_to_union(const ew::Bbox3 *b1, const ew::Bbox3 *b2)
{
  min[0] = std::min(b1->min[0], b2->min[0]);
  max[0] = std::max(b1->max[0], b2->max[0]);
  min[1] = std::min(b1->min[1], b2->min[1]);
  max[1] = std::max(b1->max[1], b2->max[1]);
  min[2] = std::min(b1->min[2], b2->min[2]);
  max[2] = std::max(b1->max[2], b2->max[2]);
}

/// Calculates the center and circumradius of a box.
/// @pre The box is not empty.
/// @param[out] c the address of an array of size 3 where the center
/// coordinates should be stored.
/// @return the radius

double
ew::Bbox3::get_radius_center(double *c) const
{
  if (min[0] == std::numeric_limits<double>::infinity()) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  c[0] = (min[0] + max[0]) / 2.0;
  c[1] = (min[1] + max[1]) / 2.0;
  c[2] = (min[2] + max[2]) / 2.0;
  double d[3];
  d[0] = (max[0] - min[0]) / 2.0;
  d[1] = (max[1] - min[1]) / 2.0;
  d[2] = (max[2] - min[2]) / 2.0;
  return sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
}

/// Compares this box with another, member by member.
/// @param a the other box

bool
ew::Bbox3::operator==(const ew::Bbox3 &a) const
{
  return
   min[0] == a.min[0] &&
   min[1] == a.min[1] &&
   min[2] == a.min[2] &&
   max[0] == a.max[0] &&
   max[1] == a.max[1] &&
   max[2] == a.max[2];
}
