#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>


// A Matrix is a 2 dimensional container for data.
template <class T>
class Matrix
{
public:

    // Default constructor
    Matrix (size_t row = 0, size_t col = 0)
    {
       SetSize(row,col);
    }

    // Destructor
    virtual ~Matrix()
    {
        FreeMatrix();
    }

    // Copy constructor
    Matrix (const Matrix<T>& m)
    {
        SetSize(m.GetRows(),m.GetCols());
        for (size_t i=0;i<rows;i++)
        {
            for (size_t j=0;j<cols;j++)
            {
                matrix[i][j] = m[i][j];
            }
        }
    }

    // Set the size of the matrix
    void SetSize (size_t row, size_t col)
    {
        rows = row;
        cols = col;
        if (rows > 0 && cols > 0)
        {
            AllocateMatrix();
        }
        else
        {
            matrix = NULL;
        }
    }

    // Change the size of the matrix
    void ReSize (size_t row, size_t col)
    {
        if (row == rows && col == cols)
        {
            Zeros();
            return;
        }
        FreeMatrix();
        SetSize(row,col);
    }

    // Change the size of the matrix
    void ReSizeQuick (size_t row, size_t col)
    {

        if(matrix != NULL)
        {
            // free memory of matrix
            delete matrix[0];
            delete [] matrix;
            matrix = NULL;
        }

        rows = row;
        cols = col;
        if (rows > 0 && cols > 0)
        {
            matrix = new T* [rows];
            matrix[0] = new T[rows*cols];
            for(size_t i=0;i<rows;i++)
            {
                matrix[i] = matrix[0] + i*cols;
            }
            memset(matrix[0],0x00,sizeof(T)*cols*rows/sizeof(char));
        }
    }

    // Assignment operators
    Matrix<T>& operator = (const Matrix<T>& m)
    {
        ReSize(m.GetRows(),m.GetCols());
        for (size_t i=0;i<rows;i++)
        {
            for (size_t j=0;j<cols;j++)
            {
                matrix[i][j] = m[i][j];
            }
        }
        return *this;
    }

    // Get number of rows
    size_t GetRows () const
    {
        return rows;
    }

    // Get number of columns
    size_t GetCols () const
    {
        return cols;
    }

    // Get matrix row
    T*& operator[](int row)
    {
        return matrix[row];
    }

    // Get matrix row
    T* operator[](int row) const
    {
        return matrix[row];
    }

    void set(size_t i,size_t j,T value)
    {
        matrix[i][j] = value;
    }

    T get(size_t i,size_t j)
    {
        return matrix[i][j];
    }

    // We want to be able to choose if we want to use (i,j) or [i][j]
    T& operator () (size_t row, size_t col);
    T  operator () (size_t row, size_t col) const;

    // Overloaded operators
    Matrix<T> operator + ();
    Matrix<T> operator - ();

    Matrix<T>& operator += (const T& c);
    Matrix<T>& operator -= (const T& c);
    Matrix<T>& operator *= (const T& c);
    Matrix<T>& operator /= (const T& c);

    Matrix<T>& operator += (const Matrix<T>& m);
    Matrix<T>& operator -= (const Matrix<T>& m);
    Matrix<T>& operator *= (const Matrix<T>& m);
    Matrix<T>& operator /= (const Matrix<T>& m);

    Matrix<T>& operator ^= (const int pow);

    void SwapRows(size_t n1, size_t n2);
    void SwapCols(size_t m1, size_t m2);

    // Fill matrix
    void Zeros ();
    void Ones ();
    void Identity ();

    // Utility methods
    Matrix<T> Inv ();
    T Det () const;
    T Norm () const;
    T TotalSum () const;

    // Check matrix
    bool IsSquare () const;
    bool IsSingular () const;
    bool IsDiagonal () const;
    bool IsScalar () const;
    bool IsIdentity () const;
    bool IsEmpty () const;
    bool IsNull () const;
    bool IsSymmetric () const;
    bool IsUpperTriangular () const;
    bool IsLowerTriangular () const;

    void FreeMatrix()
    {
        if(matrix != NULL)
        {
            // free memory of matrix
            delete matrix[0];
            delete [] matrix;
            matrix = NULL;
        }
    }

