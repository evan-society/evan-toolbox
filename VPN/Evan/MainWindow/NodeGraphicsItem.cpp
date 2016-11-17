#include "NodeGraphicsItem.h"
#include "../Nodes/INode.h"

#include "../Utilities/Logger.h"

void NodeGraphicsItem::changeToAnnotation()
{
    hide();

    m_nodeName->getAnnotation()->setVisible(true);
    m_nodeName->getAnnotation()->setFlag(QGraphicsItem::ItemIsMovable, true);

    if(m_nodeName->getAnnotation()->toPlainText() == "")
        m_nodeName->getAnnotation()->setPlainText("Type text here");

}

NodeGraphicsItem::NodeGraphicsItem(const INode* node, QGraphicsItem* parent) : QGraphicsRectItem(parent)
{
    setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));

    m_nodeName =  new MyTitleGraphicItems(this);
    m_nodeName->setTitle("<b>" + QString(node->getName().c_str()) + "</b>");

    for(int i=0; i<node->getInputsCount(); ++i)
    {
        QGraphicsRectItem* inputRect = new QGraphicsRectItem(this);
        QGraphicsEllipseItem* inputPort =  new QGraphicsEllipseItem(5, 5 , 10, 10 ,inputRect);
        inputPort->setPen(QPen(Qt::black, 2));
        QGraphicsTextItem* inputName =  new QGraphicsTextItem("", inputRect);
        inputName->setPlainText(" " + QString(node->getInputDescription(i).c_str()));
        inputName->setPos(15, 0);

        inputRect->setPos(0, childrenBoundingRect().height());
        m_inputPorts.push_back(inputPort);
    }

    QGraphicsRectItem* spacer = new QGraphicsRectItem(this);
    spacer->setRect(5, childrenBoundingRect().height()+5, 10, 10);
    spacer->setPen(QPen(Qt::transparent,0));

    for(int i=0; i<node->getOutputsCount(); ++i)
    {
        QGraphicsRectItem* outputRect = new QGraphicsRectItem(this);
        QGraphicsTextItem* outputName =  new QGraphicsTextItem("", outputRect);
        outputName->setPlainText(" " + QString(node->getOutputDescription(i).c_str()));
        QGraphicsEllipseItem* outputPort =  new QGraphicsEllipseItem(0, 0, 10, 10 ,outputRect);
        outputPort->setPen(QPen(Qt::black, 2));

        outputRect->setPos(0, childrenBoundingRect().height());
        m_outputPorts.push_back(outputPort);
    }

    qreal nodeWidth = childrenBoundingRect().width();
    for(int i=0; i<m_outputPorts.size(); ++i)
        m_outputPorts[i]->setPos(nodeWidth+10,5);

    m_nodeName->setRect(0,0,childrenBoundingRect().width()+5, m_nodeName->childrenBoundingRect().height());
    setRect(childrenBoundingRect());
    m_nodeName->setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_nodeName->getAnnotation()->setTextWidth(childrenBoundingRect().width()+5);
    QPointF posAnnotations(0, -m_nodeName->getAnnotation()->boundingRect().height());
    m_nodeName->getAnnotation()->setPos(posAnnotations);
}

int NodeGraphicsItem::getOutputPortIndex(const QPointF& pos) const
{
    for(int i=0; i<m_outputPorts.size(); ++i)
    {
        if(m_outputPorts[i]->contains(m_outputPorts[i]->mapFromScene(pos)))
            return i;
    }
    return -1;
}

int NodeGraphicsItem::getInputPortIndex(const QPointF& pos) const
{
    for(int i=0; i<m_inputPorts.size(); ++i)
    {
        if(m_inputPorts[i]->contains(m_inputPorts[i]->mapFromScene(pos)))
            return i;
    }
    return -1;
}

 bool NodeGraphicsItem::sceneEvent (QEvent * event)
{
    if (event->type() == QEvent::GrabMouse)
    {
        m_nodeName->getAnnotation()->setFocus();
        return true;
    }
    else if (event->type() == QEvent::UngrabMouse)
    {
        return true;
    }
    if(event->type() == QEvent::KeyPress)
    {
        event->accept();
        return m_nodeName->getAnnotation()->sceneEvent(event);
    }
    return QGraphicsRectItem::sceneEvent(event);
}

QString NodeGraphicsItem::getAnnotation() const
{
    return m_nodeName->getText();
}

void NodeGraphicsItem::setAnnotation(const QString& annotation) const
{
   m_nodeName->setText(annotation);
   QPointF posAnnotations(0,  -m_nodeName->getAnnotation()->boundingRect().height());
   m_nodeName->getAnnotation()->setPos(posAnnotations);
}

void MyTitleGraphicItems::update()
{
    nodeNameRect = new QGraphicsRectItem(this);
    nodeNameRect->setBrush(Qt::darkBlue);
    nodeName =  new QGraphicsTextItem("",nodeNameRect);
    annotations = new MyTextGraphicItems("", this);
    nodeName->setDefaultTextColor(Qt::lightGray);
    QFont f;
    f.setPointSize(10);
    nodeName->setFont(f);
    annotations->setFont(f);
}

QString MyTitleGraphicItems::getText() const
{
    return annotations->toPlainText();
}

void MyTitleGraphicItems::setText(const QString& annot) const{
    annotations->setPlainText(annot);
}

//MyTextGraphicItems
bool MyTextGraphicItems::sceneEvent (QEvent * event)
{
    if (event->type() == QEvent::FocusIn)
    {
        scene()->clearSelection();
        return true;
    }
    if(event->type() == QEvent::KeyPress)
    {

        QKeyEvent* eventKeypress = static_cast<QKeyEvent*>(event);
        if (eventKeypress)
        {
             if (eventKeypress->key() == Qt::Key_Return)
             {
                event->accept();
                clearFocus();
                QPointF position;
                position.setX(0);
                position.setY(-boundingRect().height());
                setPos(position);
                return true;
             }
             else
             {
                event->ignore();
                QGraphicsTextItem::sceneEvent(event);
                QPointF position;
                position.setX(0);
                position.setY(-boundingRect().height());
                setPos(position);
                return true;
             }
        }
    }
    return QGraphicsTextItem::sceneEvent(event);
}
