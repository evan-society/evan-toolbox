#include <QGraphicsScene>
#include "Scheduler.h"
#include "../Nodes/NodeLink.h"
#include "../Nodes/INode.h"
#include "../Utilities/Logger.h"


Scheduler::~Scheduler()
{
    removeLinks();
}

void Scheduler::addPortLink(INode* startNode, INode* endNode, PortLink* l)
{
    int linkIndex = findLink(startNode, endNode);
    if(linkIndex == -1)
    {
        NodeLink* newLink = new NodeLink;
        newLink->startNode = startNode;
        newLink->endNode = endNode;
        newLink->portLinks.push_back(l);
        m_links.push_back(newLink);
    }
    else
    {
        int insertIndex = 0;
        for(portLinkIterator i=m_links[linkIndex]->portLinks.begin(); i!=m_links[linkIndex]->portLinks.end(); ++i, ++insertIndex)
            if((*i)->getInputPortIndex() > l->getInputPortIndex())
                break;
        m_links[linkIndex]->portLinks.insert(insertIndex, l);
    }
    connect(l, SIGNAL(refreshLink()), this, SLOT(start()));
}

void Scheduler::validateLinks()
{
    for(int i=0; i<m_links.size(); ++i)
    {
        portLinkIterator j=m_links[i]->portLinks.begin();
        while( j!=m_links[i]->portLinks.end())
        {
            if( (*j)->toBeDeleted() )
            {
                (*j)->scene()->removeItem(*j);
                delete (*j);
                j = m_links[i]->portLinks.erase(j);
            }
            else
                ++j;
        }
    }

    linkIterator i=m_links.begin();
	while( i!=m_links.end())
    {
        if( (*i)->portLinks.size() == 0 )
        {
            delete (*i);
            i = m_links.erase(i);
        }
        else
            ++i;
    }
}

void Scheduler::flagLink(PortLink* link)
{
    for(linkIterator i=m_links.begin(); i!=m_links.end(); ++i)
    {
        int portIndex = (*i)->portLinks.indexOf(link);
        if(portIndex != -1)
            (*i)->portLinks[portIndex]->flagDelete();
    }
}

void Scheduler::flagLinks(INode* node)
{
    if(node)
    {
        for(linkIterator i=m_links.begin(); i!=m_links.end(); ++i)
        {
            if((*i)->startNode==node || (*i)->endNode==node)
                for(int j=0; j<(*i)->portLinks.size(); ++j)
                    (*i)->portLinks[j]->flagDelete();
        }
    }
}

void Scheduler::removeLinks()
{
    for(int i=0; i<m_links.size(); ++i)
        delete m_links[i];
	m_links.clear();
}

bool Scheduler::findPortLink(INode* startNode, INode* endNode, int outIndex, int inIndex)
{
    for(int i=0; i<m_links.size(); ++i)
		if( m_links[i]->startNode==startNode && m_links[i]->endNode==endNode)
            for(int j=0; j<m_links[i]->portLinks.size(); ++j)
                if( m_links[i]->portLinks[j]->getOutputPortIndex() == outIndex &&
                    m_links[i]->portLinks[j]->getInputPortIndex() == inIndex)
                    return true;
    return false;
}

int Scheduler::findLink(INode* startNode, INode* endNode)
{
     for(int i=0; i<m_links.size(); ++i)
		if( m_links[i]->startNode==startNode && m_links[i]->endNode==endNode)
            return i;
    return -1;
}

bool Scheduler::start()
{
    if(!m_running)
        return false;

    // find process order
    QList<INode*> allNodes;
    QList<INode*> orderedNodes;
    QList<INode*> startNodes;
    QList<INode*> endNodes;
    QList<NodeLink*> orderedLinks;

    // find all nodes that are in the network
    for(linkIterator j=m_links.begin(); j!=m_links.end(); ++j)
    {
        if (!allNodes.contains((*j)->startNode))
        {
            allNodes.push_back((*j)->startNode);
        }
        if (!allNodes.contains((*j)->endNode))
        {
            allNodes.push_back((*j)->endNode);
        }
        startNodes.push_back((*j)->startNode);
        endNodes.push_back((*j)->endNode);
    }

    // find the order the nodes/links should be processed
    for (int k=0;k<allNodes.size();k++)
    {
        int selectedIndex = -1;

        // find a node that does not have any inputs and put it first in the order
        for(int i=0;i<allNodes.size();i++)
        {
            if(!endNodes.contains(allNodes[i]) && !orderedNodes.contains(allNodes[i]))
            {
                selectedIndex = i;
                break;
            }
        }

        // network is circular, all nodes have inputs
        // just pick first one that has not been selected already
        if(selectedIndex == -1)
        {
            for(int i=0;i<allNodes.size();i++)
            {
                if(!orderedNodes.contains(allNodes[i]))
                {
                    selectedIndex = i;
                    break;
                }
            }
        }
        // remove all links starting from the selected node
        for(int i=0;i<startNodes.size();i++)
        {
            if(startNodes[i] == allNodes[selectedIndex])
            {
                startNodes[i] = 0;
                endNodes[i] = 0;
                // put this link next in process order
                orderedLinks.push_back(m_links[i]);
            }
        }
        orderedNodes.push_back(allNodes[selectedIndex]);
    }

    for(linkIterator j=orderedLinks.begin(); j!=orderedLinks.end(); ++j)
    {
        INode* endNode = (*j)->endNode;
        INode* startNode = (*j)->startNode;
        bool connected = false;
        QVector<int> missingPorts = endNode->isFullyConnected(connected);
        if(!connected) //Singal Input Port Error
        {
            foreach(int portIndex, missingPorts)
                emit inputMissing(endNode, portIndex,
                                endNode->getInputPort(portIndex)->inputOptionality==NodePort::MANDATORY? Qt::red : Qt::yellow);
            continue;
        }

        bool ouputRequired = false;
        for(portLinkIterator i=(*j)->portLinks.begin(); i!=(*j)->portLinks.end(); ++i)
            if(!(*i)->dataAvailable())
                ouputRequired = true;
        if( (ouputRequired || startNode->isUpdated()) && startNode->isRunnable())
            startNode->process();

        //Check for missing parameters
        for(portLinkIterator i=(*j)->portLinks.begin(); i!=(*j)->portLinks.end(); ++i)
                emit outputMissing(startNode, (*i)->getOutputPortIndex(),
                                    (*i)->dataAvailable()? Qt::transparent : Qt::red);
        //

        bool refreshInputs = false;
        for(portLinkIterator i=(*j)->portLinks.begin(); i!=(*j)->portLinks.end(); ++i)
            if((*i)->updated())
            {
                refreshInputs = true;
                (*i)->link();
                if(endNode->isRunnable())
                    if(endNode->inputChanged((*i)->getInputPortIndex()))
                        refreshInputs = false;
            }
        if( (refreshInputs || endNode->isUpdated()) && endNode->isRunnable())
            endNode->process();
    }

    return true;
}


bool Scheduler::forceStart()
{
    for (int k=0;k<m_nodes.size();k++)
    {
        if(m_nodes[k] != 0)
        {
            m_nodes[k]->reset();
        }
    }

    return true;
}

void Scheduler::addNode(INode* node)
{
    m_nodes.append(node);
}

void Scheduler::removeNode(INode* node)
{
    if(m_nodes.contains(node))
    {
        m_nodes.removeAll(node);
    }
}
