#ifndef EW_TABLEAUSPACE_H_INCLUDED
#define EW_TABLEAUSPACE_H_INCLUDED

// wdkg 2008-2010

// This class is implemented entirely in this header.
// ? XXX replace by struct, eliminate ==, use version nums instead

#include <string>
#include "ew/Dig3SetCurve.h"
#include "ew/Dig3SetSurface.h"
#include "ew/Transform3.h"

namespace ew {
  class Dig3TableauSpace {
  public:
    std::string form_filename;
    ew::Transform3 main_view;
    ew::Transform3 slice_view;
    bool show_slice_in_main;
    bool show_lmks_in_main;
    bool show_lmks_in_slice;
    int lmks_symbol;
    unsigned char lmks_col[3];
    std::vector<ew::Dig3SetCurve> curve_settings;
    std::vector<ew::Dig3SetSurface> surface_settings;
    Dig3TableauSpace();
    inline bool operator==(const ew::Dig3TableauSpace &a) const;
    inline bool operator!=(const ew::Dig3TableauSpace &a) const;
  };
}

/// @class ew::Dig3TableauSpace
/// @headerfile ew/Dig3TableauSpace.h "ew/Dig3TableauSpace.h"
/// @brief Space Element
///
/// ew::Dig3TableauSpace contains data for a space element of ew::Dig3Tableau.
///
/// ew::Dig3TableauSpace is a data structure that supports default
/// construction, copy construction, assignment and equality comparison.

/// @var std::string ew::Dig3TableauSpace::form_filename
/// This file containing the form data for the space.

/// @var ew::Transform3 ew::Dig3TableauSpace::main_view
/// The view mapping of the main view of this space.
/// The default is the identity transformation.

/// @var ew::Transform3 ew::Dig3TableauSpace::slice_view
/// The view mapping of the slice view of this space.
/// The default is the identity transformation.

/// @var bool ew::Dig3TableauSpace::show_slice_in_main
/// @c true if the slice should be displayed in the main view.
/// The default is @c true.

/// @var bool ew::Dig3TableauSpace::show_lmks_in_main
/// @c true if the landmarks should be displayed in the main view.
/// The default is @c true.

/// @var bool ew::Dig3TableauSpace::show_lmks_in_slice
/// @c true if the landmarks should be displayed in the slice view.
/// The default is @c true.

/// @var int ew::Dig3TableauSpace::lmks_symbol
/// The symbol to use for landmarks.
/// The default is 0.

/// @var unsigned char ew::Dig3TableauSpace::lmks_col[3]
/// The colour of the landmarks.
/// The default is [0, 255, 0].

/// @var std::vector<ew::Dig3SetCurve> ew::Dig3TableauSpace::curve_settings
/// The settings for curves in the space.

/// @var std::vector<ew::Dig3SetSurface> ew::Dig3TableauSpace::surface_settings
/// The settings for surfaces in the space.

/// This sets intrinsic and POD member variables to their default
/// values.

inline
ew::Dig3TableauSpace::Dig3TableauSpace() :
 main_view(ew::Transform3::identity_transform),
 slice_view(ew::Transform3::identity_transform),
 show_slice_in_main(true),
 show_lmks_in_main(true),
 show_lmks_in_slice(true),
 lmks_symbol(0)
{
  lmks_col[0] = 0;
  lmks_col[1] = 255;
  lmks_col[2] = 0;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Dig3TableauSpace::operator==(const ew::Dig3TableauSpace &a) const
{
  return
   form_filename == a.form_filename &&
   main_view == a.main_view &&
   slice_view == a.slice_view &&
   show_slice_in_main == a.show_slice_in_main &&
   show_lmks_in_main == a.show_lmks_in_main &&
   show_lmks_in_slice == a.show_lmks_in_slice &&
   lmks_symbol == a.lmks_symbol &&
   lmks_col[0] == a.lmks_col[0] &&
   lmks_col[1] == a.lmks_col[1] &&
   lmks_col[2] == a.lmks_col[2] &&
   curve_settings == a.curve_settings &&
   surface_settings == a.surface_settings;
}

/// Compares this element with another, member by member.
/// @param a the other element

inline bool
ew::Dig3TableauSpace::operator!=(const ew::Dig3TableauSpace &a) const
{
  return !operator==(a);
}

#endif
