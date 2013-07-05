#ifndef EW_VIEWWIDGET_H_INCLUDED
#define EW_VIEWWIDGET_H_INCLUDED

// wdkg 2008-2010

//XXX callback to activate context (not in destructors?)
//XXX no glErrors on calls
//XXX schedule_idle_handler exceptions passed thru to operations that attempt
// to change network
//XXX facility to facilitate automatically handling stacking order
//XXX ? use proj_to_sur or variant in pick

#include <stdexcept>
#include <string>
#include <vector>
#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNotifier.h"
#include "ew/DebugHelper.h"
#include "ew/FriendVariable.h"
#include "ew/Transform3.h"

//XXX ?polygon offset
//XXX review these
// ew::View3Item accesses #redraw_later, #set_highlight_item, #add_item and
// #del_item.

namespace ew {
// XXX forward declaration w/o include
  class View3Item;
// XXX forward declaration w/o include
  class View3Pick;

  class View3Widget : private ew::DataflowNotifier {
    friend class View3Item;
  public:
    explicit View3Widget(ew::DataflowNetwork *i_network);
    virtual ~View3Widget();
    void reset();
    void set_clip_ratio(double cr);
    inline double get_clip_ratio() const;
    void set_view_mapping(const ew::Transform3 *tr);
    void view_post_compose(const ew::Transform3 *tr);
    inline const ew::Transform3 *get_view_mapping() const;
    void get_bbox(ew::Bbox3 *b) const;
    void get_pointer_location(double *loc, double x, double y, double z = 0.0)
     const;
    void get_center_location(double *loc) const;
    void get_pointer_translate_tr(ew::Transform3 *tr,
     double x, double y, double z) const;
    void get_pointer_roll_tr(ew::Transform3 *tr, double x, double y) const;
    void get_pointer_twist_tr(ew::Transform3 *tr, double x0, double y0,
     double x1, double y1, double acc) const;
    void get_pointer_scale_tr(ew::Transform3 *tr, double x0, double y0,
     double x1, double y1, double acc) const;
    void get_jump_tr(ew::Transform3 *tr, const double *loc, bool do_z) const;
    bool get_bbox_tr(ew::Transform3 *tr, const ew::Bbox3 *bbox, bool do_z,
     bool do_scale) const;
    void set_background_color(const unsigned char *rgb);
    inline const unsigned char *get_background_color() const;
    void set_highlight_color(const unsigned char *rgb);
    inline const unsigned char *get_highlight_color() const;
    inline ew::View3Item *get_highlight_item() const;
    void clear_highlight();
    void set_use_depth(bool d);
    inline bool get_use_depth() const;
    void move_item(ew::View3Item *it, int i);
    inline ew::View3Item *const *get_items() const;
    inline int get_n_items() const;
    bool pick(double x, double y, double sz, double burrow,
     ew::View3Item *constrain_it, int constrain_cmpt, int constrain_dim,
     ew::View3Item **pick_it, int *pick_cmpt, int *pick_dim,
     double *pick_z);
    void pickv(std::vector<ew::View3Pick> &outp, double x, double y,
     double sz);
// private to the library
    ew::FriendVariable<int, ew::View3Item> dbg_last_item_n;
    ew::DataflowNetwork *const network;
    const int dbg_n;
    const ew::DebugHelper dbg;
  protected:
    void init_gl_context();
    void set_window_size(int w, int h);
    void render();
    void idle_handler();
    void set_currently_unrendered();
    void destruction_in_progress();
  private:
    inline void redraw_later();
    void set_highlight_item(ew::View3Item *it);
    void add_item(ew::View3Item *it);
    void del_item(ew::View3Item *it);
// really private
    virtual void schedule_idle_handler_cb() = 0;
    virtual void redraw_cb() = 0;
    virtual void notifier_handle_cb();
    void dataflow_check();
    int next_dbg_n();
    bool use_depth;
    bool has_double_buffer;
    bool currently_rendered;
    bool idle_pending;
    bool redraw_required;
    bool destruction_pending;
    unsigned long dataflow_check_cycle;
    double clip_ratio;
    unsigned char background_color[3];
    unsigned char highlight_color[3];
    ew::View3Item *highlight_item;
    int winw;
    int winh;
    double projmatrix[16];
    ew::Transform3 view_mapping;
    ew::Transform3 window_mapping;
    std::vector<ew::View3Item *> items;
// undefined
    View3Widget(const ew::View3Widget &that);
    View3Widget &operator=(const ew::View3Widget &that);
  };
}

/// @class ew::View3Widget
/// @headerfile ew/View3Widget.h "ew/View3Widget.h"
/// @brief 3D viewing widget
///
/// This class implements a widget that manages a 3D scene consisting of a
/// list of items.
///
/// Items, derived from ew::View3Item, may be created in the widget.
///
/// This class is independent of any particular toolkit and it is intended that
/// the class will be inherited by a wrapper that implements the necessary
/// interface with the toolkit.
///
/// All view widgets associated with a particular ew::DataflowNetwork must
/// share OpenGL contexts.
/// The context must have a depth buffer.
/// Many class methods require this context to be active when they are called.
///
/// Items in a widget are kept in a list in rendering order.
/// If the depth buffer is not being used, later items will be rendered on top
/// of earlier items.
/// It the depth buffer is being used, the rendering order only effects items
/// with exactly the same depth.
/// Items are put at the end of the order initially on creation.

/// @var ew::DataflowNetwork *const ew::View3Widget::network
/// This points to the ew::DataflowNetwork that this ew::View3Widget was
/// created with.

/// @fn virtual void ew::View3Widget::schedule_idle_handler_cb() = 0
/// This is a callback that must be implemented by the inheriting class.
/// It must arrange for #idle_handler to be called later when the GUI is idle.
/// Outstanding handlers must be cancelled by the inheriting class on
/// destruction.

/// @fn virtual void ew::View3Widget::redraw_cb() = 0
/// This is a callback than must be implemented in the inheriting class.
/// It must arrange for the widget to be rendered immediately.

/// @return the size of the list of
/// items

inline int
ew::View3Widget::get_n_items() const
{
  return items.size();
}

/// @return a pointer to the list of
/// items

inline ew::View3Item *const *
ew::View3Widget::get_items() const
{
  if (items.size() == 0) {
    return 0;
  } else {
    return &items[0];
  }
}

/// @return the current clipping
/// ratio

inline double
ew::View3Widget::get_clip_ratio() const
{
  return clip_ratio;
}

/// @return the address of an array containing the current background colour
/// components

inline const unsigned char *
ew::View3Widget::get_background_color() const
{
  return background_color;
}

/// @return the address of an array containing the current highlight colour
/// components

inline const unsigned char *
ew::View3Widget::get_highlight_color() const
{
  return highlight_color;
}

/// @return the address of the current highlighted item, or
/// 0

inline ew::View3Item *
ew::View3Widget::get_highlight_item() const
{
  return highlight_item;
}

/// @return the address of a transform containing the current view
/// mapping

inline const ew::Transform3 *
ew::View3Widget::get_view_mapping() const
{
  return &view_mapping;
}

inline void
ew::View3Widget::redraw_later()
{
  if (currently_rendered && !idle_pending && !destruction_pending) {
    schedule_idle_handler_cb();
    idle_pending = true;
  }
  redraw_required = true;
}

/// @return the current
/// setting

inline bool
ew::View3Widget::get_use_depth() const
{
  return use_depth;
}

#endif
