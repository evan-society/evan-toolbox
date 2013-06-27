#include "DropArea.h"
#include "../Nodes/NodeFactory.h"
#include "../Utilities/Logger.h"
#include <QtGui>
#include "VPNLayout.h"
#include "../Nodes/TemplandNode/TemplandNode.h"

DropArea::DropArea(QWidget *parent):	QGraphicsScene(parent),
										m_mode(NODE_MOVE),
										m_startNodeIndex(-1),
										m_endNodeIndex(-1),
										m_selectedNode(""),
//										m_selectedNodeIcon(),
										m_link(NULL),
										m_zoom(100)
{
    //m_schedulerTimer = startTimer(500);
    QObject::connect(&m_scheduler, SIGNAL(inputMissing(INode*, int, const QColor&)),
                    this, SLOT(colorInputPort(INode*, int, const QColor&)));
    QObject::connect(&m_scheduler, SIGNAL(outputMissing(INode*, int, const QColor&)),
                    this, SLOT(colorOutputPort(INode*, int, const QColor&)));
}

DropArea::~DropArea()
{
    m_scheduler.removeLinks();
	for(NodeIterator i=m_sceneNodes.begin(); i!=m_sceneNodes.end(); ++i)
		delete (*i);
	m_sceneNodes.clear();
	for(NodeIconIterator i=m_sceneNodeIcons.begin(); i!=m_sceneNodeIcons.end(); ++i)
		delete (*i);
	m_sceneNodeIcons.clear();
}

void DropArea::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_mode == VPN_RUN)
        return;
    if (m_mode == NODE_CONNECT && m_link)
    {
        QPainterPath newPath(m_linkStartPos);
        newPath.lineTo(mouseEvent->scenePos());
        m_link->setPath(newPath);
        addItem(m_link);
    }
	else
		QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void DropArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_mode == VPN_RUN)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }
	if(m_mode == NODE_CONNECT)
	{
        if(m_link)
        {
            foreach(QGraphicsItem* sceneItem, items(mouseEvent->scenePos()))
		    {
		        NodeGraphicsItem* endNodeIcon = dynamic_cast<NodeGraphicsItem*>(sceneItem);
		        if(endNodeIcon)
		        {
		            m_endNodeIndex = getSelectedNode(endNodeIcon);
                    if(m_endNodeIndex != -1)
                    {
                        INode* endNode = m_sceneNodes[m_endNodeIndex];
                        int inputIndex = endNodeIcon->getInputPortIndex(mouseEvent->scenePos());
                        if(inputIndex != -1)
                        {
                            int outputIndex = m_link->getOutputPortIndex();
                            if(canConnect(outputIndex, inputIndex))
                            {
                                insertPortLink(outputIndex, inputIndex);
                                emit linkCreated();
                            }
                            else if ( endNode->getInputConnectivity(inputIndex) != NodePort::CONNECTED )
                                QMessageBox::information(0, "Connection error", QString("Cannot Connect Ports.\n")+
                                                                            "Incompatible Data Types. Or Port Already connected!");
                        }
                    }
		        }
		    }
		    removeItem(m_link);
            delete m_link;
            m_link = NULL;
            refreshView();
        }

	}
	m_mode = NODE_MOVE;
	QGraphicsScene::mouseReleaseEvent(mouseEvent);

}

void DropArea::insertNode(const QPointF& pos,const QString& annotation)
{
    if(m_selectedNode != "")
    {
        INode* insertedNode = NodeFactory::getNode(m_selectedNode);
        if(insertedNode)
        {
//            QGraphicsPixmapItem* addedIcon = addPixmap(m_selectedNodeIcon);
            NodeGraphicsItem* addedIcon = new NodeGraphicsItem(insertedNode);
            addedIcon->setPos(pos);
            addedIcon->setFlag(QGraphicsItem::ItemIsMovable, true);
            addedIcon->setFlag(QGraphicsItem::ItemIsSelectable, true);
            addedIcon->setAnnotation(annotation);
            addItem(addedIcon);
            insertedNode->fromString(m_selectedNodeParams);
            m_sceneNodes.push_back(insertedNode);
            m_sceneNodeIcons.push_back(addedIcon);
            if(insertedNode->getType()=="AnnotationNode")
            {
                addedIcon->changeToAnnotation();
            }
            m_scheduler.addNode(insertedNode);
        }
        else
            Logger::getInstance()->log("Couldn't create Node!\nCheck node configuration file.", Logger::SYS_ERROR);

        emit nodeInserted();
    }
}

