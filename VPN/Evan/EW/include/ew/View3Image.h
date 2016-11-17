#ifndef EW_VIEW3IMAGE_H_INCLUDED
#define EW_VIEW3IMAGE_H_INCLUDED

// wdkg 2008-2010

//XXX explicit about regular/non regular
//XXX implement

#include "ew/Bbox3.h"
#include "ew/DataflowImage3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DebugHelper.h"
#include "ew/View3Item.h"
#include "ew/View3Widget.h"

namespace ew {
  class View3Image : public ew::View3Item {
  public:
    explicit View3Image(ew::View3Widget *view, int i_type = 0);
    void set_image(const ew::DataflowImage3 *im);
    inline const ew::DataflowImage3 *get_image() const;
    virtual const ew::Bbox3 *get_bbox();
    void set_color(const unsigned char *rgb);
    inline const unsigned char *get_color() const;
    const ew::DebugHelper dbg;
  private:
    virtual ~View3Image();
    virtual bool check_dataflow();
    virtual void clear_highlight();
    virtual void prepare();
    virtual void render();
    const ew::DataflowImage3 *image;
    unsigned long image_checked_version;
    unsigned char color[3];
    unsigned int dlist;
//XXX temporary
    ew::Bbox3 bbox;
  };
}

/// @class ew::View3Image
/// @headerfile ew/View3Image.h "ew/View3Image.h"
/// @brief Image Item
///
/// ew::View3Image is an ew::View3Widget item to display a
/// ew::DataflowImage3, a raster image projected onto an array of points.
///
/// ew::View3Image is a class without assignment or comparison.
/// There are private member variables.
///
/// This is barely implemented, and currently just displays a solid rectangle,
/// on a 2x2x1 rectangular array of points.
///
/// The default colour is [128, 128, 128].

/// @return
///   A pointer to the current image node being displayed, or 0.

inline const ew::DataflowImage3 *
ew::View3Image::get_image() const
{
  return image;
}

/// @return
///   A pointer to the current colour for the image.

inline const unsigned char *
ew::View3Image::get_color() const
{
  return color;
}

#endif
