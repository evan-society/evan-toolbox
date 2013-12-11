#include <cmath>
#include <string>
#include <vector>
#include "ew/Bbox3.h"
#include "ew/Debug.h"
#include "ew/Surface3.h"
#include "ew/Time.h"
#include "src/Geom3.h"
#include "src/Index3.h"
#include "testlowercase.h"

namespace {
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
}

static double
MyRand()
{
  static int RandSeed = 123459876;
  int tmp = RandSeed / 12773;
  RandSeed = 16807 * (RandSeed - tmp * 127773) - 2836 * tmp;
  if (RandSeed < 0) {
    RandSeed += 2147483647;
  }
  RandSeed &= (((static_cast<unsigned int>(0xfffffff)) << 4) | 0xf);
  return RandSeed * (1.0 / 2147483647);
}

static const char Name01[] = "make_index";

struct stats {
  std::vector<int> ncnt;
  std::vector<int> fcnt;
  double vol0;
  double vol1;
  int dpmax;
  int dpmin;
  double dv;
  unsigned int lmax;
  unsigned int lmin;
  double lv;
};

static void
ChkPolytopeList(ew::Index3 *ind, unsigned int *subdivides,
 unsigned int *offsets, unsigned int *ff, int l)
{
  ew::Bbox3 b1, b2;
  for (int k = 0; k < 3; k += 1) {
    b1.min[k] = b2.min[k] =
     ind->b_cube.min[k] + (ind->side * offsets[k]) / subdivides[k];
    b1.max[k] = b2.max[k] =
     ind->b_cube.min[k] + (ind->side * (offsets[k] + 1)) / subdivides[k];
    b1.min[k] += 1e-10;
    b1.max[k] -= 1e-10;
    b2.min[k] -= 1e-10;
    b2.max[k] += 1e-10;
  }
  double outp[85 * 3];
  double vv[9];
  int j = 0;
  for (int i = 0; i < ind->in_npolytopes; i += 1) {
    for (int k1 = 0; k1 < 3; k1 += 1) {
      int vk1 = ind->in_polytopes[3 * i + k1];
      for (int k2 = 0; k2 < 3; k2 += 1) {
        vv[3 * k1 + k2] = ind->in_points[3 * vk1 + k2];
      }
    }
    if (j < l && static_cast<unsigned int>(i) == ff[j]) {
      j += 1;
// Expect polytope to intersect sub-box.
      if (ew::Geom3::crop_polytope(outp, vv, 3, &b2) == 0) {
        Error("missing polytope");
      }
    } else {
// Don't expect polytope to intersect sub-box.
      if (ew::Geom3::crop_polytope(outp, vv, 3, &b1) != 0) {
        Error("extra polytope");
      }
    }
  }
}

