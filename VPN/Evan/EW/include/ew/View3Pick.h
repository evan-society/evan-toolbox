#ifndef EW_VIEW3PICK_H_INCLUDED
#define EW_VIEW3PICK_H_INCLUDED

// wdkg 2010

// This class is implemented entirely in this header.
// ? XXX replace by struct, transfer methods elsewhere

#include "ew/View3Item.h"

namespace ew {
  class View3Pick {
  public:
    ew::View3Item *item;
    int item_component;
    int dim;
    double z_near;
    double z_far;
    int hit;
    inline bool operator==(const ew::View3Pick &a) const;
    inline bool operator!=(const ew::View3Pick &a) const;
    inline bool operator<(const ew::View3Pick &a) const;
    inline bool operator>(const ew::View3Pick &a) const;
    inline bool operator<=(const ew::View3Pick &a) const;
    inline bool operator>=(const ew::View3Pick &a) const;
  };
}

/// @brief Pick Data
///
/// @headerfile ew/View3Pick.h "ew/View3Pick.h"
/// ew::View3Pick contains data returned by ew::View3Widget::pickv.
///
/// ew::View3Pick is a data structure that supports default construction,
/// copy construction, assignment, equality comparison and order comparison.

/// @var ew::View3Item *ew::View3Pick::item
/// The item picked.

/// @var int ew::View3Pick::item_component
/// The component of the item picked.

/// @var int ew::View3Pick::item_component
/// The dimension of the component of the item picked.

/// @var double ew::View3Pick::z_near
/// The z coordinate in the viewing frame of the closest part of the pick.

/// @var double ew::View3Pick::z_far
/// The z coordinate in the viewing frame of the furthest part of the pick.

/// @var double ew::View3Pick::hit
/// The position of the pick in the hit list.

/// Compares this pick with another, member by member.
/// @param a the other pick

inline bool
ew::View3Pick::operator==(const ew::View3Pick &a) const
{
  return item == a.item && item_component == a.item_component &&
   dim == a.dim && z_near == a.z_near && z_far == a.z_far && hit == a.hit;
}

/// Compares this pick with another, member by member.
/// @param a the other pick

inline bool
ew::View3Pick::operator!=(const ew::View3Pick &a) const
{
  return !operator==(a);
}

/// Compares this pick with another.
/// This returns @c true if @a a has a greater #z_near, or equal #z_near and
/// lesser #hit.
/// The lesser ew::View3Pick will, in general, be more visible.
/// Note that this order is weaker that the equality relation.
/// @param a the other pick

inline bool
ew::View3Pick::operator<(const ew::View3Pick &a) const
{
  return (z_near < a.z_near || (z_near == a.z_near && hit > a.hit));
}

/// Compares this pick with another.
/// This is opposite of ew::View3Pick::operator<.
/// @param a the other pick

inline bool
ew::View3Pick::operator>=(const ew::View3Pick &a) const
{
  return !operator<(a);
}

/// Compares this pick with another.
/// This is the reflexive symmetry of ew::View3Pick::operator<.
/// @param a the other pick

inline bool
ew::View3Pick::operator>(const ew::View3Pick &a) const
{
  return (z_near > a.z_near || (z_near == a.z_near && hit < a.hit));
}

/// Compares this pick with another.
/// This is opposite of ew::View3Pick::operator>.
/// @param a the other pick

inline bool
ew::View3Pick::operator<=(const ew::View3Pick &a) const
{
  return !operator>(a);
}

#endif