    T** GetMatrix() {return matrix;}
    T* GetSinglePtr() {return matrix[0];}
    void transform(const Matrix<T>& trans)                        //YS: 29/01/09 to be used by any DataType containing a Matrix
    {
        Matrix<T> temp(4,1);
        for(unsigned int i=0; i<rows; ++i)
        {
            temp[0][0] = matrix[i][0];
            temp[1][0] = matrix[i][1];
            temp[2][0] = matrix[i][2];
            temp[3][0] = 1;
            temp = trans * temp;
            matrix[i][0] = temp[0][0];
            matrix[i][1] = temp[1][0];
            matrix[i][2] = temp[2][0];
        }
    }

protected:

    T** matrix;
    size_t rows;
    size_t cols;

    void AllocateMatrix()
    {
        matrix = new T* [rows];
        matrix[0] = new T[rows*cols];
        for(size_t i=0;i<rows;i++)
        {
            matrix[i] = matrix[0] + i*cols;
        }
// memset fails for Matrix<string>
// use Zeros to set all elements to zero
        Zeros();
    }
};

// A Row is a (1 x m) Matrix
template <class T>
class Row : public Matrix<T>
{
public:

    // Default constructor
    Row (size_t col = 0) : Matrix<T>(1,col)
    {
    }

    // Destructor
    virtual ~Row()
    {
    }

    // Set the size and allocate memory
    void SetRowSize(size_t col)
    {
        this->SetSize(1,col);
    }

    // Get the size
    size_t GetSize() const
    {
        return this->cols;
    }

    T& operator[](int idx)
    {
        return this->matrix[0][idx];
    }

    T operator[](int idx) const
    {
        return this->matrix[0][idx];
    }

    T& operator () (size_t idx)
    {
        return this->matrix[0][idx-1];
    }

    T  operator () (size_t idx) const
    {
        return this->matrix[0][idx-1];
    }

    // Assignment operator
    Row<T>&
    operator = (const Row<T> &r)
    {
        SetRowSize(r.GetSize());
        for (size_t i=0;i<r.GetSize();i++)
        {
            this->matrix[0][i] = r[i];
        }
        return *this;
    }
};

// A Column is an (n x 1) Matrix
template <class T>
class Column : public Matrix<T>
{
public:

    // Default constructor
    Column (size_t n = 0) : Matrix<T>(n,1)
    {
    }

    // Destructor
    virtual ~Column()
    {
    }

    // Set the size and allocate memory
    void SetColumnSize(size_t n)
    {
        this->SetSize(n,1);
    }

    // Get the size
    size_t GetSize() const
    {
        return this->rows;
    }

    T& operator[](int idx)
    {
        return this->matrix[idx][0];
    }

    T operator[](int idx) const
    {
        return this->matrix[idx][0];
    }

    T& operator () (size_t idx)
    {
        return this->matrix[idx-1][0];
    }

    T  operator () (size_t idx) const
    {
        return this->matrix[idx-1][0];
    }

    // Assignment operator
    Column<T>&
    operator = (const Column<T> &c)
    {
        SetColumnSize(c.GetSize());
        for (size_t i=0;i<c.GetSize();i++)
        {
            this->matrix[i][0] = c[i];
        }
        return *this;
    }
};

// Report Matrix Error
inline void MatrixError (const char* msg)
{
    std::cout << msg << std::endl;
    exit(1);
}
/*
template <class T> inline void
swap(T *a, T *b)
{
    T temp = *b;
    *b = *a;
    *a = temp;
}
*/

inline void swap(double *a, double *b)
{
    double temp = *b;
    *b = *a;
    *a = temp;
}

template <class T> inline void
Matrix<T>::SwapRows(size_t n1, size_t n2)
{
    for (size_t i=0;i<cols;i++)
    {
        T temp = matrix[n2-1][i];
        matrix[n2-1][i] = matrix[n1-1][i];
        matrix[n1-1][i] = temp;
    }
}

template <class T> inline void
Matrix<T>::SwapCols(size_t m1, size_t m2)
{
    for (size_t i=0;i<rows;i++)
    {
        T temp = matrix[i][m2-1];
        matrix[i][m2-1] = matrix[i][m1-1];
        matrix[i][m1-1] = temp;
    }
}

// subscript operator to set individual elements
template <class T> inline T&
Matrix<T>::operator () (size_t row, size_t col)
{
    if (row == 0 || row >= rows + 1 || col == 0 || col >= cols + 1)
    {
        MatrixError( "Matrix<T>::operator (): Index out of range!");
    }
    return matrix[row-1][col-1];
}

