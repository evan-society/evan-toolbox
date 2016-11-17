#ifndef WARPINGS_H
#define WARPINGS_H

#include "MatrixD.h"

class Warpings : public IDataType
{
private:
    QVector<DataTypePtr> m_warpMatrices;
    QVector<double> m_sizeLoadings;

public:
    Warpings() : IDataType(), m_warpMatrices()
    {
        addType(WARPINGS_T);
        addContainsRelation(MATRIXD_T);
    }
    ~Warpings()
    {clear();}

    void addWarp(MatrixD* ls)           {m_warpMatrices.push_back(DataTypePtr(ls));}
    void setWarp(int i, MatrixD* ls)
    {
        if(i<(int)getSize())
            m_warpMatrices[i] = DataTypePtr(ls);
        else
            addWarp(ls);
    }

    MatrixD* getWarp(int i)             const { return dynamic_cast<MatrixD *>( m_warpMatrices.value(i).getPtr() ); }

    void clear()
    {
        for(int i=0; i<m_warpMatrices.size(); ++i)
            m_warpMatrices[i].release();
        m_warpMatrices.clear();
        m_sizeLoadings.clear();
    }
    unsigned int getSize()              const {return m_warpMatrices.size();}

    void addPart(DataTypePtr& part, const QString& desc)
    {
        int i = m_warpMatrices.indexOf(part);
        if(i >= 0 ) m_warpMatrices[i] = part;
        else        m_warpMatrices.push_back(part);
    }
    void removePart(DataTypePtr& part)
    {
        int i = m_warpMatrices.indexOf(part);

        if(i >= 0 )
        {
            m_warpMatrices.remove(i);
            //part.release();
        }
    }

    void addSizeLoadings(double loadings)
    {
        m_sizeLoadings.push_back(loadings);
    }
    double getSizeLoadings(int i)
    {
        if(i<m_sizeLoadings.size())
        {
            return m_sizeLoadings[i];
        }
        return 0.0;
    }

    bool isValid() const {return m_warpMatrices.size() > 0;}
};

#endif
