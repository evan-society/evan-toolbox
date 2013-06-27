#ifndef EW_TRANSFORM2_H_INCLUDED
#define EW_TRANSFORM2_H_INCLUDED

// wdkg 2008-2010

namespace ew {
  class Transform2 {
  public:
    double orthog[2][2];
    double scale;
    double translate[2];
    unsigned int comps_cnt;
    void set_to_identity();
    void set_to_inverse(const ew::Transform2 *a);
    void set_to_composition(const ew::Transform2 *a, const ew::Transform2 *b);
    bool set_to_interpolation(const ew::Transform2 *a, const ew::Transform2 *b,
     double e);
    void get_matrix_gl(double *buffer) const;
    inline void apply(float *dst, const float *src) const;
    inline void apply(double *dst, const double *src) const;
    double get_denormalization() const;
    void set_to_normalization(const ew::Transform2 *a);
    bool get_valid() const;
    bool operator==(const ew::Transform2 &a) const;
    inline bool operator!=(const ew::Transform2 &a) const;
    static const ew::Transform2 identity_transform;
  };

}

/// @class ew::Transform2
/// @headerfile ew/Transform2.h "ew/Transform2.h"
/// @brief 2D Similarity Transformation
///
/// ew::Transform2 represents similarity transformations of @f$\mathbb{R}^2@f$,
/// each as the composition of an orthogonal linear map, a scaling and a
/// translation.
/// The translation is applied after the orthogonal linear map and the scaling.
///
/// ew::Transform2 is a POD type class.

/// @var double ew::Transform2::orthog[2][2]
/// This is the matrix representing the orthogonal part of the transformation.
/// The matrix elements are ordered by row, and the matrix is applied on the
/// left.
/// The coordinates should be finite.

/// @var double ew::Transform2::scale
/// This is the scale part of the transformation.
/// This should be positive.

/// @var double ew::Transform2::translate[2]
/// This is the translation part of the transformation.
/// The coordinates should be finite.

/// @var unsigned int ew::Transform2::comps_cnt
/// This is a count of the accumulated number of compositions performed in the
/// calculation of the current transformation.
/// It is reset by #set_to_identity and #set_to_normalization and set by
/// #set_to_inverse, #set_to_composition and #set_to_interpolation.
/// If #orthog is set directly, #comps_cnt should be set appropriately.
///
/// #set_to_composition will automatically normalize its result when #comps_cnt
/// exceeds an internal threshold.
///
/// This is to prevent exponential deviation of the orthogonal part from
/// orthogonality under iterated compositions.

/// This applies the transformation to a point.
/// @param[out] dst
///   The result is stored in this array of size 2.
/// @param[in] src
///   The original point is contained in this array of size 2.

inline void
ew::Transform2::apply(float *dst, const float *src) const
{
  double f0 = translate[0] +
   scale * (orthog[0][0] * src[0] + orthog[0][1] * src[1]);
  double f1 = translate[1] +
   scale * (orthog[1][0] * src[0] + orthog[1][1] * src[1]);
  dst[0] = f0;
  dst[1] = f1;
}

/// This applies the transformation to a point.
/// @param[out] dst
///   The result is stored in this array of size 2.
/// @param[in] src
///   The original point is contained in this array of size 2.

inline void
ew::Transform2::apply(double *dst, const double *src) const
{
  double f0 = translate[0] +
   scale * (orthog[0][0] * src[0] + orthog[0][1] * src[1]);
  double f1 = translate[1] +
   scale * (orthog[1][0] * src[0] + orthog[1][1] * src[1]);
  dst[0] = f0;
  dst[1] = f1;
}

/// Compares this transform with another, member by member, except for
/// #comps_cnt, which is ignored.
/// @param a the other transform

inline bool
ew::Transform2::operator!=(const ew::Transform2 &a) const
{
  return !operator==(a);
}

#endif
