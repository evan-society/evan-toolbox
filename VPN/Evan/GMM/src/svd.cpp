#include <svd.h>

// The function dgesvd_ is stored in the clapack.dll
// This dll has to be accessible if we want to call this function.

// Convert data from a double pointer to a single pointer
double * DoubleToSingle(double ** doublePtr, int rows, int cols);

// Convert data from a single pointer to a double pointer
void SingleToDouble(double * singlePtr, double ** doublePtr, int rows, int cols);

// Convert data from a single pointer to a double pointer and transpose
void SingleToDoubleTranspose(double * singlePtr, double ** doublePtr, int rows, int cols);

extern "C" void dgesvd_(char *jobu, char *jobvt, int *m, int *n,
             double *a, int *lda, double *s, double *u,
             int *ldu, double *vt, int *ldvt, double *work,
             int *lwork, int *info);

// Convert data from a double pointer to a single pointer
double * DoubleToSingle(double ** doublePtr, int rows, int cols)
{
    double *singlePtr = new double[rows*cols];
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<cols; j++)
        {
            singlePtr[i+j*rows] = doublePtr[i][j];
        }
    }
    return singlePtr;
}

// Convert data from a single pointer to a double pointer
void SingleToDouble(double * singlePtr, double ** doublePtr, int rows, int cols)
{
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<cols; j++)
        {
            doublePtr[i][j] = singlePtr[i+j*rows];
        }
    }
}

// Convert data from a single pointer to a double pointer and transpose
void SingleToDoubleTranspose(double * singlePtr, double ** doublePtr, int rows, int cols)
{
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<cols; j++)
        {
            doublePtr[j][i] = singlePtr[i+j*rows];
        }
    }
}

// A wrapper for the function dgesvd_
int svd(Matrix<double> A, Matrix<double> * U, Matrix<double> * S, Matrix<double> * V, char jobu, char jobvt) //RP 28/12/08
{
    /*
        jobu:    Specifies options for computing U.
                 A: all M columns of U are returned in array U;
                 S: the first min(m,n) columns of U (the left
    		        singular vectors) are returned in the array U;
    		     O: the first min(m,n) columns of U (the left
                    singular vectors) are overwritten on the array A;
                 N: no columns of U (no left singular vectors) are
                    computed.

        jobvt   Specifies options for computing VT.
                A: all N rows of V**T are returned in the array VT;
                S: the first min(m,n) rows of V**T (the right
                   singular vectors) are returned in the array VT;
                O: the first min(m,n) rows of V**T (the right
    		       singular vectors) are overwritten on the array A;
    		    N: no rows of V**T (no right singular vectors) are
    		       computed.
    */

    int lda, ldu, ldvt, lwork, info;
    double *a, *s, *u, *vt, *work;

    int m = A.GetRows();
    int n = A.GetCols();

    // If A is not a square matrix, we have to make some decisions
    // based on which dimension is shorter.
    int minmn = std::min(m,n);
    int maxmn = std::max(m,n);

    // Make sure dimensions are correct
    S->ReSize(m,n);
    U->ReSize(m,m);
    V->ReSize(n,n);

    // The leading dimension of the matrix a.
    lda = m;

    // Convert the matrix A from double pointer
    // C form to single pointer Fortran form.
    a = DoubleToSingle(A.GetMatrix(), m, n);

    // Left singular vector matrix
    ldu = m;
    u = new double[m*m];

    // Right singular vector matrix
    ldvt = n;

    vt = new double[n*n];

    // Set up the work array, larger than needed.
    lwork = 5 * maxmn;
    work = new double[lwork];

    s = new double[minmn];
    dgesvd_(&jobu, &jobvt, &m, &n, a, &lda, s, u,
            &ldu, vt, &ldvt, work, &lwork, &info);

    for (int i=0;i<minmn;i++)
    {
        (*S)[i][i] = s[i];
    }

    // Convert back to double pointer form
    SingleToDouble(u, U->GetMatrix(), m, m);

    // dgesvd_ returns V transposed so we need to correct
    SingleToDoubleTranspose(vt, V->GetMatrix(), n, n);

    delete[] a;
    delete[] s;
    delete[] u;
    delete[] vt;
    delete[] work;

    return info;
}

Row<double> svd(Matrix<double> A)
{
    int lda, ldu, ldvt, lwork, info;
    double *a, *s, *u, *vt, *work;
    char jobu = 'N';
    char jobvt = 'N';

    int m = A.GetRows();
    int n = A.GetCols();

    // If A is not a square matrix, we have to make some decisions
    // based on which dimension is shorter.
    int minmn = std::min(m,n);
    int maxmn = std::max(m,n);

    Row<double> S(minmn);

    // The leading dimension of the matrix a.
    lda = m;

    // Convert the matrix A from double pointer
    // C form to single pointer Fortran form.
    a = DoubleToSingle(A.GetMatrix(), m, n);

    // Left singular vector matrix
    ldu = m;
    u = new double[m*m];

    // Right singular vector matrix
    ldvt = n;
    vt = new double[n*n];

    // Set up the work array, larger than needed.
    lwork = 5 * maxmn;
    work = new double[lwork];

    s = new double[minmn];
    dgesvd_(&jobu, &jobvt, &m, &n, a, &lda, s, u,
            &ldu, vt, &ldvt, work, &lwork, &info);

    for (int i=0;i<minmn;i++)
    {
        S[i] = s[i];
    }

    delete[] a;
    delete[] s;
    delete[] u;
    delete[] vt;
    delete[] work;

    return S;
}

int svd(const Matrix<double> A, Matrix<double> * S)
{
    Row<double> rowS = svd(A);
    int success = -1;
    if (!rowS.IsEmpty())
    {
        S->ReSize(A.GetRows(), A.GetCols());
        for (size_t i=0;i<rowS.GetCols();i++)
        {
            S->GetMatrix()[i][i] = rowS[i];
        }
        success = 0;
    }
    return success;
}

int svd(const Matrix<double> A, Row<double> * S)
{
    *S = svd(A);
    return 0;
}

