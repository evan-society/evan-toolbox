#ifndef LMKSETRENDERABLE_H
#define LMKSETRENDERABLE_H

#include "IRenderable.h"
#include <QColor>

class LMKSetRenderable : public IRenderable
{
private:
    unsigned int m_rows;
    unsigned int m_cols;
    float m_landmarkSize;
    double** m_data;

public:
    LMKSetRenderable() : m_rows(0), m_cols(0), m_landmarkSize(3.0f), m_data(NULL)
    {}

    bool initialize(const QString&);
    void update();
    void setLandmarkSize(float s) {m_landmarkSize = s;}
    float getLandmarkSize() const {return m_landmarkSize;}
    void setData(unsigned int r, unsigned int c, double** mat)
    {
        m_rows = r;
        m_cols = c;
        m_data = mat;
    }
    bool isValid() const {return (m_data && m_rows && m_cols);}
};

#endif

