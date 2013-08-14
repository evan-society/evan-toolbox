#ifndef CONCATNODE_H
#define CONCATNODE_H

#include "../INode.h"
#include "../../DataTypes/Specimens.h"

class ConcatNode : public INode
{
private:
	Specimens* m_specimensInput0;
	Specimens* m_specimensInput1;
	Specimens* m_specimensOutput;

protected:
	Specimens* getInputPortData0()const { return dynamic_cast<Specimens *>( getInputData( 0 ) ); }
	Specimens* getInputPortData1()const { return dynamic_cast<Specimens *>( getInputData( 1 ) ); }
	void setOutputPort0(Specimens* p){setOutputData(0,p);}

public:
	ConcatNode();
	~ConcatNode();

	void process();
};
#endif