static void
Recurse01(ew::Index3 *ind, int node, stats *st, int dp,
 unsigned int *subdivides, unsigned int *offsets)
{
  const ew::Debug &dbg = ew::Debug::instance();
  bool test_long = dbg.get_active("long") && !dbg.get_active("nolong");
  unsigned int subdividesl[3];
  unsigned int subdividesh[3];
  unsigned int offsetsl[3];
  unsigned int offsetsh[3];
  int dp1 = dp + 1;
  int dir = dp % 3;
  double v = 1.0;
  for (int i = 0; i < 3; i += 1) {
    v /= subdivides[i];
    subdividesl[i] = subdividesh[i] = subdivides[i];
    offsetsl[i] = offsetsh[i] = offsets[i];
  }
  subdividesl[dir] *= 2;
  subdividesh[dir] *= 2;
  offsetsl[dir] *= 2;
  offsetsh[dir] *= 2;
  offsetsh[dir] += 1;
  if (node >= VectorSize(ind->nodes) || st->ncnt[node] != 0) {
    Error("bad node");
    return;
  }
  st->ncnt[node] += 1;
  unsigned int ty = (ind->nodes[node] >> 28);
  unsigned int da = (ind->nodes[node] & 0xfffffff);
  if (ind->nodes[node] != (da | (ty << 28))) {
    Error("bad decomposition");
    return;
  }
  unsigned int l;
  switch (ty) {
  case ew::Index3::BRANCH_LH:
    Recurse01(ind, da, st, dp1, subdividesl, offsetsl);
    Recurse01(ind, da + 1, st, dp1, subdividesh, offsetsh);
    return;
  case ew::Index3::BRANCH_L:
    Recurse01(ind, da, st, dp1, subdividesl, offsetsl);
    if (test_long) {
      ChkPolytopeList(ind, subdividesh, offsetsh, 0, 0);
    }
    st->vol0 += v / 2.0;
    return;
  case ew::Index3::BRANCH_H:
    if (test_long) {
      ChkPolytopeList(ind, subdividesl, offsetsl, 0, 0);
    }
    Recurse01(ind, da, st, dp1, subdividesh, offsetsh);
    st->vol0 += v / 2.0;
    return;
  case ew::Index3::POLYTOPE:
    if (da > static_cast<unsigned int>(ind->in_npolytopes)) {
      Error("bad polytope");
    }
    l = 1;
    if (test_long) {
      ChkPolytopeList(ind, subdivides, offsets, &da, l);
    }
    break;
  case ew::Index3::POLYTOPE_2:
  case ew::Index3::POLYTOPE_3:
  case ew::Index3::POLYTOPE_4:
  case ew::Index3::POLYTOPE_5:
  case ew::Index3::POLYTOPE_6:
  case ew::Index3::POLYTOPE_7:
  case ew::Index3::POLYTOPE_8:
  case ew::Index3::POLYTOPE_9:
  case ew::Index3::POLYTOPE_10:
  case ew::Index3::POLYTOPE_11:
  case ew::Index3::POLYTOPE_12:
    l = ty - ew::Index3::POLYTOPE_2 + 2;
    if (da + l < da || da + l >= ind->polytope_lists.size()) {
      Error("bad polytope index");
    } else {
      if (test_long) {
        ChkPolytopeList(ind, subdivides, offsets, &ind->polytope_lists[da],
         l);
      }
      unsigned int lf;
      for (unsigned int i = 0; i < l; i += 1) {
        unsigned int f = ind->polytope_lists[da + i];
        if (st->fcnt[da + i] != 0 || (i != 0 && f <= lf) ||
         f > static_cast<unsigned int>(ind->in_npolytopes)) {
          Error("bad polytope 2");
        }
        st->fcnt[da + i] += 1;
        lf = f;
      }
    }
    break;
  case ew::Index3::POLYTOPE_N:
    if (da >= ind->polytope_lists.size() || st->fcnt[da] != 0) {
      Error("bad polytope cnt");
    } else {
      l = ind->polytope_lists[da];
    }
    st->fcnt[da] += 1;
    if (l <= 12 || l > static_cast<unsigned int>(ind->in_npolytopes)) {
      Error("bad polytope cnt 2");
    }
    if (da + l + 1 < da || da + l + 1 > ind->polytope_lists.size()) {
      Error("bad polytope index 2 %u %u", da, l);
    } else {
      if (test_long) {
        ChkPolytopeList(ind, subdivides, offsets,
         &ind->polytope_lists[da + 1], l);
      }
      unsigned int lf;
      for (unsigned int i = 0; i < l; i += 1) {
        unsigned int f = ind->polytope_lists[da + i + 1];
        if (st->fcnt[da + i + 1] != 0 || (i != 0 && f <= lf) ||
         f > static_cast<unsigned int>(ind->in_npolytopes)) {
          Error("bad polytope 3");
        }
        st->fcnt[da + i + 1] += 1;
        lf = f;
      }
    }
    break;
  default:
    Error("bad type");
    break;
  }
  st->vol1 += v;
  if (st->dpmax < dp) {
    st->dpmax = dp;
  }
  if (st->dpmin > dp) {
    st->dpmin = dp;
  }
  st->dv += v * dp;
  if (st->lmax < l) {
    st->lmax = l;
  }
  if (st->lmin > l) {
    st->lmin = l;
  }
  st->lv += v * l;
}

