#ifndef CENTROIDSIZES_H
#define CENTROIDSIZES_H

#include "IDataType.h"
#include "Variables.h"

class CentroidSizes : public Variables
{
public:
    CentroidSizes(size_t n = 0) : Variables(n,1)
    {
        addType(CENTROIDSIZES_T);
        setColumnLabel(0, "Centroid Sizes");
    }

    bool isValid() const {return !IsEmpty();}
};

#endif
