// wdkg 2008-2010

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
#include "ew/Form3.h"
#include "ew/Form3Curve.h"
#include "ew/Form3Embedding.h"
#include "ew/Form3Pointset.h"
#include "ew/Form3Reflection.h"
#include "ew/Form3Surface.h"
#include "ew/Form3Volume.h"
#include "ew/String.h"
#include "XmlReader.h"
#include "XmlWriter.h"

namespace {

  inline const char *
  CastCcp(const xmlChar *p)
  {
    return reinterpret_cast<const char *>(p);
  }

  template<typename T>
  inline bool
  Search(int *position, const char *id, const std::vector<T> &vec)
  {
    int l = 0;
    int h = vec.size();
    int p, r;
    while (h > l) {
      p = (h + l) / 2;
      r = std::strcmp(id, vec[p].id.c_str());
      if (r == 0) {
        *position = p;
        return true;
      } else if (r > 0) {
        l = p + 1;
      } else {
        h = p;
      }
    }
    *position = l;
    return false;
  }

  template<typename T>
  inline const T*
  VectorData(const std::vector<T> &v)
  {
    if (v.empty()) {
      return 0;
    } else {
      return &(v[0]);
    }
  }

  template<typename T>
  inline T*
  VectorData(std::vector<T> &v)
  {
    if (v.empty()) {
      return 0;
    } else {
      return &(v[0]);
    }
  }

  template<typename T>
  inline int
  VectorSize(const std::vector<T> &v)
  {
    return static_cast<int>(v.size());
  }

  template<typename T>
  inline void
  VectorErase(std::vector<T> &v, int n)
  {
    v.erase(v.begin() + n);
  }