void DropArea::insertLink(int sIndex, int eIndex, const QString& portLinks)
{
    m_startNodeIndex = sIndex;
    m_endNodeIndex = eIndex;
    QStringList ports = portLinks.split(",");
    foreach(QString portLinkData, ports)
    {
        if(portLinkData != "" && !portLinkData.isEmpty())
        {
            QStringList portIndeces = portLinkData.split(":");
            if(portIndeces.count() < 3)
                continue;
            QPointF cp1, cp2;
            if(portIndeces.count() == 5)
            {
                QStringList cp1Coord = portIndeces[3].split("|");
                QStringList cp2Coord = portIndeces[4].split("|");
                if(cp1Coord.count()==2 && cp2Coord.count()==2)
                {
                    cp1.setX(cp1Coord[0].toDouble());
                    cp1.setY(cp1Coord[1].toDouble());

                    cp2.setX(cp2Coord[0].toDouble());
                    cp2.setY(cp2Coord[1].toDouble());
                }
            }
            insertPortLink(portIndeces[0].toInt(),portIndeces[1].toInt(), true, portIndeces[2].toInt(), cp1, cp2);
        }
    }
}

void DropArea::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton && m_mode != VPN_RUN)
	{
	    switch(m_mode)
        {
        case NODE_INSERT:
            insertNode(mouseEvent->scenePos(), QString(""));
            break;

        case NODE_MOVE:
            {
                foreach(QGraphicsItem* sceneItem, items(mouseEvent->scenePos()))
                {
                    NodeGraphicsItem* startNodeIcon = dynamic_cast<NodeGraphicsItem*>(sceneItem);
                    if(startNodeIcon)
                    {
                        m_startNodeIndex = getSelectedNode(startNodeIcon);
                        if(m_startNodeIndex != -1)
                        {
                            INode* startNode = m_sceneNodes[m_startNodeIndex];
                            int outputIndex = startNodeIcon->getOutputPortIndex(mouseEvent->scenePos());
                            if(outputIndex != -1)
                            {
                                m_link = new PortLink(QPainterPath(mouseEvent->scenePos()));
                                m_linkStartPos = mouseEvent->scenePos();
                                m_link->setOutputPort(startNode->getOutputPort(outputIndex),outputIndex);
                                m_link->setZValue(-1000.0);
                                m_mode = NODE_CONNECT;
                                QGraphicsScene::mousePressEvent(mouseEvent);
                                markIncompatibleInputs();
                                return;
                            }
                        }
                    }
                }
                m_link = NULL;
            }
            break;
        default: break;
        }
	}
    else if (mouseEvent->button() == Qt::RightButton && m_mode != VPN_RUN)
    {
        foreach(QGraphicsItem* sceneItem, items(mouseEvent->scenePos()))
        {
            NodeGraphicsItem* startNodeIcon = dynamic_cast<NodeGraphicsItem*>(sceneItem);
            if(startNodeIcon)
            {
                m_startNodeIndex = getSelectedNode(startNodeIcon);
                if(m_startNodeIndex != -1)
                {
                    QMenu* contextMenu = new QMenu();
                    QAction *setTextAction = new QAction("Set Text", contextMenu);
                    contextMenu->addAction(setTextAction);
                    QObject::connect(setTextAction, SIGNAL(triggered()), this, SLOT(setNodeText()));

                    contextMenu->exec(QCursor::pos());
                    delete contextMenu;
                }
            }
        }
    }
	QGraphicsScene::mousePressEvent(mouseEvent);
}

void DropArea::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton )
		return;

    if(m_mode == NODE_MOVE || m_mode == VPN_RUN)
	{
        foreach (QGraphicsItem *item, items(mouseEvent->scenePos()))
        {
            int nodeIndex = getSelectedNode(item);
            if(nodeIndex != -1)
                m_sceneNodes[nodeIndex]->showGUI();
        }
	}
	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

int DropArea::getSelectedNode(const QGraphicsItem* item)
{
	for(int i=0; i<m_sceneNodeIcons.size(); ++i)
		if( m_sceneNodeIcons[i] == item)
			return i;
	return -1;
}

bool DropArea::flagListItems(const QGraphicsItem* item)
{
    for(NodeIconIterator i=m_sceneNodeIcons.begin(); i!=m_sceneNodeIcons.end(); ++i)
		if( (*i) == item )
        {
            m_sceneNodes.at(m_sceneNodeIcons.indexOf(*i))->flagDelete();
            return true;
        }
    return false;
}

