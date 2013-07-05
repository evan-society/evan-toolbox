// wdkg 2008-2010
//XXX sort after reading

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlversion.h>
#include <libxml/xmlwriter.h>
#include "ew/ErrorIO.h"
#include "ew/String.h"
#include "ew/Dig3Tableau.h"
#include "ew/Dig3SetCurve.h"
#include "ew/Dig3TableauSpace.h"
#include "ew/Dig3SetSurface.h"
#include "ew/Transform3.h"
#include "ew/View3Landmarks.h"
#include "XmlReader.h"
#include "XmlWriter.h"

namespace {

  inline const char *
  CastCcp(const xmlChar *p)
  {
    return reinterpret_cast<const char *>(p);
  }

  template<typename T>
  inline int
  VectorSize(const std::vector<T> &v)
  {
    return static_cast<int>(v.size());
  }

// This clears the vector and frees the reserve capacity.
  template<typename T>
  inline void
  VectorReset(std::vector<T> &v)
  {
    std::vector<T> v1;
    v.swap(v1);
  }
}

/// This resets the object to its initial
/// state.

void
ew::Dig3Tableau::reset()
{
  slice_clip_ratio = 0.01;
  bg[0] = 0;
  bg[1] = 0;
  bg[2] = 0;
  space[0].main_view.set_to_identity();
  space[0].slice_view.set_to_identity();
  space[0].show_slice_in_main = true;
  space[0].show_lmks_in_main = true;
  space[0].show_lmks_in_slice = true;
  space[0].lmks_symbol = ew::View3Landmarks::SYMBOL_CROSS;
  space[0].lmks_col[0] = 0;
  space[0].lmks_col[1] = 255;
  space[0].lmks_col[2] = 0;
  VectorReset(space[0].curve_settings);
  VectorReset(space[0].surface_settings);
  space[1].main_view.set_to_identity();
  space[1].slice_view.set_to_identity();
  space[1].show_slice_in_main = true;
  space[1].show_lmks_in_main = true;
  space[1].show_lmks_in_slice = true;
  space[1].lmks_symbol = ew::View3Landmarks::SYMBOL_CROSS;
  space[1].lmks_col[0] = 0;
  space[1].lmks_col[1] = 255;
  space[1].lmks_col[2] = 0;
  VectorReset(space[1].curve_settings);
  VectorReset(space[1].surface_settings);
}

/// This reads a save file.
/// The file can be compressed with gzip compression.
/// @param[in] outp
///   The vector to store the records read in.
/// @param file
///   The file name.
/// @exception std::bad_alloc
/// @exception ew::IOError

