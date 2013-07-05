#ifndef EW_INDEX3_H_INCLUDED
#define EW_INDEX3_H_INCLUDED

// wdkg 2009-2010

// Polytope Index (supports nv = 2 or 3)

//XXX use uint32_t

// Use a simple octree rather than a more sophisticated index such as a kd-tree
// for fast creation speed, to avoid the need for caching the index.

// Without care, the polytope list could get pathologically large.
// Bound size of index, to limit memory usage.
// Bound depth so to simplify lookups, and so recursion is safe.
// Worst case behaviour is a single node containing all polytopes (i.e. no
// index).

// A node is an unsigned int encoded as follows:
//   4 bits: type
//   28 bits: data
//     BRANCH_LH      fork, offset of branch nodes, which are consecutive
//     BRANCH_L       fork with high branch empty, offset of low branch node
//     BRANCH_H       fork with low branch empty, offset of high branch node
//     POLYTOPE       single polytope, index (not an offset)
//     POLYTOPE_2     offset of 2 polytope indices
//     ...
//     POLYTOPE_12    offset of 12 polytope indices
//     POLYTOPE_N     offset of length followed by polytope indices
// The cut direction and value corresponding to a node is defined by the depth
// and location of the node.
// Separate polytope and node lists are used because it simplifies index
// construction: as the index is refined, nodes are modified in place.

#include <cstring>
#include <vector>
#include "ew/Bbox3.h"
#include "AutoArray.h"

namespace ew {

  class Index3 {
  public:
    enum {
      BRANCH_LH,
      BRANCH_L,
      BRANCH_H,
      POLYTOPE,
      POLYTOPE_2, POLYTOPE_3, POLYTOPE_4, POLYTOPE_5, POLYTOPE_6, POLYTOPE_7,
      POLYTOPE_8, POLYTOPE_9, POLYTOPE_10, POLYTOPE_11, POLYTOPE_12,
      POLYTOPE_N
    };
    void make_index(const float *in_points, int i_in_npoints,
     const int *i_in_polytopes, int i_in_nv, int i_in_npolytopes);
    void reset();
    double nearest_on_polytopes(int *polytope, double *proj, double *coeffs,
     const double *p);
    inline bool touch_polytope(int f);
    inline void untouch_polytopes();
    const float *in_points;
    int in_npoints;
    const int *in_polytopes;
    int in_nv;
    int in_npolytopes;
    ew::Bbox3 b_cube;
    double side;
    std::vector<unsigned int> nodes;
    std::vector<unsigned int> polytope_lists;
    std::vector<unsigned int> polytopes_checked_mask;
    int mask_bytes;
    ew::AutoArray<int> mask_touches;
    int n_masks_touched;
    int max_masks_touched;
    int n_polytopes_tried;
  };

}

// polytopes_checked_ma is sized in_n_polytopes / 32, and is kept zero between
// calls to nearest_on_polytopes.

// make_index must be called on a non-empty curve or surface, with more than 1
// distinct point.

// nearest_on_polytopes must be called after make_index.

// touch_polytope returns true if the polytope was previously untouched (i.e.
// the polytope needs checking).

// n_polytopes_tried is just for tuning.

// The following is to avoid initializing polytopes_checked_mask (a medium size
// array) each search.

inline bool
ew::Index3::touch_polytope(int f)
{
  int f1 = (f >> 5);
  int f2 = (f & 31);
  unsigned int fm = (1 << f2);
  unsigned int m = polytopes_checked_mask[f1];
  if (m == 0) {
    polytopes_checked_mask[f1] = fm;
    if (n_masks_touched < max_masks_touched) {
      mask_touches[n_masks_touched] = f1;
      n_masks_touched += 1;
    }
    return true;
  } else {
    if (m & fm) {
      return false;
    } else {
      polytopes_checked_mask[f1] |= fm;
      return true;
    }
  }
}

inline void
ew::Index3::untouch_polytopes()
{
  if (n_masks_touched >= max_masks_touched) {
    std::memset(&polytopes_checked_mask[0], 0, mask_bytes);
  } else {
    for (int i = 0; i < n_masks_touched; i += 1) {
      polytopes_checked_mask[mask_touches[i]] = 0;
    }
  }
  n_masks_touched = 0;
}

#endif
