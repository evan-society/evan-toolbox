#include "INode.h"

unsigned long INode::m_execution = 0;

INode::INode() : m_delete(false), m_updated(false)
{}


INode::~INode()
{
    for(IoIterator i=m_inputs.begin(); i!=m_inputs.end(); ++i)
        delete (*i);
	m_inputs.clear();

	for(IoIterator i=m_outputs.begin(); i!=m_outputs.end(); ++i)
        delete (*i);

	m_outputs.clear();
}
