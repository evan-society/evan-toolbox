#include "MatrixD.h"
#include "DataTypePtr.h"

#ifdef USE_R_
#include "RList.h"

bool MatrixD::setFromContainer(DataTypePtr& rList, unsigned int partIndex)
{
    if(rList.isValid() && rList->isType(RLIST_T))
    {
        RList* results = dynamic_cast<RList *>( rList.getPtr() );
        if(partIndex >= results->getSize() || partIndex < 0)
            return false;
        DataTypePtr dt = results->getDataType(partIndex);
//        if(matches(dt.getPtr()))
        {
            MatrixD* mat = dynamic_cast<MatrixD *>( dt.getPtr() );
            reAssign(mat->GetRows(), mat->GetCols(), mat->GetMatrix());
        }
    }
    return true;
}

#endif
