#ifndef GENERATESURFACENODE_H
#define GENERATESURFACENODE_H

#include "../INode.h"
#include "../../DataTypes/LandmarkSet.h"
#include "../../DataTypes/Surface.h"

class GenerateSurfaceNode : public INode
{
private:
	LandmarkSet* m_sourceLMKInput;
	LandmarkSet* m_targetLMKInput;
	Surface* m_surfaceInput;
	Surface* m_surfaceOutput;

protected:
	LandmarkSet* getInputPortData0()const {return (LandmarkSet*)getInputData(0);}
	LandmarkSet* getInputPortData1()const {return (LandmarkSet*)getInputData(1);}
	Surface* getInputPortData2()const {return (Surface*)getInputData(2);}
	void setOutputPort0(Surface* p){setOutputData(0,p);}

public:
	GenerateSurfaceNode();
	~GenerateSurfaceNode();
	void process();
	bool isUpdated() const {return false;}
};
#endif