void DropArea::deleteSelection()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    foreach (QGraphicsItem *item, selectedItems())
    {
        // first check if this is Annotation node
        QGraphicsItem *title = item->parentItem();
        QGraphicsItem *node =NULL;

        if (title)
        {
            node = title->parentItem();
        }

        if(title != 0 && !title->isVisible() && node != 0 && !node->isVisible())
        {
            m_sceneNodes[getSelectedNode(node)]->flagDelete();
            removeItem(node);
        }
        else if(!flagListItems(item))
        {
            m_scheduler.flagLink((PortLink*)item);
        }
        else
        {
            m_scheduler.removeNode(m_sceneNodes[getSelectedNode(item)]);

            if(m_sceneNodes[getSelectedNode(item)]->getType() == "TemplandNode")
            {
                if(((TemplandNode*)m_sceneNodes[getSelectedNode(item)])->canDelete())
                {
                    m_scheduler.flagLinks(m_sceneNodes[getSelectedNode(item)]);
                    removeItem(item);
                }
                else
                {
                    m_sceneNodes[getSelectedNode(item)]->unflagDelete();
                    Logger::getInstance()->log("Cannot delete open Templand Node");
                }
            }
            else
            {
                m_scheduler.flagLinks(m_sceneNodes[getSelectedNode(item)]);
                removeItem(item);
            }
        }
    }
    m_scheduler.validateLinks();
    validateNodes();
    QApplication::restoreOverrideCursor();
}

void DropArea::validateNodes()
{
    NodeIterator j=m_sceneNodes.begin();
	while( j!=m_sceneNodes.end())
    {
        if( (*j)->toBeDeleted() )
        {
            QGraphicsItem* nodeIcon = m_sceneNodeIcons[m_sceneNodes.indexOf(*j)];
            m_sceneNodeIcons.removeAt(m_sceneNodes.indexOf(*j));
            delete nodeIcon;
            delete (*j);
            j = m_sceneNodes.erase(j);
        }
        else
            ++j;
    }
}

void DropArea::runNetwork()
{
    if(m_mode == VPN_RUN)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        emit setStatusBar("VPN Running..",0);
        m_scheduler.setRunning(true);
        m_scheduler.start();
        emit setStatusBar("VPN Finished Running.",2000);
        QApplication::restoreOverrideCursor();
    }
}

void DropArea::forceRunNetwork()
{
    if(m_mode == VPN_RUN)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        emit setStatusBar("VPN Running..",0);
        m_scheduler.setRunning(true);
        m_scheduler.forceStart();
        emit setStatusBar("VPN Finished Running.",2000);
        QApplication::restoreOverrideCursor();
    }
}

bool DropArea::canConnect(int outPortIndex, int inPortIndex) const
{
    if(m_startNodeIndex==-1 || m_endNodeIndex==-1)
        return false;

    INode* startNode = m_sceneNodes[m_startNodeIndex];
    INode* endNode = m_sceneNodes[m_endNodeIndex];

    const NodePort* outputPort = startNode->getOutputPort(outPortIndex);
    const NodePort* inputPort = endNode->getInputPort(inPortIndex);

    if( !inputPort->dataType->matches(outputPort->dataType.getPtr()) )
        return false;
    if( endNode->getInputConnectivity(inPortIndex) == NodePort::CONNECTED )
        return false;
    if( endNode->getInputConnectivity(inPortIndex) == NodePort::PART_CONNECTED &&
        inputPort->dataType->getRelationType(outputPort->dataType.getPtr()) == IDataType::DIRECT )
        return false;
    return true;
}