  template<typename T>
  inline void
  VectorInsert(std::vector<T> &v, int n, const T &val)
  {
    v.insert(v.begin() + n, val);
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

const char *const ew::Form3::type_names[] = {
  "landmark",
  "semi-landmark",
  "point",
  "line",
  "plane",
  "frame",
  0
};

const char *const ew::Form3::state_names[] = {
  "set",
  "unset",
  "warped",
  "provisional",
  "projected",
  "optimized",
  0
};

/// This resets the form to its initial
/// state.

void
ew::Form3::reset()
{
  VectorReset(volumes);
  VectorReset(surfaces);
  VectorReset(curves);
  VectorReset(pointsets);
  VectorReset(embeddings);
  VectorReset(reflections);
}

/// This reads a form from a file.
/// The file must be a real filename, URL's are not supported.
/// The file can be compressed with gzip compression.
/// Relative filenames with initial path component "." inside the file are
/// converted to canonical filenames using the directory part of @a file.
/// If an exception is thrown, the form is unchanged.
/// @param file
///   The canonical name of the file to read.
/// @exception std::bad_alloc
/// @exception ew::ErrorIO

void
ew::Form3::read_file(const char *file)
{
  ew::Form3 f;
  ew::XmlReader rd(file, "ew_form3", "1.0");
  int nv = 0, ns = 0, nc = 0, np = 0, nps = 0, ne = 0, nr = 0;
  for (xmlNode *child = rd.get_children(); child != 0; child = child->next) {
    if (child->type == XML_ELEMENT_NODE) {
      if (std::strcmp(CastCcp(child->name), "volume") == 0) {
        nv += 1;
      } else if (std::strcmp(CastCcp(child->name), "surface") == 0) {
        ns += 1;
      } else if (std::strcmp(CastCcp(child->name), "curve") == 0) {
        nc += 1;
      } else if (std::strcmp(CastCcp(child->name), "pointset") == 0) {
        nps += 1;
      } else if (std::strcmp(CastCcp(child->name), "embedding") == 0) {
        ne += 1;
      } else if (std::strcmp(CastCcp(child->name), "reflection") == 0) {
        nr += 1;
      }
    }
  }
  f.volumes.resize(nv);
  f.surfaces.resize(ns);
  f.curves.resize(nc);
  f.pointsets.resize(nps);
  f.embeddings.resize(ne);
  f.reflections.resize(nr);
  nv = ns = nc = np = nps = ne = nr = 0;
  for (xmlNode *child = rd.get_children(); child != 0; child = child->next) {
    if (child->type == XML_ELEMENT_NODE) {
      if (std::strcmp(CastCcp(child->name), "volume") == 0) {
        rd.read_attribute(&f.volumes[nv].id, "id", child);
        rd.read_attribute_filename(&f.volumes[nv].file, "file", child);
        nv += 1;
      } else if (std::strcmp(CastCcp(child->name), "surface") == 0) {
        rd.read_attribute(&f.surfaces[ns].id, "id", child);
        rd.read_attribute_filename(&f.surfaces[ns].file, "file",
         child);
        ns += 1;
      } else if (std::strcmp(CastCcp(child->name), "curve") == 0) {
        rd.read_attribute(&f.curves[nc].id, "id", child);
        rd.read_attribute_filename(&f.curves[nc].file, "file", child);
        rd.read_attribute_list(&f.curves[nc].state, "state", state_names,
         child, 0);
        nc += 1;
      } else if (std::strcmp(CastCcp(child->name), "pointset") == 0) {
        rd.read_attribute(&f.pointsets[nps].id, "id", child);
        rd.read_attribute_list(&f.pointsets[nps].state, "state", state_names,
         child, 0);
        rd.read_attribute_list(&f.pointsets[nps].type, "type", type_names,
         child, 0);
        int n = 0;
        rd.read_attribute_n(&n, "n", child, 1);
        if (n < 0) {
          throw rd.err_key_line("n", child->line);
        }
        f.pointsets[nps].n = n;
        xmlNode *grandchild = child->xmlChildrenNode;
        while (grandchild != 0) {
          if (grandchild->type == XML_ELEMENT_NODE) {
            if (std::strcmp(CastCcp(grandchild->name), "locations") == 0) {
              f.pointsets[nps].locations.resize(3 * n);
              rd.read_doubles(VectorData(f.pointsets[nps].locations),
               3 * n, grandchild);
            } else if (std::strcmp(CastCcp(grandchild->name),
             "relax_dims") == 0) {
              f.pointsets[nps].relax_dims.resize(n);
              rd.read_ints(VectorData(f.pointsets[nps].relax_dims), n,
               grandchild);
            } else if (std::strcmp(CastCcp(grandchild->name),
             "relax_params") == 0) {
              f.pointsets[nps].relax_params.resize(3 * n);
              rd.read_doubles(VectorData(f.pointsets[nps].relax_params),
               3 * n, grandchild);
            } else if (std::strcmp(CastCcp(grandchild->name),
             "orientations") == 0) {
              f.pointsets[nps].orientations.resize(9 * n);
              rd.read_doubles(VectorData(f.pointsets[nps].orientations),
               9 * n, grandchild);
            } else if (std::strcmp(CastCcp(grandchild->name), "sizes") == 0) {
              f.pointsets[nps].sizes.resize(n);
              rd.read_doubles(VectorData(f.pointsets[nps].sizes), n,
               grandchild);
            }
          }
          grandchild = grandchild->next;
        }
        if (f.pointsets[nps].relax_params.size() == 0) {
          for (int j = 0; j < VectorSize(f.pointsets[nps].relax_dims);
           j += 1) {
            if (f.pointsets[nps].relax_dims[j] == 1 ||
             f.pointsets[nps].relax_dims[j] == 2) {
              throw rd.err_desc_line("missing relax_params field",
               child->line);
            }
          }
        }
        nps += 1;
      } else if (std::strcmp(CastCcp(child->name), "embedding") == 0) {
        rd.read_attribute(&f.embeddings[ne].subset_id, "subset", child);
        rd.read_attribute(&f.embeddings[ne].superset_id, "superset",
         child);
        if (f.embeddings[ne].subset_id.size() == 0 ||
         f.embeddings[ne].superset_id.size() == 0) {
          throw rd.err_key_line("subset or superset", child->line);
        }
        ne += 1;
      } else if (std::strcmp(CastCcp(child->name), "reflection") == 0) {
        rd.read_attribute(&f.reflections[nr].left_id, "left", child);
        rd.read_attribute(&f.reflections[nr].right_id, "right", child);
        if (f.reflections[nr].left_id.size() == 0 ||
         f.reflections[nr].right_id.size() ==0) {
          throw rd.err_key_line("reflection", child->line);
        }
        nr += 1;
      }
    }
  }
// Sort entries and check id's not empty strings.
// Optimize for this being so in the file.
  swap(f);
}

/// This writes a form to a file.
/// Filenames written into the output are converted to relative filenames with
/// initial path component "." if their path contains the directory part of @a
/// file as an initial segment.
/// @param file
///   The canonical name of the file to write.
/// @param compress
///   If @c true, write the file compressed with gzip compression.
/// @exception std::bad_alloc
/// @exception ew::ErrorIO

void
ew::Form3::write_file(const char *file, bool compress) const
{
  ew::XmlWriter wr(file, "ew_form3", "1.0", compress);
  for (int i = 0; i < VectorSize(volumes); i += 1) {
    wr.start_element("volume");
    wr.write_attribute(volumes[i].id.c_str(), "id");
    wr.write_attribute_filename(volumes[i].file.c_str(), "file");
    wr.end_element();
    if (i + 1 == VectorSize(volumes)) {
      wr.write_string("\n\n");
    } else {
      wr.write_string("\n");
    }
  }
  for (int i = 0; i < VectorSize(surfaces); i += 1) {
    wr.start_element("surface");
    wr.write_attribute(surfaces[i].id.c_str(), "id");
    wr.write_attribute_filename(surfaces[i].file.c_str(), "file");
    wr.end_element();
    if (i + 1 == VectorSize(surfaces)) {
      wr.write_string("\n\n");
    } else {
      wr.write_string("\n");
    }
  }
  for (int i = 0; i < VectorSize(curves); i += 1) {
    wr.start_element("curve");
    wr.write_attribute(curves[i].id.c_str(), "id");
    wr.write_attribute_filename(curves[i].file.c_str(), "file");
    wr.write_attribute_list(curves[i].state, state_names, N_STATES, "state",
     0);
    wr.end_element();
    if (i + 1 == VectorSize(curves)) {
      wr.write_string("\n\n");
    } else {
      wr.write_string("\n");
    }
  }
  for (int i = 0; i < VectorSize(pointsets); i += 1) {
    wr.start_element("pointset");
    wr.write_attribute(pointsets[i].id.c_str(), "id");
    wr.write_attribute_list(pointsets[i].type, type_names, N_TYPES, "type",
     0);
    wr.write_attribute_list(pointsets[i].state, state_names, N_STATES,
     "state", 0);
    wr.write_attribute_n(pointsets[i].n, "n", 1);
    wr.write_string("\n");
    int stride = 0;
    if (pointsets[i].n > 1) {
      stride = 1;
    }
    if (pointsets[i].locations.size() > 0) {
      wr.start_element("locations");
      wr.write_doubles(VectorData(pointsets[i].locations),
       pointsets[i].locations.size(), stride * 3);
      wr.end_element();
      wr.write_string("\n");
    }
    bool dodims = false;
    if (pointsets[i].relax_dims.size() > 0) {
      for (int j = 0; j < VectorSize(pointsets[i].relax_dims);
       j += 1) {
        if (pointsets[i].relax_dims[j] != 0) {
          dodims = true;
          break;
        }
      }
    }
    if (dodims) {
      wr.start_element("relax_dims");
      wr.write_ints(VectorData(pointsets[i].relax_dims),
       pointsets[i].relax_dims.size(), stride);
      wr.end_element();
      wr.write_string("\n");
    }
    bool doparams = false;
    if (dodims && pointsets[i].relax_params.size() > 0) {
      for (int j = 0; j < VectorSize(pointsets[i].relax_dims);
       j += 1) {
        if (pointsets[i].relax_dims[j] != 0 &&
         pointsets[i].relax_dims[j] != 3) {
          doparams = true;
          break;
        }
      }
    }
    if (doparams) {
      wr.start_element("relax_params");
      wr.write_doubles(VectorData(pointsets[i].relax_params),
       pointsets[i].relax_params.size(), stride * 3);
      wr.end_element();
      wr.write_string("\n");
    }
    if (pointsets[i].orientations.size() > 0) {
      wr.start_element("orientations");
      wr.write_doubles(VectorData(pointsets[i].orientations),
       pointsets[i].orientations.size(), stride * 9);
      wr.end_element();
      wr.write_string("\n");
    }
    if (pointsets[i].sizes.size() > 0) {
      wr.start_element("sizes");
      wr.write_doubles(VectorData(pointsets[i].sizes),
       pointsets[i].sizes.size(), stride);
      wr.end_element();
      wr.write_string("\n");
    }
    wr.end_element();
    wr.write_string("\n\n");
  }
  for (int i = 0; i < VectorSize(embeddings); i += 1) {
    wr.start_element("embedding");
    wr.write_attribute(embeddings[i].subset_id.c_str(), "subset");
    wr.write_attribute(embeddings[i].superset_id.c_str(), "superset");
    wr.end_element();
    if (i + 1 == VectorSize(embeddings)) {
      wr.write_string("\n\n");
    } else {
      wr.write_string("\n");
    }
  }
  for (int i = 0; i < VectorSize(reflections); i += 1) {
    wr.start_element("reflection");
    wr.write_attribute(reflections[i].left_id.c_str(), "left");
    wr.write_attribute(reflections[i].right_id.c_str(), "right");
    wr.end_element();
    if (i + 1 == VectorSize(reflections)) {
      wr.write_string("\n\n");
    } else {
      wr.write_string("\n");
    }
  }
  wr.close();
}

/// This searches the volumes in the form for an id.
/// @param[out] position
///   The index in the vector that a volume with this id would be if
///   inserted into the vector of volumes.
/// @param id
///   The id to search for.
/// @return
///   @c true if the id is an existing volume id.

bool
ew::Form3::search_volume(int *position, const char *id) const
{
  return Search(position, id, volumes);
}

/// This searches the surfaces in the form for an id.
/// @param[out] position
///   The index in the vector that a surface with this id would be if
///   inserted into the vector of surfaces.
/// @param id
///   The id to search for.
/// @return
///   @c true if the id is an existing surface id.

bool
ew::Form3::search_surface(int *position, const char *id) const
{
  return Search(position, id, surfaces);
}

/// This searches the curves in the form for an id.
/// @param[out] position
///   The index in the vector that a curve with this id would be if
///   inserted into the vector of curves.
/// @param id
///   The id to search for.
/// @return
///   @c true if the id is an existing curve id.

bool
ew::Form3::search_curve(int *position, const char *id) const
{
  return Search(position, id, curves);
}

/// This searches the point sets in the form for an id.
/// @param[out] position
///   The index in the vector that a point set with this id would be if
///   inserted into the vector of point sets.
/// @param id
///   The id to search for.
/// @return
///   @c true if the id is an existing point set id.

bool
ew::Form3::search_pointset(int *position, const char *id) const
{
  return Search(position, id, pointsets);
}

/// This searches the embeddings in the form for the id's.
/// @param[out] position
///   The index in the vector that a embedding with these id's would be if
///   inserted into the vector of embeddings.
/// @param id1
///   The subset_id to search for.
/// @param id2
///   The superset_id to search for.
/// @return
///   @c true if the id's belong to an existing embedding.

bool
ew::Form3::search_embedding(int *position, const char *id1,
 const char *id2) const
{
  int l = 0;
  int h = embeddings.size();
  int p, r;
  while (h > l) {
    p = (h + l) / 2;
    r = std::strcmp(id1, embeddings[p].subset_id.c_str());
    if (r == 0) {
      r = std::strcmp(id2, embeddings[p].superset_id.c_str());
    }
    if (r == 0) {
      *position = p;
      return true;
    } else if (r > 0) {
      l = p + 1;
    } else {
      h = p;
    }
  }
  *position = l;
  return false;
}

/// This searches the reflections in the form for the id's.
/// @param[out] position
///   The index in the vector that a reflection with these id's would be if
///   inserted into the vector of reflections.
/// @param id1
///   The left_id to search for.
/// @param id2
///   The right_id to search for.
/// @return
///   @c true if the id's belong to an existing reflection.

bool
ew::Form3::search_reflection(int *position, const char *id1,
 const char *id2) const
{
  int l = 0;
  int h = reflections.size();
  int p, r;
  while (h > l) {
    p = (h + l) / 2;
    r = std::strcmp(id1, reflections[p].left_id.c_str());
    if (r == 0) {
      r = std::strcmp(id2, reflections[p].right_id.c_str());
    }
    if (r == 0) {
      *position = p;
      return true;
    } else if (r > 0) {
      l = p + 1;
    } else {
      h = p;
    }
  }
  *position = l;
  return false;
}

/// This searches the embedding relations to see what, if any, element is a
/// unique superset of a given element.
/// @param[in] id
///   The id of the original element.
/// @return
///   The id of the unique superset of the original element, or 0.
///   This pointer is valid until the form is changed.

const char *
ew::Form3::search_superset(const char *id) const
{
  const char *found = 0;
  for (int k = 0; k < static_cast<int>(embeddings.size()); k += 1) {
    if (std::strcmp(embeddings[k].subset_id.c_str(), id) == 0) {
      if (found != 0) {
        return 0;
      }
      found = embeddings[k].superset_id.c_str();
    }
  }
  return found;
}

/// This makes one element the unique superset of another element.
/// @param subset_id
///   The id of the element that should have a unique superset.
/// @param superset_id
///   The id of the element that be the unique superset.

void
ew::Form3::set_superset(const char *subset_id, const char* superset_id)
{
  for (int k = static_cast<int>(embeddings.size()) - 1; k >= 0; k -= 1) {
    if (std::strcmp(embeddings[k].subset_id.c_str(), subset_id) == 0) {
      VectorErase(embeddings, k);
    }
  }
  int index;
  search_embedding(&index, subset_id, superset_id);
  ew::Form3Embedding emb;
  emb.subset_id = subset_id;
  emb.superset_id = superset_id;
  VectorInsert(embeddings, index, emb);
}

/// Compares this form with another, member by member.
/// @param a the other form

bool
ew::Form3::operator==(const ew::Form3 &a) const
{
  return
   volumes == a.volumes &&
   surfaces == a.surfaces &&
   curves == a.curves &&
   pointsets == a.pointsets &&
   embeddings == a.embeddings &&
   reflections == a.reflections;
}