static void
Test01()
{
  const ew::Debug &dbg = ew::Debug::instance();
  bool test_long = dbg.get_active("long") && !dbg.get_active("nolong");
  if (test_long) {
    dbg.dprintf("%s doing exhaustive check of index", "long");
  }
  double t0, t1, t2;
  ew::Surface3 sur;
  ew::Index3 index;
  std::string fname = FullPath("mandible.obj");
  t0 = ew::Time::elapsed();
  sur.read_file(fname.c_str());
  t1 = ew::Time::elapsed();
  float *surd = VectorData(sur.points);
  int surs = VectorSize(sur.points);
  int *facd = VectorData(sur.faces);
  int facs = VectorSize(sur.faces);
  index.make_index(surd, surs / 3, facd, 3, facs / 3);
  t2 = ew::Time::elapsed();
  if (dbg.get_active("timing")) {
    dbg.dprintf("%s read faces=%d points=%d %.6f", "timing", facs / 3,
     surs / 3, t1 - t0);
    dbg.dprintf("%s index nodes=%d facelist=%d %.6f", "timing",
     VectorSize(index.nodes), VectorSize(index.polytope_lists),
     t2 - t1);
  }
  ew::Bbox3 b1;
  b1.set_to_points(surd, surs / 3);
  double s = 0.0;
  for (int i = 0; i < 3; i += 1) {
    double f = b1.max[i] - b1.min[i];
    if (s < f) {
      s = f;
    }
  }
  if (s != index.side) {
    Error("side");
  }
  for (int i = 0; i < 3; i += 1) {
    if (b1.min[i] < index.b_cube.min[i] || b1.max[i] > index.b_cube.max[i] ||
     std::fabs(index.b_cube.max[i] - index.b_cube.min[i]) < 1e-13) {
      Error("bbox");
      break;
    }
  }
  if (index.nodes.size() == 0) {
    Error("no nodes");
  }
  stats st;
  st.ncnt.resize(index.nodes.size());
  st.fcnt.resize(index.polytope_lists.size());
  st.vol0 = st.vol1 = st.dv = st.lv = 0.0;
  st.dpmax = 0;
  st.dpmin = 9999;
  st.lmax = 0;
  st.lmin = 9999;
  unsigned int offsets[3] = {0, 0, 0};
  unsigned int subdivides[3] = {1, 1, 1};
  Recurse01(&index, 0, &st, 0, subdivides, offsets);
  for (int i = 0; i < VectorSize(index.nodes); i += 1) {
    if (st.ncnt[i] != 1) {
      Error("skipped node %d", i);
    }
  }
  for (int i = 0; i < VectorSize(index.polytope_lists); i += 1) {
    if (st.fcnt[i] != 1) {
      Error("skipped polytope %d", i);
    }
  }
  if (std::fabs(st.vol0 + st.vol1 - 1.0) > 1e-10) {
    Error("bad volumes");
  }
  if (dbg.get_active("timing")) {
    dbg.dprintf("%s index-vol %.6f", "timing", st.vol1);
    dbg.dprintf("%s depth min %d max %d avg %.2f", "timing", st.dpmin,
     st.dpmax, st.dv / st.vol1);
    dbg.dprintf("%s faces min %u max %u avg %.2f", "timing", st.lmin, st.lmax,
     st.lv / st.vol1);
  }
}

static const char Name02[] = "nearest_on_polytopes";

