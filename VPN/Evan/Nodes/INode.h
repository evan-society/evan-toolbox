#ifndef INODE_H
#define INODE_H

#include <QString>
#include "NodePort.h"
using namespace std;

class QObject;

class INode
{
private:
    static unsigned long m_execution;

    typedef QVector<NodePort*>::iterator IoIterator;
    QVector<NodePort*> m_inputs;
    QVector<NodePort*> m_outputs;

    string m_name;
    string m_type;
    bool m_delete;

protected:
    void setOutputData(int index,IDataType* inPtr);
    //void setInputData(int index, IDataType* outPtr){ m_inputs[index]->dataType = DataTypePtr(outPtr);}
    //IDataType* getOutputData(int index)     const  {return m_outputs[index]->dataType.getPtr();}
    IDataType* getInputData(int index)      const  {return m_inputs[index]->dataType.getPtr();}

    bool m_updated;

public:
	INode();
    virtual ~INode();

    void setName(const string& name){m_name = name;}
    const string& getName()    const{return m_name;}
    void setType(const string& type){m_type = type;}
    const string& getType()    const{return m_type;}

    void unflagDelete()        {m_delete=false;}
    void flagDelete()        {m_delete=true;}
    bool toBeDeleted() const {return m_delete;}

    void addInput(const string&, const string&, NodePort::Optionality);
    void addOutput(const string&, const string&);

    void setInputConnected(int index, NodePort::Connectivity flag)
    {
        m_inputs[index]->connected = flag;
        if(flag == NodePort::PART_CONNECTED)
            m_inputs[index]->partsConnected++;
    }
    void setOutputConnected(int index, NodePort::Connectivity flag) { m_outputs[index]->connected = flag;}
    bool isInputConnected(int index)        const {return (m_inputs[index]->connected != NodePort::UNCONNECTED);}
    bool isOutputConnected(int index)       const {return (m_outputs[index]->connected != NodePort::UNCONNECTED);}
    int getInputsCount()                    const {return m_inputs.size();}
    int getOutputsCount()                   const {return m_outputs.size();}
    string getInputDescription(int i)       const {return m_inputs[i]->description;}
    string getOutputDescription(int i)      const {return m_outputs[i]->description;}
//    unsigned int getInputPartCount(int i)   const {return m_inputs[i]->partsConnected;}
//    unsigned int getOutputPartCount(int i)  const {return m_outputs[i]->partsConnected;}
    NodePort* getInputPort(int index)       const {return m_inputs[index];}
    NodePort* getOutputPort(int index)      const {return m_outputs[index];}

    NodePort::Connectivity getInputConnectivity(int index) const  {return m_inputs[index]->connected;}
    QVector<int> isFullyConnected(bool&) const;
    bool isRunnable() const;

	virtual void showGUI()                    {};
	virtual const QObject* getDialog()  const {return NULL;}
	virtual QString toString()          const {return "";}
	virtual void fromString(const QString&)   {}
    virtual bool inputChanged(int)            {return false;}

    virtual bool isUpdated() const  {return m_updated;}
	virtual void process()          = 0;

    virtual void reset(){m_updated = true;}

};

extern "C" INode* createNode();

#include "INode.hpp"
#endif
