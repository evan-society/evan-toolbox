#ifndef ILABELVALUE_H
#define ILABELVALUE_H

class ILabelValue
{
public:
    enum LabelValueType {STRING_VALUE, SCALAR_VALUE};

    ILabelValue(){}
    virtual ~ILabelValue(){}

    virtual bool equals(const ILabelValue*) = 0;
    virtual bool moreThan(const ILabelValue*) = 0;
    virtual bool lessThan(const ILabelValue*) = 0;
    virtual LabelValueType getType() const = 0;
};

#endif
