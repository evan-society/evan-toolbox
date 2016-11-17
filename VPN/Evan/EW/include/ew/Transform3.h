#ifndef EW_TRANSFORM3_H_INCLUDED
#define EW_TRANSFORM3_H_INCLUDED

// wdkg 2008-2010

//XXX implement apply inverse, etc

namespace ew {
  class Transform3 {
  public:
    double orthog[3][3];
    double scale;
    double translate[3];
    unsigned int comps_cnt;
    void set_to_identity();
    void set_to_inverse(const ew::Transform3 *a);
    void set_to_composition(const ew::Transform3 *a, const ew::Transform3 *b);
    bool set_to_interpolation(const ew::Transform3 *a, const ew::Transform3 *b,
     double e);
    void get_matrix_gl(double *buffer) const;
    inline void apply(float *dst, const float *src) const;
    inline void apply(double *dst, const double *src) const;
    double get_denormalization() const;
    void set_to_normalization(const ew::Transform3 *a);
    bool get_valid() const;
    bool scan(const char **se, const char *s);
    bool format(char *buf, int bufl, int *outl) const;
    bool operator==(const ew::Transform3 &a) const;
    inline bool operator!=(const ew::Transform3 &a) const;
    static const int FORMAT_LEN = 480;
    static const ew::Transform3 identity_transform;
  };
}

/// @class ew::Transform3
/// @headerfile ew/Transform3.h "ew/Transform3.h"
/// @brief 3D Similarity Transformation
///
/// ew::Transform3 represents similarity transformations of @f$\mathbb{R}^3@f$,
/// each as the composition of an orthogonal linear map, a scaling and a
/// translation.
/// The translation is applied after the orthogonal linear map and the scaling.
///
/// ew::Transform3 is a POD type class.

/// @var double ew::Transform3::orthog[3][3]
/// This is the matrix representing the orthogonal part of the transformation.
/// The matrix elements are ordered by row, and the matrix is applied on the
/// left.
/// The coordinates should be finite.

/// @var double ew::Transform3::scale
/// This is the scale part of the transformation.
/// This should be positive.

/// @var double ew::Transform3::translate[3]
/// This is the translation part of the transformation.
/// The coordinates should be finite.

/// @var unsigned int ew::Transform3::comps_cnt
/// This is a count of the accumulated number of compositions performed in the
/// calculation of the current transformation.
/// It is reset by #set_to_identity and #set_to_normalization and set by
/// #set_to_inverse, #set_to_composition, #set_to_interpolation and #scan.
/// If #orthog is set directly, #comps_cnt should be set appropriately.
///
/// #set_to_composition will automatically normalize its result when #comps_cnt
/// exceeds an internal threshold.
///
/// This is to prevent exponential deviation of the orthogonal part from
/// orthogonality under iterated compositions.

/// @var const int ew::Transform3::FORMAT_LEN
/// This is the length of buffer that is guaranteed to be long enough for the
/// output of #format.

// This is 13 * ew::String::FORMAT_DOUBLE_LEN + 64.
// Don't use ew::String::FORMAT_DOUBLE_LEN to avoid the dependency.

/// This applies the transformation to a point.
/// @param[out] dst
///   The result is stored in this array of size 3.
/// @param[in] src
///   The original point is contained in this array of size 3.

inline void
ew::Transform3::apply(float *dst, const float *src) const
{
  double f0 = translate[0] + scale *
   (orthog[0][0] * src[0] + orthog[0][1] * src[1] + orthog[0][2] * src[2]);
  double f1 = translate[1] + scale *
   (orthog[1][0] * src[0] + orthog[1][1] * src[1] + orthog[1][2] * src[2]);
  double f2 = translate[2] + scale *
   (orthog[2][0] * src[0] + orthog[2][1] * src[1] + orthog[2][2] * src[2]);
  dst[0] = f0;
  dst[1] = f1;
  dst[2] = f2;
}

/// This applies the transformation to a point.
/// @param[out] dst
///   The result is stored in this array of size 3.
/// @param[in] src
///   The original point is contained in this array of size 3.

inline void
ew::Transform3::apply(double *dst, const double *src) const
{
  double f0 = translate[0] + scale *
   (orthog[0][0] * src[0] + orthog[0][1] * src[1] + orthog[0][2] * src[2]);
  double f1 = translate[1] + scale *
   (orthog[1][0] * src[0] + orthog[1][1] * src[1] + orthog[1][2] * src[2]);
  double f2 = translate[2] + scale *
   (orthog[2][0] * src[0] + orthog[2][1] * src[1] + orthog[2][2] * src[2]);
  dst[0] = f0;
  dst[1] = f1;
  dst[2] = f2;
}

/// Compares this transform with another, member by member, except for
/// #comps_cnt, which is ignored.
/// @param a the other transform

inline bool
ew::Transform3::operator!=(const ew::Transform3 &a) const
{
  return !operator==(a);
}

#endif
