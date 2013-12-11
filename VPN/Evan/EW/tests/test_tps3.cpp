#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include "ew/Debug.h"
#include "ew/Time.h"
#include "ew/Tps3.h"
#include "testlowercase.h"

static const int D = 3;
static const int D1 = D + 1;
static const int DD1 = D * D1;
enum {RELAX_DIM_0, RELAX_DIM_1, RELAX_DIM_2, RELAX_DIM_3};
static const int RELAX_DIM_D = D;
static const double Tol = 1e-10;
static const double TolCacheMap = 1e-3;
static const double TolVtk = 1e-5;
static const double TolContinuous = 5.0;
static const double TolMinimize = 0.1;

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

typedef struct {
  std::size_t size;
// Ensure (p + alloc_head) will be aligned for int, float and double.
  union {
    int i;
    float f;
    double d;
  };
  unsigned char border[100];
} alloc_head;

typedef struct {
  unsigned char border[100];
} alloc_tail;

inline static void *
AllocAdd(void *p, std::size_t n)
{
  return static_cast<void *>(static_cast<char *>(p) + n);
}

inline static void *
AllocSub(void *p, std::size_t n)
{
  return static_cast<void *>(static_cast<char *>(p) - n);
}

void *
operator new[](std::size_t size) throw (std::bad_alloc)
{
  void *p = malloc(size + sizeof(alloc_head) + sizeof(alloc_tail));
  if (p == 0) {
    throw std::bad_alloc();
  }
  alloc_head *h = static_cast<alloc_head *>(p);
  void *v = AllocAdd(p, sizeof(alloc_head));
  alloc_tail *t = static_cast<alloc_tail *>(AllocAdd(v, size));
  h->size = size;
  for (int i = 0; i < 100 ; i += 1) {
    h->border[i] = 136 + i;
    t->border[i] = 137 + i;
  }
  return v;
}

void
operator delete[](void *v) throw()
{
  void *p = AllocSub(v, sizeof(alloc_head));
  alloc_head *h = static_cast<alloc_head *>(p);
  std::size_t size = h->size;
  alloc_tail *t = static_cast<alloc_tail *>(AllocAdd(v, size));
// Check no tresspassing.
  int bad = 0;
  for (int i = 0; i < 100 ; i += 1) {
    if (h->border[i] != 136 + i || t->border[i] != 137 + i) {
      bad = 1;
    }
  }
  if (bad) {
    Error("alloc bounds broken");
  }
  free(p);
}

class test {
public:
  test(int n, double *lmks, int *relax_dims, double *relax_params,
   double *lmk_images, int l, double *points);
  ~test();
  void check_rtype(int exp);
  void check_singular(int exp);
  void check_map(int exp, double *a = 0);
  int n;
  double *lmks;
  int *relax_dims;
  double *relax_params;
  double *lmk_images;
  int l;
  double *points;
  int m;
  double *L;
  int t_size;
  bool is_mixed;
  bool is_reduced;
  double *T;
  int *pivots;
  double is_nonsingular;
  double *E;
  double *P;
  double *ev;
  std::vector<double> lmkscentroid;
  std::vector<double> lmksaxes;
  std::vector<double> lmksss;
  std::vector<double> unitrans;
  double *unibasis;
  double *spline;
  double energy;
  double *relax_lmk_images;
  double *point_images;
  const ew::Debug &dbg;
};

