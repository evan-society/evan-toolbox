#include "Float.h"

#ifdef USE_R_
#include "RList.h"

bool Float::setFromContainer(DataTypePtr& rList, unsigned int partIndex)
{
    if(rList.isValid() && rList->isType(RLIST_T))
    {
        RList* results = dynamic_cast<RList *>( rList.getPtr() );
        if(partIndex >= results->getSize() /* || partIndex < 0 */ )
            return false;

        DataTypePtr dt = results->getDataType(partIndex);
        if(directMatch(dt.getPtr()))
        {
            Float* real = dynamic_cast<Float *>( dt.getPtr() );
            setData(real->getData());
        }
    }
    return true;
}
#endif