// subscript operator to get individual elements
template <class T> inline T
Matrix<T>::operator () (size_t row, size_t col) const
{
    if (row == 0 || row >= rows + 1 || col == 0 || col >= cols + 1)
    {
        MatrixError( "Matrix<T>::operator (): Index out of range!");
    }
    return matrix[row-1][col-1];
}

// combined addition and assignment operator
template <class T> inline Matrix<T>&
Matrix<T>::operator += (const T& c)
{
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            matrix[i][j] += c;
        }
    }
    return *this;
}

// combined addition and assignment operator
template <class T> inline Matrix<T>&
Matrix<T>::operator += (const Matrix<T>& m)
{
    if (rows != m.GetRows() || cols != m.GetCols())
    {
        MatrixError( "Matrix<T>::operator += : Inconsistent Matrix sizes in addition!");
    }

    for (size_t i=0; i < m.GetRows(); i++)
    {
        for (size_t j=0; j < m.GetCols(); j++)
        {
            matrix[i][j] += m[i][j];
        }
    }
    return *this;
}

// combined subtraction and assignment operator
template <class T> inline Matrix<T>&
Matrix<T>::operator -= (const T& c)
{
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            matrix[i][j] -= c;
        }
    }
    return *this;
}

// combined subtraction and assignment operator
template <class T> inline Matrix<T>&
Matrix<T>::operator -= (const Matrix<T>& m)
{
    if (rows != m.GetRows() || cols != m.GetCols())
    {
        MatrixError( "Matrix<T>::operator -= : Inconsistent Matrix sizes in subtraction!");
    }

    for (size_t i=0; i < m.GetRows(); i++)
    {
        for (size_t j=0; j < m.GetCols(); j++)
        {
            matrix[i][j] -= m[i][j];
        }
    }
    return *this;
}

// combined scalar multiplication and assignment operator
template <class T> inline Matrix<T>&
Matrix<T>::operator *= (const T& c)
{
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            matrix[i][j] *= c;
        }
    }
    return *this;
}

// combined Matrix multiplication and assignment operator
template <class T> inline Matrix<T>&
Matrix<T>::operator *= (const Matrix<T>& m)
{
    if (cols != m.GetRows())
    {
        MatrixError( "Matrix<T>::operator *= : Inconsistent Matrix sizes in multiplication!");
    }

    Matrix<T> temp(rows,m.GetCols());

    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < m.GetCols(); j++)
        {
            temp[i][j] = T(0);
            for (size_t k=0; k < cols; k++)
            {
                temp[i][j] += matrix[i][k] * m[k][j];
            }
        }
    }
    *this = temp;

    return *this;
}

// combined scalar division and assignment operator
template <class T> inline Matrix<T>&
Matrix<T>::operator /= (const T& c)
{
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            matrix[i][j] /= c;
        }
    }
    return *this;
}

// combined power and assignment operator
template <class T> inline Matrix<T>&
Matrix<T>::operator ^= (const int pow)
{
    Matrix<T> temp(*this);
    int i = 1;
    while (i < pow)
    {
        *this *= temp;
        i++;
    }
    return *this;
}

// unary transpose operator
template <class T> inline Matrix<T>
operator ~ (const Matrix<T>& m)
{
    Matrix<T> temp(m.GetCols(),m.GetRows());
    for (size_t i=0; i < m.GetRows(); i++)
    {
        for (size_t j=0; j < m.GetCols(); j++)
        {
            temp[j][i] = m[i][j];
        }
    }
    return temp;
}

// transpose row operator
template <class T> inline Column<T>
operator ~ (const Row<T>& r)
{
    Column<T> temp(r.GetSize());
    for (size_t i=0; i < r.GetSize(); i++)
    {
        temp[i] = r[i];
    }
    return temp;
}

// transpose column operator
template <class T> inline Row<T>
operator ~ (const Column<T>& c)
{
    Row<T> temp(c.GetSize());
    for (size_t i=0; i < c.GetSize(); i++)
    {
        temp[i] = c[i];
    }
    return temp;
}

// unary inverse operator
template <class T> inline Matrix<T>
operator ! (const Matrix<T>& m)
{
    Matrix<T> temp(m);
    temp.Inv();
    return temp;
}

