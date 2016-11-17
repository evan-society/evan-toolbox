#ifndef LABELS_H
#define LABELS_H

#include <QMap>
#include "IDataType.h"
#include "../Labels/ILabelValue.h"

class Labels : public QMap<QString, ILabelValue*>, public IDataType
{
public:
    Labels() : IDataType()
    {}
    virtual ~Labels()
    {
        QMap<QString, ILabelValue*>::const_iterator i = this->constBegin();
        while (i != this->constEnd())
        {
            if(i.value())
                delete i.value();
            ++i;
        }

    }
    bool match(const QString&);
    void addLabel(const QString&, const QString&);
    QString getLabel(const QString&) const;
    bool isValid() const {return (size() > 0);}
};

#endif

