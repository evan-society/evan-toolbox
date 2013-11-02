#ifndef FLOATVECTOR_H
#define FLOATVECTOR_H

#include "Float.h"

class FloatVector : public IDataType
{
private:
    QVector<DataTypePtr> m_floats;

public:
    FloatVector() : IDataType(), m_floats()
    {
        addType(FLOATVECTOR_T);
        addContainsRelation(FLOAT_T);
    }
    ~FloatVector()
    {clear();}

    void addFloat(double f)             {m_floats.push_back(DataTypePtr(new Float(f)));}
    void addFloat(Float* f)             {m_floats.push_back(DataTypePtr(f));}
    void setFloat(int i, Float* f)      {m_floats[i] = DataTypePtr(f);}
    void setFloat(int i, double f)      {m_floats[i] = DataTypePtr(new Float(f));}

    double getFloat(int i)              const {return  dynamic_cast<Float *>( m_floats[i].getPtr() )->getData(); }

    void clear()
    {
        for(int i=0; i<m_floats.size(); ++i)
            m_floats[i].release();
        m_floats.clear();
    }
    unsigned int getSize()              const {return m_floats.size();}

    void addPart(DataTypePtr& part, const QString& desc)
    {
        int i = m_floats.indexOf(part);
        if(i >= 0 ) m_floats[i] = part;
        else        m_floats.push_back(part);
    }
    void removePart(DataTypePtr& part)
    {
        int i = m_floats.indexOf(part);
        if(i >= 0 )
        {
            m_floats.remove(i);
            //part.release();
        }
    }
    bool isValid() const {return m_floats.size() > 0;}
};

#endif

