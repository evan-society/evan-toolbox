#ifndef NODEPORT_H
#define NODEPORT_H

#include <string>
#include "../DataTypes/DataTypePtr.h"
using namespace std;


struct NodePort
{
    enum Optionality {MANDATORY, OPTIONAL, ONE_OF_OPTIONAL};
    enum Connectivity{UNCONNECTED, CONNECTED, PART_CONNECTED};

    DataTypePtr dataType;
    Connectivity connected;
    Optionality inputOptionality;
    unsigned long executionID;
    unsigned int partsConnected;
    string description;

    NodePort() : connected(UNCONNECTED), inputOptionality(MANDATORY), executionID(0), partsConnected(0)
    {}
};

#endif