// unary plus operator
template <class T> inline Matrix<T>
Matrix<T>::operator + ()
{
    return *this;
}

// unary negation operator
template <class T> inline Matrix<T>
Matrix<T>::operator - ()
{
    Matrix<T> temp(rows,cols);
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            temp[i][j] = - matrix[i][j];
        }
    }
    return temp;
}

// binary addition operator
template <class T> inline Matrix<T>
operator + (const Matrix<T>& m1, const Matrix<T>& m2)
{
    if (m1.GetRows() != m2.GetRows() || m1.GetCols() != m2.GetCols())
    {
        MatrixError( "Matrix<T>::operator + : Inconsistent Matrix sizes in addition!");
    }
    Matrix<T> temp(m1);
    temp += m2;
    return temp;
}

// binary addition operator
template <class T> inline Matrix<T>
operator + (const Matrix<T>& m, const T& c)
{
    Matrix<T> temp(m);
    temp += c;
    return temp;
}

// binary addition operator
template <class T> inline Matrix<T>
operator + (const T& c, const Matrix<T>& m)
{
    return m + c;
}

// binary subtraction operator
template <class T> inline Matrix<T>
operator - (const Matrix<T>& m1, const Matrix<T>& m2)
{
    if (m1.GetRows() != m2.GetRows() || m1.GetCols() != m2.GetCols())
    {
        MatrixError( "Matrix<T>::operator - : Inconsistent Matrix sizes in subtraction!");
    }

    Matrix<T> temp(m1);
    temp -= m2;
    return temp;
}

// binary subtraction operator
template <class T> inline Matrix<T>
operator - (const Matrix<T>& m, const T& c)
{
    Matrix<T> temp(m);
    temp -= c;
    return temp;
}

// binary subtraction operator
template <class T> inline Matrix<T>
operator - (const T& c, const Matrix<T>& m)
{
    Matrix<T> temp(m);
    for (size_t i=0; i < m.GetRows(); i++)
    {
        for (size_t j=0; j < m.GetCols(); j++)
        {
            temp[i][j] = c - m[i][j];
        }
    }
    return temp;
}

// binary scalar multiplication operator
template <class T> inline Matrix<T>
operator * (const Matrix<T>& m, const T& c)
{
    Matrix<T> temp(m);
    temp *= c;
    return temp;
}

// binary scalar multiplication operator
template <class T> inline Matrix<T>
operator * (const T& c, const Matrix<T>& m)
{
    return m * c;
}

// binary Matrix multiplication operator
template <class T> inline Matrix<T>
operator * (const Matrix<T>& m1, const Matrix<T>& m2)
{
    if (m1.GetCols() != m2.GetRows())
    {
        MatrixError( "operator * : Inconsistent Matrix sizes in multiplication!");
    }
    Matrix<T> temp(m1);
    temp *= m2;
    return temp;
}

// binary scalar division operator
template <class T> inline Matrix<T>
operator / (const Matrix<T>& m, const T& c)
{
    Matrix<T> temp(m);
    temp /= c;
    return temp;
}

// binary scalar division operator
template <class T> inline Matrix<T>
operator / (const T& c, const Matrix<T>& m)
{
    return (!m * c);
}

// binary Matrix division operator
template <class T> inline Matrix<T>
operator / (const Matrix<T>& m1, const Matrix<T>& m2)
{
    return (m1 * !m2);
}

// binary power operator
template <class T> inline Matrix<T>
operator ^ (const Matrix<T>& m, const size_t& pow)
{
    Matrix<T> temp(m);
    temp ^= pow;
    return temp;
}

// output stream function for Matrix
template <class T> inline std::ostream&
operator << (std::ostream& out, Matrix<T>& m)
{
    for (size_t i=0; i < m.GetRows(); i++)
    {
        out << m[i][0];
        for (size_t j=1; j < m.GetCols(); j++)
        {
              out  << "\t" << m[i][j] ;
        }
        out << std::endl;
    }
    return out;
}

