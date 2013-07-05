// wdkg 2008-2010

#include <stdexcept>
#include <string>
#include <vector>
#include "ew/DebugHelper.h"
#include "ew/Debug.h"
#include "ew/ErrorLogic.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowNode.h"
#include "ew/DataflowSpline3.h"
#include "ew/Dig3.h"
#include "ew/Dig3Space.h"
#include "ew/Dig3View.h"
#include "ew/String.h"
#include "ew/Dig3Tableau.h"
#include "ew/View3Curve.h"
#include "ew/View3Item.h"
#include "ew/View3Surface.h"
#include "ew/View3Widget.h"

// It is perfectly safe to create more than one dig3, simultaneously or
// serially.
// In normal use, however, only one will be created in an application.
// It is not worth the visual distraction to have debug messages containing
// ew::Dig3 object numbers.

namespace {
  const char ClassName[] = "ew::Dig3";

  template<typename T>
  inline int
  VectorSize(const std::vector<T> &v)
  {
    return static_cast<int>(v.size());
  }
}

ew::Dig3::Dig3(ew::DataflowNetwork *i_network) :
 network(i_network),
 dbg(ClassName)
{
  dbg.on && dbg.dprintf("%s::%s(...) {", dbg.fn, "CTOR");
//XXX put these 3 into initializer
  spaces[0] = 0;
  spaces[1] = 0;
  spl = 0;
  try {
//XXX use auto_node
    spaces[0] = new ew::Dig3Space(this, 0);
    spaces[1] = new ew::Dig3Space(this, 1);
    spl = new ew::DataflowSpline3(network);
    spl->set_template(spaces[0]->get_form_node());
    spl->set_specimen(spaces[1]->get_form_node());
//XXX eliminate this catch
  } catch (std::exception) {
    delete spaces[0];
    delete spaces[1];
    if (spl) {
      spl->ew::DataflowNode::decr_ref_count();
    }
    throw;
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "CTOR");
}

ew::Dig3::~Dig3()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "DTOR");
  for (int i = get_n_views() - 1; i >= 0; i -= 1) {
    delete views[i];
  }
  spl->ew::DataflowNode::decr_ref_count();
  delete spaces[0];
  delete spaces[1];
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "DTOR");
}

void
ew::Dig3::save_tableau(ew::Dig3Tableau *outp,
 int lhs_main, int lhs_slice,
 int rhs_main, int rhs_slice)
{
  dbg.on && dbg.dprintf("%s::%s(*,%d,%d,%d,%d) {", dbg.fn, "save_tableau",
   lhs_main, lhs_slice, rhs_main, rhs_slice);
//XXX check args
  for (int i = 0; i < 3; i += 1) {
    outp->bg[i] =
     views[lhs_main]->ew::View3Widget::get_background_color()[i];
  }
  outp->slice_clip_ratio =
   views[lhs_slice]->ew::View3Widget::get_clip_ratio();
  for (int sp = 0; sp < 2; sp += 1) {
    int x_main;
    int x_slice;
    if (sp == 0) {
      x_main = lhs_main;
      x_slice = lhs_slice;
    } else {
      x_main = rhs_main;
      x_slice = rhs_slice;
    }
    outp->space[sp].main_view =
     *views[x_main]->ew::View3Widget::get_view_mapping();
    outp->space[sp].slice_view =
     *views[x_slice]->ew::View3Widget::get_view_mapping();
    outp->space[sp].show_slice_in_main =
     views[x_main]->slice_item_index[x_slice]->ew::View3Item::get_state();
    outp->space[sp].show_lmks_in_main =
     views[x_main]->get_landmarks_item()->ew::View3Item::get_state();
    outp->space[sp].show_lmks_in_slice =
     views[x_slice]->get_landmarks_item()->ew::View3Item::get_state();
    outp->space[sp].lmks_symbol =
     views[x_main]->get_landmarks_item()->get_symbol();
    for (int j = 0; j < 3; j += 1) {
      outp->space[sp].lmks_col[j] =
       views[x_main]->get_landmarks_item()->get_color()[j];
    }
    const ew::Form3 *form = spaces[sp]->get_form_node()->get_data();
    int n = form->curves.size();
    outp->space[sp].curve_settings.resize(n);
    for (int i = 0; i < n; i += 1) {
      outp->space[sp].curve_settings[i].id = form->curves[i].id;
      outp->space[sp].curve_settings[i].show_in_main =
       views[x_main]->get_curve_items()[i]->ew::View3Item::get_state();
      outp->space[sp].curve_settings[i].show_in_slice =
       views[x_slice]->get_curve_items()[i]->ew::View3Item::get_state();
      for (int j = 0; j < 3; j += 1) {
        outp->space[sp].curve_settings[i].col[j] =
         views[x_main]->get_curve_items()[i]->get_color()[j];
      }
    }
    n = form->surfaces.size();
    outp->space[sp].surface_settings.resize(n);
    for (int i = 0; i < n; i += 1) {
      outp->space[sp].surface_settings[i].id = form->surfaces[i].id;
      outp->space[sp].surface_settings[i].show_in_main =
       views[x_main]->get_surface_items()[i]->ew::View3Item::get_state();
      outp->space[sp].surface_settings[i].show_in_slice =
       views[x_slice]->get_surface_items()[i]->ew::View3Item::get_state();
      for (int j = 0; j < 3; j += 1) {
        outp->space[sp].surface_settings[i].front_col[j] =
         views[x_main]->get_surface_items()[i]->get_front_color()[j];
      }
      for (int j = 0; j < 3; j += 1) {
        outp->space[sp].surface_settings[i].back_col[j] =
         views[x_main]->get_surface_items()[i]->get_back_color()[j];
      }
    }
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "save_tableau");
}
//XXX forgot x_show_slice
//XXX need to add slice cols etc

