#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include "INode.h"

class NodeFactory
{
public:
	static INode* getNode(const QString&);
};

#endif
