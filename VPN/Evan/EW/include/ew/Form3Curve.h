#ifndef EW_FORM3CURVE_H_INCLUDED
#define EW_FORM3CURVE_H_INCLUDED

// wdkg 2008-2010

// This class is implemented entirely in this header.
// ? XXX replace by struct, eliminate ==, use version nums instead

#include <string>

namespace ew {
  class Form3Curve {
  public:
    std::string id;
    std::string file;
    int state;
    inline Form3Curve();
    inline bool operator==(const ew::Form3Curve &a) const;
    inline bool operator!=(const ew::Form3Curve &a) const;
  };
}

/// @class ew::Form3Curve
/// @headerfile ew/Form3Curve.h "ew/Form3Curve.h"
/// @brief Curve Element
///
/// ew::Form3Curve contains data for a curve element of ew::Form3.
///
/// ew::Form3Curve is a data structure that supports default construction,
/// copy construction, assignment and equality comparison.

/// @var std::string ew::Form3Curve::id
/// The id of this element used as a key to refer to this element and as a
/// label.

/// @var std::string ew::Form3Curve::file
/// The canonical name of the file containing the data for this element, or an
/// empty string indicating that there is no associated file.

/// @var int ew::Form3Curve::state
/// A code indicating the digitizing state of the curve.
/// Its value is a ew::Form3::state_t.
/// The default is 0.

/// This sets intrinsic member variables to their default
/// values.

inline
ew::Form3Curve::Form3Curve() :
 state(0)
{
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Curve::operator==(const ew::Form3Curve &a) const
{
  return id == a.id && file == a.file && state == a.state;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Curve::operator!=(const ew::Form3Curve &a) const
{
  return !operator==(a);
}

#endif
