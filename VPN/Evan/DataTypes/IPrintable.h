#ifndef IPRINTABLE_H
#define IPRINTABLE_H

#include "IDataType.h"

class IPrintable : public IDataType
{

public:
    IPrintable() : IDataType()
    {addType(IPRINTABLE_T);}

    virtual ~IPrintable()
    {}

};

#endif
