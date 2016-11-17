#include "DataTypeFactory.h"
#include "MatrixD.h"
#include "VectorD.h"
#include "LandmarkSet.h"
#include "Surface.h"
#include "SurfaceVector.h"
#include "Specimens.h"
#include "Loadings.h"
#include "Warpings.h"
#include "PCScore.h"
#include "ProcrustesDistances.h"
#include "TransformMatrix.h"
#include "Float.h"
#include "FloatVector.h"
#include "Renderables.h"
#include "CentroidSizes.h"
#include "Variables.h"
#include "WarpGrid.h"
#include "Volume.h"
#include "Printables.h"

#ifdef USE_R_
#include "RList.h"
#endif

IDataType* DataTypeFactory::getDataType(const string& dataType)
{
    if(dataType == "MatrixD")
        return new MatrixD();
    else if(dataType == "VectorD")
        return new VectorD();
    else if(dataType == "LandmarkSet")
        return new LandmarkSet();
    else if(dataType == "Surface")
        return new Surface();
    else if(dataType == "SurfaceVector")
        return new SurfaceVector();
    else if(dataType == "Specimens")
        return new Specimens();
    else if(dataType == "PCScore")
        return new PCScore();
    else if(dataType == "Loadings")
        return new Loadings();
    else if(dataType == "Warpings")
        return new Warpings();
    else if(dataType == "ProcrustesDistances")
        return new ProcrustesDistances();
    else if(dataType == "TransformMatrix")
        return new TransformMatrix();
    else if(dataType == "Float")
        return new Float();
    else if(dataType == "FloatVector")
        return new FloatVector();
#ifdef USE_R_
    else if(dataType == "RList")
        return new RList();
#endif
    else if(dataType == "Renderables")
        return new Renderables();
    else if(dataType == "CentroidSizes")
        return new CentroidSizes();
    else if(dataType == "Variables")
        return new Variables();
    else if(dataType == "WarpGrid")
        return new WarpGrid();
    else if(dataType == "MyClipPlane")
        return new MyClipPlane();
    else if(dataType == "GroupWarpGrids")
        return new GroupWarpGrids();
    else if(dataType == "Volume")
        return new Volumes();
    else if(dataType == "Printables")
            return new Printables();
        return NULL;
}