test::test(int na, double *lmksa, int *relax_dimsa, double *relax_paramsa,
 double *lmk_imagesa, int la, double *pointsa)
 : n(na),
 lmks(lmksa),
 relax_dims(relax_dimsa),
 relax_params(relax_paramsa),
 lmk_images(lmk_imagesa),
 l(la),
 points(pointsa),
 m(n + D1),
 L(0),
 T(0),
 pivots(0),
 E(0),
 P(0),
 ev(0),
 lmkscentroid(D),
 lmksaxes(D * D),
 lmksss(D),
 unitrans(5 * D * D),
 unibasis(0),
 spline(0),
 relax_lmk_images(0),
 point_images(0),
 dbg(ew::Debug::instance())
{
  L = new double[m * m];
  ew::Tps3::interaction(L, lmks, n);
  ew::Tps3::algebraic_size(&t_size, &is_mixed, &is_reduced, relax_dims, n);
// Check t_size satisfies basic constraints.
  int min_ts;
  if (is_mixed) {
    min_ts = DD1;
  } else {
    min_ts = D1;
  }
  if (t_size < min_ts || t_size > (n + D1) * D) {
    Error("bad sizes");
  }
  T = new double[t_size * t_size];
  pivots = new int[t_size];
  ew::Tps3::factorize(T, pivots, &is_nonsingular, L, relax_dims, relax_params,
   t_size, is_mixed, is_reduced, n);
  if (is_nonsingular < Tol || lmk_images == 0) {
    return;
  }
  spline = new double[m * D];
  int nev;
  if (is_mixed) {
    nev = n * D;
  } else {
    nev = n;
  }
  E = new double[nev * nev];
  P = new double[nev * nev];
  ev = new double[nev];
  unibasis = new double[5 * n * D];
  ew::Tps3::bending_energy(E, T, pivots, relax_dims, relax_params, n, t_size,
   is_mixed, is_reduced);
  ew::Tps3::principal_warps(P, ev, E, n, is_mixed);
  ew::Tps3::principal_axes(&lmkscentroid[0], &lmksss[0], &lmksaxes[0], lmks, n);
  ew::Tps3::uniform_basis(unibasis, &unitrans[0], &lmkscentroid[0], &lmksss[0],
   &lmksaxes[0], lmks, n);
  double *spline2 = new double[m * D];
  relax_lmk_images = new double[n * D];
// Exercise code paths with zero energy and relax_lmk_images, and check results
// are the same.
  ew::Tps3::solve(spline, 0, 0, T, pivots, lmk_images,
   relax_dims, relax_params, L, n, t_size, is_mixed, is_reduced);
  ew::Tps3::solve(spline2, &energy, relax_lmk_images, T, pivots, lmk_images,
   relax_dims, relax_params, L, n, t_size, is_mixed, is_reduced);
  double diffsp = 0.0;
  for (int i = 0; i < m * D; i += 1) {
    double d = std::fabs(spline2[i] - spline[i]);
    if (d > diffsp) {
      diffsp = d;
    }
  }
  if (diffsp > Tol) {
    Error("spline changed %.2e", diffsp);
  }
  delete[] spline2;
// Check energy is correct and not negative.
  double e = 0.0;
  for (int i = 0; i < n * D; i += 1) {
    e += spline[i + DD1] * lmk_images[i];
  }
  if (fabs(e - energy) > Tol) {
    Error("wrong energy %e != %e", e, energy);
  }
  if (e < -Tol) {
    Error("negative energy %e", e);
  }
  double e1 = 0.0;
  if (is_mixed == 0) {
    for (int i = 0; i < n; i += 1) {
      for (int j = 0; j < n; j += 1) {
        e1 += lmk_images[D * j] * lmk_images[D * i] * E[n * i + j];
        e1 += lmk_images[D * j + 1] * lmk_images[D * i + 1] * E[n * i + j];
        e1 += lmk_images[D * j + 2] * lmk_images[D * i + 2] * E[n * i + j];
      }
    }
  } else {
    for (int i = 0; i < D * n; i += 1) {
      for (int j = 0; j < D * n; j += 1) {
        e1 += lmk_images[j] * lmk_images[i] * E[D * n * i + j];
      }
    }
  }
  if (fabs(e1 - energy) > Tol) {
    Error("wrong bending energy %e != %e", e1, energy);
  }
  double *map_lmks = new double[D * n];
  ew::Tps3::map(map_lmks, lmks, n, spline, lmks, n);
// Check bending energy matrix.
  int notsym = 0;
  for (int i = 0; i < nev; i += 1) {
    double s;
    for (int j = 0; j < nev; j += 1) {
      s = E[j * nev + i] - E[i * nev + j];
    }
    if (s < -Tol || s > Tol) {
      notsym = 1;
    }
  }
  if (notsym) {
    Error("energy matrix not symmetric");
  }
  int notaff0 = 0;
  if (is_mixed == 0) {
    for (int i = 0; i < nev; i += 1) {
      for (int j = 0; j < nev; j += 1) {
        double s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
        for (int k = 0; k < nev; k += 1) {
          s1 += E[j * nev + k];
          s2 += E[j * nev + k] * lmks[D * k];
          s3 += E[j * nev + k] * lmks[D * k + 1];
          s4 += E[j * nev + k] * lmks[D * k + 2];
        }
        if (s1 < -Tol || s1 > Tol ||
         s2 < -Tol || s2 > Tol ||
         s3 < -Tol || s3 > Tol ||
         s4 < -Tol || s4 > Tol) {
          notaff0 = 1;
        }
      }
    }
  } else {
    for (int i = 0; i < nev; i += 1) {
      for (int j = 0; j < nev; j += 1) {
        double s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
        double s5 = 0.0, s6 = 0.0, s7 = 0.0, s8 = 0.0;
        for (int k = 0; k < nev; k += D) {
          s1 += E[j * nev + k];
          s2 += E[j * nev + k] * lmks[k];
          s3 += E[j * nev + k] * lmks[k + 1];
          s4 += E[j * nev + k] * lmks[k + 2];
          s5 += E[j * nev + k + 1];
          s6 += E[j * nev + k + 1] * lmks[k];
          s7 += E[j * nev + k + 1] * lmks[k + 1];
          s8 += E[j * nev + k + 1] * lmks[k + 2];
        }
        if (s1 < -Tol || s1 > Tol ||
         s2 < -Tol || s2 > Tol ||
         s3 < -Tol || s3 > Tol ||
         s4 < -Tol || s4 > Tol ||
         s5 < -Tol || s5 > Tol ||
         s6 < -Tol || s6 > Tol ||
         s7 < -Tol || s7 > Tol ||
         s8 < -Tol || s8 > Tol) {
          notaff0 = 1;
        }
      }
    }
  }
  if (notaff0) {
    Error("affine maps not in energy null space");
  }
  int notrel0 = 0;
  if (is_mixed != 0) {
    for (int i = 0; i < n; i += 1) {
      if (relax_dims[i] == D) {
        for (int j = 0; j < nev; j += 1) {
          double s1 = E[D * i * nev + j];
          double s2 = E[(D * i + 1) * nev + j];
          double s3 = E[(D * i + 2) * nev + j];
          if (s1 < -Tol || s1 > Tol ||
           s2 < -Tol || s2 > Tol ||
           s3 < -Tol || s3 > Tol) {
            notrel0 = 1;
          }
        }
      }
    }
    for (int i = 0; i < n; i += 1) {
      if (relax_dims[i] == 1) {
        for (int j = 0; j < nev; j += 1) {
          double s = E[D * i * nev + j] * relax_params[D * i] +
           E[(D * i + 1) * nev + j] * relax_params[D * i + 1] +
           E[(D * i + 2) * nev + j] * relax_params[D * i + 2];
          if (s < -Tol || s > Tol) {
            notrel0 = 1;
          }
        }
      }
    }
    for (int i = 0; i < n; i += 1) {
      if (relax_dims[i] == 2) {
        for (int j = 0; j < nev; j += 1) {
          double s1 = E[D * i * nev + j] * relax_params[D * i + 1] -
           E[(D * i + 1) * nev + j] * relax_params[D * i];
          double s2 = E[D * i * nev + j] * relax_params[D * i + 2] -
           E[(D * i + 2) * nev + j] * relax_params[D * i];
          double s3 = E[(D * i + 1) * nev + j] * relax_params[D * i + 2] -
           E[(D * i + 2) * nev + j] * relax_params[D * i + 1];
          if (s1 < -Tol || s1 > Tol ||
           s2 < -Tol || s2 > Tol ||
           s3 < -Tol || s3 > Tol) {
            notrel0 = 1;
          }
        }
      }
    }
  } else if (is_reduced != 0) {
    for (int i = 0; i < n; i += 1) {
      if (relax_dims[i] != 0) {
        for (int j = 0; j < n; j += 1) {
          double s = E[i * n + j];
          if (s < -Tol || s > Tol) {
            notrel0 = 1;
          }
        }
      }
    }
  }
  if (notrel0) {
    Error("relax maps not in energy null space");
  }
// Check principal warps.
  int badev = 0;
  for (int i = 1; i < nev; i += 1) {
    if (ev[i] < ev[i - 1]) {
      badev = 1;
    }
  }
  for (int i = 0; i < nev; i += 1) {
    if (ev[i] < -Tol) {
      badev = 1;
    }
  }
  int evi0 = nev;
  for (int i = 0; i < nev; i += 1) {
    if (ev[i] >= Tol) {
      evi0 = i;
      break;
    }
  }
  if (is_mixed == 0) {
    if (evi0 != D + 1 + n + D + 1 - t_size) {
      badev = 1;
    }
  } else {
    if (evi0 != D * (D + 1) + D * (n + D + 1) - t_size) {
      badev = 1;
    }
  }
  if (badev) {
    Error("bad eigenvalues");
  }
  int notortho = 0;
  for (int i = 0; i < nev; i += 1) {
    for (int j = i; j < nev; j += 1) {
      double s = 0.0;
      for (int k = 0; k < nev; k += 1) {
        s += P[i * nev + k] * P[j * nev + k];
      }
      if (i == j) {
        s -= 1.0;
      }
      if (s < -Tol || s > 1e-1) {
        notortho = 1;
      }
    }
  }
  if (notortho) {
    Error("principal warps not orthonormal");
  }
  int notev = 0;
  for (int i = 0; i < nev; i += 1) {
    for (int j = 0; j < nev; j += 1) {
      double s = 0.0;
      for (int k = 0; k < nev; k += 1) {
        s += E[j * nev + k] * P[i * nev + k];
      }
      s -= ev[i] * P[i * nev + j];
      if (s < -Tol || s > Tol) {
        notev = 1;
      }
    }
  }
  if (notev) {
    Error("principal warps not evals");
  }
// Check principal axes (configuration is non-degenerate here).
  int badax = 0;
  if (lmksss[0] < lmksss[1] || lmksss[1] < lmksss[2] || lmksss[2] < Tol) {
    badax = 1;
  } else if (
   std::fabs(lmksaxes[0] * lmksaxes[0] + lmksaxes[1] * lmksaxes[1] +
   lmksaxes[2] * lmksaxes[2] - 1.0) > Tol ||
   std::fabs(lmksaxes[3] * lmksaxes[3] + lmksaxes[4] * lmksaxes[4] +
   lmksaxes[5] * lmksaxes[5] - 1.0) > Tol ||
   std::fabs(lmksaxes[6] * lmksaxes[6] + lmksaxes[7] * lmksaxes[7] +
   lmksaxes[8] * lmksaxes[8] - 1.0) > Tol ||
   std::fabs(lmksaxes[0] * lmksaxes[3] + lmksaxes[1] * lmksaxes[4] +
   lmksaxes[2] * lmksaxes[5]) > Tol ||
   std::fabs(lmksaxes[0] * lmksaxes[6] + lmksaxes[1] * lmksaxes[7] +
   lmksaxes[2] * lmksaxes[8]) > Tol ||
   std::fabs(lmksaxes[3] * lmksaxes[6] + lmksaxes[4] * lmksaxes[7] +
   lmksaxes[5] * lmksaxes[8]) > Tol ||
   std::fabs(
   lmksaxes[0] * (lmksaxes[4] * lmksaxes[8] - lmksaxes[5] * lmksaxes[7]) +
   lmksaxes[1] * (lmksaxes[5] * lmksaxes[6] - lmksaxes[3] * lmksaxes[8]) +
   lmksaxes[2] * (lmksaxes[3] * lmksaxes[7] - lmksaxes[4] * lmksaxes[6]) -
   1.0) > Tol) {
    badax = 1;
  } else {
    double *tlmks0 = new double[D * n];
    for (int i = 0; i < n; i += 1) {
      for (int j = 0; j < D; j += 1) {
        tlmks0[D * i + j] = (lmks[D * i + j] - lmkscentroid[j]) /
         std::sqrt(lmksss[0] + lmksss[1] + lmksss[2]);
      }
    }
    double *tlmks = new double[D * n];
    for (int i = 0; i < n; i += 1) {
      for (int j = 0; j < D; j += 1) {
        tlmks[D * i + j] = tlmks0[D * i] * lmksaxes[D * j] +
         tlmks0[D * i + 1] * lmksaxes[D * j + 1] +
         tlmks0[D * i + 2] * lmksaxes[D * j + 2];
      }
    }
    for (int j = 0; j < D; j += 1) {
      double s = 0.0;
      for (int i = 0; i < n; i += 1) {
        s += tlmks[D * i + j];
      }
      if (std::fabs(s) > Tol) {
        badax = 1;
      }
    }
    for (int j = 0; j < D; j += 1) {
      for (int k = j + 1; k < D; k += 1) {
        double s = 0.0;
        for (int i = 0; i < n; i += 1) {
          s += tlmks[D * i + j] * tlmks[D * i + k];
        }
        if (std::fabs(s) > Tol) {
          badax = 1;
        }
      }
    }
    for (int j = 0; j < D; j += 1) {
      double s = 0.0;
      for (int i = 0; i < n; i += 1) {
        s += tlmks[D * i + j] * tlmks[D * i + j];
      }
      if (std::fabs(s - lmksss[j] / (lmksss[0] + lmksss[1] + lmksss[2])) >
       Tol) {
        badax = 1;
      }
    }
    delete[] tlmks0;
    delete[] tlmks;
  }
  if (badax) {
    Error("principal axes bad");
  }
// Check uniform basis.
  int baduni = 0;
  for (int sh = 0; sh < 5; sh += 1) {
    double s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0, s5 = 0.0, s6 = 0.0,
     s7 = 0.0;
    for (int i = 0; i < n; i += 1) {
      s1 += unibasis[sh * D * n + D * i];
      s2 += unibasis[sh * D * n + D * i + 1];
      s3 += unibasis[sh * D * n + D * i + 2];
      s4 += unibasis[sh * D * n + D * i] * lmks[D * i] +
       unibasis[sh * D * n + D * i + 1] * lmks[D * i + 1] +
       unibasis[sh * D * n + D * i + 2] * lmks[D * i + 2];
      s5 += unibasis[sh * D * n + D * i] * lmks[D * i + 1] -
       unibasis[sh * D * n + D * i + 1] * lmks[D * i];
      s6 += unibasis[sh * D * n + D * i] * lmks[D * i + 2] -
       unibasis[sh * D * n + D * i + 2] * lmks[D * i];
      s7 += unibasis[sh * D * n + D * i + 1] * lmks[D * i + 2] -
       unibasis[sh * D * n + D * i + 2] * lmks[D * i + 1];
    }
    if (std::fabs(s1) > Tol || std::fabs(s2) > Tol ||
     std::fabs(s3) > Tol || std::fabs(s4) > Tol ||
     std::fabs(s5) > Tol || std::fabs(s6) > Tol ||
     std::fabs(s7) > Tol) {
      baduni = 1;
    }
    double ss = 0.0;
    for (int i = 0; i < n; i += 1) {
      ss += unibasis[sh * D * n + D * i] * unibasis[sh * D * n + D * i] +
       unibasis[sh * D * n + D * i + 1] * unibasis[sh * D * n + D * i + 1] +
       unibasis[sh * D * n + D * i + 2] * unibasis[sh * D * n + D * i + 2];
    }
    if (std::fabs(ss - lmksss[0] - lmksss[1] -lmksss[2]) > Tol) {
      baduni = 1;
    }
    double *shape = new double[n * D];
    switch (sh) {
    case 0:
      for (int i = 0; i < n; i += 1) {
        double f = (lmks[D * i] * lmksaxes[3] + lmks[D * i + 1] * lmksaxes[4] +
         lmks[D * i + 2] * lmksaxes[5]);
        shape[D * i] = f * lmksaxes[0];
        shape[D * i + 1] = f * lmksaxes[1];
        shape[D * i + 2] = f * lmksaxes[2];
      }
      break;
    case 1:
      for (int i = 0; i < n; i += 1) {
        double f = (lmks[D * i] * lmksaxes[6] + lmks[D * i + 1] * lmksaxes[7] +
         lmks[D * i + 2] * lmksaxes[8]);
        shape[D * i] = f * lmksaxes[3];
        shape[D * i + 1] = f * lmksaxes[4];
        shape[D * i + 2] = f * lmksaxes[5];
      }
      break;
    case 2:
      for (int i = 0; i < n; i += 1) {
        double f = (lmks[D * i] * lmksaxes[0] + lmks[D * i + 1] * lmksaxes[1] +
         lmks[D * i + 2] * lmksaxes[2]);
        shape[D * i] = f * lmksaxes[6];
        shape[D * i + 1] = f * lmksaxes[7];
        shape[D * i + 2] = f * lmksaxes[8];
      }
      break;
    case 3:
      for (int i = 0; i < n; i += 1) {
        double f = (lmks[D * i] * lmksaxes[0] + lmks[D * i + 1] * lmksaxes[1] +
         lmks[D * i + 2] * lmksaxes[2]);
        shape[D * i] = f * lmksaxes[0];
        shape[D * i + 1] = f * lmksaxes[1];
        shape[D * i + 2] = f * lmksaxes[2];
      }
      break;
    case 4:
      for (int i = 0; i < n; i += 1) {
        double f = (lmks[D * i] * lmksaxes[3] + lmks[D * i + 1] * lmksaxes[4] +
         lmks[D * i + 2] * lmksaxes[5]) * lmksss[2];
        double g = (lmks[D * i] * lmksaxes[6] + lmks[D * i + 1] * lmksaxes[7] +
         lmks[D * i + 2] * lmksaxes[8]) * lmksss[1];
        shape[D * i] = f * lmksaxes[3] - g * lmksaxes[6];
        shape[D * i + 1] = f * lmksaxes[4] - g * lmksaxes[7];
        shape[D * i + 2] = f * lmksaxes[5] - g * lmksaxes[8];
      }
      break;
    }
// Note the naive procrustes rotation projections are not orthogonal.
// Orthogonalize them here to simplify removing procrustes fit components.
    double *r1 = new double[D * n];
    double *r2 = new double[D * n];
    double *r3 = new double[D * n];
    for (int i = 0; i < n; i += 1) {
      r1[D * i] = lmks[D * i + 1] - lmkscentroid[1];
      r1[D * i + 1] = -(lmks[D * i] - lmkscentroid[0]);
      r1[D * i + 2] = 0.0;
      r2[D * i] = lmks[D * i + 2] - lmkscentroid[2];
      r2[D * i + 1] = 0.0;
      r2[D * i + 2] = -(lmks[D * i] - lmkscentroid[0]);
      r3[D * i] = 0.0;
      r3[D * i + 1] = lmks[D * i + 2] - lmkscentroid[2];
      r3[D * i + 2] = -(lmks[D * i + 1] - lmkscentroid[1]);
    }
    double s12 = 0, s11 = 0.0, s22 = 0.0, s13 = 0.0, s23 = 0.0, s33 = 0.0;
    for (int i = 0; i < n * D; i += 1) {
      s12 += r1[i] * r2[i];
      s11 += r1[i] * r1[i];
    }
    for (int i = 0; i < n * D; i += 1) {
      r2[i] += -s12 * r1[i] / s11;
    }
    for (int i = 0; i < n * D; i += 1) {
      s13 += r1[i] * r3[i];
      s23 += r2[i] * r3[i];
      s22 += r2[i] * r2[i];
    }
    for (int i = 0; i < n * D; i += 1) {
      r3[i] += -s13 * r1[i] / s11;
      r3[i] += -s23 * r2[i] / s22;
    }
    for (int i = 0; i < n * D; i += 1) {
      s33 += r3[i] * r3[i];
    }
    s1 = s2 = s3 = s4 = s5 = s6 = s7 = 0.0;
    for (int i = 0; i < n; i += 1) {
      s1 += shape[D * i];
      s2 += shape[D * i + 1];
      s3 += shape[D * i + 2];
      s4 += shape[D * i] * (lmks[D * i] - lmkscentroid[0]) +
       shape[D * i + 1] * (lmks[D * i + 1] - lmkscentroid[1]) +
       shape[D * i + 2] * (lmks[D * i + 2] - lmkscentroid[2]);
      s5 += shape[D * i] * r1[D * i] + shape[D * i + 1] * r1[D * i + 1] +
       shape[D * i + 2] * r1[D * i + 2];
      s6 += shape[D * i] * r2[D * i] + shape[D * i + 1] * r2[D * i + 1] +
       shape[D * i + 2] * r2[D * i + 2];
      s7 += shape[D * i] * r3[D * i] + shape[D * i + 1] * r3[D * i + 1] +
       shape[D * i + 2] * r3[D * i + 2];
    }
    for (int i = 0; i < n; i += 1) {
      shape[D * i] -= s1 / n;
      shape[D * i + 1] -= s2 / n;
      shape[D * i + 2] -= s3 / n;
      shape[D * i] -= s4 * (lmks[D * i] - lmkscentroid[0]) /
       (lmksss[0] + lmksss[1] + lmksss[2]);
      shape[D * i + 1] -= s4 * (lmks[D * i + 1] - lmkscentroid[1]) /
       (lmksss[0] + lmksss[1] + lmksss[2]);
      shape[D * i + 2] -= s4 * (lmks[D * i + 2] - lmkscentroid[2]) /
       (lmksss[0] + lmksss[1] + lmksss[2]);
      shape[D * i] -= s5 * r1[D * i] / s11;
      shape[D * i + 1] -= s5 * r1[D * i + 1] / s11;
      shape[D * i + 2] -= s5 * r1[D * i + 2] / s11;
      shape[D * i] -= s6 * r2[D * i] / s22;
      shape[D * i + 1] -= s6 * r2[D * i + 1] / s22;
      shape[D * i + 2] -= s6 * r2[D * i + 2] / s22;
      shape[D * i] -= s7 * r3[D * i] / s33;
      shape[D * i + 1] -= s7 * r3[D * i + 1] / s33;
      shape[D * i + 2] -= s7 * r3[D * i + 2] / s33;
    }
    delete[] r1;
    delete[] r2;
    delete[] r3;
    ss = 0.0;
    for (int i = 0; i < n; i += 1) {
      ss += shape[D * i] * shape[D * i] + shape[D * i + 1] * shape[D * i + 1] +
       shape[D * i + 2] * shape[D * i + 2];
    }
    if (ss == 0.0) {
      baduni = 1;
    } else {
      for (int i = 0; i < n; i += 1) {
        double f = std::sqrt((lmksss[0] + lmksss[1] + lmksss[2]) / ss);
        shape[D * i] *= f;
        shape[D * i + 1] *= f;
        shape[D * i + 2] *= f;
      }
      for (int i = 0; i < n; i += 1) {
        if (std::fabs(unibasis[sh * D * n + D * i] - shape[D * i]) > Tol ||
         std::fabs(unibasis[sh * D * n + D * i + 1] - shape[D * i + 1]) >
         Tol ||
         std::fabs(unibasis[sh * D * n + D * i + 2] - shape[D * i + 2]) >
         Tol) {
          baduni = 1;
        }
      }
    }
    delete[] shape;
    for (int sh2 = sh + 1; sh2 < 5; sh2 += 1) {
      double s = 0.0;
      for (int i = 0; i < n; i += 1) {
        s += unibasis[sh * D * n + D * i] * unibasis[sh2 * D * n + D * i] +
         unibasis[sh * D * n + D * i + 1] * unibasis[sh2 * D * n + D * i + 1] +
         unibasis[sh * D * n + D * i + 2] * unibasis[sh2 * D * n + D * i + 2];
      }
      if (std::fabs(s) > Tol) {
        baduni = 1;
      }
    }
  }
  if (baduni) {
    Error("uniform basis bad");
  }
// Check landmarks map to relax_lmk_images.
  double diffml = 0;
  for (int i = 0; i < n * D; i += 1) {
    double d = std::fabs(map_lmks[i] - relax_lmk_images[i]);
    if (d > diffml) {
      diffml = d;
    }
  }
  if (diffml > Tol) {
    Error("bad map lmks %e", diffml);
  }
  delete[] map_lmks;
// Check relax_lmk_images satisfy their constraints.
  double diffrc = 0;
  for (int i = 0; i < n; i += 1) {
    if (relax_dims == 0 || relax_dims[i] == RELAX_DIM_0) {
      for (int j = 0; j < D; j += 1) {
        double d = std::fabs(relax_lmk_images[D * i + j] -
         lmk_images[D * i + j]);
        if (d > diffrc) {
          diffrc = d;
        }
      }
    } else if (relax_dims[i] == RELAX_DIM_1) {
      for (int k = 0; k < D; k += 1) {
        for (int k2 = k + 1; k2 < D; k2 += 1) {
          double d =
           relax_params[D * i + k2] *
          (relax_lmk_images[D * i + k] - lmk_images[D * i + k]) -
           relax_params[D * i + k] *
          (relax_lmk_images[D * i + k2] - lmk_images[D * i + k2]);
          if (d > diffrc) {
            diffrc = d;
          }
        }
      }
    } else if (relax_dims[i] == RELAX_DIM_D - 1) {
      double s = 0.0;
      for (int k = 0; k < D; k += 1) {
        s += relax_params[D * i + k] *
         (relax_lmk_images[D * i + k] - lmk_images[D * i + k]);
      }
      double d = std::fabs(s);
      if (d > diffrc) {
        diffrc = d;
      }
    }
  }
  if (diffrc > Tol) {
    Error("misplaced relax_lmk_images %e", diffrc);
  }
// Check spline is orthogonal to constraints.
  double diffroc = 0;
  for (int i = 0; i < n; i += 1) {
    if (relax_dims != 0 && relax_dims[i] != RELAX_DIM_0) {
      if (relax_dims[i] == RELAX_DIM_D) {
        for (int j = 0; j < D; j += 1) {
          double d = std::fabs(spline[D * (i + D1) + j]);
          if (d > diffrc) {
            diffrc = d;
          }
        }
      } else if (relax_dims[i] == RELAX_DIM_1) {
        double s = 0.0;
        for (int k = 0; k < D; k += 1) {
          s += relax_params[D * i + k] * spline[D * (i + D1) + k];
        }
        double d = std::fabs(s);
        if (d > diffrc) {
          diffrc = d;
        }
      } else if (relax_dims[i] == RELAX_DIM_D - 1) {
        for (int k = 0; k < D; k += 1) {
          for (int k2 = k + 1; k2 < D; k2 += 1) {
            double d =
             relax_params[D * i + k2] * spline[D * (i + D1) + k] -
             relax_params[D * i + k] * spline[D * (i + D1) + k2];
            if (d > diffrc) {
              diffrc = d;
            }
          }
        }
      }
    }
  }
  if (diffroc > Tol) {
    Error("misaligned relaxed spline %e", diffroc);
  }
  if (is_reduced) {
// Check energy minimized.
    bool is_mixed1, is_reduced1;
    int t_size1;
    ew::Tps3::algebraic_size(&t_size1, &is_mixed1, &is_reduced1, 0, n);
    double *T1 = new double[t_size1 * t_size1];
    int *pivots1 = new int[t_size1];
    double is_nonsingular1;
    ew::Tps3::factorize(T1, pivots1, &is_nonsingular1, L, 0, 0, t_size1,
     is_mixed1, is_reduced1, n);
    if (is_nonsingular1 < Tol) {
      Error("bad minimized nonsing %e", is_nonsingular1);
    } else {
      double *spline1 = new double[m * D];
      double *lmk_images1 = new double[n * D];
      double energy1;
      for (int i = 0; i < D * n; i += 1) {
        lmk_images1[i] = relax_lmk_images[i];
      }
      ew::Tps3::solve(spline1, &energy1, 0, T1, pivots1, lmk_images1,
       0, 0, L, n, t_size1, is_mixed1, is_reduced1);
      if (std::fabs(energy - energy1) > Tol) {
        Error("bad minimized energy %e", energy1);
      } else {
        double diffe = -1.0;
        for (int i = 0; i < n; i += 1) {
          if (relax_dims[i] == RELAX_DIM_D) {
            for (int j = 0; j < 2 * D; j += 1) {
              for (int k = 0; k < D; k += 1) {
                if (j == 2 * k) {
                  lmk_images1[D * i + k] = relax_lmk_images[D * i + k] + 0.01;
                } else if (j == 2 * k + 1) {
                  lmk_images1[D * i + k] = relax_lmk_images[D * i + k] - 0.01;
                } else {
                  lmk_images1[D * i + k] = relax_lmk_images[D * i + k];
                }
              }
              ew::Tps3::solve(spline1, &energy1, 0, T1, pivots1, lmk_images1,
               0, 0, L, n, t_size1, is_mixed1, is_reduced1);
              double d = (energy1 - energy) * 10000.0;
              if (diffe == -1.0 || d < diffe) {
                diffe = d;
              }
              for (int k = 0; k < D; k += 1) {
                lmk_images1[D * i + k] = relax_lmk_images[D * i + k];
              }
            }
          } else if (relax_dims[i] == RELAX_DIM_1) {
            for (int j = 0; j < 2; j += 1) {
              for (int k = 0; k < D; k += 1) {
                if (j == 0) {
                  lmk_images1[D * i + k] = relax_lmk_images[D * i + k] +
                   0.01 * relax_params[D * i + k];
                } else {
                  lmk_images1[D * i + k] = relax_lmk_images[D * i + k] -
                   0.01 * relax_params[D * i + k];
                }
              }
              ew::Tps3::solve(spline1, &energy1, 0, T1, pivots1, lmk_images1,
               0, 0, L, n, t_size1, is_mixed1, is_reduced1);
              double d = (energy1 - energy) * 10000.0;
              if (diffe == -1.0 || d < diffe) {
                diffe = d;
              }
              for (int k = 0; k < D; k += 1) {
                lmk_images1[D * i + k] = relax_lmk_images[D * i + k];
              }
            }
          } else if (relax_dims[i] == RELAX_DIM_2) {
            double perp[2][D];
            if (std::fabs(relax_params[D * i]) >
             std::fabs(relax_params[D * i + 1]) &&
             std::fabs(relax_params[D * i]) >
             std::fabs(relax_params[D * i + 2])) {
              perp[0][0] = -relax_params[D * i + 1];
              perp[0][1] = relax_params[D * i];
              perp[0][2] = 0.0;
              perp[1][0] = -relax_params[D * i + 2];
              perp[1][1] = 0.0;
              perp[1][2] = relax_params[D * i];
            } else if (std::fabs(relax_params[D * i + 1]) >
             std::fabs(relax_params[D * i + 2])) {
              perp[0][0] = relax_params[D * i + 1];
              perp[0][1] = -relax_params[D * i];
              perp[0][2] = 0.0;
              perp[1][0] = 0.0;
              perp[1][1] = -relax_params[D * i + 2];
              perp[1][2] = relax_params[D * i + 1];
            } else {
              perp[0][0] = relax_params[D * i + 2];
              perp[0][1] = 0.0;
              perp[0][2] = -relax_params[D * i];
              perp[1][0] = 0.0;
              perp[1][1] = relax_params[D * i + 2];
              perp[1][2] = -relax_params[D * i + 1];
            }
            for (int j = 0; j < 4; j += 1) {
              for (int k = 0; k < D; k += 1) {
                if (j % 2 == 0) {
                  lmk_images1[D * i + k] = relax_lmk_images[D * i + k] +
                   0.01 * perp[j/2][k];
                } else {
                  lmk_images1[D * i + k] = relax_lmk_images[D * i + k] -
                   0.01 * perp[j/2][k];
                }
              }
              ew::Tps3::solve(spline1, &energy1, 0, T1, pivots1, lmk_images1,
               0, 0, L, n, t_size1, is_mixed1, is_reduced1);
              double d = (energy1 - energy) * 10000.0;
              if (diffe == -1.0 || d < diffe) {
                diffe = d;
              }
              for (int k = 0; k < D; k += 1) {
                lmk_images1[D * i + k] = relax_lmk_images[D * i + k];
              }
            }
          }
        }
        if (diffe < TolMinimize) {
          Error("bad minimized %e", diffe);
        }
      }
      delete[] lmk_images1;
      delete[] spline1;
    }
    delete[] T1;
    delete[] pivots1;
  }
// Check spline is continuous.
  double *pts = new double[D * 100001];
  double *ipts = new double[D * 100001];
  double p0[D], p1[D], ss = 0.0;
  for (int k = 0; k < D; k += 1) {
    p0[k] = lmks[(n / 2) * D + k];
    if (k == 0) {
      p1[k] = 50.0;
    } else {
      p1[k] = 0.0;
    }
    double dp = p1[k] - p0[k];
    ss += dp * dp;
  }
  double dd = std::sqrt(ss);
  for (int i = 0; i <= 100000; i += 1) {
    for (int k = 0; k < D; k += 1) {
      pts[D * i + k] = p0[k] * i / 100000.0 + p1[k] * (100000 - i) / 100000.0;
    }
  }
  ew::Tps3::map(ipts, pts, 100001, spline, lmks, n);
  double diffc = 0.0;
  for (int i = 0; i < 100000; i += 1) {
    ss = 0.0;
    for (int k = 0; k < D; k += 1) {
      double dp = ipts[D * i + k] - ipts[D * i + D + k];
      ss += dp * dp;
    }
    double d = 100000.0 * sqrt(ss) / dd;
    if (d > diffc) {
      diffc = d;
    }
  }
  if (diffc > TolContinuous) {
    Error("bad continuous %e", diffc);
  }
  delete[] pts;
  delete[] ipts;
  if (relax_dims) {
// Check leaving out fully relaxed lmks changes nothing.
    double *lmks1 = new double[D * n];
    int *relax_dims1 = new int[n];
    double *relax_params1 = new double[D * n];
    double *lmk_images1 = new double[D * n];
    int n1 = 0;
    for (int i = 0; i < n; i += 1) {
      if (relax_dims[i] != RELAX_DIM_D) {
        for (int k = 0; k < D; k += 1) {
          lmks1[n1 * D + k] = lmks[i * D + k];
        }
        relax_dims1[n1] = relax_dims[i];
        if (relax_params) {
          for (int k = 0; k < D; k += 1) {
            relax_params1[n1 * D + k] = relax_params[i * D + k];
          }
        }
        for (int k = 0; k < D; k += 1) {
          lmk_images1[n1 * D + k] = lmk_images[i * D + k];
        }
        n1 += 1;
      }
    }
    int m1 = n1 + D1;
    double *L1 = new double[m1 * m1];
    ew::Tps3::interaction(L1, lmks1, n1);
    bool is_mixed1, is_reduced1;
    int t_size1;
    ew::Tps3::algebraic_size(&t_size1, &is_mixed1, &is_reduced1, relax_dims1,
     n1);
    double *T1 = new double[t_size1 * t_size1];
    int *pivots1 = new int[t_size1];
    double is_nonsingular1;
    ew::Tps3::factorize(T1, pivots1, &is_nonsingular1, L1, relax_dims1,
     relax_params1, t_size1, is_mixed1, is_reduced1, n1);
    if (is_nonsingular1 < Tol) {
      Error("bad relaxed equivalent nonsingular %e", is_nonsingular1);
    } else {
      double diffre = 0.0;
      double *spline1 = new double[m1 * D];
      double energy1;
      ew::Tps3::solve(spline1, &energy1, 0, T1, pivots1, lmk_images1,
       relax_dims1, relax_params1, L1, n1, t_size1, is_mixed1, is_reduced1);
      double d = std::fabs(energy - energy1);
      if (d > Tol) {
        Error("bad relaxed equivalent energy %e", d);
      } else {
        m1 = 0;
        for (int i = 0; i < m; i += 1) {
          if (i < D1 || relax_dims[i - D1] != RELAX_DIM_D) {
            for (int k = 0; k < D; k += 1) {
              double d = std::fabs(spline1[m1 * D + k] - spline[i * D + k]);
              if (d > diffre) {
                diffre = d;
              }
            }
            m1 += 1;
          }
        }
        delete[] spline1;
        if (diffre > Tol) {
          Error("bad relaxed equivalent %e", diffre);
        }
      }
    }
    delete[] L1;
    delete[] T1;
    delete[] pivots1;
    delete[] lmks1;
    delete[] relax_dims1;
    delete[] relax_params1;
    delete[] lmk_images1;
  }
  if (points != 0) {
// Check cached evaluation gives roughly the same result.
    point_images = new double[D * l];
    float **cache = new float*[n];
    for (int i = 0; i < n; i += 1) {
      cache[i] = new float[l];
    }
    float *pointisf = new float[D * l];
    float *pointsf = new float[D * l];
    for (int i = 0; i < D * l; i += 1) {
      pointsf[i] = float(points[i]);
    }
    double t0, t1, t2, t3;
    if (dbg.get_active("timing")) {
      t0 = ew::Time::elapsed();
    }
    ew::Tps3::map(point_images, points, l, spline, lmks, n);
    if (dbg.get_active("timing")) {
      t1 = ew::Time::elapsed();
    }
    for (int i = 0; i < n; i += 1) {
      ew::Tps3::cache(cache[i], pointsf, l, lmks + D * i);
    }
    if (dbg.get_active("timing")) {
      t2 = ew::Time::elapsed();
    }
    ew::Tps3::cache_map(pointisf, pointsf, cache, l, spline, n);
    if (dbg.get_active("timing")) {
      t3 = ew::Time::elapsed();
      dbg.dprintf("%s map %.6f cache %.6f cache_map %.6f", "timing", t1 - t0,
       t2 - t1, t3 - t2);
    }
    double diffcm = 0.0;
    for (int i = 0; i < D * l; i += 1) {
      double d = std::fabs(pointisf[i] - point_images[i]);
      if (d > diffcm) {
        diffcm = d;
      }
    }
    if (diffcm > TolCacheMap) {
      Error("cache map discrepancy %e", diffcm);
    }
    for (int i = 0; i < n; i += 1) {
      delete[] cache[i];
    }
    delete[] pointisf;
    delete[] cache;
  }
}

