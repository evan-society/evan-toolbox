#ifndef REALISENODE_H
#define REALISENODE_H

#include "../INode.h"
#include "../../DataTypes/LandmarkSet.h"
#include "../../DataTypes/Loadings.h"
#include "../../DataTypes/FloatVector.h"

class RealiseNode : public INode
{
private:
	LandmarkSet* m_specimensInput;
	Loadings* m_loadings;
	FloatVector* m_warpValues;
	LandmarkSet* m_specimensOutput;

protected:
	LandmarkSet* getInputPortData0()const { return dynamic_cast<LandmarkSet *>( getInputData( 0 ) ); }
	Loadings* getInputPortData1()const { return dynamic_cast<Loadings *>( getInputData( 1 ) ); }
	FloatVector* getInputPortData2()const { return dynamic_cast<FloatVector *>( getInputData( 2 ) ); }
	void setOutputPort0(LandmarkSet* p){setOutputData(0,p);}

public:
	RealiseNode();
	~RealiseNode();

	void process();

};
#endif
