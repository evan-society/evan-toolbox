// wdkg 2008-2010

#include <cmath>
#include <cstring>
#include <stdexcept>
#include "ew/ErrorLogic.h"
#include "ew/ErrorRuntime.h"
#include "ew/Tps2.h"
#include "AutoArray.h"

extern "C" void dsytrf_(const char *uplo, const int *n, double *a,
 const int *lda, int *ipiv, double *work, const int *lwork, int *info);
extern "C" void dsytrs_(const char *uplo, const int *n, const int *nrhs,
 const double *a, const int *lda, const int *ipiv, double *b, const int *ldb,
 int *info);
extern "C" void dsytri_(const char *uplo, const int *n, double *a,
 const int *lda, const int *ipiv, double *work, int *info);
extern "C" void dsyev_(const char *jobz, const char *uplo, const int *n,
 double *a, const int *lda, double *w, double *work, const int *lwork,
 int *info);
extern "C" int ilaenv_(const int *ispec, const char *name, const char *opts,
 const int *n1, const int *n2, const int *n3, const int *n4, const int name_l,
 const int opts_l);
// These are functions from the lapack library.

namespace {
// This is the ambient dimension.
  const int D = 2;
// These are the relax space dimensions.
  enum {RELAX_DIM_0, RELAX_DIM_1, RELAX_DIM_2};
// This is the relax space dimension of a fully relaxed landmark.
  const int RELAX_DIM_D = D;
// This is the number of affine components of the spline.
  const int D1 = D + 1;
// This is the number of affine components of a D-valued spline.
  const int DD1 = D * D1;
  const char ErrorWarpsEigenanalysis[] =
   "The principal warps could not be calculated"
   " (the eigenanalysis failed to converge).";
  const char ErrorAxesEigenanalysis[] =
   "The principal axes could not be calculated"
   " (the eigenanalysis failed to converge).";
}

/// This calculates the thin-plate spline interaction matrix of a configuration
/// of landmarks.
/// @param[out] L
///   This is a matrix of size (@a n_lmks + 3) x (@a n_lmks + 3).
///   The result is a symmetric and only the upper diagonal part is stored.
/// @param[in] lmks
///   This is a matrix of size @a n_lmks x 2 containing the landmark
///   coordinates.
/// @param[in] n_lmks
///   This is the number of landmarks.

void
ew::Tps2::interaction(double *L, const double *lmks, int n_lmks)
{
  int k;
// Constant control
  k = 0;
  L[k] = 0.0;
  k += 1;
  L[k] = 0.0;
  k += 1;
  L[k] = 0.0;
  k += 1;
  for (int j = 0; j < n_lmks; j += 1) {
    L[k] = 1.0;
    k += 1;
  }
// X control
  k = n_lmks + D1 + 1;
  L[k] = 0.0;
  k += 1;
  L[k] = 0.0;
  k += 1;
  for (int j = 0; j < n_lmks; j += 1) {
    L[k] = lmks[j * D];
    k += 1;
  }
// Y control
  k = (n_lmks + D1 + 1) * 2;
  L[k] = 0.0;
  k += 1;
  for (int j = 0; j < n_lmks; j += 1) {
    L[k] = lmks[j * D + 1];
    k += 1;
  }
// Landmark controls
  for (int i = 0; i < n_lmks; i += 1) {
    k = (n_lmks + D1 + 1) * (i + D1);
    for (int j = i; j < n_lmks; j += 1) {
      double dx = lmks[i * D] - lmks[j * D];
      double dy = lmks[i * D + 1] - lmks[j * D + 1];
      double r2 = dx * dx + dy * dy;
      if (r2 == 0.0) {
        L[k] = 0.0;
      } else {
        L[k] = 0.5 * r2 * std::log(r2);
      }
      k += 1;
    }
  }
}

/// This calculates the algebraic dimension and other characteristics of a
/// thin-plate spline configuration with semi-landmarks.
/// It is not necessary to call this function if there are no semi-landmarks.
/// @param[out] f_size
///   This is the location to store the algebraic dimension of the thin-plate
///   spline equation.
///   @a f_size is needed to allocate space for arguments to other functions.
///   If @a is_mixed is @c false, it is at most @a n_lmks + 3.
///   Otherwise it is less than 2 * (@a n_lmks + 3).
///   If there are no semi-landmarks, it is exactly @a n_lmks + 3.
///   In general, the difference between these upper limits and @a f_size is
///   the sum of the elements of @a relax_dims.
/// @param[out] is_mixed
///   This is the location to store a @c boolean value indicating whether the 2
///   dimensions are mixed in the thin-plate spline equation.
///   If not, the thin-plate spline equation can be solved in each dimension
///   separately.
///   If there are no semi-landmarks, this will be @c false.
///   It is @c true if there are semi-landmarks sliding in spaces of non-zero
///   dimension and non-zero co-dimension.
/// @param[out] is_reduced
///   This is the location to store a @c boolean value indicating whether there
///   are any non-trivial semi-landmarks, and consequently, whether the
///   thin-plate spline equation has been reduced.
///   If there are no semi-landmarks, this will be @c false.
///   It is @c true if there are semi-landmarks sliding in spaces of non-zero
///   dimension.
/// @param[in] relax_dims_opt
///   This is an array of size @a n_lmks containing the dimensions of the
///   sliding spaces for each landmark.
///   Each can be 0, 1, or 2.
///   For landmarks that are not semi-landmarks, the entries should be 0.
///   If there are no semi-landmarks, this can be @c NULL.
/// @param[in] n_lmks
///   See #interaction.

