#ifndef PROCRUSTESDISTANCES_H
#define PROCRUSTESDISTANCES_H

#include "MatrixD.h"

class ProcrustesDistances : public MatrixD
{
public:
    ProcrustesDistances(size_t row = 0, size_t col = 0) : MatrixD(row,col)
    {addType(PROCRUSTESDISTANCES_T);}

};

#endif