test::~test()
{
  delete[] L;
  delete[] T;
  delete[] E;
  delete[] P;
  delete[] ev;
  delete[] unibasis;
  delete[] pivots;
  delete[] spline;
  delete[] relax_lmk_images;
  delete[] point_images;
}

enum {UNREDUCED, UNMIXED, MIXED};

void
test::check_rtype(int exp)
{
  if (
   (exp == UNREDUCED && (is_reduced != 0 || is_mixed != 0)) ||
   (exp == UNMIXED && (is_reduced != 1 || is_mixed != 0)) ||
   (exp == MIXED && (is_reduced != 1 || is_mixed != 1))) {
    Error("bad rtype");
  }
}

enum {SINGULAR, NONSINGULAR};

void
test::check_singular(int exp)
{
  if (
   (exp == SINGULAR && (is_nonsingular < 0.0 || is_nonsingular > Tol)) ||
   (exp == NONSINGULAR && is_nonsingular <= Tol)) {
    Error("bad singular %e", is_nonsingular);
  }
}

enum {IDENTITY, AFFINE, NOTAFFINE};

void
test::check_map(int exp, double *a)
{
  if (exp == IDENTITY) {
    if (energy > Tol) {
      Error("bad identity energy %e", energy);
    }
    double diff = 0.0;
    for (int i = 0; i < DD1; i += 1) {
      double d;
      if ((i + 1) % D1 == 0) {
        d = std::fabs(spline[i] - 1.0);
      } else {
        d = std::fabs(spline[i]);
      }
      if (d > diff) {
        diff = d;
      }
    }
    for (int i = DD1; i < m * D; i += 1) {
      double d = std::fabs(spline[i]);
      if (d > diff) {
        diff = d;
      }
    }
    if (diff > Tol) {
      Error("bad identity spline %e", diff);
    }
    diff = 0.0;
    if (points) {
      for (int i = 0; i < l * D; i += 1) {
        double d = std::fabs(point_images[i] - points[i]);
        if (d > diff) {
          diff = d;
        }
      }
      if (diff > Tol) {
        Error("bad identity points %e", diff);
      }
    }
  }
  if (exp == AFFINE) {
    if (energy > Tol) {
      Error("bad affine energy %e", energy);
    }
    double diff = 0.0;
    for (int i = 0; i < DD1; i += 1) {
      double d = std::fabs(spline[i] - a[i]);
      if (d > diff) {
        diff = d;
      }
    }
    for (int i = DD1; i < m * D; i += 1) {
      double d = std::fabs(spline[i]);
      if (d > diff) {
        diff = d;
      }
    }
    if (diff > Tol) {
      Error("bad affine spline %e", diff);
    }
    diff = 0.0;
    if (points) {
      for (int i = 0; i < l; i += 1) {
        for (int k = 0; k < D; k += 1) {
          double d = std::fabs(point_images[D * i + k] - a[k] -
           a[D + k] * points[D * i] - a[2 * D + k] * points[D * i + 1] -
           a[3 * D + k] * points[D * i + 2]);
          if (d > diff) {
            diff = d;
          }
        }
      }
      if (diff > Tol) {
        Error("bad affine points %e", diff);
      }
    }
  }
  if (exp == NOTAFFINE) {
    if (energy < Tol) {
      Error("bad notaffine %e", energy);
    }
  }
}

