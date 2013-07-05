// wdkg 2009-2010
//XXX make struct s separate classes with separate sources

#include <limits>
#include <vector>
#include "ew/Bbox3.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "Geom3.h"
#include "Index3.h"
#include "Index3ToCheck.h"
#include "Index3ToIndex.h"

namespace ew {
  const char ClassName[] = "ew::Index3";
//XXX make dbg statewide, ? use debug_helper, make checks a dbg_check
#ifdef XXX
  bool XXXyes[500000];
  int XXXdir[500000];
  int XXXsubd1[500000];
  int XXXsubd2[500000];
  int XXXsubd3[500000];
  int XXXoff1[500000];
  int XXXoff2[500000];
  int XXXoff3[500000];
  void XXXchk(int l, int n, int dir, int *sub, int *off)
  {
    if (XXXdir[n] != dir ||
     XXXsubd1[n] != sub[0] ||
     XXXsubd2[n] != sub[1] ||
     XXXsubd3[n] != sub[2] ||
     XXXoff1[n] != off[0] ||
     XXXoff2[n] != off[1] ||
     XXXoff3[n] != off[2]) {
      printf("!!!XXX %d exp %d %d %d %d %d %d %d\n",l,
       XXXdir[n],
       XXXsubd1[n],
       XXXsubd2[n],
       XXXsubd3[n],
       XXXoff1[n],
       XXXoff2[n],
       XXXoff3[n]);
      printf("!!!XXX     got %d %d %d %d %d %d %d\n",
       dir,
       sub[0],
       sub[1],
       sub[2],
       off[0],
       off[1],
       off[2]);
    }
  }
#endif
// This must be at most about 32 * 3, otherwise the subdivides and offsets will
// overflow a uint32_t.
  const int MAX_DEPTH = 30;

