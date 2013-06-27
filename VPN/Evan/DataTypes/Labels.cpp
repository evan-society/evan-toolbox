#include "Labels.h"
#include "../Labels/StringLabelValue.h"
#include "../Labels/ScalarLabelValue.h"
#include <QStringList>

void Labels::addLabel(const QString& labelName, const QString& labelValue)
{
    bool ok = false;
    double dValue = labelValue.toDouble(&ok);
    if(ok)
        insert(labelName, new ScalarLabelValue(dValue));
    else
        insert(labelName, new StringLabelValue(labelValue));
}

QString Labels::getLabel(const QString& labelName) const
{
    QString result = "";
    if(this->contains(labelName))
    {
        ILabelValue* label = this->value(labelName);
        if(label->getType() == ILabelValue::STRING_VALUE)
            result = ((StringLabelValue*)label)->getValue();
        else if(label->getType() == ILabelValue::SCALAR_VALUE)
            result.setNum(((ScalarLabelValue*)label)->getValue());
    }
    return result;
}

bool Labels::match(const QString& expr)
{
    bool result = false;
    QStringList tokens = expr.split("$::$");
    if(this->contains(tokens[0]))
    {
        ILabelValue* passedValue = NULL;
        bool ok = false;
        double dValue = tokens[2].toDouble(&ok);
        if(ok)  passedValue = new ScalarLabelValue(dValue);
        else    passedValue = new StringLabelValue(tokens[2]);

        if(tokens[1] == "=")
            result = this->value(tokens[0])->equals(passedValue);
        else if(tokens[1] == "<")
            result = this->value(tokens[0])->lessThan(passedValue);
        else if(tokens[1] == ">")
            result = this->value(tokens[0])->moreThan(passedValue);
        else if(tokens[1] == ">=")
            result = this->value(tokens[0])->moreThan(passedValue) || this->value(tokens[0])->equals(passedValue);
        else if(tokens[1] == "<=")
            result = this->value(tokens[0])->lessThan(passedValue) || this->value(tokens[0])->equals(passedValue);
        else if(tokens[1] == "!=")
            result = !this->value(tokens[0])->equals(passedValue);

        delete passedValue;
    }
    return result;
}