static const char Name01[] = "too few lmks";

static void
Test01()
{
  double lmks[] = {
    1.2, 3.4, 4.3,
    4.2, 0.3, 2.1,
    2.2, 1.3, 5.1
  };
  test t(3, lmks, 0, 0, 0, 0, 0);
  t.check_rtype(UNREDUCED);
  t.check_singular(SINGULAR);
}

static const char Name02[] = "few equal lmks";

static void
Test02()
{
  double lmks[] = {
    1.2, 3.4, 4.3,
    4.2, 0.3, 2.1,
    2.2, 1.3, 5.1,
    4.1, 0.5, 0.5,
    4.2, 0.3, 2.1
  };
  test t(5, lmks, 0, 0, 0, 0, 0);
  t.check_rtype(UNREDUCED);
  t.check_singular(SINGULAR);
}

static const char Name03[] = "many equal lmks";

static void
Test03()
{
  double lmks[D * 125], image_lmks[D * 125];
  for (int i = 0; i < 5; i += 1) {
    for (int j = 0; j < 5; j += 1) {
      for (int l = 0; l < 5; l += 1) {
        lmks[(i * 25 + j * 5 + l) * D] =
         image_lmks[(i * 25 + j * 5 + l) * D] =
         i + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 1] =
         image_lmks[(i * 25 + j * 5 + l) * D + 1] =
         j + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 2] =
         image_lmks[(i * 25 + j * 5 + l) * D + 2] =
         l + MyRand() * 0.8;
      }
    }
  }
  lmks[150] = lmks[60];
  lmks[151] = lmks[61];
  lmks[152] = lmks[62];
  test t(125, lmks, 0, 0, image_lmks, 0, 0);
  t.check_rtype(UNREDUCED);
  t.check_singular(SINGULAR);
}