// logical equal-to operator
template <class T> inline bool
operator == (const Matrix<T>& m1, const Matrix<T>& m2)
{
    if (m1.GetRows() != m2.GetRows() || m1.GetCols() != m2.GetCols())
    {
        return false;
    }
    for (size_t i=0; i < m1.GetRows(); i++)
    {
        for (size_t j=0; j < m1.GetCols(); j++)
        {
            if (m1[i][j] != m2[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

// logical not-equal-to operator
template <class T> inline bool
operator != (const Matrix<T>& m1, const Matrix<T>& m2)
{
    return !(m1 == m2);
}

// calculate the (euclidian) norm of a Matrix
template <class T> inline T
Matrix<T>::Norm () const
{
    T retval = T(0);
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            retval += matrix[i][j] * matrix[i][j];
        }
    }
    retval = sqrt(retval);
    return retval;
}

// Determine if the Matrix is square
template <class T> inline bool
Matrix<T>::IsSquare () const
{
    return (rows == cols);
}

// Determine if the Matrix is singular
template <class T> inline bool
Matrix<T>::IsSingular () const
{
    if (rows != cols)
    {
        return false;
    }
    return (Det() == T(0));
}

// Determine if the Matrix is diagonal
template <class T> inline bool
Matrix<T>::IsDiagonal () const
{
    if (rows != cols)
    {
        return false;
    }
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            if (i != j && matrix[i][j] != T(0))
            {
                return false;
            }
        }
    }
    return true;
}

// Determine if the Matrix is scalar
template <class T> inline bool
Matrix<T>::IsScalar () const
{
    if (!IsDiagonal())
    {
        return false;
    }
    T val = matrix[0][0];
    for (size_t i=1; i < rows; i++)
    {
        if (matrix[i][i] != val)
        {
            return false;
        }
    }
    return true;
}

// Determine if the Matrix is an identity Matrix
template <class T> inline bool
Matrix<T>::IsIdentity () const
{
    return (IsScalar() && matrix[0][0] == T(1));
}

// Determine if this is an empty Matrix
template <class T> inline bool
Matrix<T>::IsEmpty () const
{
    return ( (rows == 0) || (cols == 0) );
}

// Determine if all elements are zero
template <class T> inline bool
Matrix<T>::IsNull () const
{
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            if (matrix[i][j] != T(0))
            {
                return false;
            }
        }
    }
    return true;
}

// Determine if the Matrix is symmetric
template <class T> inline bool
Matrix<T>::IsSymmetric () const
{
    if (rows != cols)
    {
        return false;
    }
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            if (abs(matrix[i][j] - matrix[j][i]) > 0.000001)
            {
                return false;
            }
        }
    }
    return true;
}

// Determine if the Matrix is upper triangular
template <class T> inline bool
Matrix<T>::IsUpperTriangular () const
{
    if (rows != cols)
    {
        return false;
    }
    for (size_t i=1; i < rows; i++)
    {
        for (size_t j=0; j < i; j++)
        {
            if (matrix[i][j] != T(0))
            {
                return false;
            }
        }
    }
    return true;
}

// Determine if the Matrix is lower triangular
template <class T> inline bool
Matrix<T>::IsLowerTriangular () const
{
    if (rows != cols)
    {
        return false;
    }
    for (size_t j=1; j < cols; j++)
    {
        for (size_t i=0; i < j; i++)
        {
            if (matrix[i][j] != T(0))
            {
                return false;
            }
        }
    }
    return true;
}

// Find the sum of all elements in each column
template <class T> inline Row<T>
Sum (Matrix<T> mat)
{
    size_t cols = mat.GetCols();
    size_t rows = mat.GetRows();
    Row<T> row(cols);
    for (size_t j=0; j < cols; j++)
    {
        T sum = T(0);
        for (size_t i=0; i < rows; i++)
        {
            sum += mat[i][j];
        }
        row[j] = sum;
    }
    return row;
}

// Find the mean of each column
template <class T> inline Row<T>
Mean (Matrix<T> mat)
{
    size_t cols = mat.GetCols();
    size_t rows = mat.GetRows();
    Row<T> row(cols);
    for (size_t j=0; j < cols; j++)
    {
        T sum = T(0);
        for (size_t i=0; i < rows; i++)
        {
            sum += mat[i][j];
        }
        row[j] = sum / rows;
    }
    return row;
}

// Find the sum of all elements in the Matrix
template <class T> inline T
Matrix<T>::TotalSum () const
{
    T sum = T(0);
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            sum += matrix[i][j];
        }
    }
    return sum;
}

// Find the sum of all elements in the Matrix
template <class T> inline T
TotalSum (Matrix<T> mat)
{
    return mat.TotalSum();
}

