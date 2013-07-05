#ifndef EW_FORM3VOLUME_H_INCLUDED
#define EW_FORM3VOLUME_H_INCLUDED

// wdkg 2008-2010

#include <string>

namespace ew {
  class Form3Volume {
  public:
    std::string id;
    std::string file;
    inline bool operator==(const ew::Form3Volume &a) const;
    inline bool operator!=(const ew::Form3Volume &a) const;
  };
}

/// @class ew::Form3Volume
/// @headerfile ew/Form3Volume.h "ew/Form3Volume.h"
/// @brief Volume Element
///
/// ew::Form3Volume contains data for a volume element of ew::Form3.
///
/// ew::Form3Volume is a data structure that supports default construction,
/// copy construction, assignment and equality comparison.

/// @var std::string ew::Form3Volume::id
/// The id of this element used as a key to refer to this element and as a
/// label.

/// @var std::string ew::Form3Volume::file
/// The canonical name of the file containing the data for this element.
/// An empty string indicates that there is no associated file.

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Volume::operator==(const ew::Form3Volume &a) const
{
  return id == a.id && file == a.file;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Volume::operator!=(const ew::Form3Volume &a) const
{
  return !operator==(a);
}

#endif