static const char Name04[] = "few close lmks";

static void
Test04()
{
  double lmks[] = {
    1.2, 3.4, 4.3,
    1.1e-30, 0.3, 2.1,
    2.2, 1.3, 5.1,
    4.1, 0.5, 0.5,
    1.2e-30, 0.3, 2.1
  };
  test t(5, lmks, 0, 0, 0, 0, 0);
  t.check_rtype(UNREDUCED);
  t.check_singular(SINGULAR);
}

static const char Name05[] = "many close lmks";

static void
Test05()
{
  double lmks[D * 125], image_lmks[D * 125];
  for (int i = 0; i < 5; i += 1) {
    for (int j = 0; j < 5; j += 1) {
      for (int l = 0; l < 5; l += 1) {
        lmks[(i * 25 + j * 5 + l) * D] =
         image_lmks[(i * 25 + j * 5 + l) * D] =
         i + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 1] =
         image_lmks[(i * 25 + j * 5 + l) * D + 1] =
         j + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 2] =
         image_lmks[(i * 25 + j * 5 + l) * D + 2] =
         l + MyRand() * 0.8;
      }
    }
  }
  lmks[60] = 1.1e-30;
  lmks[61] = 3.4;
  lmks[62] = 3.4;
  lmks[150] = 1.2e-30;
  lmks[151] = 3.4;
  lmks[152] = 3.4;
  test t(125, lmks, 0, 0, image_lmks, 0, 0);
  t.check_rtype(UNREDUCED);
  t.check_singular(SINGULAR);
}