void
ew::Dig3Tableau::read_file(std::vector<ew::Dig3Tableau> *outp,
 const char *file)
{
  std::vector<ew::Dig3Tableau> v;
  ew::XmlReader rd(file, "ew_dig3", "1.0");
  int n = 0;
  for (xmlNode *child = rd.get_children(); child != 0; child = child->next) {
    if (child->type != XML_ELEMENT_NODE) {
      continue;
    }
    if (std::strcmp(CastCcp(child->name), "tableau") != 0) {
      continue;
    }
    n += 1;
  }
// This does n + 1 ctors and 1 dtor.
// The overhead is unavoidable, because we need a fresh ew::Dig3Tableau for each
// iteration of the loop.
  v.resize(n);
  n = 0;
  for (xmlNode *child = rd.get_children(); child != 0; child = child->next) {
    if (child->type != XML_ELEMENT_NODE) {
      continue;
    }
    if (std::strcmp(CastCcp(child->name), "tableau") != 0) {
      continue;
    }
    for (xmlNode *gchild = child->xmlChildrenNode; gchild != 0;
     gchild = gchild->next) {
      bool istt;
      int o, sp;
      if (std::strncmp(CastCcp(gchild->name), "template_", 9) == 0) {
        istt = true;
        sp = 0;
        o = 9;
      } else if (std::strncmp(CastCcp(gchild->name), "specimen_", 9) == 0) {
        istt = true;
        sp = 1;
        o = 9;
      } else {
        istt = false;
      }
      if (!istt) {
        if (std::strcmp(CastCcp(gchild->name), "slice_clip_ratio") == 0) {
          rd.read_doubles(&v[n].slice_clip_ratio, 1, gchild);
        } else if (std::strcmp(CastCcp(gchild->name), "background") == 0) {
          int ii[3];
          rd.read_ints(ii, 3, gchild);
          if (ii[0] < 0 || ii[0] > 255 || ii[1] < 0 || ii[1] > 255 ||
           ii[2] < 0 || ii[2] > 255) {
            throw rd.err_key_line("background", gchild->line);
          }
          v[n].bg[0] = ii[0];
          v[n].bg[1] = ii[1];
          v[n].bg[2] = ii[2];
        }
      } else {
        if (std::strcmp(CastCcp(gchild->name) + o, "form") == 0) {
          rd.read_attribute_filename(&v[n].space[sp].form_filename,  "file",
           gchild);
        } else if (std::strcmp(CastCcp(gchild->name) + o, "view") == 0) {
          rd.read_transform3(&v[n].space[sp].main_view, gchild);
        } else if (std::strcmp(CastCcp(gchild->name) + o, "slice") == 0) {
          rd.read_transform3(&v[n].space[sp].slice_view, gchild);
        } else if (std::strcmp(CastCcp(gchild->name) + o, "show_slice") ==
         0) {
          int j;
          rd.read_ints(&j, 1, gchild);
          v[n].space[sp].show_slice_in_main = (j != 0);
        } else if (std::strcmp(CastCcp(gchild->name) + o,
         "show_lmks_main") == 0) {
          int j;
          rd.read_ints(&j, 1, gchild);
          v[n].space[sp].show_lmks_in_main = (j != 0);
        } else if (std::strcmp(CastCcp(gchild->name) + o,
         "show_lmks_slice") == 0) {
          int j;
          rd.read_ints(&j, 1, gchild);
          v[n].space[sp].show_lmks_in_slice = (j != 0);
        } else if (std::strcmp(CastCcp(gchild->name) + o, "lmks_symbol") ==
         0) {
          int j;
          rd.read_list(&j, ew::View3Landmarks::symbol_names, gchild);
          v[n].space[sp].lmks_symbol = j;
        } else if (std::strcmp(CastCcp(gchild->name) + o, "lmks_color") ==
         0) {
          int ii[3];
          rd.read_ints(ii, 3, gchild);
          if (ii[0] < 0 || ii[0] > 255 ||
           ii[1] < 0 || ii[1] > 255 ||
           ii[2] < 0 || ii[2] > 255) {
            throw rd.err_key_line("lmks_color", gchild->line);
          }
          v[n].space[sp].lmks_col[0] = ii[0];
          v[n].space[sp].lmks_col[1] = ii[1];
          v[n].space[sp].lmks_col[2] = ii[2];
        } else if (std::strcmp(CastCcp(gchild->name) + o, "curve") == 0) {
          ew::Dig3TableauSpace *spc = v[n].space + sp;
          int m = spc->curve_settings.size();
          spc->curve_settings.resize(m + 1);
          ew::Dig3SetCurve *sf = &spc->curve_settings[m];
          rd.read_attribute(&sf->id, "id", gchild);
          for (xmlNode *ggchild = gchild->xmlChildrenNode; ggchild != 0;
           ggchild = ggchild->next) {
            if (ggchild->type != XML_ELEMENT_NODE) {
              continue;
            }
            if (std::strcmp(CastCcp(ggchild->name), "show_main") == 0) {
              int j;
              rd.read_ints(&j, 1, ggchild);
              sf->show_in_main = (j != 0);
            } else if (std::strcmp(CastCcp(ggchild->name), "show_slice") ==
             0) {
              int j;
              rd.read_ints(&j, 1, ggchild);
              sf->show_in_slice = (j != 0);
            } else if (std::strcmp(CastCcp(ggchild->name), "color") == 0) {
              int ii[3];
              rd.read_ints(ii, 3, ggchild);
              if (ii[0] < 0 || ii[0] > 255 ||
               ii[1] < 0 || ii[1] > 255 ||
               ii[2] < 0 || ii[2] > 255) {
                throw rd.err_key_line("front", ggchild->line);
              }
              sf->col[0] = ii[0];
              sf->col[1] = ii[1];
              sf->col[2] = ii[2];
            }
          }
        } else if (std::strcmp(CastCcp(gchild->name) + o, "surface") == 0) {
          ew::Dig3TableauSpace *spc = v[n].space + sp;
          int m = spc->surface_settings.size();
          spc->surface_settings.resize(m + 1);
          ew::Dig3SetSurface *sf = &spc->surface_settings[m];
          rd.read_attribute(&sf->id, "id", gchild);
          for (xmlNode *ggchild = gchild->xmlChildrenNode; ggchild != 0;
           ggchild = ggchild->next) {
            if (ggchild->type != XML_ELEMENT_NODE) {
              continue;
            }
            if (std::strcmp(CastCcp(ggchild->name), "show_main") == 0) {
              int j;
              rd.read_ints(&j, 1, ggchild);
              sf->show_in_main = (j != 0);
            } else if (std::strcmp(CastCcp(ggchild->name), "show_slice") ==
             0) {
              int j;
              rd.read_ints(&j, 1, ggchild);
              sf->show_in_slice = (j != 0);
            } else if (std::strcmp(CastCcp(ggchild->name), "front") == 0) {
              int ii[3];
              rd.read_ints(ii, 3, ggchild);
              if (ii[0] < 0 || ii[0] > 255 ||
               ii[1] < 0 || ii[1] > 255 ||
               ii[2] < 0 || ii[2] > 255) {
                throw rd.err_key_line("front", ggchild->line);
              }
              sf->front_col[0] = ii[0];
              sf->front_col[1] = ii[1];
              sf->front_col[2] = ii[2];
            } else if (std::strcmp(CastCcp(ggchild->name), "back") == 0) {
              int ii[3];
              rd.read_ints(ii, 3, ggchild);
              if (ii[0] < 0 || ii[0] > 255 ||
               ii[1] < 0 || ii[1] > 255 ||
               ii[2] < 0 || ii[2] > 255) {
                throw rd.err_key_line("back", ggchild->line);
              }
              sf->back_col[0] = ii[0];
              sf->back_col[1] = ii[1];
              sf->back_col[2] = ii[2];
            }
          }
        }
      }
    }
    n += 1;
  }
// This does no ew::Dig3Tableau operations.
  outp->swap(v);
}

