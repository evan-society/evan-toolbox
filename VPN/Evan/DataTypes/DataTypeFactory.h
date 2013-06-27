#ifndef DATATYPEFACTORY_H
#define DATATYPEFACTORY_H

#include "IDataType.h"
#include <string>
using namespace std;

class DataTypeFactory
{
public:
	static IDataType* getDataType(const string&);
};

#endif