static const char Name06[] = "few lmks identity";

static void
Test06()
{
  double lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.5, 1.5
  };
  double image_lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.5, 1.5
  };
  double points[] = {
    1.2, 1.2, 0.4,
    9.9, 0.0, 0.0,
    0.0, 9.9, 0.0,
    1.01, 1.01, 1.01
  };
  test t(9, lmks, 0, 0, image_lmks, 4, points);
  t.check_rtype(UNREDUCED);
  t.check_singular(NONSINGULAR);
  t.check_map(IDENTITY);
}

static const char Name07[] = "many lmks identity";

static void
Test07()
{
  double lmks[D * 125], image_lmks[D * 125], *points;
  for (int i = 0; i < 5; i += 1) {
    for (int j = 0; j < 5; j += 1) {
      for (int l = 0; l < 5; l += 1) {
        lmks[(i * 25 + j * 5 + l) * D] =
         image_lmks[(i * 25 + j * 5 + l) * D] =
         i + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 1] =
         image_lmks[(i * 25 + j * 5 + l) * D + 1] =
         j + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 2] =
         image_lmks[(i * 25 + j * 5 + l) * D + 2] =
         l + MyRand() * 0.8;
      }
    }
  }
  points = new double[D * 2000];
  for (int i = 0; i < D * 2000; i += 1) {
    points[i] = 20 * MyRand();
  }
  test t(125, lmks, 0, 0, image_lmks, 2000, points);
  t.check_rtype(UNREDUCED);
  t.check_singular(NONSINGULAR);
  t.check_map(IDENTITY);
  delete[] points;
}

static const char Name08[] = "few lmks affine";

