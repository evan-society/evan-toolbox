#ifndef TRANSFORMMATRIX_H
#define TRANSFORMMATRIX_H

#include "IPrintable.h"
#include "Matrix.h"
#include <QVector>

class TransformMatrix : public IPrintable
{
private:
    QVector<Matrix<double>*> m_transformMats;
public:
    TransformMatrix() : IPrintable()
    {addType(TRANSFORMMATRIX_T);}

    virtual ~TransformMatrix()
    {clear();}

    void addTransformMatrix(Matrix<double>* mat) {m_transformMats.push_back(mat);}
    Matrix<double>* getTransformMatrix(int i) const {return m_transformMats[i];}

    void clear()
    {
        for(int i=0; i<m_transformMats.size(); i++)
            delete m_transformMats[i];
        m_transformMats.clear();
    }
    unsigned int getSize() const {return m_transformMats.size();}
    bool isValid() const {return m_transformMats.size() > 0;}
};

#endif
