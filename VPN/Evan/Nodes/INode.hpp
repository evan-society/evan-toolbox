#pragma once
#include "../DataTypes/DataTypeFactory.h"

inline void INode::addInput(const string& it, const string& desc, NodePort::Optionality optional)
{
    NodePort* input = new NodePort;
    input->dataType.acquire(DataTypeFactory::getDataType(it));
    input->description = desc;
    input->inputOptionality = optional;
    m_inputs.push_back(input);
}

inline void INode::addOutput(const string& ot, const string& desc)
{
    NodePort* output = new NodePort;
    output->dataType.acquire(DataTypeFactory::getDataType(ot));
    output->description = desc;
    m_outputs.push_back(output);
}

inline void INode::setOutputData(int index, IDataType* inPtr)
{
    m_outputs[index]->dataType = DataTypePtr(inPtr);
    if(m_outputs[index]->dataType.isValid())
    {
//        m_outputs[index]->connected = NodePort::CONNECTED;
        m_outputs[index]->executionID = ++m_execution;
    }
}

inline QVector<int> INode::isFullyConnected(bool& ok) const
{
    QVector<int> missingInputs;
    bool oneOfOptionalConnected = false;
    bool oneOfOptionalExist = false;
    ok = true;

    for(int i=0; i<m_inputs.size(); ++i)
    {
        if(!isInputConnected(i))
        {
            switch(m_inputs[i]->inputOptionality)
            {
                case NodePort::MANDATORY: missingInputs.push_back(i); ok = false; break;
                case NodePort::OPTIONAL: break;
                case NodePort::ONE_OF_OPTIONAL:
                    oneOfOptionalConnected = oneOfOptionalConnected || false;
                    oneOfOptionalExist = true;
                    missingInputs.push_back(i);
                    break;
            }
        }
        else if(isInputConnected(i) &&
                m_inputs[i]->inputOptionality==NodePort::ONE_OF_OPTIONAL)
        {
            oneOfOptionalConnected = true;
            oneOfOptionalExist = true;
        }
    }

    ok = (oneOfOptionalExist ? oneOfOptionalConnected&&ok : ok);
    return missingInputs;
}

inline bool INode::isRunnable() const
{
    bool connected = false;
    isFullyConnected(connected);
    if(!connected)
        return false;
    for(int i=0; i<m_inputs.size(); ++i)
        if(isInputConnected(i) && !m_inputs[i]->dataType.isValid())
            return false;
    return true;
}
