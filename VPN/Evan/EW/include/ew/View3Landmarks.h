#ifndef EW_VIEW3LANDMARKS_H_INCLUDED
#define EW_VIEW3LANDMARKS_H_INCLUDED

// wdkg 2009-2010

//XXX ?later highlight by id
//XXX labels, select color, select n, radius, relax, symbol(relaxdims)
//XXX settings(lmk-type)

#include "ew/Bbox3.h"
#include "ew/DataflowForm3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DebugHelper.h"
#include "ew/View3Item.h"
#include "ew/View3Widget.h"

namespace ew {
  class View3Landmarks : public ew::View3Item {
  public:
    explicit View3Landmarks(ew::View3Widget *view, int i_type = 0);
    void set_form(const ew::DataflowForm3 *frm);
    inline const ew::DataflowForm3 *get_form() const;
    virtual const ew::Bbox3 *get_bbox();
    void set_color(const unsigned char *rgb);
    inline const unsigned char *get_color() const;
    void set_highlight(int ps, int i);
    inline int get_highlight_ps() const;
    inline int get_highlight_i() const;
    enum symbol_t {
      SYMBOL_CROSS,
      SYMBOL_CIRCLE,
      SYMBOL_DOT
    };
    static const int N_SYMBOLS = 3;
    static const char *const symbol_names[];
    void set_symbol(int sym);
    inline int get_symbol() const;
    const ew::DebugHelper dbg;

    void renderColorCoded();
    bool pick(  const double *const mvpMatrix,
                const float ndcX,
                const float ndcY,
                const float ndcToleranceX,
                const float ndcToleranceY,
                int &lmID ) const;

  private:
    virtual ~View3Landmarks();
    virtual bool check_dataflow();
    virtual void clear_highlight();
    virtual void prepare();
    virtual void render();
    const ew::DataflowForm3 *form;
    unsigned long form_checked_version;
    int symbol;
    unsigned char color[3];
    unsigned int dlist;
    int highlight_ps;
    int highlight_i;
  };
}

/// @class ew::View3Landmarks
/// @headerfile ew/View3Landmarks.h "ew/View3Landmarks.h"
/// @brief Landmarks Item
///
/// ew::View3Landmarks is an ew::View3Widget item to display the landmarks in
/// a ew::DataflowForm3.
///
/// ew::View3Landmarks is a class without assignment or comparison.
/// There are private member variables.

/// @enum ew::View3Landmarks::symbol_t
/// These are the possible values for #set_symbol and #get_symbol.
/// The first, with value zero, is #SYMBOL_CROSS.

/// @var ew::View3Landmarks::SYMBOL_CROSS
/// A cross symbol.

/// @var ew::View3Landmarks::SYMBOL_CIRCLE
/// A circle symbol.

/// @var ew::View3Landmarks::SYMBOL_DOT
/// A dot symbol.

/// @var const int ew::View3Landmarks::N_SYMBOLS
/// They number of possible #symbol_t values.

/// @return
///   A pointer to the current form node being displayed, or 0.

inline const ew::DataflowForm3 *
ew::View3Landmarks::get_form() const
{
  return form;
}

/// @return
///   A pointer to the current colour for the landmark symbols.

inline const unsigned char *
ew::View3Landmarks::get_color() const
{
  return color;
}

/// @return
///   The current landmark symbol.

inline int
ew::View3Landmarks::get_symbol() const
{
  return symbol;
}

/// @return
///   The currently highlighted pointset index, or -1.

inline int
ew::View3Landmarks::get_highlight_ps() const
{
  return highlight_ps;
}

/// @return
///   The currently highlighted landmark index within the pointset, or -1.

inline int
ew::View3Landmarks::get_highlight_i() const
{
  return highlight_i;
}

#endif
