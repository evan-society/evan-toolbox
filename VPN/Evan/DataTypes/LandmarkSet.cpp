#include "LandmarkSet.h"
#include "Specimens.h"
#include <osg/Point>

bool LandmarkSet::setFromContainer(DataTypePtr& specimens, unsigned int partIndex)
{
    if(specimens.isValid())
    {
        Specimens* spec = (Specimens*)specimens.getPtr();
        if(partIndex >= spec->getSize() || ( static_cast<int>(partIndex) < 0 ) )
            return false;
        *this = *(spec->getLandmarkSet(partIndex));
    }
    return true;
}