void DropArea::insertPortLink(int outPortIndex, int inPortIndex, bool openVPN, unsigned int partIndex, const QPointF& p1, const QPointF& p2)
{
    if(m_startNodeIndex==-1 || m_endNodeIndex==-1 || m_startNodeIndex==m_endNodeIndex)
        return;

    NodeGraphicsItem* startNodeIcon = m_sceneNodeIcons[m_startNodeIndex];
    NodeGraphicsItem* endNodeIcon = m_sceneNodeIcons[m_endNodeIndex];
    INode* startNode = m_sceneNodes[m_startNodeIndex];
    INode* endNode = m_sceneNodes[m_endNodeIndex];

    if(m_scheduler.findPortLink(startNode,endNode,outPortIndex,inPortIndex))
        return;

    QGraphicsEllipseItem* outputRect = startNodeIcon->getOutputPortShape(outPortIndex);
    QGraphicsEllipseItem* inputRect = endNodeIcon->getInputPortShape(inPortIndex);
    NodePort* outputPort = startNode->getOutputPort(outPortIndex);
    NodePort* inputPort = endNode->getInputPort(inPortIndex);

    PortLink* autoLink = m_link? new PortLink(m_link->path()) : new PortLink();
    autoLink->setStartPortIcon(outputRect);
    autoLink->setEndPortIcon(inputRect);
    autoLink->setCurveCPoint1(p1);
    autoLink->setCurveCPoint2(p2);
    autoLink->setOutputPort(outputPort,outPortIndex);
    autoLink->setInputPort(inputPort,inPortIndex);
    autoLink->setZValue(-1000.0);
    m_scheduler.addPortLink(startNode, endNode, autoLink);
    QObject::connect(startNode->getDialog(), SIGNAL(dataReady()), this, SLOT(runNetwork()));
    addItem(autoLink);

    if(autoLink->getLinkType() == IDataType::CONTAINS)
    {
        colorInputPort(endNode, inPortIndex, Qt::blue);
        endNode->setInputConnected(inPortIndex, NodePort::PART_CONNECTED);
        /*outputPort->partsConnected = inputPort->partsConnected;
        QGraphicsTextItem* linkOrder =  new QGraphicsTextItem("", outputRect);
        linkOrder->setHtml(QString("<b>%1</b>").arg(outputPort->partsConnected));*/
    }
    else
    {
        if(autoLink->getLinkType() == IDataType::PART_OF)
        {
            if(openVPN)
                autoLink->setPartIndex(partIndex+1);
            else
            {
                autoLink->setSelected(true);
                bool ok;
                int i = QInputDialog::getInteger(0, tr("Specify Index"),
                                              tr("This link is a 'Many-to-One' link.\nPlease enter the data index to pass to the input port."),
                                               1, 1, 2147483647, 1, &ok);
                if (!ok)
                    i = 1;
                autoLink->setPartIndex(i);
                autoLink->setSelected(false);
            }
        }
        colorInputPort(endNode, inPortIndex, Qt::green);
        endNode->setInputConnected(inPortIndex, NodePort::CONNECTED);
    }
}

void DropArea::colorInputPort(INode* node, int inputIndex, const QColor& col)
{
    NodeGraphicsItem* nodeIcon = m_sceneNodeIcons[m_sceneNodes.indexOf(node)];
    QGraphicsEllipseItem* inputRect = nodeIcon->getInputPortShape(inputIndex);
    inputRect->setBrush(QBrush(col));
}

void DropArea::colorOutputPort(INode* node, int outputIndex, const QColor& col)
{
    NodeGraphicsItem* nodeIcon = m_sceneNodeIcons[m_sceneNodes.indexOf(node)];
    QGraphicsEllipseItem* outputRect = nodeIcon->getOutputPortShape(outputIndex);
    outputRect->setBrush(QBrush(col));
}

void DropArea::refreshView()
{
    for(int i=0; i<m_sceneNodes.size(); ++i)
    {
        INode* node = m_sceneNodes[i];
        for(int j=0; j<node->getInputsCount(); ++j)
            if(!node->isInputConnected(j))
                colorInputPort(node, j, Qt::transparent);

        for(int j=0; j<node->getOutputsCount(); ++j)
            colorOutputPort(node, j, Qt::transparent);
    }
}

void DropArea::markIncompatibleInputs()
{
    for(int i=0; i<m_sceneNodes.size(); ++i)
    {
        if(i == m_startNodeIndex)
            continue;
        INode* node = m_sceneNodes[i];
        m_endNodeIndex = i;
        for(int j=0; j<node->getInputsCount(); ++j)
        {
            if(!canConnect(m_link->getOutputPortIndex(), j) && !node->isInputConnected(j))
                colorInputPort(node, j, Qt::darkGray);
        }
    }
    m_endNodeIndex = -1;
}

void DropArea::setNodeText()
{
    if(m_startNodeIndex != -1)
    {
        INode* node = m_sceneNodes[m_startNodeIndex];
        NodeGraphicsItem* nodeIcon = m_sceneNodeIcons[m_sceneNodes.indexOf(node)];

        bool ok;
        QString descr = nodeIcon->getAnnotation();
        QString textstring = QInputDialog::getText(0, ("Enter node description"),(""),QLineEdit::Normal,descr,&ok);
        if (ok && !textstring.isEmpty())
        {
            nodeIcon->setAnnotation(textstring);
        }
    }
}
