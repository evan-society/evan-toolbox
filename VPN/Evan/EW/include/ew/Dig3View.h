#ifndef EW_DIG3VIEW_H_INCLUDED
#define EW_DIG3VIEW_H_INCLUDED

// wdkg 2008-2010

#include <vector>
#include "ew/DebugHelper.h"
#include "ew/DataflowForm3.h"
#include "ew/Form3.h"
#include "ew/Transform3.h"
#include "ew/View3Image.h"
#include "ew/View3Landmarks.h"
#include "ew/DataflowArray3E.h"
#include "ew/DataflowImage3.h"
#include "ew/DataflowNetwork.h"
#include "ew/View3Curve.h"
#include "ew/View3Surface.h"
#include "ew/View3Widget.h"

namespace ew {

// XXX forward declaration w/o include
  class Dig3;
// XXX forward declaration w/o include
  class Dig3Space;

/// @brief View Element
///
/// @headerfile ew/Dig3View.h "ew/Dig3View.h"
/// ew::Dig3View implements a view element of ew::Dig3.
///
/// ew::Dig3View is a class without assignment or comparison.
/// There are private member variables.
///
/// The settings and item settings of the base ew::View3Widget, may be
/// directly manipulated, except for the view mapping.
/// The toolkit interaction of this widget should be handled by inheriting this
/// class and implementing the pure virtual methods of ew::View3Widget.

//XXX doc and enforce conditions on links (set_link, destroy)

//XXX review these
// ew::Dig3 accesses #curve_item_index, #surface_item_index and
// #slice_item_index.
// ew::Dig3Space accesses #curve_item_index and #surface_item_index.

  class Dig3View : public ew::View3Widget {
    friend class Dig3Space;
    friend class Dig3;
  public:
    enum item_t {
/// Item that represents the primary slice.
      ITEM_SLICE,
/// Item that represents form landmarks and semi-landmarks.
      ITEM_LANDMARKS,
/// Item that represents form (note warped) curves.
      ITEM_CURVE,
/// Item that represents form (note warped) surfaces.
      ITEM_SURFACE
    };
//XXX no space/slice_mode in constuctor: set space to -1 initially
    Dig3View(ew::Dig3 *dig3, int space, int slice_mode);
    ~Dig3View();
//XXX accessor funs for view_link view_link_tr
    void set_slice_mode(int sm);
    inline int get_slice_mode() const;
    void set_space(int sp);
    inline int get_space() const;
    void set_link(ew::Dig3View *v, const ew::Transform3 *tr);
    void set_view_mapping(const ew::Transform3 *tr);
    void view_post_compose(const ew::Transform3 *tr);
    inline ew::View3Image *const *get_slice_items() const;
    inline ew::View3Curve *const *get_curve_items() const;
    inline ew::View3Surface *const *get_surface_items() const;
    inline ew::View3Landmarks *get_landmarks_item() const;
    inline int get_index() const;
    ew::Dig3 *const dig3;
    const ew::DebugHelper dbg;
  private:
    std::vector<ew::View3Curve *> curve_item_index;
    std::vector<ew::View3Surface *> surface_item_index;
    std::vector<ew::View3Image *> slice_item_index;
    void sync_view();
    int index;
    int space;
    int slice_mode;
    ew::Dig3View *view_link;
    ew::Transform3 view_link_tr;
    ew::DataflowArray3E *slice_array_node;
    ew::DataflowImage3 *slice_image_node;
    ew::View3Landmarks *landmarks_item;
// undefined
    Dig3View(const ew::Dig3View &that);
    Dig3View &operator=(const ew::Dig3View &that);
  };

}

/// @enum ew::Dig3View::item_t
/// These are the possible values for ew::View3Item::type for items created by
/// this class.

/// @fn ew::Dig3View::Dig3View(ew::Dig3 *dig3, int space, int slice_mode)
/// This creates a view element of the @a dig3.
/// @param dig3
///   The ew::Dig3 this view element will belong to.
/// @param space
///   Which space of the @a dig3 this view element will initially display.
///   Either 0 or 1.
/// @param slice_mode
///   Whether this will be a slice view.
///   Either 0 or 1.

