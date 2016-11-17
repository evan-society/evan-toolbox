#ifndef MATRIXD_H
#define MATRIXD_H

#include "IPrintable.h"
#include <Matrix.h>

class MatrixD : public Matrix<double>, public IPrintable
{
public:
    MatrixD (size_t row = 0, size_t col = 0) : Matrix<double>(row,col), IPrintable()
    {
        addType(MATRIXD_T);
        addPartOfRelation(RLIST_T);
    }
    MatrixD (const MatrixD& copy) : Matrix<double>(copy), IPrintable(copy)
    {}

    virtual ~MatrixD()
    {}

    virtual bool isValid() const {return !IsEmpty();}
    void reAssign(int x,int y, double** mat)
    {
        FreeMatrix();
        SetSize(x,y);
        for (int i=0;i<x;i++)
            for (int j=0;j<y;j++)
                matrix[i][j] = mat[i][j];
    }
    void reAssign(int x, double* vec)
    {
        FreeMatrix();
        SetSize(x,1);
        for (int i=0;i<x;i++)
            matrix[i][0] = vec[i];
    }
#ifdef USE_R_
    virtual bool setFromContainer(DataTypePtr&, unsigned int);
#endif
};

#endif