static void
Test08()
{
  double a[]= {
    1.2, 1.5, 0.8, 0.9, 0.8, 0.7,
    0.2, 0.5, 1.8, 1.9, 1.8, 1.7
  };
  double lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.5, 1.5
  };
  double image_lmks[D * 9];
  for (int i = 0; i < 9; i += 1) {
    for (int k = 0; k < D; k += 1) {
      image_lmks[D * i + k] =
       a[k] + lmks[D * i] * a[D + k] + lmks[D * i + 1] * a[2 * D + k] +
       lmks[D * i + 2] * a[3 * D + k];
    }
  }
  double points[] = {
    1.2, 1.2, 0.4,
    9.9, 0.0, 0.0,
    0.0, 9.9, 0.0,
    1.01, 1.01, 1.01
  };
  test t(9, lmks, 0, 0, image_lmks, 4, points);
  t.check_rtype(UNREDUCED);
  t.check_singular(NONSINGULAR);
  t.check_map(AFFINE, a);
}

static const char Name09[] = "many lmks affine";

static void
Test09()
{
  double a[]= {
    1.2, 1.5, 0.8, 0.9, 0.8, 0.7,
    0.2, 0.5, 1.8, 1.9, 1.8, 1.7
  };
  double lmks[D * 125], image_lmks[D * 125], *points;
  for (int i = 0; i < 5; i += 1) {
    for (int j = 0; j < 5; j += 1) {
      for (int l = 0; l < 5; l += 1) {
        lmks[(i * 25 + j * 5 + l) * D] =
         i + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 1] =
         j + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 2] =
         l + MyRand() * 0.8;
      }
    }
  }
  for (int i = 0; i < 125; i += 1) {
    for (int k = 0; k < D; k += 1) {
      image_lmks[D * i + k] =
       a[k] + lmks[D * i] * a[D + k] + lmks[D * i + 1] * a[2 * D + k] +
       lmks[D * i + 2] * a[3 * D + k];
    }
  }
  points = new double[D * 2000];
  for (int i = 0; i < D * 2000; i += 1) {
    points[i] = 20 * MyRand();
  }
  test t(125, lmks, 0, 0, image_lmks, 2000, points);
  t.check_rtype(UNREDUCED);
  t.check_singular(NONSINGULAR);
  t.check_map(AFFINE, a);
  delete[] points;
}

static const char Name10[] = "few lmks not affine";

static void
Test10()
{
  double a[]= {
    1.2, 1.5, 0.8, 0.9, 0.8, 0.7,
    0.2, 0.5, 1.8, 1.9, 1.8, 1.7
  };
  double lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.5, 1.5
  };
  double image_lmks[D * 9];
  for (int i = 0; i < 9; i += 1) {
    for (int k = 0; k < D; k += 1) {
      image_lmks[D * i + k] =
       a[k] + lmks[D * i] * a[D + k] + lmks[D * i + 1] * a[2 * D + k] +
       lmks[D * i + 2] * a[3 * D + k];
    }
  }
  image_lmks[D * 7] += 0.1;
  image_lmks[D * 9 - 1] += 1.0;
  double points[] = {
    1.2, 1.2, 0.4,
    9.9, 0.0, 0.0,
    0.0, 9.9, 0.0,
    1.01, 1.01, 1.01
  };
  test t(9, lmks, 0, 0, image_lmks, 4, points);
  t.check_rtype(UNREDUCED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
}

static const char Name11[] = "many lmks not affine";

static void
Test11()
{
  double a[]= {
    1.2, 1.5, 0.8, 0.9, 0.8, 0.7,
    0.2, 0.5, 1.8, 1.9, 1.8, 1.7
  };
  double lmks[D * 125], image_lmks[D * 125], *points;
  for (int i = 0; i < 5; i += 1) {
    for (int j = 0; j < 5; j += 1) {
      for (int l = 0; l < 5; l += 1) {
        lmks[(i * 25 + j * 5 + l) * D] =
         i + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 1] =
         j + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 2] =
         l + MyRand() * 0.8;
      }
    }
  }
  for (int i = 0; i < 125; i += 1) {
    for (int k = 0; k < D; k += 1) {
      image_lmks[D * i + k] =
       a[k] + lmks[D * i] * a[D + k] + lmks[D * i + 1] * a[2 * D + k] +
       lmks[D * i + 2] * a[3 * D + k];
    }
  }
  for (int i = D * 10; i < D * 125; i += 1) {
    image_lmks[i] += MyRand() * 0.5;
  }
  points = new double[D * 20000];
  for (int i = 0; i < D * 20000; i += 1) {
    points[i] = 20 * MyRand();
  }
  test t(125, lmks, 0, 0, image_lmks, 20000, points);
  t.check_rtype(UNREDUCED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
  delete[] points;
}

static const char Name12[] = "cf vtk";

static void
Test12()
{
  double lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.5, 1.5
  };
  double image_lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.8, 1.5
  };
  double points[] = {
    1.6, 1.5, 1.5,
    1.7, 1.5, 1.5,
    1.8, 1.5, 1.5,
    1.9, 1.5, 1.5
  };
  double exppts[] = {
    1.6, 1.75273, 1.5,
    1.7, 1.70931, 1.5,
    1.8, 1.66984, 1.5,
    1.9, 1.63448, 1.5
  };
  test t(9, lmks, 0, 0, image_lmks, 4, points);
  t.check_rtype(UNREDUCED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
// Compare with vtk results (cf script *.vtk).
  double diff = 0.0;
  for (int k = 0; k < D * 4; k += 1) {
    double d = std::fabs(t.point_images[k] - exppts[k]);
    if (d > diff) {
      diff = d;
    }
  }
  if (diff > TolVtk) {
    Error("bad cf %e", diff);
  }
}

static const char Name13[] = "fully relaxed";

static void
Test13()
{
  double lmks[] = {
    1.2, 3.4, 4.3,
    4.2, 0.3, 2.1,
    2.2, 1.3, 5.1,
    4.1, 0.5, 0.5,
  };
  int rdims[] = {RELAX_DIM_D, RELAX_DIM_D, RELAX_DIM_D, RELAX_DIM_D};
  test t(4, lmks, rdims, 0, 0, 0, 0);
  t.check_rtype(UNMIXED);
  t.check_singular(SINGULAR);
}

static const char Name14[] = "almost fully relaxed";

static void
Test14()
{
  double lmks[] = {
    1.2, 3.4, 4.3,
    4.2, 0.3, 2.1,
    2.2, 1.3, 5.1,
    4.1, 0.5, 0.5,
  };
  int rdims[] = {RELAX_DIM_D, RELAX_DIM_D, RELAX_DIM_D, RELAX_DIM_D - 1};
  double rparams[] = {
    1.0, 0.0,
    1.0, 0.0,
    1.0, 0.0,
    1.0, 0.0
  };
  test t(4, lmks, rdims, rparams, 0, 0, 0);
  t.check_rtype(MIXED);
  t.check_singular(SINGULAR);
}

static const char Name15[] = "few lmks reduced";

static void
Test15()
{
  double lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.5, 1.5
  };
  double image_lmks[] = {
    9.0, 9.0, 9.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.8, 1.5
  };
  int rdims[] = {RELAX_DIM_D, 0, RELAX_DIM_D, 0, 0, 0, 0, 0, 0};
  test t(9, lmks, rdims, 0, image_lmks, 0, 0);
  t.check_rtype(UNMIXED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
}

static const char Name16[] = "many lmks reduced";

static void
Test16()
{
  double a[]= {
    1.2, 1.5, 0.8, 0.9, 0.8, 0.7,
    0.2, 0.5, 1.8, 1.9, 1.8, 1.7
  };
  double lmks[D * 125], image_lmks[D * 125];
  int rdims[125];
  for (int i = 0; i < 5; i += 1) {
    for (int j = 0; j < 5; j += 1) {
      for (int l = 0; l < 5; l += 1) {
        lmks[(i * 25 + j * 5 + l) * D] =
         i + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 1] =
         j + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 2] =
         l + MyRand() * 0.8;
      }
    }
  }
  for (int i = 0; i < 125; i += 1) {
    for (int k = 0; k < D; k += 1) {
      image_lmks[D * i + k] =
       a[k] + lmks[D * i] * a[D + k] + lmks[D * i + 1] * a[2 * D + k] +
       lmks[D * i + 2] * a[3 * D + k];
    }
    rdims[i] = RELAX_DIM_0;
  }
  for (int i = D * 10; i < D * 125; i += 1) {
    image_lmks[i] += MyRand() * 0.5;
  }
  for (int i = 10; i < 125; i += 1) {
    if (MyRand() > 1.0 / 2) {
      rdims[i] = RELAX_DIM_D;
    }
  }
  test t(125, lmks, rdims, 0, image_lmks, 0, 0);
  t.check_rtype(UNMIXED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
}

