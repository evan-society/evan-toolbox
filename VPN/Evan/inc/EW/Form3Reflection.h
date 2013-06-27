#ifndef EW_FORM3REFLECTION_H_INCLUDED
#define EW_FORM3REFLECTION_H_INCLUDED

// wdkg 2008-2010

// This class is implemented entirely in this header.

#include <string>

namespace ew {
  class Form3Reflection {
  public:
    std::string left_id;
    std::string right_id;
    inline bool operator==(const ew::Form3Reflection &a) const;
    inline bool operator!=(const ew::Form3Reflection &a) const;
  };
}

/// @class ew::Form3Reflection
/// @headerfile ew/Form3Reflection.h "ew/Form3Reflection.h"
/// @brief Reflection Relation
///
/// ew::Form3Reflection contains data for a reflection relation of ew::Form3.
/// Only symmetries where one element is on the left and one on the right,
/// or self symmetries for elements symbolically in the center plane, are
/// supported.
///
/// ew::Form3Reflection is a data structure that supports default
/// construction, copy construction, assignment and equality comparison.

/// @var std::string ew::Form3Reflection::left_id
/// The id of the left element of the symmetry.

/// @var std::string ew::Form3Reflection::right_id
/// The id of the right element of the symmetry.

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Reflection::operator==(const ew::Form3Reflection &a) const
{
  return left_id == a.left_id && right_id == a.right_id;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Reflection::operator!=(const ew::Form3Reflection &a) const
{
  return !operator==(a);
}

#endif
