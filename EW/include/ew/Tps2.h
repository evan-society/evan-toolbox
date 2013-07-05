#ifndef EW_TPS2_H_INCLUDED
#define EW_TPS2_H_INCLUDED

// wdkg 2008-2010

namespace ew {
  class Tps2 {
  public:
    static void interaction(
     double *L,
     const double *lmks,
     int n_lmks);
    static void algebraic_size(
     int *f_size,
     bool *is_mixed,
     bool *is_reduced,
     const int *relax_dims_opt,
     int n_lmks);
    static void factorize(
     double *F,
     int *pivots,
     double *nonsingularity,
     const double *L,
     const int *relax_dims_opt,
     const double *relax_params_opt,
     int f_size,
     bool is_mixed,
     bool is_reduced,
     int n_lmks);
    static void solve(
     double *spline,
     double *energy_opt,
     double *relax_lmk_images_opt,
     const double *F,
     const int *pivots,
     const double *lmk_images,
     const int *relax_dims_opt,
     const double *relax_params_opt,
     const double *L,
     int n_lmks,
     int f_size,
     bool is_mixed,
     bool is_reduced);
    static void map(
     double *point_images,
     const double *points,
     int n_points,
     const double *spline,
     const double *lmks,
     int n_lmks);
    static void cache(
     float *cache,
     const float *points,
     int n_points,
     const double *lmk);
    static void cache_map(
     float *point_images,
     const float *points,
     float *const (*caches),
     int n_points,
     const double *spline,
     int n_lmks);
    static void bending_energy(
     double *B,
     const double *F,
     const int *pivots,
     const int *relax_dims_opt,
     const double *relax_params_opt,
     int n_lmks,
     int f_size,
     bool is_mixed,
     bool is_reduced);
    static void principal_warps(
     double *P,
     double *evals,
     const double *B,
     int n_lmks,
     bool is_mixed);
    static void principal_axes(
     double *centroid,
     double *sum_squares,
     double *axes,
     const double *lmks,
     int n_lmks);
    static void uniform_basis(
     double *basis,
     double *transforms_opt,
     const double *centroid,
     const double *sum_squares,
     const double *axes,
     const double *lmks,
     int n_lmks);
  private:
// undefined
    Tps2();
  };
}

/// @class ew::Tps2
/// @headerfile ew/Tps2.h "ew/Tps2.h"
/// @brief Thin-Plate Spline in 2D
///
/// ew::Tps2 is a class of static methods.
/// It implements the thin-plate spline in @f$\mathbb{R}^2@f$.
/// It supports semi-landmarks that are allowed to slide along affine
/// subspaces.
///
/// The method interfaces are purely procedural.
/// It is up to the caller to provide the space for the arguments and for the
/// results.
/// The only memory management performed is for internal temporary scratch
/// space.
///
/// The numerical linear algebra is performed by the LAPACK library.
/// The thin-plate spline system of equations has a symmetric, semi-definite
/// matrix of coefficients.
/// It is factorizated using the LAPACK function @c DSYTRF
/// and solve using the LAPACK function @c DSYTRS.
/// @c DSYTRF uses the Bunch-Kaufman diagonal pivoting algorithm.
/// In the calculation of bending energy, the matrix inversion is performed by
/// @c DSYTRI.
/// In the calculation of principal warps and principal axes, the eigenanalysis
/// is preformed by @c DSYEV.
/// Lapack includes an error handler, @c xerbla, which will get linked into the
/// program.
/// However, provided this class is used correctly, @c xerbla will never be
/// called.
///
/// On Mac OS X (as of 2010-02), the bundled lapack implementation can crash if
/// the array arguments are not 16 byte aligned.
/// Consequently, the array arguments of ew::Tps2 methods should be 16 byte
/// aligned.
///
/// In this documentation, the first index of a matrix element is considered to
/// be the row number, the second the column number.
/// Matrix elements of all these methods are stored consecutively in memory by
/// rows.
///
/// See @ref semilandmarks and @ref uniform_warps for details of the
/// algorithms used by these methods.

#endif
