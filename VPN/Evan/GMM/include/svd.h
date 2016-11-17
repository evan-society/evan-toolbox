#ifndef SVD_H_
#define SVD_H_

#include <Matrix.h>


// A wrapper for the lapack function dgesvd_

int svd(Matrix<double> A, Matrix<double> * U, Matrix<double> * S, Matrix<double> * V, char jobu = 'A', char jobvt='A');

int svd(const Matrix<double> A, Matrix<double> * S);

int svd(const Matrix<double> A, Row<double> * S);

Row<double> svd(Matrix<double> A);

#endif // SVD_H_
