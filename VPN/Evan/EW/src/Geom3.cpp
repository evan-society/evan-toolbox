// wdkg 2009-2010

#include <cmath>
#include "ew/Bbox3.h"
#include "ew/ErrorLogic.h"
#include "Geom3.h"

// Projection to nearest point on a polytope.
// The polytope can be degenerate.
// Projection onto a line or plane can be unreliable if any vertices are close,
// so always check the distances to the edges and vertices.
// This returns the distance from the point to the projection.

//XXX ? file cache index, ?file cache norms, ?file cache whole surface
//XXX make sl at faces w/o reliable norms relax dim = 3 ?

//XXX faster less rigorous algorithm?

// This is only implemented for nv = 2 or 3.

double
ew::Geom3::nearest_on_polytope(double *proj, double *coeffs, const double *p,
 const double *vv, int nv)
{
  double p0[3], p1[3], p2[3];
  subtract(p0, p, vv + 0);
  subtract(p1, p, vv + 3);
  double d, di;
  d = norm(p0);
  set(coeffs, 1.0, 0.0, 0.0);
  set(proj, vv + 0);
  di = norm(p1);
  if (di < d) {
    d = di;
    set(coeffs, 0.0, 1.0, 0.0);
    set(proj, vv + 3);
  }
  if (nv > 2) {
    subtract(p2, p, vv + 6);
    di = norm(p2);
    if (di < d) {
      d = di;
      set(coeffs, 0.0, 0.0, 1.0);
      set(proj, vv + 6);
    }
  }
  int ne = 0;
  double v01[3];
  subtract(v01, vv + 3, vv + 0);
  double s01 = dot(v01, v01);
  double p0v01 = dot(p0, v01);
  double m0, m1, m2, v[3], vp[3];
  if (p0v01 > 0.0 && p0v01 < s01) {
    m1 = p0v01 / s01;
    m0 = 1.0 - m1;
    if (m0 < 1.0 && m0 > 0.0 && m1 < 1.0 && m1 > 0.0)  {
      ne += 1;
      linear_combination(v, m0, vv + 0, m1, vv + 3);
      subtract(vp, v, p);
      di = norm(vp);
      if (di < d) {
        d = di;
        set(coeffs, m0, m1, 0.0);
        set(proj, v);
      }
    }
  }
  if (nv == 2) {
    return d;
  }
  double v12[3], v20[3];
  subtract(v12, vv + 6, vv + 3);
  subtract(v20, vv + 0, vv + 6);
  double s12 = dot(v12, v12);
  double s20 = dot(v20, v20);
  double p1v12 = dot(p1, v12);
  double p2v20 = dot(p2, v20);
  if (p1v12 > 0.0 && p1v12 < s12) {
    m2 = p1v12 / s12;
    m1 = 1.0 - m2;
    if (m1 < 1.0 && m1 > 0.0 && m2 < 1.0 && m2 > 0.0)  {
      ne += 1;
      linear_combination(v, m1, vv + 3, m2, vv + 6);
      subtract(vp, v, p);
      di = norm(vp);
      if (di < d) {
        d = di;
        set(coeffs, 0.0, m1, m2);
        set(proj, v);
      }
    }
  }
  if (p2v20 > 0.0 && p2v20 < s20) {
    m0 = p2v20 / s20;
    m2 = 1.0 - m0;
    if (m2 < 1.0 && m2 > 0.0 && m0 < 1.0 && m0 > 0.0)  {
      ne += 1;
      linear_combination(v, m2, vv + 6, m0, vv + 0);
      subtract(vp, v, p);
      di = norm(vp);
      if (di < d) {
        d = di;
        set(coeffs, m0, 0.0, m2);
        set(proj, v);
      }
    }
  }
  if (ne >= 2) {
    double p0v02 = -dot(p0, v20);
    double dot12 = -dot(v01, v20);
    double det = s01 * s20 - dot12 * dot12;
    if (std::fabs(det) > std::fabs(s20 * p0v01 - dot12 * p0v02) &&
     std::fabs(det) > std::fabs(-dot12 * p0v01 + s01 * p0v02)) {
      m1  = (s20 * p0v01 - dot12 * p0v02) / det;
      m2  = (-dot12 * p0v01 + s01 * p0v02) / det;
      m0  = 1.0 - m1 - m2;
      if (m0 < 1.0 && m0 > 0.0 && m1 < 1.0 && m1 > 0.0 &&
       m2 < 1.0 && m2 > 0.0)  {
        linear_combination(v, m0, vv + 0, m1, vv + 3, m2, vv + 6);
        subtract(vp, v, p);
        di = norm(vp);
        if (di < d) {
          d = di;
          set(coeffs, m0, m1, m2);
          set(proj, v);
        }
      }
    }
  }
  return d;
}

