#ifndef EW_TABLEAU_H_INCLUDED
#define EW_TABLEAU_H_INCLUDED

// wdkg 2008-2010

//XXX auto try xxx.gz first, any error -> xxx
//XXX ?? pointer to form3 as alternative to filename, so can have tableau
//        records in memory without first saving form file
//XXX id=* or more generally patterns
//XXX check when apply support mismatches
//XXX space: line width, translucency, outline, cross, ...
//XXX ? move clip ratio to space
//XXX ? view clip ratio (large to avoid clipping)
//XXX ? move bg to space
//XXX allow 4 views to be not present (-1)

#include <stdexcept>
#include <string>
#include <vector>
#include "ew/Dig3TableauSpace.h"
#include "ew/Transform3.h"

namespace ew {
  class Dig3Tableau {
  public:
    ew::Dig3TableauSpace space[2];
    double slice_clip_ratio;
    unsigned char bg[3];
    inline Dig3Tableau();
    void reset();
    static void read_file(std::vector<ew::Dig3Tableau> *outp,
     const char *file);
    static void write_file(const char *file, bool compress,
     const std::vector<ew::Dig3Tableau> *inp);
    bool operator==(const ew::Dig3Tableau &a) const;
    inline bool operator!=(const ew::Dig3Tableau &a) const;
  };
}

/// @class ew::Dig3Tableau
/// @headerfile ew/Dig3Tableau.h "ew/Dig3Tableau.h"
/// @brief Viewing State Record
///
/// ew::Dig3Tableau contains a partial record of the state of an ew::Dig3.
///
/// ew::Dig3Tableau is a data structure that supports default construction,
/// copy construction, assignment and equality comparison.
///
/// ew::Dig3Tableau records the ew::Dig3 space forms indirectly as file names.
/// It does not record the ew::Dig3 views, their widgets or their linkages.
/// It records the settings of one each of:
/// - a template main view
/// - a template slice view
/// - a specimen main view
/// - a specimen slice view
/// .
/// Some settings will be assumed to be common to some or all of the windows.
/// It does not record state that is not controlled by the library, such as the
/// window sizes, file browser history or transient state such as the selected
/// item, window configuration, or gui modal state.
///
/// ew::Dig3Tableau lists are represented in save files read by #read_file and
/// #write_file.
/// Here is an example:
/// - @ref example.sav
/// .
/// The representation is exact except that filenames in the form files are
/// abbreviated to relative filenames in common situations were possible, but
/// in ew::Dig3Tableau they are always in canonical form.

/// @var ew::Dig3TableauSpace ew::Dig3Tableau::space[2]
/// The template and specimen space states.

/// @var double ew::Dig3Tableau::slice_clip_ratio
/// The slice clip ratio.
/// The default is 0.01.

/// @var unsigned char ew::Dig3Tableau::bg[3]
/// The window background.
/// The default is [0, 0, 0].

/// This sets intrinsic member variables to their default values.

inline
ew::Dig3Tableau::Dig3Tableau() :
 slice_clip_ratio(0.01)
{
  bg[0] = 0;
  bg[1] = 0;
  bg[2] = 0;
}

/// Compares this record with another, member by member.
/// @param a the other record

inline bool
ew::Dig3Tableau::operator!=(const ew::Dig3Tableau &a) const
{
  return !operator==(a);
}

#endif
