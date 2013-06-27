#ifndef STRINGLABELVALUE_H
#define STRINGLABELVALUE_H

#include "ILabelValue.h"
#include <QString>

class StringLabelValue : public ILabelValue
{
private:
    QString m_value;

public:
    StringLabelValue() : m_value("")
    {}
    StringLabelValue(const QString& v) : m_value(v)
    {}

    const QString& getValue() const {return m_value;}

    bool equals(const ILabelValue* sv)
    {
        if(sv->getType() == this->getType())
            return ((StringLabelValue*)sv)->getValue() == this->getValue();
        return false;
    }

    bool moreThan(const ILabelValue* sv)
    {
        if(sv->getType() == this->getType())
            return this->getValue() > ((StringLabelValue*)sv)->getValue();
        return false;
    }

    bool lessThan(const ILabelValue* sv)
    {
        if(sv->getType() == this->getType())
            return this->getValue() < ((StringLabelValue*)sv)->getValue();
        return false;
    }

    LabelValueType getType() const {return STRING_VALUE;}
};

#endif