// Unordered, non-minimal vertex sets are simpler and more robust than lists of
// vertices of strictly convex planar polygons, especially after rounding has
// eroded their planarity or strict convexity.
// To avoid excessively redundent representations, and cumulative rounding
// errors, always apply to the original edge or face.

// Space for 2 points is sufficient for an edge, 85 for a face.
// The bbox must be finite.
//XXX direct algorithm?
//   original vertices if inside
//   edge intersect bbox faces
//   face intersect bbox edges

// This is only implemented for nv = 2 or 3.

int
ew::Geom3::crop_polytope(double *cropped_vertices, const double *vv, int nv,
 const ew::Bbox3 *box)
{
  const double *inp;
  double *outp, scr[39];
  int zone[39];
  int n = nv, ni;
  static const int MaxN[2][3] = {{2, 2, 2}, {5, 13, 85}};
  for (int dir = 0; dir < 3; dir += 1) {
    switch(dir) {
    case 0:
      inp = vv;
      outp = cropped_vertices;
      break;
    case 1:
      inp = cropped_vertices;
      outp = scr;
      break;
    case 2:
      inp = scr;
      outp = cropped_vertices;
      break;
    }
    ni = 0;
    int nil = 0, nir = 0;
    for (int i = 0; i < n; i += 1) {
      if (inp[3 * i + dir] < box->min[dir]) {
        zone[i] = -1;
        nil += 1;
      } else if (inp[3 * i + dir] > box->max[dir]) {
        zone[i] = 1;
        nir += 1;
      } else {
        zone[i] = 0;
        set(outp + 3 * ni, inp + 3 * i);
        ni += 1;
      }
    }
    if (nil == n || nir == n) {
      return 0;
    }
    for (int c = -1; c <= 0; c += 1) {
      double cut;
      if (c == -1) {
        cut = box->min[dir];
      } else {
        cut = box->max[dir];
      }
      for (int i = 0; i < n; i += 1) {
        if (zone[i] > c) {
          continue;
        }
        for (int j = 0; j < n; j += 1) {
          if (zone[j] <= c) {
            continue;
          }
          double c0 = (inp[3 * j + dir] - cut) /
           (inp[3 * j + dir] - inp[3 * i + dir]);
          double c1 = 1.0 - c0;
          double v[3];
          for (int k = 0; k < 3; k += 1) {
            if (k == dir) {
              v[k] = cut;
            } else {
              v[k] = c0 * inp[3 * i + k] + c1 * inp[3 * j + k];
              if (inp[3 * i + k] > inp[3 * j + k]) {
                if (v[k] > inp[3 * i + k]) {
                  v[k] = inp[3 * i + k];
                } else if (v[k] < inp[3 * j + k]) {
                  v[k] = inp[3 * j + k];
                }
              } else {
                if (v[k] > inp[3 * j + k]) {
                  v[k] = inp[3 * j + k];
                } else if (v[k] < inp[3 * i + k]) {
                  v[k] = inp[3 * i + k];
                }
              }
            }
          }
          if (ni >= MaxN[nv - 2][dir]) {
            throw ew::ErrorLogic(__FILE__, __LINE__);
          }
          set(outp + 3 * ni, v);
          ni += 1;
        }
      }
    }
    n = ni;
  }
  return n;
}
