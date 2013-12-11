#ifndef EW_VIEW3ITEM_H_INCLUDED
#define EW_VIEW3ITEM_H_INCLUDED

// wdkg 2008-2010

#include <string>
#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DebugHelper.h"
#include "ew/View3Widget.h"

// Cannot make constructor argument dbg_final_name a virtual function because
// it is needed in the base constructor.

// The item destructor is protected to prevent static (non-new) item
// objects, which could not be deleted in the widget destructor.

//XXX review this
// ew::View3Widget accesses the private pure virtual functions, #get_prepared
// and #set_index.

namespace ew {
  class View3Item {
    friend class View3Widget;
  public:
    inline bool get_state() const;
    void set_state(bool st);
    virtual const ew::Bbox3 *get_bbox() = 0;
    inline int get_index() const;
    inline void destroy();
    ew::DataflowNetwork *const network;
    ew::View3Widget *const view;
    const int type;
    const int dbg_n;
    const ew::DebugHelper dbg;
  protected:
    View3Item(ew::View3Widget *i_view, int i_type,
     const char *dbg_final_name);
    virtual ~View3Item();
    inline void redraw_view_later();
    inline void set_this_highlight_item();
    bool prepared;
  private:
    virtual bool check_dataflow() = 0;
    virtual void clear_highlight() = 0;
    virtual void prepare() = 0;
    virtual void render() = 0;
    inline bool get_prepared() const;
    inline void set_index(int n);
    int next_dbg_n();
    bool state;
    int index;
// undefined
    View3Item(const ew::View3Item &that);
    View3Item &operator=(const ew::View3Item &that);
  };
}

/// @class ew::View3Item
/// @headerfile ew/View3Item.h "ew/View3Item.h"
/// @brief Item Base Class
///
/// ew::View3Item is the base class for items displayed in ew::View_widget.
///
/// ew::View3Item is a class without assignment or comparison.
/// There are private member variables.
///
/// Items must be created with @c new and then belong to the ew::View3Widget.
/// They must only be deleted indirectly with the #destroy method.
/// Remaining items will be automatically deleted when the ew::View3Widget is
/// destroyed.

/// @var ew::DataflowNetwork *const ew::View3Item::network
/// This points to the ew::DataflowNetwork that the #view was created
/// with.

/// @var ew::View3Widget *const ew::View3Item::view
/// This points to the ew::View3Widget the this ew::View3Item was created
/// with.

/// @var const int ew::View3Item::type
/// This is an arbitrary value supplied in the constructor.
/// It is client data, not touched by ew::View3Item or ew::View3Widget.

/// @fn virtual const ew::Bbox3 *ew::View3Item::get_bbox() = 0
/// @return
///   A pointer to a bounding box for the item as currently being displayed.

inline void
ew::View3Item::redraw_view_later()
{
  view->redraw_later();
}

inline void
ew::View3Item::set_this_highlight_item()
{
  view->set_highlight_item(this);
}

/// An item is displayed iff the state is @c true.
/// @return
///   The current state of the item.

inline bool
ew::View3Item::get_state() const
{
  return state;
}

inline bool
ew::View3Item::get_prepared() const
{
  return prepared;
}

/// @return
///   The current index of the item in the widget's item list.

inline int
ew::View3Item::get_index() const
{
  return index;
}

/// This removes the item from the widget and destroys
/// it.

inline void
ew::View3Item::destroy()
{
  delete this;
}

inline void
ew::View3Item::set_index(int index_v)
{
  index = index_v;
}

#endif
