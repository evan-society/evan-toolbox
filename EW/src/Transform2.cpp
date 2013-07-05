// wdkg 2008-2010

#include <cmath>
#include <cstring>
#include "ew/Transform2.h"
#include "ew/String.h"

namespace {
  const unsigned int AUTO_NORM_OPS = 100;
}

/// This is the identity
/// transform.

const ew::Transform2 ew::Transform2::identity_transform = {
  {{1.0, 0.0}, {0.0, 1.0}},
  1.0,
  {0.0, 0.0},
  0
};

/// This sets this transformation to the identity
/// transformation.

void
ew::Transform2::set_to_identity()
{
  translate[0] = translate[1] = 0;
  scale = 1.0;
  orthog[0][0] = orthog[1][1] = 1.0;
  orthog[0][1] = orthog[1][0] = 0.0;
  comps_cnt = 0;
}

/// This sets this transformation to the inverse of another transformation.
/// @param a
///   This points to the transformation to invert.
///   @a a can equal @c this.

void
ew::Transform2::set_to_inverse(const ew::Transform2 *a)
{
  ew::Transform2 tr;
  if (this == a) {
    tr = *a;
    a = &tr;
  }
  translate[0] = -(
   a->orthog[0][0] * a->translate[0] +
   a->orthog[1][0] * a->translate[1]) /
   a->scale;
  translate[1] = -(
   a->orthog[0][1] * a->translate[0] +
   a->orthog[1][1] * a->translate[1]) /
   a->scale;
  scale = 1.0 / a->scale;
  orthog[0][0] = a->orthog[0][0];
  orthog[0][1] = a->orthog[1][0];
  orthog[1][0] = a->orthog[0][1];
  orthog[1][1] = a->orthog[1][1];
  comps_cnt = a->comps_cnt;
}

/// This sets this transformation to the composition of two transformations.
/// @param a
///   This points to the transformation that is applied last in the
///   composition.
///   @a a can equal @c this.
/// @param b
///   This points to the transformation that is applied first in the
///   composition.
///   @a b can equal @c this.

void
ew::Transform2::set_to_composition(const ew::Transform2 *a,
 const ew::Transform2 *b)
{
  ew::Transform2 tr;
  if (this == a) {
    tr = *a;
    a = &tr;
  }
  if (this == b) {
    tr = *b;
    b = &tr;
  }
  translate[0] =
   a->translate[0] +
   a->scale *
   (a->orthog[0][0] * b->translate[0] + a->orthog[0][1] * b->translate[1]);
  translate[1] =
   a->translate[1] +
   a->scale *
   (a->orthog[1][0] * b->translate[0] + a->orthog[1][1] * b->translate[1]);
  scale = a->scale * b->scale;
  orthog[0][0] =
   a->orthog[0][0] * b->orthog[0][0] +
   a->orthog[0][1] * b->orthog[1][0];
  orthog[0][1] =
   a->orthog[0][0] * b->orthog[0][1] +
   a->orthog[0][1] * b->orthog[1][1];
  orthog[1][0] =
   a->orthog[1][0] * b->orthog[0][0] +
   a->orthog[1][1] * b->orthog[1][0];
  orthog[1][1] =
   a->orthog[1][0] * b->orthog[0][1] +
   a->orthog[1][1] * b->orthog[1][1];
  unsigned int nc = a->comps_cnt + b->comps_cnt + 1;
  if (a->comps_cnt >= AUTO_NORM_OPS || b->comps_cnt >= AUTO_NORM_OPS ||
   nc >= AUTO_NORM_OPS) {
    set_to_normalization(this);
  } else {
    comps_cnt = nc;
  }
}

/// This sets this transformation to an interpolation between two
/// transformations.
/// @param a
///   This points to the first transformation.
///   @a a can equal @c this.
/// @param b
///   This points to the second transformation.
///   @a b can equal @c this.
/// @param e
///   This is the interpolation paramater.
///   When 0.0, the transformation is set to @c *a, when 1.0 to @c *b.
///   @a e is not restricted to the range [0.0, 1.0].
/// @return
///   @c true if a smooth canonical interpolation was possible.
///   If @c false, the transformation is set to either the first or second
///   transformation, depending on whether @a e is closer to 0.0 or 1.0.
///   This can occur if one transformation is a reflection and the other
///   transformation is a rotation, or if the orthogonal parts differ by a half
///   turn.

