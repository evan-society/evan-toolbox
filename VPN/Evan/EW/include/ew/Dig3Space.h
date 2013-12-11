#ifndef EW_DIG3SPACE_H_INCLUDED
#define EW_DIG3SPACE_H_INCLUDED

// wdkg 2008-2010

//XXX this has not really much to do with dig3, could be called something else
//XXX for now, forms must be empty or have a filename (last load and save,
// not reset, but changes ok) before save records can be recorded

#include <string>
#include <vector>
#include "ew/Bbox3.h"
#include "ew/DataflowCurve3E.h"
#include "ew/DataflowForm3.h"
#include "ew/DataflowNetwork.h"
#include "ew/DataflowSurface3E.h"
#include "ew/DebugHelper.h"
#include "ew/Form3.h"
#include "ew/Form3Embedding.h"
#include "ew/Form3Pointset.h"

// ew::Dig3 has no concept of filename, but ew::Tableau must have filenames
// for the forms3's.
// This does not keep track of form3 filenames.
// When creating a save record, these must be filled in.
// When loading a save record, the form3 files must be read if necessary and
// loaded first.

//XXX review this
// ew::Dig3 accesses the constructor and destructor.

namespace ew {

// XXX forward declaration w/o include
  class Dig3;
// XXX forward declaration w/o include
  class Dig3View;

  class Dig3Space {
    friend class Dig3;
  public:
    inline const ew::DataflowForm3 *get_form_node();
    void reset_form();
    void set_form_data(const ew::Form3 *form_data);
    inline const ew::Form3 *get_form_data() const;
//XXX version cnt, or dirty flag
//XXX ??? filename of form, ?if not saved, ?dirty
    int set_form_curve(bool *replaced, const ew::Form3Curve *cr);
    void remove_form_curve(int n);
    int set_form_surface(bool *replaced, const ew::Form3Surface *sr);
    void remove_form_surface(int n);
    inline int set_form_pointset(bool *replaced, const ew::Form3PointSet *ps);
    inline void set_form_pointset_location(int n, int i, const double *loc);
    inline void set_form_pointset_relax(int n, int i, int rdim,
     const double *rparam);
    inline void remove_form_pointset(int n);
    inline int set_form_embedding(bool *replaced,
     const ew::Form3Embedding *em);
    inline void set_form_superset(const char *subset_id,
     const char *superset_id);
    inline void remove_form_embedding(int n);
    int get_curve_of_pointset(int ps) const;
    int get_surface_of_pointset(int ps) const;
    void get_bbox(ew::Bbox3 *b) const;
    inline int get_n_curve_nodes() const;
    inline const ew::DataflowCurve3E *const *get_curve_nodes() const;
    inline int get_n_surface_nodes() const;
    inline const ew::DataflowSurface3E *const *get_surface_nodes() const;
    bool project(int *rdim, double *rparam, double *proj, const double *p,
     const char *id) const;
    ew::DataflowNetwork *const network;
    ew::Dig3 *const dig3;
    const int index;
    const ew::DebugHelper dbg;
  private:
    Dig3Space(ew::Dig3 *dig3, int ind);
    ~Dig3Space();
    ew::DataflowForm3 *form_node;
    std::vector<const ew::DataflowCurve3E *> curve_nodes;
    std::vector<const ew::DataflowSurface3E *> surface_nodes;
// undefined
    Dig3Space(const ew::Dig3Space &that);
    Dig3Space &operator=(const ew::Dig3Space &that);
  };

}

/// @class ew::Dig3Space
/// @headerfile ew/Dig3Space.h "ew/Dig3Space.h"
/// @brief Space Element
///
/// ew::Dig3Space implements a space element of ew::Dig3.
/// There are 2 spaces, the template space and the specimen space.

/// @var ew::DataflowNetwork *const ew::Dig3Space::network
/// This points to the ew::DataflowNetwork that the #dig3 was created with.

/// @var ew::Dig3 *const ew::Dig3Space::dig3
/// This points to the ew::Dig3 that this ew::Dig3Space is contained in.

