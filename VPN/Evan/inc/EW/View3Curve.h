#ifndef EW_VIEW3CURVE_H_INCLUDED
#define EW_VIEW3CURVE_H_INCLUDED

// wdkg 2008-2010

#include "ew/DebugHelper.h"
#include "ew/Bbox3.h"
#include "ew/DataflowCurve3.h"
#include "ew/DataflowNetwork.h"
#include "ew/View3Item.h"
#include "ew/View3Widget.h"

//XXX linewidth

namespace ew {
  class View3Curve : public ew::View3Item {
  public:
    explicit View3Curve(ew::View3Widget *view, int i_type = 0);
    void set_curve(const ew::DataflowCurve3 *cur);
    inline const ew::DataflowCurve3 *get_curve() const;
    virtual const ew::Bbox3 *get_bbox();
    void set_color(const unsigned char *rgb);
    inline const unsigned char *get_color() const;
    void set_linewidth(double w);
    inline double get_linewidth() const;
    const ew::DebugHelper dbg;
  private:
    virtual ~View3Curve();
    virtual bool check_dataflow();
    virtual void clear_highlight();
    virtual void prepare();
    virtual void render();
    const ew::DataflowCurve3 *curve;
    unsigned long curve_checked_version;
    unsigned char color[3];
    double linewidth;
    unsigned int cur_dlist;
    unsigned int dlist;
  };
}

/// @class ew::View3Curve
/// @headerfile ew/View3Curve.h "ew/View3Curve.h"
/// @brief Curve Item
///
/// ew::View3Curve is an ew::View3Widget item to display a
/// ew::DataflowCurve3, a piecewise-linear curve.
///
/// ew::View3Curve is a class without assignment or comparison.
/// There are private member variables.

/// @return
///   A pointer to the current curve node being displayed, or 0.

inline const ew::DataflowCurve3 *
ew::View3Curve::get_curve() const
{
  return curve;
}

/// @return
///   A pointer to the current colour for the curve.

inline const unsigned char *
ew::View3Curve::get_color() const
{
  return color;
}

inline double
ew::View3Curve::get_linewidth() const
{
  return linewidth;
}

#endif
