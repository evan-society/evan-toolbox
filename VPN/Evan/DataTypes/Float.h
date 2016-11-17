#ifndef FLOAT_H
#define FLOAT_H

#include "IDataType.h"
#include "DataTypePtr.h"

class Float : public IDataType
{
private:
    double m_fData;
public:
    Float() : m_fData(0.0f)
    {
        addType(FLOAT_T);
        addPartOfRelation(RLIST_T);
    }

    Float(double f) : m_fData(f)
    {addType(FLOAT_T);}

    double getData() const {return m_fData;}
    void setData(double f) {m_fData = f;}

    void copyData(const DataTypePtr& f){ (static_cast<Float*>(f.getPtr()))->setData(m_fData);}
    bool isValid() const {return true;}

#ifdef USE_R_
    virtual bool setFromContainer(DataTypePtr& rList, unsigned int);
#endif
};

#endif