static void
Test02()
{
  const ew::Debug &dbg = ew::Debug::instance();
  bool test_long = dbg.get_active("long") && !dbg.get_active("nolong");
  ew::Surface3 sur;
  ew::Index3 index;
  std::string fname = FullPath("mandible.obj");
  sur.read_file(fname.c_str());
  float *surd = VectorData(sur.points);
  int surs = VectorSize(sur.points);
  int *facd = VectorData(sur.faces);
  int facs = VectorSize(sur.faces);
  index.make_index(surd, surs / 3, facd, 3, facs / 3);
  int N = 100;
  if (test_long) {
    N = 1000;
    dbg.dprintf("%s checking %d points", "long", N);
  }
  std::vector<double> pts(3 * N), d1(N), d2(N), proj(3 * N), coeffs(3 * N);
  std::vector<int> fc(N), fn(N);
  double t0, t1, t2;
  double proj1[3];
  double coeffs1[3];
  for (int rep = 0; rep < 2; rep += 1) {
    switch (rep) {
    case 0:
      for (int i = 0; i < N; i += 1) {
        pts[3 * i + 0] = index.b_cube.min[0] + index.side * MyRand();
        pts[3 * i + 1] = index.b_cube.min[1] + index.side * MyRand();
        pts[3 * i + 2] = index.b_cube.min[2] + index.side * MyRand();
      }
      break;
    case 1:
      for (int i = 0; i < N; i += 1) {
        int f = static_cast<int>(MyRand() * index.in_npolytopes);
        double vv[9];
        for (int k1 = 0; k1 < 3; k1 += 1) {
          int vk1 = index.in_polytopes[3 * f + k1];
          for (int k2 = 0; k2 < 3; k2 += 1) {
            vv[3 * k1 + k2] = index.in_points[3 * vk1 + k2];
          }
        }
        double t[3];
        ew::Geom3::subtract(t, vv, vv + 3);
        double d = ew::Geom3::norm(t);
        pts[3 * i + 0] = vv[0] + 2 * d * MyRand();
        pts[3 * i + 1] = vv[1] + 2 * d * MyRand();
        pts[3 * i + 2] = vv[2] + 2 * d * MyRand();
      }
      break;
    }
    t0 = ew::Time::elapsed();
    for (int i = 0; i < N; i += 1) {
      d1[i] = index.nearest_on_polytopes(&fn[i], &proj[3 * i], &coeffs[3 * i],
       &pts[3 * i]);
      fc[i] = index.n_polytopes_tried;
    }
    t1 = ew::Time::elapsed();
    for (int i = 0; i < N; i += 1) {
      d2[i] = 9.999e100;
      for (int j = 0; j < index.in_npolytopes; j += 1) {
        double vv[9];
        for (int k1 = 0; k1 < 3; k1 += 1) {
          int vk1 = index.in_polytopes[3 * j + k1];
          for (int k2 = 0; k2 < 3; k2 += 1) {
            vv[3 * k1 + k2] = index.in_points[3 * vk1 + k2];
          }
        }
        double d = ew::Geom3::nearest_on_polytope(proj1, coeffs1, &pts[3 * i],
         vv, 3);
        if (d2[i] > d) {
          d2[i] = d;
        }
      }
    }
    t2 = ew::Time::elapsed();
    for (int i = 0; i < N; i += 1) {
      if (std::fabs(d1[i] - d2[i]) > 1e-12) {
        Error("bad minimal distance %g %g", d1[i], d2[i]);
      }
      if (fn[i] < 0 || fn[i] > index.in_npolytopes) {
        Error("polytope out of range");
      } else {
        double vv[9];
        for (int k1 = 0; k1 < 3; k1 += 1) {
          int vk1 = index.in_polytopes[3 * fn[i] + k1];
          for (int k2 = 0; k2 < 3; k2 += 1) {
            vv[3 * k1 + k2] = index.in_points[3 * vk1 + k2];
          }
        }
        double d = ew::Geom3::nearest_on_polytope(proj1, coeffs1, &pts[3 * i],
         vv, 3);
        if (std::fabs(d1[i] - d) > 1e-12) {
          Error("inconsistent distance");
        }
        if (std::fabs(proj1[0] - proj[3 * i + 0]) > 1e-12 ||
         std::fabs(proj1[1] - proj[3 * i + 1]) > 1e-12 ||
         std::fabs(proj1[2] - proj[3 * i + 2]) > 1e-12) {
          Error("inconsistent proj");
        }
        if (std::fabs(coeffs1[0] - coeffs[3 * i + 0]) > 1e-12 ||
         std::fabs(coeffs1[1] - coeffs[3 * i + 1]) > 1e-12 ||
         std::fabs(coeffs1[2] - coeffs[3 * i + 2]) > 1e-12) {
          Error("inconsistent coeffs");
        }
      }
    }
    int minf = fc[0];
    int maxf = fc[0];
    int sumf = fc[0];
    for (int i = 1; i < N; i += 1) {
      if (minf > fc[i]) {
        minf = fc[i];
      }
      if (maxf < fc[i]) {
        maxf = fc[i];
      }
      sumf += fc[i];
    }
    if (dbg.get_active("timing")) {
      if (rep == 0) {
        dbg.dprintf("%s %d uniform random points", "timing", N);
      } else {
        dbg.dprintf("%s %d random points close to surface", "timing", N);
      }
      dbg.dprintf("%s   w/o index: elapsed=%.6f avg %.6f", "timing", t2 - t1,
       (t2 - t1) / N);
      dbg.dprintf("%s   indexed:   elapsed=%.6f avg %.6f", "timing", t1 - t0,
       (t1 - t0) / N);
      dbg.dprintf("%s     faces min %d max %d avg %.2f", "timing", minf, maxf,
       sumf * 1.0 / N);
    }
  }
}

static test_t Tests[] = {
  {Name01, &Test01},
  {Name02, &Test02},
  {0, 0}
};

int
main(int ac, char **av)
{
  return ProcessCmdline("test_index3",
   "Exercise the 3D polytope indexing [timing, long].\n", 0, Tests, ac, av);
}