void
ew::Tps2::algebraic_size(int *f_size, bool *is_mixed, bool *is_reduced,
 const int *relax_dims_opt, int n_lmks)
{
  int fn;
  bool mx, rd;
  if (relax_dims_opt != 0) {
    fn = DD1;
    mx = rd = false;
    for (int i = 0; i < n_lmks; i += 1) {
      switch (relax_dims_opt[i]) {
      case RELAX_DIM_0:
        fn += D;
        break;
      case RELAX_DIM_1:
        fn += 1;
        mx = true;
        rd = true;
        break;
      case RELAX_DIM_D:
        rd = true;
        break;
      default:
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
    }
    if (!mx) {
      fn = fn / D;
    }
  } else {
    fn = n_lmks + D1;
    mx = rd = false;
  }
  *f_size = fn;
  *is_mixed = mx;
  *is_reduced = rd;
}

/// This calculates the algebraic equation corresponding to the semi-landmark
/// constraints, and factorizes the coefficient matrix.
/// If @a is_mixed is @c true, this equation is mixed.
/// Otherwise it separates into equations in each coordinate direction.
/// It uses outputs from #interaction and #algebraic_size.
/// @param[out] F
///   This is a matrix of size @a f_size x @a f_size.
///   The factorization of the coefficient matrix is stored here.
///   It is not output if the thin-plate spline is singular.
/// @param[out] pivots
///   This is an array of size @a f_size.
///   The pivots used in the factorization are stored here.
///   It is not output if the thin-plate spline is singular.
/// @param[out] nonsingularity
///   This is the inverse of the condition number of the block diagonal matrix
///   resulting from the matrix factorization.
///   If this is exactly 0.0, the other outputs will not have been set and they
///   must not used in #solve.
///   If this is close to 0.0, the other outputs are unreliable and should also
///   not be used.
///   A spline is exactly singular when there are 2 landmarks with the same
///   coordinates, or when the landmarks are colinear.
///   In this case, the @a nonsingularity might not be exactly 0.0 due to
///   numerical error.
///   Conversely, the spline equation might be numerically unsolvable without
///   it being exactly singular, for example, if there are an excessive number
///   of landmarks, if some landmarks are very close together, or if the
///   landmarks are nearly colinear.
///   The lower limit for the @a nonsingularity should be high enough to catch
///   these cases, but not so high as to reject too many usable splines.
///   A limit of @c 1.0e-8 is a sensible starting point.
/// @param[in] L
///   See #interaction.
/// @param[in] relax_dims_opt
///   See #algebraic_size.
/// @param[in] relax_params_opt
///   This is a matrix of size @a n_lmks x 2.
///   For every semi-landmark sliding along a line, the corresponding row of
///   this matrix should be a unit vector normal to the line.
///   The other rows need not be set.
///   If there are no semi-landmarks of dimension 1, this can be @c NULL.
/// @param[in] f_size
///   See #algebraic_size.
/// @param[in] is_mixed
///   See #algebraic_size.
/// @param[in] is_reduced
///   See #algebraic_size.
/// @param[in] n_lmks
///   See #interaction.
/// @exception std::bad_alloc

void
ew::Tps2::factorize(double *F, int *pivots, double *nonsingularity,
 const double *L, const int *relax_dims_opt, const double *relax_params_opt,
 int f_size, bool is_mixed, bool is_reduced, int n_lmks)
{
  if (n_lmks < D1 || (is_mixed && f_size == D1 * D) ||
   (!is_mixed && f_size == D1)) {
    *nonsingularity = 0.0;
    return;
  }
  if (!is_reduced) {
// Copy the upper diagonal part of L.
// Here f_size == n_lmks + D1.
    for (int i = 0; i < n_lmks + D1; i += 1) {
      for (int j = i; j < n_lmks + D1; j += 1) {
        F[i * f_size + j] = L[i * (n_lmks + D1) + j];
      }
    }
  } else if (!is_mixed) {
// Copy the upper diagonal part of L, skipping the rows and columns of fully
// relaxed landmarks.
    int a = 0;
    for (int i = 0; i < n_lmks + D1; i += 1) {
      if (i < D1 || relax_dims_opt[i - D1] == RELAX_DIM_0) {
        int b = a;
        for (int j = i; j < n_lmks + D1; j += 1) {
          if (j < D1 || relax_dims_opt[j - D1] == RELAX_DIM_0) {
            F[a * f_size + b] = L[i * (n_lmks + D1) + j];
            b += 1;
          }
        }
        a += 1;
      } else if (relax_dims_opt[i - D1] != RELAX_DIM_D) {
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
    }
  } else {
// Calculate R^T * L~D * R, where L~D is the tensor product of L and the D x D
// identity matrix and R is as described above, using and setting only the
// upper diagonal parts.
    int a = 0, b;
    for (int i = 0; i < n_lmks + D1; i += 1) {
      int rdoi;
      if (i < D1) {
        rdoi = RELAX_DIM_0;
      } else {
        rdoi = relax_dims_opt[i - D1];
      }
      switch (rdoi) {
      case RELAX_DIM_0:
        b = a;
        for (int j = i; j < n_lmks + D1; j += 1) {
          int rdoj;
          if (j < D1) {
            rdoj = RELAX_DIM_0;
          } else {
            rdoj = relax_dims_opt[j - D1];
          }
          switch (rdoj) {
          case RELAX_DIM_0:
            for (int x = 0; x < D; x += 1) {
              for (int y = 0; y < D; y += 1) {
                if (x == y) {
                  F[(a + x) * f_size + b + y] = L[i * (n_lmks + D1) + j];
                } else if (y > x || i != j) {
                  F[(a + x) * f_size + b + y] = 0.0;
                }
              }
            }
            b += D;
            break;
          case RELAX_DIM_1:
            for (int x = 0; x < D; x += 1) {
              F[(a + x) * f_size + b] =
               relax_params_opt[(j - D1) * D + x] * L[i * (n_lmks + D1) + j];
            }
            b += 1;
            break;
          }
        }
        a += D;
        break;
      case RELAX_DIM_1:
        b = a;
        for (int j = i; j < n_lmks + D1; j += 1) {
          int rdoj;
          if (j < D1) {
            rdoj = RELAX_DIM_0;
          } else {
            rdoj = relax_dims_opt[j - D1];
          }
          switch (rdoj) {
          case RELAX_DIM_0:
            for (int y = 0; y < D; y += 1) {
              F[a * f_size + b + y] =
               relax_params_opt[(i - D1) * D + y] * L[(n_lmks + D1) * i + j];
            }
            b += D;
            break;
          case RELAX_DIM_1:
            F[a * f_size + b] =
             (relax_params_opt[(i - D1) * D] *
             relax_params_opt[(j - D1) * D] +
             relax_params_opt[(i - D1) * D + 1] *
             relax_params_opt[(j - D1) * D + 1]) *
             L[i * (n_lmks + D1) + j];
            b += 1;
            break;
          }
        }
        a += 1;
        break;
      case RELAX_DIM_D:
        break;
      default:
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
    }
  }
  int fispec = 1, fn1 = f_size, fn2 = -1, fn3 = -1, fn4 = -1;
  int r = ilaenv_(&fispec, "dsytrf", "L", &fn1, &fn2, &fn3, &fn4, 6, 1);
  if (r <= 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  int scratchsize = r * f_size;
  ew::AutoArray<double> scratch(scratchsize);
  int fn = f_size, flda = f_size, flwork = scratchsize, finfo;
  dsytrf_("L", &fn, F, &flda, pivots, scratch, &flwork, &finfo);
  if (finfo < 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  if (finfo > 0) {
    *nonsingularity = 0.0;
    return;
  }
  double max = -1.0, min = -1.0;
  for (int i = 0; i < f_size; i += 1) {
    if (pivots[i] > 0) {
      double f1 = std::fabs(F[i * f_size + i]);
      if (f1 > max) {
        max = f1;
      }
      if (f1 < min || min < 0.0) {
        min = f1;
      }
    } else {
      double fm = (F[i * f_size + i] + F[(i + 1) * f_size + i + 1]) / 2.0;
      double fd = (F[i * f_size + i] - F[(i + 1) * f_size + i + 1]) / 2.0;
      fd = std::sqrt(fd * fd +
       F[i * f_size + i + 1] * F[i * f_size + i + 1]);
      double f1 = std::fabs(fm + fd);
      double f2 = std::fabs(fm - fd);
      if (f1 > f2) {
        if (f1 > max) {
          max = f1;
        }
        if (f2 < min || min < 0.0) {
          min = f2;
        }
      } else {
        if (f2 > max) {
          max = f2;
        }
        if (f1 < min || min < 0.0) {
          min = f1;
        }
      }
      i += 1;
    }
  }
  if (min < 0.0) {
    *nonsingularity = 0.0;
  } else {
    *nonsingularity = min / max;
  }
}

/// This calculates a thin-plate spline from a configuration of landmark
/// images.
/// It uses outputs from #interaction, #algebraic_size and #factorize.
/// It should not be called if the thin-plate spline is singular.
/// @param[out] spline
///   This is a matrix of size (@a n_lmks + 3) x 2.
///   The coefficients of the solution thin-plate spline are stored here.
/// @param[out] energy_opt
///   This is the location to store the bending energy of the thin-plate
///   spline.
///   It can be @c NULL, in which case the energy is not calculated.
/// @param[out] relax_lmk_images_opt
///   This is a matrix of size @a n_lmks x 2.
///   It is the location to store the optimized landmark images.
///   These are the landmark images that would minimize bending energy
///   consistent
///   with the semi-landmark constraints.
///   It can be @c NULL, in which case they are not calculated.
/// @param[in] F
///   See #factorize.
/// @param[in] pivots
///   See #factorize.
/// @param[in] lmk_images
///   This is a matrix of size @a n_lmks x 2 containing the landmark image
///   coordinates.
/// @param[in] relax_dims_opt
///   See #algebraic_size.
/// @param[in] relax_params_opt
///   See #factorize.
/// @param[in] L
///   See #interaction.
/// @param[in] n_lmks
///   See #interaction.
/// @param[in] f_size
///   See #algebraic_size.
/// @param[in] is_mixed
///   See #algebraic_size.
/// @param[in] is_reduced
///   See #algebraic_size.
/// @exception std::bad_alloc

void
ew::Tps2::solve(double *spline, double *energy_opt,
 double *relax_lmk_images_opt, const double *F, const int *pivots,
 const double *lmk_images, const int *relax_dims_opt,
 const double *relax_params_opt, const double *L, int n_lmks, int f_size,
 bool is_mixed, bool is_reduced)
{
  int scratchsize;
  if (is_mixed) {
    scratchsize = f_size;
  } else {
    scratchsize = f_size * D;
  }
  ew::AutoArray<double> scratch(scratchsize);
  if (!is_reduced) {
// Copy lmk_images, adding D1 initial zeros to each column.
// Here f_size == n_lmks + D1.
    for (int j = 0; j < D; j += 1) {
      for (int i = 0; i < f_size; i += 1) {
        if (i < D1) {
          scratch[j * f_size + i] = 0.0;
        } else {
          scratch[j * f_size + i] = lmk_images[(i - D1) * D + j];
        }
      }
    }
    int fn = f_size, fnrhs = D, flda = f_size, fldb = f_size, finfo;
    dsytrs_("L", &fn, &fnrhs, F, &flda, pivots, scratch, &fldb, &finfo);
    if (finfo < 0) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
// Rearrange result from D x (n_lmks + D1) to (n_lmks + D1) x D.
    for (int j = 0; j < D; j += 1) {
      for (int i = 0; i < f_size; i += 1) {
        spline[i * D + j] = scratch[j * f_size + i];
      }
    }
  } else if (!is_mixed) {
// Copy lmk_images, adding D1 initial zeros to each column and skipping the
// rows of the fully relaxed landmarks.
    int a = 0;
    for (int j = 0; j < D; j += 1) {
      for (int i = 0; i < D1; i += 1) {
        scratch[a] = 0.0;
        a += 1;
      }
      for (int i = 0; i < n_lmks; i += 1) {
        if (relax_dims_opt[i] == RELAX_DIM_0) {
          scratch[a] = lmk_images[i * D + j];
          a += 1;
        } else if (relax_dims_opt[i] != RELAX_DIM_D) {
          throw ew::ErrorLogic(__FILE__, __LINE__);
        }
      }
    }
    int fn = f_size, fnrhs = D, flda = f_size, fldb = f_size, finfo;
    dsytrs_("L", &fn, &fnrhs, F, &flda, pivots, scratch, &fldb, &finfo);
    if (finfo < 0) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
// Rearrange result from D x (n_lmks + D1) to (n_lmks + D1) x D, and add rows
// of zeros for the fully relaxed controls.
    a = 0;
    for (int j = 0; j < D; j += 1) {
      for (int i = 0; i < n_lmks + D1; i += 1) {
        if (i < D1 || relax_dims_opt[i - D1] == RELAX_DIM_0) {
          spline[i * D + j] = scratch[a];
          a += 1;
        } else {
          spline[i * D + j] = 0.0;
        }
      }
    }
  } else {
// Calculate R^T * lmk_images, where R is the matrix described above.
    int a = 0;
    for (int i = 0; i < D1; i += 1) {
      scratch[a] = 0.0;
      a += 1;
      scratch[a] = 0.0;
      a += 1;
    }
    for (int i = 0; i < n_lmks; i += 1) {
      switch (relax_dims_opt[i]) {
      case RELAX_DIM_0:
        scratch[a] = lmk_images[D * i];
        a += 1;
        scratch[a] = lmk_images[D * i + 1];
        a += 1;
        break;
      case RELAX_DIM_1:
        scratch[a] = lmk_images[D * i] * relax_params_opt[D * i] +
         lmk_images[D * i + 1] * relax_params_opt[D * i + 1];
        a += 1;
        break;
      case RELAX_DIM_D:
        break;
      default:
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
    }
    int fn = f_size, fnrhs = 1, flda = f_size, fldb = f_size, finfo;
    dsytrs_("L", &fn, &fnrhs, F, &flda, pivots, scratch, &fldb, &finfo);
    if (finfo < 0) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
// Calculate R * solution-vector, where R is the matrix described above.
    a = 0;
    for (int i = 0; i < n_lmks + D1; i += 1) {
      int rdoi;
      if (i < D1) {
        rdoi = RELAX_DIM_0;
      } else {
        rdoi = relax_dims_opt[i - D1];
      }
      switch (rdoi) {
      case RELAX_DIM_0:
        spline[D * i] = scratch[a];
        a += 1;
        spline[D * i + 1] = scratch[a];
        a += 1;
        break;
      case RELAX_DIM_1:
        spline[D * i] = relax_params_opt[D * (i - D1)] * scratch[a];
        spline[D * i + 1] = relax_params_opt[D * (i - D1) + 1] * scratch[a];
        a += 1;
        break;
      case RELAX_DIM_D:
        spline[D * i] = spline[D * i + 1] = 0;
        break;
      }
    }
  }
  if (energy_opt != 0) {
    double e = 0.0;
    for (int i = 0; i < n_lmks * D; i += 1) {
      e += spline[i + DD1] * lmk_images[i];
    }
    if (e <= 0.0) {
      e = 0.0;
    }
    *energy_opt = e;
  }
  if (relax_lmk_images_opt != 0) {
    for (int i = 0; i < n_lmks; i += 1) {
      if (relax_dims_opt == 0 || relax_dims_opt[i] == RELAX_DIM_0) {
        relax_lmk_images_opt[D * i] = lmk_images[D * i];
        relax_lmk_images_opt[D * i + 1] = lmk_images[D * i + 1];
      } else {
        double sx = 0.0, sy = 0.0;
        for (int j = 0; j < n_lmks + D1; j += 1) {
          if (i + D1 <= j) {
            sx += L[(i + D1) * (n_lmks + D1) + j] * spline[D * j];
            sy += L[(i + D1) * (n_lmks + D1) + j] * spline[D * j + 1];
          } else {
            sx += L[j * (n_lmks + D1) + i + D1] * spline[D * j];
            sy += L[j * (n_lmks + D1) + i + D1] * spline[D * j + 1];
          }
        }
        relax_lmk_images_opt[D * i] = sx;
        relax_lmk_images_opt[D * i + 1] = sy;
      }
    }
  }
}

/// This calculates the images of a list of points under a thin-plate spline.
/// It uses outputs of #solve.
/// It should not be called if the thin-plate spline is singular.
/// @param[in] point_images
///   This is a matrix of size @a n_points x 2.
///   It is the location to store the coordinates of the images of the points.
/// @param[in] points
///   This is a matrix of size @a n_points x 2, containing the coordinates of
///   the points
/// @param[in] n_points
///   This is the number of points.
/// @param[in] spline
///   See #solve.
/// @param[in] lmks
///   See #interaction.
/// @param[in] n_lmks
///   See #interaction.

void
ew::Tps2::map(double *point_images, const double *points, int n_points,
 const double *spline, const double *lmks, int n_lmks)
{
  for (int i = 0; i < n_points; i += 1) {
    double x = points[i * D];
    double y = points[i * D + 1];
    double sx = 0.0;
    double sy = 0.0;
    for (int j = 0; j < n_lmks; j += 1) {
      double dx = lmks[j * D] - x;
      double dy = lmks[j * D + 1] - y;
      double r2 = dx * dx + dy * dy;
      double v;
      if (r2 == 0.0) {
        v = 0.0;
      } else {
        v = 0.5 * r2 * std::log(r2);
      }
      sx += v * spline[j * D + DD1];
      sy += v * spline[j * D + DD1 + 1];
    }
    sx += spline[0] + spline[D] * x + spline[2 * D] * y;
    sy += spline[1] + spline[D + 1] * x + spline[2 * D + 1] * y;
    point_images[i * D] = sx;
    point_images[i * D + 1] = sy;
  }
}

/// This evaluates a single thin-plate spline kernel function at a list of
/// points.
/// It is intended for speed critical situations where incremental changes
/// are made to a thin-plate spline that is being applied to a fixed set of
/// points.
/// It requires a @a cache for each landmark, which could be a require a
/// significant amount of memory.
/// The @a points and @a cache are of type @c float rather than @c double to
/// save space.
/// As a consequence, the accuracy of the mapped points is reduced.
/// @param[out] cache
///   This is an array of size n_points.
///   The cached evaluations are stored here.
/// @param[in] points
///   See #map, except that this is a @c float array rather than @c double.
/// @param[in] n_points
///   See #map.
/// @param[in] lmk
///   This points to an array of size 2 containing the coordinates of the
///   landmark.

void
ew::Tps2::cache(float *cache, const float *points, int n_points,
 const double *lmk)
{
  for (int i = 0; i < n_points; i += 1) {
    double dx = points[i * D] - lmk[0];
    double dy = points[i * D + 1] - lmk[1];
    double r2 = dx * dx + dy * dy;
    double v;
    if (r2 == 0.0) {
      v = 0.0;
    } else {
      v = 0.5 * r2 * std::log(r2);
    }
    cache[i] = v;
  }
}

/// This uses previously calculated cached kernel evaluations to calculate the
/// images of a list of points under a thin-plate spline.
/// It uses outputs from #solve and #cache.
/// It should not be called if the thin-plate spline is singular.
/// @param[in] point_images
///   See #map.
/// @param[in] points
///   See #map.
/// @param[in] caches
///   This is an array of size @a n_lmks.
///   It is an array of pointers, each pointing to the corresponding cache.
/// @param[in] n_points
///   See #map.
/// @param[in] spline
///   See #solve.
/// @param[in] n_lmks
///   See #interaction.

void
ew::Tps2::cache_map(float *point_images, const float *points,
 float *const (*caches), int n_points, const double *spline, int n_lmks)
{
  for (int i = 0; i < n_points; i += 1) {
    point_images[D * i] = 0.0;
    point_images[D * i + 1] = 0.0;
  }
  for (int j = 0; j < n_lmks; j += 1) {
    float *u = caches[j];
    for (int i = 0; i < n_points; i += 1) {
      point_images[D * i] += u[i] * spline[D * j + DD1];
      point_images[D * i + 1] += u[i] * spline[D * j + DD1 + 1];
    }
  }
  for (int i = 0; i < n_points; i += 1) {
    point_images[D * i] += spline[0] + spline[D] * points[D * i] +
     spline[2 * D] * points[D * i + 1];
    point_images[D * i + 1] += spline[1] + spline[D + 1] * points[D * i] +
     spline[2 * D + 1] * points[D * i + 1];
  }
}

/// This calculates the bending energy matrix.
/// If @a is_mixed, it is a quadratic form on the mixed coordinates.
/// Otherwise, it is a quadratic form that is applied separately to the x
/// coordinates and to the y coordinates.
/// If there are any non-trivial semi-landmarks, this is the form for the
/// minimized bending energy.
/// It uses outputs from #interaction, #algebraic_size and #factorize.
/// It should not be called if the thin-plate spline is singular.
/// @param[out] B
///   This is the bending energy matrix.
///   It is a symmetric matrix, with both upper and lower diagonal entries
///   stored.
///   If @a is_mixed, it has size (2 * @a n_lmks) x (2 * @a n_lmks).
///   Otherwise, it has size @a n_lmks x @a n_lmks.
/// @param[in] F
///   See #factorize.
/// @param[in] pivots
///   See #factorize.
/// @param[in] relax_dims_opt
///   See #algebraic_size.
/// @param[in] relax_params_opt
///   See #factorize.
/// @param[in] n_lmks
///   See #interaction.
/// @param[in] f_size
///   See #algebraic_size.
/// @param[in] is_mixed
///   See #algebraic_size.
/// @param[in] is_reduced
///   See #algebraic_size.
/// @exception std::bad_alloc

void
ew::Tps2::bending_energy(double *B, const double *F, const int *pivots,
 const int *relax_dims_opt, const double *relax_params_opt, int n_lmks,
 int f_size, bool is_mixed, bool is_reduced)
{
  int scratchsize = f_size;
  ew::AutoArray<double> scratch(scratchsize);
  ew::AutoArray<double> f_inv(f_size * f_size);
  int fn = f_size, flda = f_size, finfo;
  std::memcpy(f_inv, F, f_size * f_size * sizeof(double));
  dsytri_("L", &fn, f_inv, &flda, pivots, scratch, &finfo);
  if (finfo != 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  if (!is_reduced) {
// Copy f_inv, fill in the lower diagonal part and crop the first D1 rows and
// columns.
// Here f_size == n_lmks + D1 and B is n_lmks * n_lmks.
    for (int i = 0; i < n_lmks; i += 1) {
      for (int j = i; j < n_lmks; j += 1) {
        B[i * n_lmks + j] = B[j * n_lmks + i] =
         f_inv[(i + D1) * (n_lmks + D1) + j + D1];
      }
    }
  } else if (!is_mixed) {
// Copy f_inv, fill in the lower diagonal part, crop the first D1 rows and
// columns and insert D rows of zeros and D columns of zeros for each fully
// relaxed landmark.
// Here B is n_lmks * n_lmks.
    int a = D1;
    for (int i = 0; i < n_lmks; i += 1) {
      if (relax_dims_opt[i] == RELAX_DIM_0) {
        int b = a;
        for (int j = i; j < n_lmks; j += 1) {
          if (relax_dims_opt[j] == RELAX_DIM_0) {
            B[i * n_lmks + j] = B[j * n_lmks + i] = f_inv[a * f_size + b];
            b += 1;
          } else if (relax_dims_opt[j] == RELAX_DIM_D) {
            B[i * n_lmks + j] = B[j * n_lmks + i] = 0.0;
          }
        }
        a += 1;
      } else if (relax_dims_opt[i] == RELAX_DIM_D) {
        for (int j = i; j < n_lmks; j += 1) {
          B[i * n_lmks + j] = B[j * n_lmks + i] = 0.0;
        }
      } else {
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
    }
  } else {
// Calculate R * f_inv * R^T, using only the upper diagonal part of f_inv and
// crop the first D1 rows and columns.
// Here B is D * n_lmks * D * n_lmks.
    int a = DD1, b;
    for (int i = 0; i < n_lmks; i += 1) {
      switch (relax_dims_opt[i]) {
      case RELAX_DIM_0:
        b = a;
        for (int j = i; j < n_lmks; j += 1) {
          switch (relax_dims_opt[j]) {
          case RELAX_DIM_0:
            for (int x = 0; x < D; x += 1) {
              for (int y = 0; y < D; y += 1) {
                if (x > y && a == b ) {
                  B[(i * D + x) * n_lmks * D + j * D + y] =
                   B[(j * D + y) * n_lmks * D + i * D + x] =
                   f_inv[(b + y) * f_size + a + x];
                } else {
                  B[(i * D + x) * n_lmks * D + j * D + y] =
                   B[(j * D + y) * n_lmks * D + i * D + x] =
                   f_inv[(a + x) * f_size + b + y];
                }
              }
            }
            b += D;
            break;
          case RELAX_DIM_1:
            for (int x = 0; x < D; x += 1) {
              for (int y = 0; y < D; y += 1) {
                B[(i * D + x) * n_lmks * D + j * D + y] =
                 B[(j * D + y) * n_lmks * D + i * D + x] =
                 f_inv[(a + x) * f_size + b] * relax_params_opt[j * D + y];
              }
            }
            b += 1;
            break;
          case RELAX_DIM_D:
            for (int x = 0; x < D; x += 1) {
              for (int y = 0; y < D; y += 1) {
                B[(i * D + x) * n_lmks * D + j * D + y] =
                 B[(j * D + y) * n_lmks * D + i * D + x] = 0.0;
              }
            }
            break;
          }
        }
        a += D;
        break;
      case RELAX_DIM_1:
        b = a;
        for (int j = i; j < n_lmks; j += 1) {
          switch (relax_dims_opt[j]) {
          case RELAX_DIM_0:
            for (int x = 0; x < D; x += 1) {
              for (int y = 0; y < D; y += 1) {
                B[(i * D + x) * n_lmks * D + j * D + y] =
                 B[(j * D + y) * n_lmks * D + i * D + x] =
                 f_inv[a * f_size + (b + y)] * relax_params_opt[i * D + x];
              }
            }
            b += D;
            break;
          case RELAX_DIM_1:
            for (int x = 0; x < D; x += 1) {
              for (int y = 0; y < D; y += 1) {
                B[(i * D + x) * n_lmks * D + j * D + y] =
                 B[(j * D + y) * n_lmks * D + i * D + x] =
                 f_inv[a * f_size + b] *
                 relax_params_opt[i * D + x] * relax_params_opt[j * D + y];
              }
            }
            b += 1;
            break;
          case RELAX_DIM_D:
            for (int x = 0; x < D; x += 1) {
              for (int y = 0; y < D; y += 1) {
                B[(i * D + x) * n_lmks * D + j * D + y] =
                 B[(j * D + y) * n_lmks * D + i * D + x] = 0.0;
              }
            }
            break;
          }
        }
        a += 1;
        break;
      case RELAX_DIM_D:
        for (int j = i; j < n_lmks; j += 1) {
          for (int x = 0; x < D; x += 1) {
            for (int y = 0; y < D; y += 1) {
              B[(i * D + x) * n_lmks * D + j * D + y] =
               B[(j * D + y) * n_lmks * D + i * D + x] = 0.0;
            }
          }
        }
        break;
      default:
        throw ew::ErrorLogic(__FILE__, __LINE__);
      }
    }
  }
}

/// This calculates the principal warps.
/// If @a is_mixed, they are a basis of the full space of landmark
/// displacements.
/// Otherwise, they are a basis of the space of landmarks in a single
/// coordinate directions, either x or y.
/// It uses outputs from #bending_energy.
/// It should not be called if the thin-plate spline is singular.
/// @param[out] P
///   If @a is_mixed is @c false, this is a matrix of size
///   @a n_lmks x @a n_lmks.
///   Otherwise it is a matrix of size (2 * @a n_lmks) x (2 * @a n_lmks).
///   The eigenvectors are stored here.
///   The form an orthogonal matrix.
/// @param[out] evals
///   If @a is_mixed is @c false, this is an array of size @a n_lmks.
///   Otherwise it is an array of size 2 * @a n_lmks.
///   The eigenvalues are stored here.
///   The eigenvectors are ordered so that the eigenvalues are in
///   non-decreasing order.
/// @param[in] B
///   See #bending_energy.
/// @param[in] n_lmks
///   See #interaction.
/// @param[in] is_mixed
///   See #algebraic_size.
/// @exception std::runtime_error
///   The eigenanalysis failed to converge.
/// @exception std::bad_alloc

void
ew::Tps2::principal_warps(double *P, double *evals, const double *B,
 int n_lmks, bool is_mixed)
{
  int n = n_lmks;
  if (is_mixed) {
    n *= D;
  }
  double work0;
  std::memcpy(P, B, n * n * sizeof(double));
  int fn = n, flda = n, flwork = -1, finfo;
  dsyev_("V", "L", &fn, P, &flda, evals, &work0, &flwork, &finfo);
  flwork = 1 + static_cast<int>(work0);
  ew::AutoArray<double> work(flwork);
  dsyev_("V", "L", &fn, P, &flda, evals, work, &flwork, &finfo);
  if (finfo < 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  if (finfo > 0) {
    throw ew::ErrorRuntime(ErrorWarpsEigenanalysis);
  }
}

/// This calculates a set of principal axes for the landmark configuration.
/// The axes are not canonical, which is why their calculation and their use
/// in #uniform_basis are separated into different functions.
/// They are canonical up to sign if the sum_squares are different.
/// It should not be called with an empty configuration of landmarks.
/// @param[out] centroid
///   This points to an array of size 2 to store the centroid of the
///   configuration.
/// @param[out] sum_squares
///   This points to an array of size 2 to store the sum of squared
///   coordinates in the principal axes.
/// @param[out] axes
///   This is a matrix of size @a n_lmks x @a n_lmks.
///   It is the location to store the principal axes.
///   They form an orthogonal matrix.
/// @param[in] lmks
///   See #interaction.
/// @param[in] n_lmks
///   See #interaction.
/// @exception std::runtime_error
///   The eigenanalysis failed to converge.
/// @exception std::bad_alloc

void
ew::Tps2::principal_axes(double *centroid, double *sum_squares,
 double *axes, const double *lmks, int n_lmks)
{
  for (int i = 0; i < D; i += 1) {
    centroid[i] = 0.0;
  }
  for (int i = 0; i < n_lmks; i += 1) {
    for (int j = 0; j < D; j += 1) {
      centroid[j] += lmks[D * i + j] / n_lmks;
    }
  }
  for (int i = 0; i < D; i += 1) {
    for (int j = i; j < D; j += 1) {
      axes[D * i + j] = 0.0;
      for (int k = 0; k < n_lmks; k += 1) {
        axes[D * i + j] +=
         (lmks[D * k + i] - centroid[i]) * (lmks[D * k + j] - centroid[j]);
      }
      axes[D * j + i] = axes[D * i + j];
    }
  }
  int fn = D, flda = D, flwork = -1, finfo;
  double work0;
  dsyev_("V", "L", &fn, axes, &flda, sum_squares, &work0, &flwork, &finfo);
  flwork = 1 + static_cast<int>(work0);
  ew::AutoArray<double> work(flwork);
  dsyev_("V", "L", &fn, axes, &flda, sum_squares, work, &flwork, &finfo);
  if (finfo < 0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  if (finfo > 0) {
    throw ew::ErrorRuntime(ErrorAxesEigenanalysis);
  }
// Reverse ordering (largest first).
  double f = sum_squares[0];
  sum_squares[0] = sum_squares[D - 1];
  sum_squares[D - 1] = f;
  for (int i = 0; i < D; i += 1) {
    f = axes[i];
    axes[i] = axes[D * (D - 1) + i];
    axes[D * (D - 1) + i] = f;
  }
// Check orientation.
  if (axes[0] * axes[3] - axes[1] * axes[2] < 0.0) {
    for (int i = 0; i < D; i += 1) {
      axes[D * (D - 1) + i] *= -1.0;
    }
  }
}

/// This calculates a canonical basis of the uniform subspace of the space of
/// shape variations, given a set of principal axes for the configuration.
/// The result is relative to the original axes, not the principal axes.
/// If the configuration is degenerate (contained within a line), this should
/// not be called.
/// @param[out] basis
///   This is a matrix of size 2 x @a n_lmks x 2.
///   The shape variables of the 2 basis elements are stored here.
///   The first index is the number of the basis element.
/// @param[out] transforms_opt
///   This points to an array of size 2 * 2 * 2.
///   Uniform shape variations are linear maps, so can be represented as 2 x 2
///   matrices.
///   The matrices of the 2 basis elements are stored here.
///   The first index is the number of the basis element.
///   This can be @c NULL, in which case the transformations are not
///   calculated.
/// @param[in] centroid
///   See #principal_axes.
/// @param[in] sum_squares
///   See #principal_axes.
/// @param[in] axes
///   See #principal_axes.
/// @param[in] lmks
///   See #interaction.
/// @param[in] n_lmks
///   See #interaction.

void
ew::Tps2::uniform_basis(double *basis, double *transforms_opt,
 const double *centroid, const double *sum_squares, const double *axes,
 const double *lmks, int n_lmks)
{
  if (sum_squares[0] <= 0.0 || sum_squares[1] <= 0.0) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  double c1 = std::sqrt(sum_squares[0] / sum_squares[1]);
  double c2 = 1.0 / c1;
  for (int sh = 0; sh < 2; sh += 1) {
    double m[D * D];
    switch(sh) {
    case 0:
      m[0] = 0.0;
      m[1] = c1;
      m[2] = c2;
      m[3] = 0.0;
      break;
    case 1:
      m[0] = -c2;
      m[1] = 0.0;
      m[2] = 0.0;
      m[3] = c1;
      break;
    }
    double mr[D * D];
    for (int i = 0; i < D; i += 1) {
      for (int j = 0; j < D; j += 1) {
        mr[D * i + j] = m[D * i] * axes[j] + m[D * i + 1] * axes[D + j];
      }
    }
    double rmr[D * D];
    for (int i = 0; i < D; i += 1) {
      for (int j = 0; j < D; j += 1) {
        rmr[D * i + j] = axes[i] * mr[j] + axes[D + i] * mr[D + j];
      }
    }
    if (transforms_opt != 0) {
      for (int i = 0; i < D * D; i += 1) {
        transforms_opt[D * D * sh + i] = rmr[i];
      }
    }
    for (int i = 0; i < n_lmks; i += 1) {
      basis[D * n_lmks * sh + D * i] =
       rmr[0] * (lmks[D * i] - centroid[0]) +
       rmr[1] * (lmks[D * i + 1] - centroid[1]);
      basis[D * n_lmks * sh + D * i + 1] =
       rmr[2] * (lmks[D * i] - centroid[0]) +
       rmr[3] * (lmks[D * i + 1] - centroid[1]);
    }
  }
}
