#ifndef EW_TABLEAUCURVE_H_INCLUDED
#define EW_TABLEAUCURVE_H_INCLUDED

// wdkg 2008-2010

// This class is implemented entirely in this header.
// ? XXX replace by struct, eliminate ==, use version nums instead

#include <string>

namespace ew {
  class Dig3SetCurve {
  public:
    Dig3SetCurve();
    std::string id;
    bool show_in_main;
    bool show_in_slice;
    unsigned char col[3];
//XXX linewidth
    inline bool operator==(const ew::Dig3SetCurve &a) const;
    inline bool operator!=(const ew::Dig3SetCurve &a) const;
  };
}

/// @class ew::Dig3SetCurve
/// @headerfile ew/Dig3SetCurve.h "ew/Dig3SetCurve.h"
/// @brief Curve Element
///
/// ew::Dig3SetCurve contains data for a curve element of
/// ew::Tableau.
///
/// ew::Dig3SetCurve is a data structure that supports default
/// construction, copy construction, assignment and equality comparison.

/// @var std::string ew::Dig3SetCurve::id
/// The ew::Form3Curve::id of the curve this field should apply to.

/// @var bool ew::Dig3SetCurve::show_in_main
/// @c true if the curve should be displayed in the main view.
/// The default is @c true.

/// @var bool ew::Dig3SetCurve::show_in_slice
/// @c true if the curve should be displayed in the slice view.
/// The default is @c true.

/// @var unsigned char ew::Dig3SetCurve::col[3]
/// The colour of the curve.
/// The default is [128, 128, 128].

/// This sets intrinsic member variables to their default
/// values.

inline
ew::Dig3SetCurve::Dig3SetCurve() :
 show_in_main(true),
 show_in_slice(true)
{
  col[0] = 128;
  col[1] = 128;
  col[2] = 128;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Dig3SetCurve::operator==(const ew::Dig3SetCurve &a) const
{
  return
   id == a.id &&
   show_in_main == a.show_in_main &&
   show_in_slice == a.show_in_slice &&
   col[0] == a.col[0] &&
   col[1] == a.col[1] &&
   col[2] == a.col[2];
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Dig3SetCurve::operator!=(const ew::Dig3SetCurve &a) const
{
  return !operator==(a);
}

#endif