  template<typename T>
  inline int
  VectorSize(const std::vector<T> &v)
  {
    return static_cast<int>(v.size());
  }

// This shrinks the capacity of a vector to its size.
  template<typename T>
  inline void
  VectorShrink(std::vector<T> &v)
  {
    std::vector<T> v1;
    v1.reserve(v.size());
    for (int i = 0; i < VectorSize(v); i += 1) {
      v1.push_back(v[i]);
    }
    v.swap(v1);
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

// Don't construct index on creation, so can contain in a class that doesn't
// always have a valid curve or surface.

//XXX check for overflow
//XXX check behaviour if, e.g.,  all points same
void
ew::Index3::make_index(const float *i_in_points, int i_in_npoints,
 const int *i_in_polytopes, int i_in_nv, int i_in_npolytopes)
{
  const ew::Debug &dbg = ew::Debug::instance();
  bool dbg_on = dbg.get_active(ClassName);
  dbg_on && dbg.dprintf("%s::make_index(*,%d,*,%d,%d) {", ClassName,
   i_in_npoints, i_in_nv, i_in_npolytopes);
  in_points = i_in_points;
  in_npoints = i_in_npoints;
  in_polytopes = i_in_polytopes;
  in_nv = i_in_nv;
  in_npolytopes = i_in_npolytopes;
  ew::Bbox3 bcube0;
//XXX use as arg
  ew::Bbox3 brect0 = ew::Bbox3::empty_box;
  for (int i = 0; i < in_npoints * 3; i += 3) {
    brect0.add(in_points + i);
  }
  double side0 = 0.0;
  for (int i = 0; i < 3; i += 1) {
    double sz = brect0.max[i] - brect0.min[i];
    if (sz > side0) {
      side0 = sz;
    }
  }
  for (int i = 0; i < 3; i += 1) {
    double c = (brect0.min[i] + brect0.max[i]) / 2.0;
    bcube0.min[i] = c - side0 / 2.0;
    bcube0.max[i] = c + side0 / 2.0;
  }
  if (dbg_on) {
    int nsides = 0;
    double sum = 0;
    for (int i = 0; i < in_npolytopes * in_nv; i += in_nv) {
      double v[9];
      for (int j = 0; j < in_nv; j += 1) {
        for (int k = 0; k < 3; k += 1) {
          v[j * 3 + k] = in_points[in_polytopes[i + j] * 3 + k];
        }
      }
      double s[3];
      ew::Geom3::subtract(s, v, v + 3);
      sum += ew::Geom3::norm(s);
      nsides += 1;
      if (in_nv == 3) {
        ew::Geom3::subtract(s, v, v + 6);
        sum += ew::Geom3::norm(s);
        nsides += 1;
        ew::Geom3::subtract(s, v + 3, v + 6);
        sum += ew::Geom3::norm(s);
        nsides += 1;
      }
    }
    dbg.dprintf("%s::make_index   in_npoints=%d", ClassName, in_npoints);
    dbg.dprintf("%s::make_index   in_npolytopes=%d", ClassName,
     in_npolytopes);
    dbg.dprintf("%s::make_index   side=%g", ClassName, side0);
    dbg.dprintf("%s::make_index   megs=%.2f", ClassName,
     (in_npoints * 3 * sizeof(float) +
     in_npolytopes * in_nv * sizeof(int)) / 1000000.0);
    dbg.dprintf("%s::make_index   avg_edge=%g", ClassName, sum / nsides);
  }
  std::vector<unsigned int> nodes0;
  std::vector<unsigned int> plsts0;
  nodes0.resize(1);
  std::vector<ew::Index3ToIndex> pending;
  pending.resize(1);
  pending[0].node = 0;
  pending[0].polytopes.resize(in_npolytopes);
  for (int i = 0; i < in_npolytopes; i += 1) {
    pending[0].polytopes[i] = i;
  }
  pending[0].offsets[0] = 0;
  pending[0].offsets[1] = 0;
  pending[0].offsets[2] = 0;
  int depth = 0;
  int dir = 0;
  int subdivide[3];
  subdivide[0] = subdivide[1] = subdivide[2] = 1;
//XXX tune
  int max_size = (in_npoints * 3 + in_npolytopes * in_nv) / 2;
  int polytopes_stop = 12;
  int cnt_fa = 0;
  int cnt_br2 = 0;
  int cnt_br1 = 0;
  while (pending.size() > 0) {
    std::vector<ew::Index3ToIndex> new_pending;
    int size = nodes0.size() + plsts0.size();
    for (int i = 0; i < VectorSize(pending); i += 1) {
      size += pending[i].polytopes.size() + 1;
    }
    int stop = (depth >= MAX_DEPTH || size >= max_size);
    for (int i = 0; i < VectorSize(pending); i += 1) {
#ifdef XXX
      bool XXXhasit=false;
      for (int j = 0; j < VectorSize(pending[i].polytopes); j += 1) {
        if(pending[i].polytopes[j]==231611) {
          XXXhasit=true;
        }
      }
      XXXyes[pending[i].node] = XXXhasit;
      XXXdir[pending[i].node] = dir;
      XXXsubd1[pending[i].node] = subdivide[0];
      XXXsubd2[pending[i].node] = subdivide[1];
      XXXsubd3[pending[i].node] = subdivide[2];
      XXXoff1[pending[i].node] = pending[i].offsets[0];
      XXXoff2[pending[i].node] = pending[i].offsets[1];
      XXXoff3[pending[i].node] = pending[i].offsets[2];
#endif
      if (VectorSize(pending[i].polytopes) <= polytopes_stop ||
       (pending[i].polytopes.size() <= 12 && stop)) {
        if (pending[i].polytopes.size() == 1) {
          nodes0[pending[i].node] =
           ((POLYTOPE << 28) | pending[i].polytopes[0]);
        } else {
          nodes0[pending[i].node] =
           (((POLYTOPE_2 + pending[i].polytopes.size() - 2) << 28) |
           plsts0.size());
          for (int j = 0; j < VectorSize(pending[i].polytopes); j += 1) {
            plsts0.push_back(pending[i].polytopes[j]);
          }
        }
        cnt_fa += 1;
      } else if (stop) {
        nodes0[pending[i].node] = ((POLYTOPE_N << 28) | plsts0.size());
        plsts0.push_back(pending[i].polytopes.size());
        for (int j = 0; j < VectorSize(pending[i].polytopes); j += 1) {
          plsts0.push_back(pending[i].polytopes[j]);
        }
        cnt_fa += 1;
      } else {
        std::vector<int> lowf;
        std::vector<int> highf;
        double cverts[85 * 3];
        double fverts[9];
        ew::Bbox3 box;
        for (int j = 0; j < VectorSize(pending[i].polytopes); j += 1) {
          for (int k = 0; k < in_nv; k += 1) {
            for (int l = 0; l < 3; l += 1) {
              fverts[k * 3 + l] =
               in_points[in_polytopes[pending[i].polytopes[j] * in_nv + k] *
               3 + l];
            }
          }
          for (int k = 0; k < 3; k += 1) {
            box.min[k] = bcube0.min[k] +
             pending[i].offsets[k] * side0 / subdivide[k];
            box.max[k] = bcube0.min[k] +
             (pending[i].offsets[k] + 1) * side0 / subdivide[k];
          }
          int n = ew::Geom3::crop_polytope(cverts, fverts, in_nv, &box);
          bool low = false;
          bool high = false;
          double div = bcube0.min[dir] +
           (2 * pending[i].offsets[dir] + 1) * side0 / (2 * subdivide[dir]);
          for (int k = 0; k < n && !(low && high); k += 1) {
            if (cverts[k * 3 + dir] <= div) {
              low = true;
            } else {
              high = true;
            }
          }
          if (!low && !high) {
// Could happen occasionally with rounding errors or corners exactly midway.
            dbg_on && dbg.dprintf(
             "!!! %s::make_index   polytope-disappeared=%d !!!", ClassName, n);
            continue;
          }
          if (low) {
            lowf.push_back(pending[i].polytopes[j]);
          }
          if (high) {
            highf.push_back(pending[i].polytopes[j]);
          }
        }
        int nsz = nodes0.size();
        int psz = new_pending.size();
// Don't dispose of polytope list nodes here because in later version might
// want more complex termination conditions which require trial subdivision.
        if (lowf.size() != 0 && highf.size() != 0) {
          nodes0[pending[i].node] = ((BRANCH_LH << 28) | nsz);
          nodes0.push_back(0);
          nodes0.push_back(0);
          new_pending.resize(psz + 2);
          new_pending[psz].node = nsz;
          new_pending[psz].polytopes.swap(lowf);
          for (int j = 0; j < 3; j += 1) {
            if (dir == j) {
              new_pending[psz].offsets[j] = 2 * pending[i].offsets[j];
            } else {
              new_pending[psz].offsets[j] = pending[i].offsets[j];
            }
          }
          new_pending[psz + 1].node = nsz + 1;
          new_pending[psz + 1].polytopes.swap(highf);
          for (int j = 0; j < 3; j += 1) {
            if (dir == j) {
              new_pending[psz + 1].offsets[j] = 2 * pending[i].offsets[j] + 1;
            } else {
              new_pending[psz + 1].offsets[j] = pending[i].offsets[j];
            }
          }
          cnt_br2 += 1;
        } else if (lowf.size() != 0) {
          nodes0[pending[i].node] = ((BRANCH_L << 28) | nodes0.size());
          nodes0.push_back(0);
          new_pending.resize(psz + 1);
          new_pending[psz].node = nsz;
          new_pending[psz].polytopes.swap(lowf);
          for (int j = 0; j < 3; j += 1) {
            if (dir == j) {
              new_pending[psz].offsets[j] = 2 * pending[i].offsets[j];
            } else {
              new_pending[psz].offsets[j] = pending[i].offsets[j];
            }
          }
          cnt_br1 += 1;
        } else {
          nodes0[pending[i].node] = ((BRANCH_H << 28) | nodes0.size());
          nodes0.push_back(0);
          new_pending.resize(psz + 1);
          new_pending[psz].node = nsz;
          new_pending[psz].polytopes.swap(highf);
          for (int j = 0; j < 3; j += 1) {
            if (dir == j) {
              new_pending[psz].offsets[j] = 2 * pending[i].offsets[j] + 1;
            } else {
              new_pending[psz].offsets[j] = pending[i].offsets[j];
            }
          }
          cnt_br1 += 1;
        }
      }
    }
    pending.swap(new_pending);
    depth += 1;
    subdivide[dir] *= 2;
    dir = (dir + 1) % 3;
    if (dbg_on) {
      int pf = 0;
      for (int i = 0; i < VectorSize(pending); i += 1) {
        pf += pending[i].polytopes.size();
      }
      dbg.dprintf("%s::make_index   interation=%d", ClassName, depth);
      dbg.dprintf("%s::make_index     pending=%d", ClassName,
       VectorSize(pending));
      dbg.dprintf("%s::make_index     avgppolys=%.1f", ClassName,
       pf * 1.0 / VectorSize(pending));
      dbg.dprintf("%s::make_index     nodes=%d", ClassName,
       VectorSize(nodes0));
      dbg.dprintf("%s::make_index     polys=%d", ClassName,
       VectorSize(plsts0));
      dbg.dprintf("%s::make_index     megs=%.1f+%.1f", ClassName,
       (VectorSize(nodes0) + VectorSize(plsts0)) * sizeof(int) /
       1000000.0, pf * sizeof(int) / 1000000.0);
      dbg.dprintf("%s::make_index     sides=%g/%g/%g", ClassName,
       side0 / subdivide[0], side0 / subdivide[1], side0 / subdivide[2]);
      dbg.dprintf("%s::make_index     node_cnts=%d/%d/%d", ClassName,
       cnt_br2, cnt_br1, cnt_fa);
    }
  }
  VectorShrink(nodes0);
  VectorShrink(plsts0);
  b_cube = bcube0;
  side = side0;
  nodes.swap(nodes0);
  polytope_lists.swap(plsts0);
  VectorReset(polytopes_checked_mask);
  int nm = (in_npolytopes + 31) / 32;
  polytopes_checked_mask.resize(nm);
  mask_bytes = nm * sizeof(int);
  n_masks_touched = 0;
  max_masks_touched = (nm + 3) / 4;
  mask_touches.resize(max_masks_touched);
  dbg_on && dbg.dprintf("%s::make_index }", ClassName);
}

double
ew::Index3::nearest_on_polytopes(int *polytope, double *proj, double *coeffs,
 const double *point)
{
  double p1[3];
  for (int i = 0; i < 3; i += 1) {
    p1[i] = (point[i] - b_cube.min[i]) / side;
  }
  int npending;
  ew::Index3ToCheck pending[MAX_DEPTH + 5];
  npending = 0;
  unsigned int nd0 = nodes[0];
  int subdivide0[3];
  subdivide0[0] = subdivide0[1] = subdivide0[2] = 1;
  int offset0[3];
  offset0[0] = offset0[1] = offset0[2] = 0;
  int dir0 = 0;
// Initial descent through tree.
// Follow best choices, leave alternate branches on the pending list.
  unsigned int npolytopes;
  unsigned int *polytopep;
  while (1) {
    unsigned int ty = nd0 >> 28;
    unsigned int da = (nd0 & 0xfffffff);
    if (ty == BRANCH_LH) {
      double div = (offset0[dir0] + 0.5) / subdivide0[dir0];
      subdivide0[dir0] *= 2;
      offset0[dir0] = 2 * offset0[dir0];
      pending[npending].subdivide[0] = subdivide0[0];
      pending[npending].subdivide[1] = subdivide0[1];
      pending[npending].subdivide[2] = subdivide0[2];
      pending[npending].offsets[0] = offset0[0];
      pending[npending].offsets[1] = offset0[1];
      pending[npending].offsets[2] = offset0[2];
      if (p1[dir0] < div) {
        pending[npending].node = da + 1;
        nd0 = nodes[da];
        pending[npending].offsets[dir0] += 1;
      } else {
        pending[npending].node = da;
        nd0 = nodes[da + 1];
        offset0[dir0] += 1;
      }
      pending[npending].dir = dir0 = (dir0 + 1) % 3;
      npending += 1;
      continue;
    } else if (ty <= BRANCH_H) {
      nd0 = nodes[da];
      subdivide0[dir0] *= 2;
      offset0[dir0] *= 2;
      if (ty == BRANCH_H) {
        offset0[dir0] += 1;
      }
      dir0 = (dir0 + 1) % 3;
      continue;
    } else if (ty == POLYTOPE) {
      npolytopes = 1;
      polytopep = &polytope_lists[da];
    } else if (ty == POLYTOPE_N) {
      npolytopes = polytope_lists[da];
      polytopep = &polytope_lists[da + 1];
    } else {
      npolytopes = (ty - POLYTOPE_2) + 2;
      polytopep = &polytope_lists[da];
    }
    break;
  }
#ifdef XXX
  for (int i=0; i < npending;i += 1) {
    chk(__LINE__, pending[i].node,
     pending[i].dir,
     pending[i].subdivide,
     pending[i].offsets);
  }
#endif
// Try all polytopes in this box.
  n_polytopes_tried = 0;
  double bestd = std::numeric_limits<double>::infinity();
  double proj1[3];
  double coeffs1[3];
  for (int i = 0; i < static_cast<int>(npolytopes); i += 1) {
    if (!touch_polytope(polytopep[i])) {
      continue;
    }
    n_polytopes_tried += 1;
// Geom routines use doubles, curves and surfaces use floats.
    double vv[9];
    for (int k1 = 0; k1 < in_nv; k1 += 1) {
      int vk1 = in_polytopes[in_nv * polytopep[i] + k1];
      for (int k2 = 0; k2 < 3; k2 += 1) {
        vv[3 * k1 + k2] = in_points[3 * vk1 + k2];
      }
    }
    double d = ew::Geom3::nearest_on_polytope(proj1, coeffs1, point, vv,
     in_nv);
    if (bestd > d) {
      bestd = d;
      for (int j = 0; j < 3; j += 1) {
        proj[j] = proj1[j];
        coeffs[j] = coeffs1[j];
        *polytope = polytopep[i];
      }
    }
  }
// Work up from bottom, working down from each pending.
  while (npending > 0) {
    npending -= 1;
    ew::Index3ToCheck *p = pending + npending;
    unsigned int nd = nodes[p->node];
    unsigned int ty = nd >> 28;
    unsigned int da = (nd & 0xfffffff);
    if (ty == BRANCH_LH) {
// Check that the box is feasible, then add both branches, most likely last.
      double d2l = 0.0, d2h = 0.0;
      bool hbest;
      for (int k = 0; k < 3; k += 1) {
        if (p->dir == k) {
          double l = static_cast<double>(p->offsets[k]) / p->subdivide[k];
          double m = (p->offsets[k] + 0.5) / p->subdivide[k];
          double h = (p->offsets[k] + 1.0) / p->subdivide[k];
          if (p1[k] < l) {
            d2l += (l - p1[k]) * (l - p1[k]);
            d2h += (m - p1[k]) * (m - p1[k]);
            hbest = false;
          } else if (p1[k] > h) {
            d2l += (p1[k] - m) * (p1[k] - m);
            d2h += (p1[k] - h) * (p1[k] - h);
            hbest = true;
          } else if (p1[k] < m) {
            d2h += (m - p1[k]) * (m - p1[k]);
            hbest = false;
          } else {
            d2l += (p1[k] - m) * (p1[k] - m);
            hbest = true;
          }
        } else {
          double l = static_cast<double>(p->offsets[k]) / p->subdivide[k];
          double h = (p->offsets[k] + 1.0) / p->subdivide[k];
          if (p1[k] < l) {
            d2l += (l - p1[k]) * (l - p1[k]);
            d2h += (l - p1[k]) * (l - p1[k]);
          } else if (p1[k] > h) {
            d2l += (p1[k] - h) * (p1[k] - h);
            d2h += (p1[k] - h) * (p1[k] - h);
          }
        }
      }
      int pdir0 = p->dir;
      p->dir = (p->dir + 1) % 3;
      p->subdivide[pdir0] *= 2;
      p->offsets[pdir0] *= 2;
// Worst branch.
      bool done_one = false;
      double d2b;
      if (hbest) {
        d2b = d2l;
      } else {
        d2b = d2h;
      }
      if (d2b < (bestd / side) * (bestd / side)) {
        if (!hbest) {
          p->offsets[pdir0] += 1;
          p->node = da + 1;
#ifdef XXX
          chk(__LINE__, pending[npending].node,
           pending[npending].dir,
           pending[npending].subdivide,
           pending[npending].offsets);
#endif
        } else {
          p->node = da;
#ifdef XXX
          chk(__LINE__, pending[npending].node,
           pending[npending].dir,
           pending[npending].subdivide,
           pending[npending].offsets);
#endif
        }
        npending += 1;
        done_one = true;
      }
// Best branch.
      if (hbest) {
        d2b = d2h;
      } else {
        d2b = d2l;
      }
      if (d2b < (bestd / side) * (bestd / side)) {
        if (done_one) {
          p[1].dir = p->dir;
          for (int i = 0; i < 3; i += 1) {
            p[1].offsets[i] = p->offsets[i];
            p[1].subdivide[i] = p->subdivide[i];
          }
          if (hbest) {
            p[1].node = da + 1;
            p[1].offsets[pdir0] += 1;
#ifdef XXX
            chk(__LINE__, pending[npending].node,
             pending[npending].dir,
             pending[npending].subdivide,
             pending[npending].offsets);
#endif
          } else {
            p[1].node = da;
            p[1].offsets[pdir0] -= 1;
#ifdef XXX
            chk(__LINE__, pending[npending].node,
             pending[npending].dir,
             pending[npending].subdivide,
             pending[npending].offsets);
#endif
          }
        } else {
          if (hbest) {
            p->offsets[pdir0] += 1;
            p->node = da + 1;
#ifdef XXX
            XXXchk(__LINE__, pending[npending].node,
             pending[npending].dir,
             pending[npending].subdivide,
             pending[npending].offsets);
#endif
          } else {
            p->node = da;
#ifdef XXX
            XXXchk(__LINE__, pending[npending].node,
             pending[npending].dir,
             pending[npending].subdivide,
             pending[npending].offsets);
#endif
          }
        }
        npending += 1;
      }
      continue;
    } else if (ty <= BRANCH_H) {
// Just follow unbranched node, leave at end of pending list.
      p->subdivide[p->dir] *= 2;
      p->offsets[p->dir] *= 2;
      if (ty == BRANCH_H) {
        p->offsets[p->dir] += 1;
      }
      p->dir = (p->dir + 1) % 3;
      p->node = da;
#ifdef XXX
      XXXchk(__LINE__, pending[npending].node,
       pending[npending].dir,
       pending[npending].subdivide,
       pending[npending].offsets);
#endif
      npending += 1;
#ifdef XXX
// for (int i=0; i < npending;i += 1)
// chk(__LINE__, pending[i].node,
// pending[i].dir,
// pending[i].subdivide,
// pending[i].offsets);
#endif
      continue;
    } else if (ty == POLYTOPE) {
      npolytopes = 1;
      polytopep = &polytope_lists[da];
    } else if (ty == POLYTOPE_N) {
      npolytopes = polytope_lists[da];
      polytopep = &polytope_lists[da + 1];
    } else {
      npolytopes = (ty - POLYTOPE_2) + 2;
      polytopep = &polytope_lists[da];
    }
    double d2 = 0.0;
    for (int k = 0; k < 3; k += 1) {
      double l = static_cast<double>(p->offsets[k]) / p->subdivide[k];
      double h = (p->offsets[k] + 1.0) / p->subdivide[k];
      if (p1[k] < l) {
        d2 += (l - p1[k]) * (l - p1[k]);
      } else if (p1[k] > h) {
        d2 += (p1[k] - h) * (p1[k] - h);
      }
    }
    if (d2 >= (bestd / side) * (bestd / side)) {
      continue;
    }
    for (int i = 0; i < static_cast<int>(npolytopes); i += 1) {
      if (!touch_polytope(polytopep[i])) {
        continue;
      }
      n_polytopes_tried += 1;
      double vv[9];
      for (int k1 = 0; k1 < in_nv; k1 += 1) {
        int vk1 = in_polytopes[in_nv * polytopep[i] + k1];
        for (int k2 = 0; k2 < 3; k2 += 1) {
          vv[3 * k1 + k2] = in_points[3 * vk1 + k2];
        }
      }
      double d = ew::Geom3::nearest_on_polytope(proj1, coeffs1, point, vv,
       in_nv);
      if (bestd > d) {
        bestd = d;
        for (int j = 0; j < 3; j += 1) {
          proj[j] = proj1[j];
          coeffs[j] = coeffs1[j];
        }
        *polytope = polytopep[i];
      }
    }
  }
  untouch_polytopes();
  return bestd;
}
//XXX try using float Geom3 routines

void
ew::Index3::reset()
{
  in_points = 0;
  in_npoints = 0;
  in_polytopes = 0;
  in_npolytopes = 0;
  b_cube.set_to_empty();
  side = 0.0;
  VectorReset(nodes);
  VectorReset(polytope_lists);
  VectorReset(polytopes_checked_mask);
  mask_bytes = 0;
  mask_touches.resize(0);
  n_masks_touched = 0;
  max_masks_touched = 0;
}
