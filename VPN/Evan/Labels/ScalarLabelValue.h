#ifndef SCALARLABELVALUE_H
#define SCALARLABELVALUE_H

#include "ILabelValue.h"

class ScalarLabelValue : public ILabelValue
{
private:
    double m_value;

public:
    ScalarLabelValue() : m_value(QString("NaN").toDouble())
    {}
    ScalarLabelValue(double v) : m_value(v)
    {}

    double getValue() const {return m_value;}

    bool equals(const ILabelValue* sv)
    {
        if(sv->getType() == this->getType())
            return ((ScalarLabelValue*)sv)->getValue() == this->getValue();
        return false;
    }

    bool moreThan(const ILabelValue* sv)
    {
        if(sv->getType() == this->getType())
            return this->getValue() > ((ScalarLabelValue*)sv)->getValue();
        return false;
    }

    bool lessThan(const ILabelValue* sv)
    {
        if(sv->getType() == this->getType())
            return this->getValue() < ((ScalarLabelValue*)sv)->getValue();
        return false;
    }

    LabelValueType getType() const {return SCALAR_VALUE;}
};

#endif