/// @var const int ew::Dig3Space::index
/// This is 0 if this is the template space of #dig3, 1 if the specimen space.

/// @return
///   A pointer to the ew::From3 data of the space.

inline const ew::Form3 *
ew::Dig3Space::get_form_data() const
{
  return form_node->get_data();
}

inline int
ew::Dig3Space::get_n_curve_nodes() const
{
  return static_cast<int>(curve_nodes.size());
}

/// @return
///   The curve node index.
///   The i'th entry is a pointer to the curve node corresponding to the i'th
///   curve, or zero if this curve has no data.

inline const ew::DataflowCurve3E *const *
ew::Dig3Space::get_curve_nodes() const
{
  if (curve_nodes.empty()) {
    return 0;
  } else {
    return &curve_nodes[0];
  }
}

inline int
ew::Dig3Space::get_n_surface_nodes() const
{
  return static_cast<int>(surface_nodes.size());
}

/// @return
///   The surface node index.
///   The i'th entry is a pointer to the surface node corresponding to the i'th
///   surface, or zero if this surface has no data.

inline const ew::DataflowSurface3E *const *
ew::Dig3Space::get_surface_nodes() const
{
  if (surface_nodes.empty()) {
    return 0;
  } else {
    return &surface_nodes[0];
  }
}

/// This returns a pointer to the ew::DataflowForm3 node managed by the
/// ew::Dig3Space.
/// This node can be changed using ew::Dig3Space wrappers for the
/// ew::DataflowForm3 methods.

inline const ew::DataflowForm3 *
ew::Dig3Space::get_form_node()
{
  return form_node;
}

/// This adds or replaces a pointset in the space.
/// @param[out] replaced
///   @c true if the point set replaced an existing point set.
/// @param ps
///   A pointer to the point set data to copy.
/// @return
///   The index of the point set.

inline int
ew::Dig3Space::set_form_pointset(bool *replaced, const ew::Form3PointSet *ps)
{
  return form_node->set_pointset(replaced, ps);
}

/// This changes the coordinates of an element of the point set.
/// @param n
///   The index of the point set.
/// @param i
///   The index of the element in the point set.
/// @param[in] loc
///   A pointer to the new coordinates of this entry.

inline void
ew::Dig3Space::set_form_pointset_location(int n, int i, const double *loc)
{
  form_node->set_pointset_location(n, i, loc);
}

/// This changes the relaxation parameters of an element of the point set.
/// @param n
///   The index of the point set.
/// @param i
///   The index of the element in the point set.
/// @param[in] rdim
///   The new relax_dims of this entry.
/// @param[in] rparam
///   A pointer to the new relax_params of this entry.

inline void
ew::Dig3Space::set_form_pointset_relax(int n, int i, int rdim,
 const double *rparam)
{
  form_node->set_pointset_relax(n, i, rdim, rparam);
}

/// This deletes a point set from the space.
/// @param n
///   The index of the point set to delete.

inline void
ew::Dig3Space::remove_form_pointset(int n)
{
  form_node->remove_pointset(n);
}

/// This adds or replaces a embedding in the space.
/// @param[out] replaced
///   @c true if the embedding replaced an existing embedding.
/// @param em
///   A pointer to the embedding data to copy.
/// @return
///   The index of the embedding.

inline int
ew::Dig3Space::set_form_embedding(bool *replaced,
 const ew::Form3Embedding *em)
{
  return form_node->set_embedding(replaced, em);
}

/// This makes one element of the form the unique superset of another element.
/// @param subset_id
///   The id of the element that should have a unique superset.
/// @param superset_id
///   The id of the element that be the unique superset.

inline void
ew::Dig3Space::set_form_superset(const char *subset_id,
 const char *superset_id)
{
  form_node->set_superset(subset_id, superset_id);
}

/// This deletes a embedding from the space.
/// @param n
///   The index of the embedding to delete.

inline void
ew::Dig3Space::remove_form_embedding(int n)
{
  form_node->remove_embedding(n);
}

#endif
