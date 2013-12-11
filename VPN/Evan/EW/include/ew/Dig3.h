#ifndef EW_DIG3_H_INCLUDED
#define EW_DIG3_H_INCLUDED

// wdkg 2008-2010

//XXX for now, forms must be empty or have a filename (last load and save,
// not reset, but changes ok) before save records can be recorded

#include <string>
#include <vector>
#include "ew/DebugHelper.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowSpline3.h"
#include "ew/Dig3Tableau.h"

namespace ew {

// XXX forward declaration w/o include
  class Dig3Space;
// XXX forward declaration w/o include
  class Dig3View;

/// @brief Digitizing Application
///
/// @headerfile ew/Dig3.h "ew/Dig3.h"
/// ew::Dig3 implements a 3D digitizing application.
///
/// ew::Dig3 is a class without assignment or comparison.
/// There are private member variables.
///
/// An ew::Dig3 maintains 2 space elements, the template and the specimen.
///
/// View elements can be added and removed by creating or destroying
/// ew::Dig3View objects.

//XXX save/load state must be done in conjunction with save/load form
// latter not done automatically
// so dig3 does not have to deal with filenames
//XXX later maybe ? save file dirty or version
//XXX later maybe partial save file read/write
//XXX ? save -> state
// Full ew::Dig state consists of
//  specimen_form template_form
//  views, and their spaces, slice modes and links
//  save file (primary main and primary slice configuration)
//  configurations of other windows
// Subsiduary main/slice windows can have different on/off's but won't be
// recorded in save file.
// e.g may want to duplicate main window with surfaces transparent

//XXX review this
// ew::Dig3View accesses #views.

  class Dig3 {
    friend class Dig3View;
  public:
    enum space_index_t {
/// The index of the template space.
      SPACE_TEMPLATE = 0,
/// The index of the specimen space.
      SPACE_SPECIMEN = 1
    };
    enum tableau_flags_t {
/// Set the view mapping.
      TABLEAU_VIEW = 0x01,
/// Set the settings.
      TABLEAU_SETTINGS = 0x02,
/// Set everything.
      TABLEAU_ALL = 0x03
    };
    explicit Dig3(ew::DataflowNetwork *network);
    ~Dig3();
    inline int get_n_views() const;
    inline ew::Dig3View *const *get_views() const;
    inline ew::Dig3Space *const *get_spaces() const;
    inline const ew::DataflowSpline3 *get_spline_node() const;
    void save_tableau(ew::Dig3Tableau *outp,
     int template_main, int template_slice,
     int specimen_main, int specimen_slice);
//XXX ?could put this in ew::Dig3View
    void load_tableau(const ew::Dig3Tableau *inp, int view, unsigned int flags);
//XXX ??? do we allow file surface records in tableau but not in form file
//XXX     (tableau/form mismatch)
//XXX     would allow save file just to specify colour schemes, load form later
//XXX      instead of a speciific savefile/formfile combination
    void interpolate_tableau(const ew::Dig3Tableau *inp1,
     const ew::Dig3Tableau *inp2, double e, int view);
    ew::DataflowNetwork *const network;
    const ew::DebugHelper dbg;
    void dbg_check(const char *filename, int line);
  private:
    ew::Dig3Space *spaces[2];
    ew::DataflowSpline3 *spl;
    std::vector<ew::Dig3View *> views;
// undefined
    Dig3(const ew::Dig3 &that);
    Dig3 &operator=(const ew::Dig3 &that);
  };
}

/// @enum ew::Dig3::space_index_t
/// These are the indices of the 2 spaces in the array of spaces.

/// @enum ew::Dig3::tableau_flags_t
/// These are flags that can be or-ed and passed to #load_tableau.

/// @fn explicit ew::Dig3::Dig3(ew::DataflowNetwork *network)
/// This creates an empty digitizing application.
/// @param network
///   A pointer to a dataflow network.

/// @fn ew::Dig3::~Dig3()
/// This destroys the digitizing application.
/// Any ew::Dig3View widgets associated with it are also destroyed.

/// @fn int ew::Dig3::get_n_views() const
/// @return
///   The number of views.

/// @fn ew::Dig3View *const *ew::Dig3::get_views() const
/// @return
///   The view index.

/// @fn ew::Dig3Space *const *ew::Dig3::get_spaces() const
/// @return
///   The space index.
///   There are always 2 spaces and the index is constant.

/// @fn inline const ew::DataflowSpline3 *ew::Dig3::get_spline_node() const
/// @return
///    A pointer to the spline node that this #ew::Dig3 manages.

/// @fn void ew::Dig3::save_tableau(ew::Dig3Tableau *outp,
///  int template_main, int template_slice,
///  int specimen_main, int specimen_slice)
/// This creates a tableau record from the indicated views, specified by index.
/// The form filename fields of the tableau are left blank, and must be
/// explicitly set.
/// @param outp
///   Where to store the tableau record.
/// @param template_main
///   The index of a template main view to record.
/// @param template_slice
///   The index of a template slice view to record.
/// @param specimen_main
///   The index of a specimen main view to record.
/// @param specimen_slice
///   The index of a specimen slice view to record.

/// @fn void ew::Dig3::load_tableau(const ew::Dig3Tableau *inp, int view,
///  unsigned int flags)
/// This applies a tableau record to the indicated view.
/// The type of view and its space determine which part of the record is
/// applied.
/// The form filename fields are not applied.
/// The forms must be explicitly set beforehand.
/// @param inp
///   The tableau to apply.
/// @param view
///   The index of the view to apply it to.
/// @param flags
///   Flags of type #tableau_flags_t indicating which parts of the tableau
///   record to apply.

/// @fn void ew::Dig3::interpolate_tableau(const ew::Dig3Tableau *inp1,
///  const ew::Dig3Tableau *inp2, double e, int view)
/// This interpolates between tableau records and applies the result to the
/// indicated view.
/// Only the views are applied.
/// The settings of the tableau and the forms cannot be interpolated and need
/// to be manually applied when a new frame is reached in a filmstrip.
/// @param inp1, inp2
///   The tableaus to interpolate.
/// @param e
///   The interpolation parameter, with 0.0 corresponding to @a inp1 and 1.0
///   to @a inp.
///   @a e is not restricted to [0.0, 1.0].
/// @param view
///   The index of the view to apply it to.

/// @var ew::DataflowNetwork *const ew::Dig3::network
/// This points to the ew::DataflowNetwork that this ew::Dig3 was created
/// with.

inline ew::Dig3Space *const *
ew::Dig3::get_spaces() const
{
  return spaces;
}

inline const ew::DataflowSpline3 *
ew::Dig3::get_spline_node() const
{
  return spl;
}

inline int
ew::Dig3::get_n_views() const
{
  return views.size();
}

inline ew::Dig3View *const *
ew::Dig3::get_views() const
{
  if (views.empty()) {
    return 0;
  } else {
    return &views[0];
  }
}

#endif