bool
ew::Transform2::set_to_interpolation(const ew::Transform2 *a,
 const ew::Transform2 *b, double e)
{
  double sc = std::pow(b->scale / a->scale, e) * a->scale;
  double tr0 = a->translate[0] * (1.0 - e) + b->translate[0] * e;
  double tr1 = a->translate[1] * (1.0 - e) + b->translate[1] * e;
  ew::Transform2 ai, bai, r;
  ai.set_to_inverse(a);
  bai.set_to_composition(b, &ai);
  double cosa = (bai.orthog[0][0] + bai.orthog[1][1]) / 2.0;
  double sina = (bai.orthog[0][1] - bai.orthog[1][0]) / 2.0;
  if (cosa <= -0.99999 || cosa * cosa + sina * sina < 0.5) {
    if (e <= 0.5) {
      *this = *a;
    } else {
      *this = *b;
    }
    return false;
  }
  double ang = std::atan2(sina, cosa) * e;
  cosa = std::cos(ang);
  sina = std::sin(ang);
  r.set_to_identity();
  r.orthog[0][0] = cosa;
  r.orthog[0][1] = sina;
  r.orthog[1][0] = -sina;
  r.orthog[1][1] = cosa;
  r.comps_cnt = 0;
  set_to_composition(&r, a);
  scale = sc;
  translate[0] = tr0;
  translate[1] = tr1;
  return true;
}

/// This outputs this transformation as a 4 x 4 matrix in the format used by
/// @b OpenGL.
/// @param[out] buffer
///   The result is stored in this array of size 16.

void
ew::Transform2::get_matrix_gl(double *buffer) const
{
  buffer[0] = orthog[0][0] * scale;
  buffer[1] = orthog[1][0] * scale;
  buffer[2] = 0.0;
  buffer[3] = 0.0;
  buffer[4] = orthog[0][1] * scale;
  buffer[5] = orthog[1][1] * scale;
  buffer[6] = 0.0;
  buffer[7] = 0.0;
  buffer[8] = 0.0;
  buffer[9] = 0.0;
  buffer[10] = 1.0;
  buffer[11] = 0.0;
  buffer[12] = translate[0];
  buffer[13] = translate[1];
  buffer[14] = 0.0;
  buffer[15] = 1.0;
}

/// This checks that all components of #orthog, #scale and #translate are
/// finite floating point numbers and that #scale is positive and not
/// sub-normal.
/// @return
///   @c true if all these conditions are satisfied.

bool
ew::Transform2::get_valid() const
{
  if (!std::isnormal(scale) || scale <= 0.0 ||
   !std::isfinite(translate[0]) ||
   !std::isfinite(translate[1]) ||
   !std::isfinite(orthog[0][0]) ||
   !std::isfinite(orthog[0][1]) ||
   !std::isfinite(orthog[1][0]) ||
   !std::isfinite(orthog[1][1])) {
    return false;
  }
  return true;
}

/// This calculates how far #orthog is from being orthogonal.
/// @return
///   The return value is a non-negative number representing this distance.

double
ew::Transform2::get_denormalization() const
{
  double s = 0.0;
  for (int i = 0; i < 2; i += 1) {
    for (int j = 0; j < 2; j += 1) {
      double d = 0.0;
      for (int k = 0; k < 2; k += 1) {
        d += orthog[i][k] * orthog[j][k];
      }
      if (j == i) {
        d -= 1.0;
      }
      s += d * d;
    }
  }
  return std::sqrt(s);
}

/// This sets this transformation to another transformation and then coerces
/// #orthog to be orthogonal.
/// @param a
///   This points to the transformation to normalize.
///   @a a can equal @c this.

void
ew::Transform2::set_to_normalization(const ew::Transform2 *a)
{
  double d = std::sqrt(a->orthog[0][0] * a->orthog[0][0] +
   a->orthog[1][0] * a->orthog[1][0]);
  if (std::fabs(d) < 1e-5) {
    orthog[0][0] = 1.0;
    orthog[1][0] = 0.0;
  } else {
    orthog[0][0] = a->orthog[0][0] / d;
    orthog[1][0] = a->orthog[1][0] / d;
  }
  d = orthog[0][0] * a->orthog[0][1] + orthog[1][0] * a->orthog[1][1];
  orthog[0][1] = a->orthog[0][1] - d * orthog[0][0];
  orthog[1][1] = a->orthog[1][1] - d * orthog[1][0];
  d = std::sqrt(orthog[0][1] * orthog[0][1] + orthog[1][1] * orthog[1][1]);
  if (std::fabs(d) < 1e-5) {
    orthog[0][1] = -orthog[1][0];
    orthog[1][1] = orthog[0][0];
  } else {
    orthog[0][1] /= d;
    orthog[1][1] /= d;
  }
  comps_cnt = 0;
  translate[0] = a->translate[0];
  translate[1] = a->translate[1];
  scale = a->scale;
}

/// Compares this transform with another, member by member, except for
/// #comps_cnt, which is ignored.
/// @param a the other transform

bool
ew::Transform2::operator==(const ew::Transform2 &a) const
{
  return scale == a.scale &&
   translate[0] == a.translate[0] &&
   translate[1] == a.translate[1] &&
   orthog[0][0] == a.orthog[0][0] &&
   orthog[0][1] == a.orthog[0][1] &&
   orthog[1][0] == a.orthog[1][0] &&
   orthog[1][1] == a.orthog[1][1];
}