// Find the cumulative sum of all elements by columns
template <class T> inline Row<T>
CumSum (Matrix<T> mat)
{
    size_t cols = mat->GetCols();
    size_t rows = mat->GetRows();
    Row<T> row(cols);
    T sum = T(0);
    for (size_t j=0; j < cols; j++)
    {
        for (size_t i=0; i < rows; i++)
        {
            sum += mat[i][j];
        }
        row[j] = sum;
    }
    return row;
}

// set all elements to zero
template <class T> inline void
Matrix<T>::Zeros ()
{
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            matrix[i][j] = T(0);
        }
    }
}

// set all elements to one
template <class T> inline void
Matrix<T>::Ones ()
{
    for (size_t i=0; i < rows; i++)
    {
        for (size_t j=0; j < cols; j++)
        {
            matrix[i][j] = T(1);
        }
    }
}

// set this Matrix to identity matrix
template <class T> inline void
Matrix<T>::Identity ()
{
    Zeros();
    for (size_t i=0; i < rows; i++)
    {
        matrix[i][i] = T(1);
    }
    return;
}

// Inverse matrix using Gauss-Jordan elimination with partial pivoting
template <class T> inline Matrix<T>
Matrix<T>::Inv ()
{
    if (rows != cols)
    {
        MatrixError( "Matrix<T>::Inv(): Inversion of a non-square Matrix");
    }

    size_t i,j,k;
    size_t n = rows;

    Matrix<T> temp(*this);

    // We begin with an identity matrix
    Matrix<T> inverse(n,n);
    inverse.Identity();

    for (k=0;k<n;k++)
    {
        // Find pivot.
        size_t p = k;
        for (i = k+1; i < n; i++)
        {
            if (fabs(temp[i][k]) > fabs(temp[p][k]))
            {
                p = i;
            }
        }

        // Swap rows if necessary.
        if (p != k)
        {
            temp.SwapRows(k+1,p+1);
            inverse.SwapRows(k+1,p+1);
        }

        T x = temp[k][k];
        if (x == T(0))
        {
            MatrixError( "Matrix<T>::Inv(): Matrix is singular");
        }
        for (j=0;j<n;j++)
        {
            if (j>k)
            {
                temp[k][j] /= x;    // Don't bother with previous entries
            }
            inverse[k][j] /= x;
        }
        for (i=0;i<n;i++)
        {
            // Loop over rows
            if (i == k)
            {
                continue;
            }
            x = temp[i][k];
            for (j=0;j<n;j++)
            {
                if (j>k)
                {
                    temp[i][j] -= temp[k][j] * x;
                }
                inverse[i][j] -= inverse[k][j] * x;
            }
        }
    }

    *this = inverse;

    return *this;
}

// Calculate determinant using Gauss-Jordan elimination with partial pivoting
template <class T> inline T
Matrix<T>::Det () const
{
    if (rows != cols)
    {
        MatrixError( "Matrix<T>::Det(): Matrix must be square");
    }

    size_t i,j,k;
    size_t n = rows;
    int sign = 1;

    Matrix<T> temp(*this);

    for (k=0;k<n;k++)
    {
        // Find pivot.
        size_t p = k;
        for (i = k+1; i < n; i++)
        {
            if (fabs(temp[i][k]) > fabs(temp[p][k]))
            {
                p = i;
            }
        }
        // Swap rows if necessary.
        if (p != k)
        {
            temp.SwapRows(k+1,p+1);
            sign = -sign;
        }

        if (temp[k][k] == T(0))
        {
            // Matrix is singular, return zero.
            return T(0);
        }
        for (j=k+1;j<n;j++)
        {
            temp[k][j] /= temp[k][k];    // Don't bother with previous entries
        }
        for (i=0;i<n;i++)
        {
            // Loop over rows
            if (i == k)
            {
                continue;
            }
            for (j=k+1;j<n;j++)
            {
                temp[i][j] -= temp[k][j] * temp[i][k];
            }
        }
    }

    // Find determinant.
    T det = T(1) * sign;
    for (i = 0; i < n; i++)
    {
        det *= temp[i][i];
    }

    return det;
}

template <class T> Matrix<T>
Inverse (Matrix<T> m)
{
    return m.Inv();
}

template <class T> Matrix<T>
Transpose (Matrix<T> m)
{
    return ~m;
}

template <class T> T
Det (Matrix<T> m)
{
    return m.Det();
}


#endif // MATRIX_H
