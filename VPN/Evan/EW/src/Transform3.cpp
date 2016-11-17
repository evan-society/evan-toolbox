// wdkg 2008-2010

#include <cmath>
#include <cstring>
#include "ew/Transform3.h"
#include "ew/String.h"

namespace {
  const unsigned int AUTO_NORM_OPS = 100;
}

/// This is the identity
/// transform.

const ew::Transform3 ew::Transform3::identity_transform = {
  {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
  1.0,
  {0.0, 0.0, 0.0},
  0
};

/// This sets this transformation to the identity
/// transformation.

void
ew::Transform3::set_to_identity()
{
  translate[0] = translate[1] = translate[2] = 0;
  scale = 1.0;
  orthog[0][0] = orthog[1][1] = orthog[2][2] = 1.0;
  orthog[0][1] = orthog[1][2] = orthog[2][0] = 0.0;
  orthog[0][2] = orthog[1][0] = orthog[2][1] = 0.0;
  comps_cnt = 0;
}

/// This sets this transformation to the inverse of another transformation.
/// @param a
///   This points to the transformation to invert.
///   @a a can equal @c this.

void
ew::Transform3::set_to_inverse(const ew::Transform3 *a)
{
  ew::Transform3 tr;
  if (this == a) {
    tr = *a;
    a = &tr;
  }
  translate[0] = -(
   a->orthog[0][0] * a->translate[0] +
   a->orthog[1][0] * a->translate[1] +
   a->orthog[2][0] * a->translate[2]) /
   a->scale;
  translate[1] = -(
   a->orthog[0][1] * a->translate[0] +
   a->orthog[1][1] * a->translate[1] +
   a->orthog[2][1] * a->translate[2]) /
   a->scale;
  translate[2] = -(
   a->orthog[0][2] * a->translate[0] +
   a->orthog[1][2] * a->translate[1] +
   a->orthog[2][2] * a->translate[2]) /
   a->scale;
  scale = 1.0 / a->scale;
  orthog[0][0] = a->orthog[0][0];
  orthog[0][1] = a->orthog[1][0];
  orthog[0][2] = a->orthog[2][0];
  orthog[1][0] = a->orthog[0][1];
  orthog[1][1] = a->orthog[1][1];
  orthog[1][2] = a->orthog[2][1];
  orthog[2][0] = a->orthog[0][2];
  orthog[2][1] = a->orthog[1][2];
  orthog[2][2] = a->orthog[2][2];
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
ew::Transform3::set_to_composition(const ew::Transform3 *a,
 const ew::Transform3 *b)
{
  ew::Transform3 tr;
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
   a->scale * (
   a->orthog[0][0] * b->translate[0] +
   a->orthog[0][1] * b->translate[1] +
   a->orthog[0][2] * b->translate[2]);
  translate[1] =
   a->translate[1] +
   a->scale * (
   a->orthog[1][0] * b->translate[0] +
   a->orthog[1][1] * b->translate[1] +
   a->orthog[1][2] * b->translate[2]);
  translate[2] =
   a->translate[2] +
   a->scale * (
   a->orthog[2][0] * b->translate[0] +
   a->orthog[2][1] * b->translate[1] +
   a->orthog[2][2] * b->translate[2]);
  scale = a->scale * b->scale;
  orthog[0][0] =
   a->orthog[0][0] * b->orthog[0][0] +
   a->orthog[0][1] * b->orthog[1][0] +
   a->orthog[0][2] * b->orthog[2][0];
  orthog[0][1] =
   a->orthog[0][0] * b->orthog[0][1] +
   a->orthog[0][1] * b->orthog[1][1] +
   a->orthog[0][2] * b->orthog[2][1];
  orthog[0][2] =
   a->orthog[0][0] * b->orthog[0][2] +
   a->orthog[0][1] * b->orthog[1][2] +
   a->orthog[0][2] * b->orthog[2][2];
  orthog[1][0] =
   a->orthog[1][0] * b->orthog[0][0] +
   a->orthog[1][1] * b->orthog[1][0] +
   a->orthog[1][2] * b->orthog[2][0];
  orthog[1][1] =
   a->orthog[1][0] * b->orthog[0][1] +
   a->orthog[1][1] * b->orthog[1][1] +
   a->orthog[1][2] * b->orthog[2][1];
  orthog[1][2] =
   a->orthog[1][0] * b->orthog[0][2] +
   a->orthog[1][1] * b->orthog[1][2] +
   a->orthog[1][2] * b->orthog[2][2];
  orthog[2][0] =
   a->orthog[2][0] * b->orthog[0][0] +
   a->orthog[2][1] * b->orthog[1][0] +
   a->orthog[2][2] * b->orthog[2][0];
  orthog[2][1] =
   a->orthog[2][0] * b->orthog[0][1] +
   a->orthog[2][1] * b->orthog[1][1] +
   a->orthog[2][2] * b->orthog[2][1];
  orthog[2][2] =
   a->orthog[2][0] * b->orthog[0][2] +
   a->orthog[2][1] * b->orthog[1][2] +
   a->orthog[2][2] * b->orthog[2][2];
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

// Here we need to find the axis and angle of rotation from an orthogonal
// matrix.  This is easy in 2D.  In 3D You could do this with eigenvectors and
// eigenvalues.  A better way is with use the following formuli.  Let R be the
// rotation about axis v through angle a.  Let v" be the matrix such that
// v" t = v x t.  Then
//   R = vv' + cos(a) (I - vv') + sin(a) v"
//   (R + R') / 2 = cos(a) I + (1 - cos(a)) vv'
//   trace R = 2 cos (a) + 1
//   (R - R') / 2 = sin(a) v"
// If det(R) != 1 there is no canonical way to calculate the interpolation.

bool
ew::Transform3::set_to_interpolation(const ew::Transform3 *a,
 const ew::Transform3 *b, double e)
{
  double sc = std::pow(b->scale / a->scale, e) * a->scale;
  double tr0 = a->translate[0] * (1.0 - e) + b->translate[0] * e;
  double tr1 = a->translate[1] * (1.0 - e) + b->translate[1] * e;
  double tr2 = a->translate[2] * (1.0 - e) + b->translate[2] * e;
  ew::Transform3 ai, bai, r;
  ai.set_to_inverse(a);
  bai.set_to_composition(b, &ai);
  double ocosa =
   (bai.orthog[0][0] + bai.orthog[1][1] + bai.orthog[2][2] - 1.0) / 2.0;
  double sv0 = (bai.orthog[2][1] - bai.orthog[1][2]) / 2.0;
  double sv1 = (bai.orthog[0][2] - bai.orthog[2][0]) / 2.0;
  double sv2 = (bai.orthog[1][0] - bai.orthog[0][1]) / 2.0;
  double osina = std::sqrt(sv0 * sv0 + sv1 * sv1 + sv2 * sv2);
  double det = bai.orthog[0][0] *
   (bai.orthog[1][1] * bai.orthog[2][2] -
   bai.orthog[2][1] * bai.orthog[1][2]) -
   bai.orthog[0][1] *
   (bai.orthog[1][0] * bai.orthog[2][2] -
   bai.orthog[2][0] * bai.orthog[1][2]) +
   bai.orthog[0][2] *
   (bai.orthog[1][0] * bai.orthog[2][1] -
   bai.orthog[2][0] * bai.orthog[1][1]);
  if (ocosa <= -0.99999 || det < 0.0) {
    if (e <= 0.5) {
      *this = *a;
    } else {
      *this = *b;
    }
    return false;
  }
  r.set_to_identity();
// Deliberate test for equality with no tolerance.
  if (osina != 0.0) {
    double ang = std::atan2(osina, ocosa) * e;
    double cosa = std::cos(ang);
    double sina = std::sin(ang);
    sv0 *= sina / osina;
    sv1 *= sina / osina;
    sv2 *= sina / osina;
    r.orthog[0][0] = cosa + sv0 * sv0 / (1.0 + cosa);
    r.orthog[0][1] = sv0 * sv1 / (1.0 + cosa) - sv2;
    r.orthog[0][2] = sv0 * sv2 / (1.0 + cosa) + sv1;
    r.orthog[1][0] = sv1 * sv0 / (1.0 + cosa) + sv2;
    r.orthog[1][1] = cosa + sv1 * sv1 / (1.0 + cosa);
    r.orthog[1][2] = sv1 * sv2 / (1.0 + cosa) - sv0;
    r.orthog[2][0] = sv2 * sv0 / (1.0 + cosa) - sv1;
    r.orthog[2][1] = sv2 * sv1 / (1.0 + cosa) + sv0;
    r.orthog[2][2] = cosa + sv2 * sv2 / (1.0 + cosa);
    r.comps_cnt = 0;
  }
  set_to_composition(&r, a);
  scale = sc;
  translate[0] = tr0;
  translate[1] = tr1;
  translate[2] = tr2;
  return true;
}

/// This outputs this transformation as a 4 x 4 matrix in the format used by
/// @b OpenGL.
/// @param[out] buffer
///   The result is stored in this array of size 16.

void
ew::Transform3::get_matrix_gl(double *buffer) const
{
  buffer[0] = orthog[0][0] * scale;
  buffer[1] = orthog[1][0] * scale;
  buffer[2] = orthog[2][0] * scale;
  buffer[3] = 0.0;
  buffer[4] = orthog[0][1] * scale;
  buffer[5] = orthog[1][1] * scale;
  buffer[6] = orthog[2][1] * scale;
  buffer[7] = 0.0;
  buffer[8] = orthog[0][2] * scale;
  buffer[9] = orthog[1][2] * scale;
  buffer[10] = orthog[2][2] * scale;
  buffer[11] = 0.0;
  buffer[12] = translate[0];
  buffer[13] = translate[1];
  buffer[14] = translate[2];
  buffer[15] = 1.0;
}

/// This checks that all components of #orthog, #scale and #translate are
/// finite floating point numbers and that #scale is positive and not
/// sub-normal.
/// @return
///   @c true if all these conditions are satisfied.

bool
ew::Transform3::get_valid() const
{
  if (!std::isnormal(scale) || scale <= 0.0 ||
   !std::isfinite(translate[0]) ||
   !std::isfinite(translate[1]) ||
   !std::isfinite(translate[2]) ||
   !std::isfinite(orthog[0][0]) ||
   !std::isfinite(orthog[0][1]) ||
   !std::isfinite(orthog[0][2]) ||
   !std::isfinite(orthog[1][0]) ||
   !std::isfinite(orthog[1][1]) ||
   !std::isfinite(orthog[1][2]) ||
   !std::isfinite(orthog[2][0]) ||
   !std::isfinite(orthog[2][1]) ||
   !std::isfinite(orthog[2][2])) {
    return false;
  }
  return true;
}

/// This calculates how far #orthog is from being orthogonal.
/// @return
///   The return value is a non-negative number representing this distance.

double
ew::Transform3::get_denormalization() const
{
  double s = 0.0;
  for (int i = 0; i < 3; i += 1) {
    for (int j = 0; j < 3; j += 1) {
      double d = 0.0;
      for (int k = 0; k < 3; k += 1) {
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
ew::Transform3::set_to_normalization(const ew::Transform3 *a)
{
  double d = std::sqrt(
   a->orthog[0][0] * a->orthog[0][0] +
   a->orthog[1][0] * a->orthog[1][0] +
   a->orthog[2][0] * a->orthog[2][0]);
  if (std::fabs(d) < 1e-5) {
    orthog[0][0] = 1.0;
    orthog[1][0] = 0.0;
    orthog[2][0] = 0.0;
  } else {
    orthog[0][0] = a->orthog[0][0] / d;
    orthog[1][0] = a->orthog[1][0] / d;
    orthog[2][0] = a->orthog[2][0] / d;
  }
  d =
   orthog[0][0] * a->orthog[0][1] +
   orthog[1][0] * a->orthog[1][1] +
   orthog[2][0] * a->orthog[2][1];
  orthog[0][1] = a->orthog[0][1] - d * orthog[0][0];
  orthog[1][1] = a->orthog[1][1] - d * orthog[1][0];
  orthog[2][1] = a->orthog[2][1] - d * orthog[2][0];
  d = std::sqrt(
   orthog[0][1] * orthog[0][1] +
   orthog[1][1] * orthog[1][1] +
   orthog[2][1] * orthog[2][1]);
  if (std::fabs(d) < 1e-5) {
    double a0 = std::fabs(orthog[0][0]);
    double a1 = std::fabs(orthog[1][0]);
    double a2 = std::fabs(orthog[2][0]);
    if (a2 <= a1 && a2 <= a0) {
      orthog[0][1] = -orthog[1][0];
      orthog[1][1] = orthog[0][0];
      orthog[2][1] = 0.0;
    } else if (a1 <= a0) {
      orthog[0][1] = -orthog[2][0];
      orthog[1][1] = 0.0;
      orthog[2][1] = orthog[0][0];
    } else {
      orthog[0][1] = 0.0;
      orthog[1][1] = -orthog[2][0];
      orthog[2][1] = orthog[1][0];
    }
    d = std::sqrt(
     orthog[0][1] * orthog[0][1] +
     orthog[1][1] * orthog[1][1] +
     orthog[2][1] * orthog[2][1]);
  }
  orthog[0][1] /= d;
  orthog[1][1] /= d;
  orthog[2][1] /= d;
  d =
   orthog[0][0] * a->orthog[0][2] +
   orthog[1][0] * a->orthog[1][2] +
   orthog[2][0] * a->orthog[2][2];
  orthog[0][2] = a->orthog[0][2] - d * orthog[0][0];
  orthog[1][2] = a->orthog[1][2] - d * orthog[1][0];
  orthog[2][2] = a->orthog[2][2] - d * orthog[2][0];
  d =
   orthog[0][1] * orthog[0][2] +
   orthog[1][1] * orthog[1][2] +
   orthog[2][1] * orthog[2][2];
  orthog[0][2] -= d * orthog[0][1];
  orthog[1][2] -= d * orthog[1][1];
  orthog[2][2] -= d * orthog[2][1];
  d = std::sqrt(
   orthog[0][2] * orthog[0][2] +
   orthog[1][2] * orthog[1][2] +
   orthog[2][2] * orthog[2][2]);
  if (std::fabs(d) < 1e-5) {
    orthog[0][2] =
     orthog[1][0] * orthog[2][1] - orthog[2][0] * orthog[1][1];
    orthog[1][2] =
     orthog[2][0] * orthog[0][1] - orthog[0][0] * orthog[2][1];
    orthog[2][2] =
     orthog[0][0] * orthog[1][1] - orthog[1][0] * orthog[0][1];
  } else {
    orthog[0][2] /= d;
    orthog[1][2] /= d;
    orthog[2][2] /= d;
  }
  comps_cnt = 0;
  translate[0] = a->translate[0];
  translate[1] = a->translate[1];
  translate[2] = a->translate[2];
  scale = a->scale;
}

/// This formats this transform as a text string.
///
/// The format used to represent transforms is as follows:
/// <CENTER>
/// [tr1 tr2 tr3] [or11 or12 or13 or21 or22 or23 or31 or32 or33] sc
/// </CENTER>
///
/// For example, this represents a transformation consisting of an enlargement
/// by a factor 1.5, followed by a rotation about the z axis, and then a
/// translation along the y axis:
/// <CENTER>
/// [0 4.5 0] [0.6 0.8 0 -0.8 0.6 0 0 0 1] 1.5
/// </CENTER>
///
/// The floating point numbers are represented exactly, but efficiently.
/// @param[out] buf
///   The output is written here.
/// @param bufl
///   The length of @a buf.
/// @param[out] outl
///   If non-null, the length of the output (even if it was not written),
///   not including the terminating null.
/// @return
///   @c true if the buffer is long enough for a successful format.

bool
ew::Transform3::format(char *buf, int bufl, int *outl) const
{
  int l = 0;
  for (int part = 0; part < 3; part += 1) {
    if (part < 2) {
      if (l < bufl) {
        buf[l] = '[';
      }
      l += 1;
    }
    int n;
    const double *dd;
    if (part == 0) {
      n = 3;
      dd = translate;
    } else if (part == 1) {
      n = 9;
      dd = orthog[0];
    } else {
      n = 1;
      dd = &scale;
    }
    for (int i = 0; i < n; i += 1) {
      int e;
      ew::String::format_double(buf + l, bufl - l, &e, dd[i]);
      l += e;
      if (i < n - 1) {
        if (l < bufl) {
          buf[l] = ' ';
        }
        l += 1;
      }
    }
    if (part < 2) {
      if (l + 1 < bufl) {
        buf[l] = ']';
        buf[l + 1] = ' ';
      }
      l += 2;
    }
  }
  if (l < bufl) {
    buf[l] = 0;
  }
  if (outl) {
    *outl = l;
  }
  return (l < bufl);
}

/// This scans a text string for a representation of a transform.
/// Initial whitespace is not skipped, so would result in failure.
/// The string must be null terminated.
/// If successfully scanned, #comps_cnt is set to zero.
/// The transform is not automatically normalized, but can be manually.
/// @param[out] se
///   If non-zero, this is set to the address of the first unprocessed
///   character.
/// @param[in] s
///   This is a pointer to the text to scan.
/// @return
///   @c true if the initial segment of the text is a correctly formatted
///   transform.

bool
ew::Transform3::scan(const char **se, const char *s)
{
  if (*s != '[') {
    if (se) {
      *se = s;
    }
    return false;
  }
  ew::Transform3 tr;
  for (int part = 0; part < 3; part += 1) {
    if (part != 0) {
      ew::String::scan_ws(&s, s);
    }
    if (part != 2) {
      if (s[0] != '[') {
        if (se) {
          *se = s;
        }
        return false;
      }
      s += 1;
    }
    int n;
    double *dd;
    if (part == 0) {
      n = 3;
      dd = tr.translate;
    } else if (part == 1) {
      n = 9;
      dd = tr.orthog[0];
    } else {
      n = 1;
      dd = &tr.scale;
    }
    for (int i = 0; i < n; i += 1) {
      ew::String::scan_ws(&s, s);
      if (!ew::String::scan_finite_double(dd + i, &s, s)) {
        if (se) {
          *se = s;
        }
        return false;
      }
    }
    if (part != 2) {
      ew::String::scan_ws(&s, s);
      if (s[0] != ']') {
        if (se) {
          *se = s;
        }
        return false;
      }
      s += 1;
    }
  }
  if (se) {
    *se = s;
  }
  tr.comps_cnt = 0;
  *this = tr;
  return true;
}

/// Compares this transform with another, member by member, except for
/// #comps_cnt, which is ignored.
/// @param a the other transform

bool
ew::Transform3::operator==(const ew::Transform3 &a) const
{
  return scale == a.scale &&
   translate[0] == a.translate[0] &&
   translate[1] == a.translate[1] &&
   translate[2] == a.translate[2] &&
   orthog[0][0] == a.orthog[0][0] &&
   orthog[0][1] == a.orthog[0][1] &&
   orthog[0][2] == a.orthog[0][2] &&
   orthog[1][0] == a.orthog[1][0] &&
   orthog[1][1] == a.orthog[1][1] &&
   orthog[1][2] == a.orthog[1][2] &&
   orthog[2][0] == a.orthog[2][0] &&
   orthog[2][1] == a.orthog[2][1] &&
   orthog[2][2] == a.orthog[2][2];
}
