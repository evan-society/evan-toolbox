#ifndef EW_VIEW3SURFACE_H_INCLUDED
#define EW_VIEW3SURFACE_H_INCLUDED

// wdkg 2008-2010

#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowSurface3.h"
#include "ew/DebugHelper.h"
#include "ew/View3Item.h"
#include "ew/View3Widget.h"

namespace ew {
  class View3Surface : public ew::View3Item {
  public:
    explicit View3Surface(ew::View3Widget *view, int i_type = 0);
    void set_surface(const ew::DataflowSurface3 *sur);
    inline const ew::DataflowSurface3 *get_surface() const;
    virtual const ew::Bbox3 *get_bbox();
    void set_front_color(const unsigned char *rgb);
    inline const unsigned char *get_front_color() const;
    void set_back_color(const unsigned char *rgb);
    inline const unsigned char *get_back_color() const;
//XXX not currently supported
    enum render_mode_t {
      MODE_SMOOTH,
      MODE_SILHOUETTE,
      MODE_WIREFRAME
    };
    static const int N_MODES = 3;
    static const char *const mode_names[];
    void set_render_mode(int m);
    inline int get_render_mode() const;
    void set_wireframe_linewidth(double w);
    inline double get_wireframe_linewidth() const;
    const ew::DebugHelper dbg;
  private:
    virtual ~View3Surface();
    virtual bool check_dataflow();
    virtual void clear_highlight();
    virtual void prepare();
    virtual void render();
    const ew::DataflowSurface3 *surface;
    unsigned long surface_checked_version;
    int render_mode;
    unsigned char front_color[3];
    unsigned char back_color[3];
    double wireframe_linewidth;
    unsigned int sur_dlist;
    unsigned int dlist;
  };
}

/// @class ew::View3Surface
/// @headerfile ew/View3Surface.h "ew/View3Surface.h"
/// @brief Surface Item
///
/// ew::View3Surface is an ew::View3Widget item to display a
/// ew::DataflowSurface3, a triangulated surface.
///
/// ew::View3Surface is a class without assignment or comparison.
/// There are private member variables.

/// @return
///   A pointer to the current surface node being displayed, or 0.

inline const ew::DataflowSurface3 *
ew::View3Surface::get_surface() const
{
  return surface;
}

/// @return
///   A pointer to the current colour for the front of the surface.

inline const unsigned char *
ew::View3Surface::get_front_color() const
{
  return front_color;
}

/// @return
///   A pointer to the current colour for the back of the surface.

inline const unsigned char *
ew::View3Surface::get_back_color() const
{
  return back_color;
}

inline double
ew::View3Surface::get_wireframe_linewidth() const
{
  return wireframe_linewidth;
}

inline int
ew::View3Surface::get_render_mode() const
{
  return render_mode;
}

#endif