//XXX ??make this take an array, not a vector

/// This writes a save file.
/// @param file
///   The file name.
/// @param compress
///   If @c true, write the file compressed with gzip compression.
/// @param[in] outp
///   The vector of records to write.
/// @exception std::bad_alloc
/// @exception EW::ErrorIO

void
ew::Dig3Tableau::write_file(const char *file, bool compress,
 const std::vector<ew::Dig3Tableau> *outp)
{
  ew::XmlWriter wr(file, "ew_dig3", "1.0", compress);
  for (int fr = 0; fr < VectorSize(*outp); fr += 1) {
    const ew::Dig3Tableau *sv = &(*outp)[fr];
    wr.start_element("tableau");
    wr.write_string("\n");
    if (sv->slice_clip_ratio != 0.01) {
      wr.start_element("slice_clip_ratio");
      wr.write_double(sv->slice_clip_ratio);
      wr.end_element();
      wr.write_string("\n");
    }
    if (sv->bg[0] != 0 || sv->bg[1] != 0 || sv->bg[2] != 0) {
      wr.start_element("background");
      wr.write_int(sv->bg[0]);
      wr.write_string(" ");
      wr.write_int(sv->bg[1]);
      wr.write_string(" ");
      wr.write_int(sv->bg[2]);
      wr.end_element();
      wr.write_string("\n");
    }
    ew::Transform3 tr0;
    tr0.set_to_identity();
    for (int sp = 0; sp < 2; sp  += 1) {
      const ew::Dig3TableauSpace *spc = &sv->space[sp];
      if (!spc->form_filename.empty()) {
        if (sp == 0) {
          wr.start_element("template_form");
        } else {
          wr.start_element("specimen_form");
        }
        wr.write_attribute_filename(spc->form_filename.c_str(), "file");
        wr.end_element();
        wr.write_string("\n");
      }
      if (spc->main_view != tr0) {
        if (sp == 0) {
          wr.start_element("template_view");
        } else {
          wr.start_element("specimen_view");
        }
        wr.write_transform3(&spc->main_view);
        wr.end_element();
        wr.write_string("\n");
      }
      if (spc->slice_view != tr0) {
        if (sp == 0) {
          wr.start_element("template_slice");
        } else {
          wr.start_element("specimen_slice");
        }
        wr.write_transform3(&spc->slice_view);
        wr.end_element();
        wr.write_string("\n");
      }
      if (!spc->show_slice_in_main) {
        if (sp == 0) {
          wr.start_element("template_show_slice");
        } else {
          wr.start_element("specimen_show_slice");
        }
        wr.write_string("0");
        wr.end_element();
        wr.write_string("\n");
      }
      if (!spc->show_lmks_in_main) {
        if (sp == 0) {
          wr.start_element("template_show_lmks_main");
        } else {
          wr.start_element("specimen_show_lmks_main");
        }
        wr.write_string("0");
        wr.end_element();
        wr.write_string("\n");
      }
      if (!spc->show_lmks_in_slice) {
        if (sp == 0) {
          wr.start_element("template_show_lmks_slice");
        } else {
          wr.start_element("specimen_show_lmks_slice");
        }
        wr.write_string("0");
        wr.end_element();
        wr.write_string("\n");
      }
      if (spc->lmks_symbol != ew::View3Landmarks::SYMBOL_CROSS &&
       spc->lmks_symbol >= 0 &&
       spc->lmks_symbol < ew::View3Landmarks::N_SYMBOLS) {
        if (sp == 0) {
          wr.start_element("template_lmks_symbol");
        } else {
          wr.start_element("specimen_lmks_symbol");
        }
        wr.write_string(ew::View3Landmarks::symbol_names[spc->lmks_symbol]);
        wr.end_element();
        wr.write_string("\n");
      }
      if (spc->lmks_col[0] != 0 || spc->lmks_col[1] != 255 ||
       spc->lmks_col[2] != 0) {
        if (sp == 0) {
          wr.start_element("template_lmks_color");
        } else {
          wr.start_element("specimen_lmks_color");
        }
        wr.write_int(spc->lmks_col[0]);
        wr.write_string(" ");
        wr.write_int(spc->lmks_col[1]);
        wr.write_string(" ");
        wr.write_int(spc->lmks_col[2]);
        wr.end_element();
      }
      for (int i = 0; i < VectorSize(spc->curve_settings); i += 1) {
        const ew::Dig3SetCurve *sf = &spc->curve_settings[i];
        if (sp == 0) {
          wr.start_element("template_curve");
        } else {
          wr.start_element("specimen_curve");
        }
        wr.write_attribute(sf->id.c_str(), "id");
        if (!sf->show_in_main) {
          wr.start_element("show_main");
          wr.write_string("0");
          wr.end_element();
        }
        if (!sf->show_in_slice) {
          wr.start_element("show_slice");
          wr.write_string("0");
          wr.end_element();
        }
        if (sf->col[0] != 128 || sf->col[1] != 128 || sf->col[2] != 128) {
          wr.start_element("color");
          wr.write_int(sf->col[0]);
          wr.write_string(" ");
          wr.write_int(sf->col[1]);
          wr.write_string(" ");
          wr.write_int(sf->col[2]);
          wr.end_element();
        }
        wr.end_element();
        wr.write_string("\n");
      }
      for (int i = 0; i < VectorSize(spc->surface_settings); i += 1) {
        const ew::Dig3SetSurface *sf = &spc->surface_settings[i];
        if (sp == 0) {
          wr.start_element("template_surface");
        } else {
          wr.start_element("specimen_surface");
        }
        wr.write_attribute(sf->id.c_str(), "id");
        if (!sf->show_in_main) {
          wr.start_element("show_main");
          wr.write_string("0");
          wr.end_element();
        }
        if (!sf->show_in_slice) {
          wr.start_element("show_slice");
          wr.write_string("0");
          wr.end_element();
        }
        if (sf->front_col[0] != 128 || sf->front_col[1] != 128 ||
         sf->front_col[2] != 128) {
          wr.start_element("front");
          wr.write_int(sf->front_col[0]);
          wr.write_string(" ");
          wr.write_int(sf->front_col[1]);
          wr.write_string(" ");
          wr.write_int(sf->front_col[2]);
          wr.end_element();
        }
        if (sf->back_col[0] != 128 || sf->back_col[1] != 128 ||
         sf->back_col[2] != 128) {
          wr.start_element("back");
          wr.write_int(sf->back_col[0]);
          wr.write_string(" ");
          wr.write_int(sf->back_col[1]);
          wr.write_string(" ");
          wr.write_int(sf->back_col[2]);
          wr.end_element();
        }
        wr.end_element();
        wr.write_string("\n");
      }
    }
    wr.end_element();
    wr.write_string("\n\n");
  }
  wr.close();
}

bool
ew::Dig3Tableau::operator==(const ew::Dig3Tableau &a) const
{
  return
   space[0] == a.space[0] &&
   space[1] == a.space[1] &&
   slice_clip_ratio == a.slice_clip_ratio &&
   bg[0] == a.bg[0] &&
   bg[1] == a.bg[1] &&
   bg[2] == a.bg[2];
}
