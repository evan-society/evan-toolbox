#ifndef EW_TABLEAUSURFACE_H_INCLUDED
#define EW_TABLEAUSURFACE_H_INCLUDED

// wdkg 2008-2010

// This class is implemented entirely in this header.

#include <string>

namespace ew {
  class Dig3SetSurface {
  public:
    Dig3SetSurface();
    std::string id;
    bool show_in_main;
    bool show_in_slice;
    unsigned char front_col[3];
    unsigned char back_col[3];
    inline bool operator==(const ew::Dig3SetSurface &a) const;
    inline bool operator!=(const ew::Dig3SetSurface &a) const;
  };
}

/// @class ew::Dig3SetSurface
/// @headerfile ew/Dig3SetSurface.h "ew/Dig3SetSurface.h"
/// @brief Surface Element
///
/// ew::Dig3SetSurface contains data for a surface element of
/// ew::Tableau.
///
/// ew::Dig3SetSurface is a data structure that supports default
/// construction, copy construction, assignment and equality comparison.

/// @var std::string ew::Dig3SetSurface::id
/// The ew::Form3Surface::id of the surface this field should apply to.

/// @var bool ew::Dig3SetSurface::show_in_main
/// @c true if the surface should be displayed in the main view.
/// The default is @c true.

/// @var bool ew::Dig3SetSurface::show_in_slice
/// @c true if the surface should be displayed in the slice view.
/// The default is @c true.

/// @var unsigned char ew::Dig3SetSurface::front_col[3]
/// The colour of the front side of the surface.
/// The default is [128, 128, 128].

/// @var unsigned char ew::Dig3SetSurface::back_col[3]
/// The colour of the back side of the surface.
/// The default is [128, 128, 128].

/// This sets intrinsic member variables to their default
/// values.

inline
ew::Dig3SetSurface::Dig3SetSurface() :
 show_in_main(true),
 show_in_slice(true)
{
  front_col[0] = 128;
  front_col[1] = 128;
  front_col[2] = 128;
  back_col[0] = 128;
  back_col[1] = 128;
  back_col[2] = 128;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Dig3SetSurface::operator==(const ew::Dig3SetSurface &a) const
{
  return
   id == a.id &&
   show_in_main == a.show_in_main &&
   show_in_slice == a.show_in_slice &&
   front_col[0] == a.front_col[0] &&
   front_col[1] == a.front_col[1] &&
   front_col[2] == a.front_col[2] &&
   back_col[0] == a.back_col[0] &&
   back_col[1] == a.back_col[1] &&
   back_col[2] == a.back_col[2];
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Dig3SetSurface::operator!=(const ew::Dig3SetSurface &a) const
{
  return !operator==(a);
}

#endif
