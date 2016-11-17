#ifndef EW_GEOM3_H_INCLUDED
#define EW_GEOM3_H_INCLUDED

// wdkg 2009-2010

// Geometry utilities.

// cut_polygon assumes the input poly is planar and strictly convex, to with
// calculation error
// a polygon exactly on the cut plane is returned on the arbitrary side.
// a polygon approximately on the cut plane is arbitrarily cut.
// nearest_on_polytope and crop_polytope supports only nv = 2 or 3.

#include <cmath>
#include "ew/Bbox3.h"

namespace ew {

  class Geom3 {
  public:
    inline static void subtract(double *r, const double *a, const double *b);
    inline static void linear_combination(double *r, double m0,
     const double *v0);
    inline static void linear_combination(double *r, double m0,
     const double *v0, double m1, const double *v1);
    inline static void linear_combination(double *r, double m0,
     const double *v0, double m1, const double *v1, double m2,
     const double *v2);
    inline static double dot(const double *a, const double *b);
    inline static void cross(double *r, const double *a, const double *b);
    inline static double norm(const double *a);
    inline static void set(double *p, double x, double y, double z);
    inline static void set(double *p, const double *q);
    static double nearest_on_polytope(double *proj, double *coeffs,
     const double *p, const double *vv, int nv);
    static int crop_polytope(double *cropped_vertices, const double *vv,
     int nv, const ew::Bbox3 *box);
  private:
// undefined
    Geom3();
  };

}

inline void
ew::Geom3::subtract(double *r, const double *a, const double *b)
{
  r[0] = a[0] - b[0];
  r[1] = a[1] - b[1];
  r[2] = a[2] - b[2];
}

inline void
ew::Geom3::linear_combination(double *r, double m0, const double *v0)
{
  r[0] = m0 * v0[0];
  r[1] = m0 * v0[1];
  r[2] = m0 * v0[2];
}

inline void
ew::Geom3::linear_combination(double *r, double m0, const double *v0,
 double m1, const double *v1)
{
  r[0] = m0 * v0[0] + m1 * v1[0];
  r[1] = m0 * v0[1] + m1 * v1[1];
  r[2] = m0 * v0[2] + m1 * v1[2];
}

inline void
ew::Geom3::linear_combination(double *r, double m0, const double *v0,
 double m1, const double *v1, double m2, const double *v2)
{
  r[0] = m0 * v0[0] + m1 * v1[0] + m2 * v2[0];
  r[1] = m0 * v0[1] + m1 * v1[1] + m2 * v2[1];
  r[2] = m0 * v0[2] + m1 * v1[2] + m2 * v2[2];
}

inline double
ew::Geom3::dot(const double *a, const double *b)
{
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline void
ew::Geom3::cross(double *r, const double *a, const double *b)
{
  r[0] = a[1] * b[2] - a[2] * b[1];
  r[1] = a[2] * b[0] - a[0] * b[2];
  r[2] = a[0] * b[1] - a[1] * b[0];
}

inline double
ew::Geom3::norm(const double *a)
{
  return std::sqrt(dot(a, a));
}

inline void
ew::Geom3::set(double *p, double x, double y, double z)
{
  p[0] = x;
  p[1] = y;
  p[2] = z;
}

inline void
ew::Geom3::set(double *p, const double *q)
{
  p[0] = q[0];
  p[1] = q[1];
  p[2] = q[2];
}

#endif
