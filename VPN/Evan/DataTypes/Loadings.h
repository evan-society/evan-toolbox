#ifndef LOADINGS_H
#define LOADINGS_H

#include "Variables.h"

class Loadings : public Variables
{
private:
    vector<double> m_mean;
    vector<double> m_min;
    vector<double> m_max;
    int m_numScores;

public:
    Loadings(size_t row = 0, size_t col = 0) :  Variables(row,col),
                                                m_mean(col)
    {
        addType(LOADINGS_T);
        m_numScores = 0;
    }

    void setMean(vector<double> &mean) {m_mean = mean;}
    vector<double>* getMean() {return &m_mean;} // ditto

    void setMin(vector<double> &min) {m_min = min;}
    vector<double>* getMin() {return &m_min;} // ditto
    double getMin(unsigned int i)
    {
        if(i>=m_min.size())
            return 0.0;
        return m_min[i];
    }

    void setMax(vector<double> &max) {m_max = max;}
    vector<double>* getMax() {return &m_max;} // ditto
    double getMax(unsigned int i)
    {
        if(i>=m_max.size())
            return 1.0;
        return m_max[i];
    }

    void setNumScores(int numScores)
    {
        m_numScores = numScores;
    }
    int getNumScores()
    {
        return m_numScores;
    }

#ifdef USE_R_
    bool setFromContainer(DataTypePtr& rList, unsigned int partIndex)
    {
        return MatrixD::setFromContainer(rList,partIndex);
    }
#endif
    bool isValid() const {return !IsEmpty();}

};

#endif