void
ew::Dig3::load_tableau(const ew::Dig3Tableau *inp, int view,
 unsigned int flags)
{
  dbg.on && dbg.dprintf("%s::%s(*,%d,%u) {", dbg.fn, "load_tableau", view,
   flags);
  int sp = views[view]->get_space();
  bool is_slice = views[view]->get_slice_mode();
  if (flags & TABLEAU_VIEW) {
    if (is_slice) {
      views[view]->set_view_mapping(&inp->space[sp].slice_view);
    } else {
      views[view]->set_view_mapping(&inp->space[sp].main_view);
    }
  }
//XXX maybe treat on/off separate from settings
  if (flags & TABLEAU_SETTINGS) {
    views[view]->ew::View3Widget::set_background_color(inp->bg);
    if (is_slice) {
      views[view]->ew::View3Widget::set_clip_ratio(inp->slice_clip_ratio);
      views[view]->get_landmarks_item()->ew::View3Item::set_state(
       inp->space[sp].show_lmks_in_slice);
    } else {
      for (int i = 0; i < get_n_views(); i += 1) {
// Just apply to primary slice view.
//XXX this might need fixing
        if (views[view]->slice_item_index[i] != 0) {
          views[view]->slice_item_index[i]->ew::View3Item::set_state(
           inp->space[sp].show_slice_in_main);
          break;
        }
      }
      views[view]->get_landmarks_item()->
       ew::View3Item::set_state(inp->space[sp].show_lmks_in_main);
    }
    views[view]->get_landmarks_item()->set_symbol(inp->space[sp].lmks_symbol);
    views[view]->get_landmarks_item()->set_color(inp->space[sp].lmks_col);
    for (int i = 0;
     i < VectorSize(inp->space[sp].curve_settings); i += 1) {
      int pos;
      const ew::Form3 *form = spaces[sp]->get_form_node()->get_data();
      if (form->search_curve(&pos,
       inp->space[sp].curve_settings[i].id.c_str())) {
        views[view]->get_curve_items()[pos]->set_color(
         inp->space[sp].curve_settings[i].col);
        if (is_slice) {
          views[view]->get_curve_items()[pos]->ew::View3Item::set_state(
           inp->space[sp].curve_settings[i].show_in_slice);
        } else {
          views[view]->get_curve_items()[pos]->ew::View3Item::set_state(
           inp->space[sp].curve_settings[i].show_in_main);
        }
      }
    }
    for (int i = 0;
     i < VectorSize(inp->space[sp].surface_settings); i += 1) {
      int pos;
      const ew::Form3 *form = spaces[sp]->get_form_node()->get_data();
      if (form->search_surface(&pos,
       inp->space[sp].surface_settings[i].id.c_str())) {
        views[view]->get_surface_items()[pos]->set_front_color(
         inp->space[sp].surface_settings[i].front_col);
        views[view]->get_surface_items()[pos]->set_back_color(
         inp->space[sp].surface_settings[i].back_col);
        if (is_slice) {
          views[view]->get_surface_items()[pos]->ew::View3Item::set_state(
           inp->space[sp].surface_settings[i].show_in_slice);
        } else {
          views[view]->get_surface_items()[pos]->ew::View3Item::set_state(
           inp->space[sp].surface_settings[i].show_in_main);
        }
      }
    }
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "load_tableau");
}

