#ifndef VECTORD_H
#define VECTORD_H

#include "MatrixD.h"

class VectorD : public MatrixD
{
public:
    VectorD (size_t size = 0) : MatrixD(1,size)
    {addType(VECTORD_T);}
    virtual ~VectorD()
    {}

    double& operator[](int idx)
    {
        return this->matrix[0][idx];
    }

    double operator[](int idx) const
    {
        return this->matrix[0][idx];
    }

    int GetSize()
    {
        return GetCols();
    }

    void ReSize(int i)
    {
        MatrixD::ReSize(1,i);
    }

    double* GetVector()
    {return matrix[0];}

    virtual bool isValid() const {return !IsEmpty();}
};

#endif