/// @fn ew::Dig3View::~Dig3View()
/// This removes the view from #dig3, and destroys it.

/// @fn void ew::Dig3View::set_slice_mode(int sm)
/// @param sm
///   Whether this view should now be a slice view.
///   Either 0 or 1.

/// @fn int ew::Dig3View::get_slice_mode() const
/// @return
///   Whether this is a slice view.
///   Either 0 or 1.

/// @fn void ew::Dig3View::set_space(int sp)
/// @param sp
///   The index of the space this view should now be associated with.
///   Either 0 or 1.

/// @fn int ew::Dig3View::get_space() const
/// @return
///   The index of the space this view is currently associated with.
///   Either 0 or 1.

/// @fn void ew::Dig3View::set_link(ew::Dig3View *v,
///  const ew::Transform3 *tr)
/// This sets up a link between the view mapping of this view and of another
/// one.
/// Links must be disjoint cycles.
/// The composition of the transforms should be the identity for sensible
/// results.
/// @param v
///   The view to link to.
/// @param tr
///   The transform that should be left-composed with this view's view mapping
///   to get the link views view mapping.

/// @fn void ew::Dig3View::set_view_mapping(const ew::Transform3 *tr)
/// This operates on the view mapping of the base ew::View3Widget, and
/// synchronizes with other views and with slices as necessary.
/// This method has the same name as a non-virtual method in the base class.
/// @param tr
///   As for ew::View3Widget::set_view_mapping().

/// @fn void ew::Dig3View::view_post_compose(const ew::Transform3 *tr)
/// This operates on the view mapping of the base ew::View3Widget, and
/// synchronizes with other views and with slices as necessary.
/// This method has the same name as a non-virtual method in the base class.
/// @param tr
///   As for ew::View3Widget::view_post_compose().

/// @fn inline ew::View3Image *const *ew::Dig3View::get_slice_items() const
/// @return
///   The slice item index.
///   The i'th entry is the item displaying the slice of the i'th view of the
///   #dig3, or zero if the i'th view is not in slice mode, or is not
///   associated with the same space.

/// @fn ew::View3Curve *const *ew::Dig3View::get_curve_items() const
/// @return
///   The curve item index.
///   The i'th entry is the item displaying the i'th curve of the space this
///   view is currently associated with.

/// @fn ew::View3Surface *const *ew::Dig3View::get_surface_items() const
/// @return
///   The surface item index.
///   The i'th entry is the item displaying the i'th surface of the space this
///   view is currently associated with.

/// @fn ew::View3Landmarks *const *ew::Dig3View::get_landmarks_item() const
/// @return
///   The landmarks item.

/// @fn int ew::Dig3View::get_index() const
/// @return
///   The current index of this view element in the view index of #dig3.

/// @var ew::Dig3 *const ew::Dig3View::dig3
/// This points to the ew::Dig3 that this ew::Dig3View was created with.

inline int
ew::Dig3View::get_slice_mode() const
{
  return slice_mode;
}

inline int
ew::Dig3View::get_space() const
{
  return space;
}

inline ew::View3Image *const *
ew::Dig3View::get_slice_items() const
{
  if (slice_item_index.empty()) {
    return 0;
  } else {
    return &slice_item_index[0];
  }
}

inline ew::View3Curve *const *
ew::Dig3View::get_curve_items() const
{
  if (curve_item_index.empty()) {
    return 0;
  } else {
    return &curve_item_index[0];
  }
}

inline ew::View3Surface *const *
ew::Dig3View::get_surface_items() const
{
  if (surface_item_index.empty()) {
    return 0;
  } else {
    return &surface_item_index[0];
  }
}

inline ew::View3Landmarks *
ew::Dig3View::get_landmarks_item() const
{
  return landmarks_item;
}

inline int
ew::Dig3View::get_index() const
{
  return index;
}

#endif
