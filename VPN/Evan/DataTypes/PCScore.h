#ifndef PCSCORE_H
#define PCSCORE_H

#include "Variables.h"

class PCScore : public Variables
{
public:
    PCScore(unsigned int row = 0, unsigned int col = 0) : Variables(row,col)
    {
        addType(PCSCORE_T);
        for (unsigned int i=0;i<col;i++)
        {
            char tmpchar[255];
            sprintf(tmpchar,"PC %i",i+1);
            setColumnLabel(i,tmpchar);
        }
        for (unsigned int i=0;i<row;i++)
        {
            char tmpchar[255];
            sprintf(tmpchar,"%i",i+1);
            setRowLabel(i,tmpchar);
        }
    }
};

#endif
