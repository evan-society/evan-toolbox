#ifndef EW_FORM3SURFACE_H_INCLUDED
#define EW_FORM3SURFACE_H_INCLUDED

// wdkg 2008-2010

// This class is implemented entirely in this header.

#include <string>

namespace ew {
  class Form3Surface {
  public:
    std::string id;
    std::string file;
    inline bool operator==(const ew::Form3Surface &a) const;
    inline bool operator!=(const ew::Form3Surface &a) const;
  };
}

/// @class ew::Form3Surface
/// @headerfile ew/Form3Surface.h "ew/Form3Surface.h"
/// @brief Surface Element
///
/// ew::Form3Surface contains data for a surface element of ew::Form3.
///
/// ew::Form3Surface is a data structure that supports default construction,
/// copy construction, assignment and equality comparison.

/// @var std::string ew::Form3Surface::id
/// The id of this element used as a key to refer to this element and as a
/// label.

/// @var std::string ew::Form3Surface::file
/// The canonical name of the file containing the data for this element.
/// An empty string indicates that there is no associated file.

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Surface::operator==(const ew::Form3Surface &a) const
{
  return id == a.id && file == a.file;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Surface::operator!=(const ew::Form3Surface &a) const
{
  return !operator==(a);
}

#endif