void
ew::Dig3::interpolate_tableau(const ew::Dig3Tableau *inp1,
 const ew::Dig3Tableau *inp2, double e, int view)
{
  dbg.on && dbg.dprintf("%s::%s(*,*,%g,%d) {", dbg.fn, "interpolate_tableau",
   e, view);
  int sp = views[view]->get_space();
  ew::Transform3 tr, tr1, tr2;
  if (views[view]->get_slice_mode()) {
    tr1.set_to_inverse(&inp1->space[sp].slice_view);
    tr2.set_to_inverse(&inp2->space[sp].slice_view);
  } else {
    tr1.set_to_inverse(&inp1->space[sp].main_view);
    tr2.set_to_inverse(&inp2->space[sp].main_view);
  }
  tr.set_to_interpolation(&tr1, &tr2, e);
  tr.set_to_inverse(&tr);
  views[view]->set_view_mapping(&tr);
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "interpolate_tableau");
}

void
ew::Dig3::dbg_check(const char *filename, int line)
{
  try {
    for (int j = 0; j < get_n_views(); j += 1) {
      ew::Dig3View *v = get_views()[j];
      if (v->get_slice_mode() < -1 || v->get_slice_mode() > 1 ||
       v->get_space() < -1 || v->get_space() > 1 || v->get_index() != j) {
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
      if (v->get_space() == -1) {
        if (VectorSize(v->curve_item_index) != 0) {
          throw ew::ErrorLogic(__FILE__, __LINE__);
        }
      } else {
        if (VectorSize(v->curve_item_index) !=
         spaces[v->get_space()]->get_n_curve_nodes()) {
          throw ew::ErrorLogic(__FILE__, __LINE__);
        }
      }
      for (int i = 0; i < VectorSize(v->curve_item_index); i += 1) {
        if (v->get_curve_items()[i] == 0) {
          throw ew::ErrorLogic(__FILE__, __LINE__);
        }
      }
      if (v->get_space() == -1) {
        if (VectorSize(v->surface_item_index) != 0) {
          throw ew::ErrorLogic(__FILE__, __LINE__);
        }
      } else {
        if (VectorSize(v->surface_item_index) !=
         spaces[v->get_space()]->get_n_surface_nodes()) {
          throw ew::ErrorLogic(__FILE__, __LINE__);
        }
      }
      for (int i = 0; i < VectorSize(v->surface_item_index); i += 1) {
        if (v->get_surface_items()[i] == 0) {
          throw ew::ErrorLogic(__FILE__, __LINE__);
        }
      }
      if (VectorSize(v->slice_item_index) != get_n_views()) {
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
      for (int i = 0; i < VectorSize(v->slice_item_index); i += 1) {
        ew::Dig3View *vi = get_views()[i];
        if (v->get_space() != vi->get_space() || v->get_space() == -1 ||
         vi->get_space() == -1) {
          if (v->slice_item_index[i] != 0) {
            throw ew::ErrorLogic(__FILE__, __LINE__);
          }
        } else if (v->get_slice_mode() == -1 || vi->get_slice_mode() == -1) {
          if (v->slice_item_index[i] != 0) {
            throw ew::ErrorLogic(__FILE__, __LINE__);
          }
        } else if (v->get_slice_mode() == 1 && i != j) {
          if (v->slice_item_index[i] != 0) {
            throw ew::ErrorLogic(__FILE__, __LINE__);
          }
        } else if (v->get_slice_mode() == 1 && i == j) {
          if (v->slice_item_index[i] == 0) {
            throw ew::ErrorLogic(__FILE__, __LINE__);
          }
        } else if (vi->get_slice_mode() == 0) {
          if (v->slice_item_index[i] != 0) {
            throw ew::ErrorLogic(__FILE__, __LINE__);
          }
        } else {
          if (v->slice_item_index[i] == 0) {
            throw ew::ErrorLogic(__FILE__, __LINE__);
          }
        }
      }
    }
    for (int sp = 0; sp < 2; sp += 1) {
      if (spaces[sp]->get_n_curve_nodes() !=
       VectorSize(spaces[sp]->get_form_node()->get_data()->curves)) {
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
    }
    for (int sp = 0; sp < 2; sp += 1) {
      if (spaces[sp]->get_n_surface_nodes() !=
       VectorSize(spaces[sp]->get_form_node()->get_data()->surfaces)) {
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
    }
  } catch (ew::ErrorLogic &e) {
    dbg.on && dbg.dprintf("%s::%s failed from %s:%d", dbg.fn, "dbg_check",
     filename, line);
    throw;
  }
}