static const char Name17[] = "few lmks semi-line";

static void
Test17()
{
  double lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.5, 1.5
  };
  double image_lmks[] = {
    9.0, 9.0, 9.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.8, 1.5
  };
  int rdims[] = {RELAX_DIM_D, 0, RELAX_DIM_1, 0, 0, 0, 0, 0, RELAX_DIM_1};
  double rparams[] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.6, 0.8, 0.0,
  };
  test t(9, lmks, rdims, rparams, image_lmks, 0, 0);
  t.check_rtype(MIXED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
}

static const char Name18[] = "few lmks semi-plane";

static void
Test18()
{
  double lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.5, 1.5
  };
  double image_lmks[] = {
    9.0, 9.0, 9.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.8, 1.5
  };
  int rdims[] = {RELAX_DIM_D, 0, RELAX_DIM_2, 0, 0, 0, 0, 0, RELAX_DIM_2};
  double rparams[] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.6, 0.8, 0.0,
  };
  test t(9, lmks, rdims, rparams, image_lmks, 0, 0);
  t.check_rtype(MIXED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
}

static const char Name19[] = "few lmks mixed";

static void
Test19()
{
  double lmks[] = {
    1.0, 1.0, 1.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.5, 1.5
  };
  double image_lmks[] = {
    9.0, 9.0, 9.0,
    2.0, 1.0, 1.0,
    1.0, 2.0, 1.0,
    2.0, 2.0, 1.0,
    1.0, 1.0, 2.0,
    2.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 2.0, 2.0,
    1.5, 1.8, 1.5
  };
  int rdims[] = {RELAX_DIM_D, 0, RELAX_DIM_1, 0, 0, 0, 0, 0, RELAX_DIM_2};
  double rparams[] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.6, 0.8, 0.0,
  };
  test t(9, lmks, rdims, rparams, image_lmks, 0, 0);
  t.check_rtype(MIXED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
}

static const char Name20[] = "many lmks semi-line";

static void
Test20()
{
  double a[]= {
    1.2, 1.5, 0.8, 0.9, 0.8, 0.7,
    0.2, 0.5, 1.8, 1.9, 1.8, 1.7
  };
  double lmks[D * 125], image_lmks[D * 125], rparams[D * 125];
  int rdims[125];
  for (int i = 0; i < 5; i += 1) {
    for (int j = 0; j < 5; j += 1) {
      for (int l = 0; l < 5; l += 1) {
        lmks[(i * 25 + j * 5 + l) * D] =
         i + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 1] =
         j + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 2] =
         l + MyRand() * 0.8;
      }
    }
  }
  for (int i = 0; i < 125; i += 1) {
    for (int k = 0; k < D; k += 1) {
      image_lmks[D * i + k] =
       a[k] + lmks[D * i] * a[D + k] + lmks[D * i + 1] * a[2 * D + k] +
       lmks[D * i + 2] * a[3 * D + k];
    }
    rdims[i] = RELAX_DIM_0;
    double th = MyRand() * 3.0;
    double th2 = MyRand() * 3.0;
    rparams[D * i] = std::sin(th) * std::sin(th2);
    rparams[D * i + 1] = std::sin(th) * std::cos(th2);
    rparams[D * i + 2] = std::cos(th);
  }
  for (int i = D * 10; i < D * 125; i += 1) {
    image_lmks[i] += MyRand() * 0.5;
  }
  for (int i = 10; i < 125; i += 1) {
    if (MyRand() < 1.0 / 3) {
      rdims[i] = RELAX_DIM_1;
    } else if (MyRand() < 1.0 / 2) {
      rdims[i] = RELAX_DIM_D;
    }
  }
  test t(125, lmks, rdims, rparams, image_lmks, 0, 0);
  t.check_rtype(MIXED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
}

static const char Name21[] = "many lmks semi-plane";

static void
Test21()
{
  double a[]= {
    1.2, 1.5, 0.8, 0.9, 0.8, 0.7,
    0.2, 0.5, 1.8, 1.9, 1.8, 1.7
  };
  double lmks[D * 125], image_lmks[D * 125], rparams[D * 125];
  int rdims[125];
  for (int i = 0; i < 5; i += 1) {
    for (int j = 0; j < 5; j += 1) {
      for (int l = 0; l < 5; l += 1) {
        lmks[(i * 25 + j * 5 + l) * D] =
         i + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 1] =
         j + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 2] =
         l + MyRand() * 0.8;
      }
    }
  }
  for (int i = 0; i < 125; i += 1) {
    for (int k = 0; k < D; k += 1) {
      image_lmks[D * i + k] =
       a[k] + lmks[D * i] * a[D + k] + lmks[D * i + 1] * a[2 * D + k] +
       lmks[D * i + 2] * a[3 * D + k];
    }
    rdims[i] = RELAX_DIM_0;
    double th = MyRand() * 3.0;
    double th2 = MyRand() * 3.0;
    rparams[D * i] = std::sin(th) * std::sin(th2);
    rparams[D * i + 1] = std::sin(th) * std::cos(th2);
    rparams[D * i + 2] = std::cos(th);
  }
  for (int i = D * 10; i < D * 125; i += 1) {
    image_lmks[i] += MyRand() * 0.5;
  }
  for (int i = 10; i < 125; i += 1) {
    if (MyRand() < 1.0 / 3) {
      rdims[i] = RELAX_DIM_2;
    } else if (MyRand() < 1.0 / 2) {
      rdims[i] = RELAX_DIM_D;
    }
  }
  test t(125, lmks, rdims, rparams, image_lmks, 0, 0);
  t.check_rtype(MIXED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
}

static const char Name22[] = "many lmks mixed";

static void
Test22()
{
  double a[]= {
    1.2, 1.5, 0.8, 0.9, 0.8, 0.7,
    0.2, 0.5, 1.8, 1.9, 1.8, 1.7
  };
  double lmks[D * 125], image_lmks[D * 125], rparams[D * 125];
  int rdims[125];
  for (int i = 0; i < 5; i += 1) {
    for (int j = 0; j < 5; j += 1) {
      for (int l = 0; l < 5; l += 1) {
        lmks[(i * 25 + j * 5 + l) * D] =
         i + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 1] =
         j + MyRand() * 0.8;
        lmks[(i * 25 + j * 5 + l) * D + 2] =
         l + MyRand() * 0.8;
      }
    }
  }
  for (int i = 0; i < 125; i += 1) {
    for (int k = 0; k < D; k += 1) {
      image_lmks[D * i + k] =
       a[k] + lmks[D * i] * a[D + k] + lmks[D * i + 1] * a[2 * D + k] +
       lmks[D * i + 2] * a[3 * D + k];
    }
    rdims[i] = RELAX_DIM_0;
    double th = MyRand() * 3.0;
    double th2 = MyRand() * 3.0;
    rparams[D * i] = std::sin(th) * std::sin(th2);
    rparams[D * i + 1] = std::sin(th) * std::cos(th2);
    rparams[D * i + 2] = std::cos(th);
  }
  for (int i = D * 10; i < D * 125; i += 1) {
    image_lmks[i] += MyRand() * 0.5;
  }
  for (int i = 10; i < 125; i += 1) {
    if (MyRand() < 1.0 / 4) {
      rdims[i] = RELAX_DIM_1;
    } else if (MyRand() < 1.0 / 3) {
      rdims[i] = RELAX_DIM_2;
    } else if (MyRand() < 1.0 / 2) {
      rdims[i] = RELAX_DIM_D;
    }
  }
  test t(125, lmks, rdims, rparams, image_lmks, 0, 0);
  t.check_rtype(MIXED);
  t.check_singular(NONSINGULAR);
  t.check_map(NOTAFFINE);
}

static test_t Tests[] = {
  {Name01, &Test01},
  {Name02, &Test02},
  {Name03, &Test03},
  {Name04, &Test04},
  {Name05, &Test05},
  {Name06, &Test06},
  {Name07, &Test07},
  {Name08, &Test08},
  {Name09, &Test09},
  {Name10, &Test10},
  {Name11, &Test11},
  {Name12, &Test12},
  {Name13, &Test13},
  {Name14, &Test14},
  {Name15, &Test15},
  {Name16, &Test16},
  {Name17, &Test17},
  {Name18, &Test18},
  {Name19, &Test19},
  {Name20, &Test20},
  {Name21, &Test21},
  {Name22, &Test22},
  {0, 0}
};

int
main(int ac, char **av)
{
  return ProcessCmdline("test_tps3",
   "Exercise the 3D thin-plate spline functions [timing].\n", 0, Tests, ac,
   av);
}
