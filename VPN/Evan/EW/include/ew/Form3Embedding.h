#ifndef EW_FORM3EMBEDDING_H_INCLUDED
#define EW_FORM3EMBEDDING_H_INCLUDED

// wdkg 2008-2010

// This class is implemented entirely in this header.

#include <string>

namespace ew {
  class Form3Embedding {
  public:
    std::string subset_id;
    std::string superset_id;
    inline bool operator==(const ew::Form3Embedding &a) const;
    inline bool operator!=(const ew::Form3Embedding &a) const;
  };
}

/// @class ew::Form3Embedding
/// @headerfile ew/Form3Embedding.h "ew/Form3Embedding.h"
/// @brief Embedding Relation
///
/// ew::Form3Embedding contains data for an embedding relation of ew::Form3.
/// It is used to indicate subset relationships between point, curve and
/// surface elements.
///
/// ew::Form3Embedding is a data structure that supports default construction,
/// copy construction, assignment and equality comparison.

/// @var std::string ew::Form3Embedding::subset_id
/// The id of the lower dimensional element of the embedding.

/// @var std::string ew::Form3Embedding::superset_id
/// The id of the higher dimensional element of the embedding.

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Embedding::operator==(const ew::Form3Embedding &a) const
{
  return subset_id == a.subset_id && superset_id == a.superset_id;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Form3Embedding::operator!=(const ew::Form3Embedding &a) const
{
  return !operator==(a);
}

#endif
