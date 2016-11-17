#include "NodeLink.h"
#include <QtGui>
#include "../Utilities/Logger.h"
PortLink::PortLink(QGraphicsItem *parent) :                         QGraphicsPathItem(parent),
                                                                    m_outputPort(NULL),
                                                                    m_inputPort(NULL),
                                                                    m_startPortIcon(NULL),
                                                                    m_endPortIcon(NULL),
                                                                    m_pIndexIcon(NULL),
                                                                    m_delete(false),
                                                                    m_outputIndex(-1),
                                                                    m_inputIndex(-1),
                                                                    m_partIndex(1),
                                                                    m_linkType(IDataType::INVALID)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setCursor(Qt::OpenHandCursor);
}

PortLink::PortLink(const QPainterPath& path, QGraphicsItem * parent) :  QGraphicsPathItem(path,parent),
                                                                        m_outputPort(NULL),
                                                                        m_inputPort(NULL),
                                                                        m_startPortIcon(NULL),
                                                                        m_endPortIcon(NULL),
                                                                        m_pIndexIcon(NULL),
                                                                        m_delete(false),
                                                                        m_outputIndex(-1),
                                                                        m_inputIndex(-1),
                                                                        m_partIndex(1),
                                                                        m_linkType(IDataType::INVALID)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setCursor(Qt::OpenHandCursor);
}

PortLink::~PortLink()
{
    if(m_inputPort)
    {
        if(m_linkType == IDataType::CONTAINS)
        {
            m_inputPort->dataType->removePart(m_outputPort->dataType);
            if(--m_inputPort->partsConnected == 0)
            {
                m_inputPort->connected = NodePort::UNCONNECTED;
                QGraphicsEllipseItem* outPortIcon = dynamic_cast<QGraphicsEllipseItem*>(m_endPortIcon);
                if(outPortIcon) outPortIcon->setBrush(QBrush());
            }
        }
        else
        {
            m_inputPort->connected = NodePort::UNCONNECTED;
            m_inputPort->partsConnected = 0;
            QGraphicsEllipseItem* outPortIcon = dynamic_cast<QGraphicsEllipseItem*>(m_endPortIcon);
            if(outPortIcon) outPortIcon->setBrush(QBrush());
        }
    }
}

void PortLink::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_curveCPoint1 = event->scenePos();
    setCursor(Qt::ClosedHandCursor);
}

void PortLink::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ( !m_startPortIcon || !m_endPortIcon || m_delete)
    {
        event->ignore();
        return;
    }
    setCursor(Qt::ClosedHandCursor);
    QPointF inputMidPoint = mapFromItem(m_startPortIcon,5,5);
    QPointF outputMidPoint = mapFromItem(m_endPortIcon,5,10);

    QPainterPath curve(inputMidPoint);
    m_curveCPoint2 = event->scenePos();
    curve.cubicTo(m_curveCPoint1, m_curveCPoint2, outputMidPoint);
    setPath(curve);
}

void PortLink::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    setCursor(Qt::OpenHandCursor);
}

void PortLink::paint(QPainter *painter, const QStyleOptionGraphicsItem * ot, QWidget * w)
{
    painter->setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPainterPath myPath = path();

    if ( !m_startPortIcon || !m_endPortIcon || m_delete)
    {
		//return painter->drawPath(myPath);
    	painter->drawPath(myPath);
    	return;
    }

	QPointF inputMidPoint = mapFromItem(m_startPortIcon,5,5);
    QPointF outputMidPoint = mapFromItem(m_endPortIcon,5,10);

    if(myPath.elementCount() == 0)
    {
         myPath.moveTo(inputMidPoint);
         if(m_curveCPoint1 == m_curveCPoint2)
            myPath.lineTo(outputMidPoint);
         else
            myPath.cubicTo(m_curveCPoint1, m_curveCPoint2, outputMidPoint);
    }
    else
    {
        myPath.setElementPositionAt(0, inputMidPoint.x(),inputMidPoint.y());
        myPath.setElementPositionAt(myPath.elementCount()-1, outputMidPoint.x(),outputMidPoint.y());
    }
	setPath(myPath);
	painter->drawPath(myPath);

	if(m_pIndexIcon && m_linkType==IDataType::PART_OF)
        m_pIndexIcon->setPos(myPath.pointAtPercent(0.5));

	if (isSelected())
	{
	    QPainterPathStroker stroke;
	    stroke.setWidth(8);
		painter->setPen(QPen(Qt::blue, 1, Qt::DashLine));
		//myPath.translate(0, 4.0);
		painter->drawPath(stroke.createStroke(myPath));
		//myPath.translate(0,-8.0);
		//painter->drawPath(myPath);
	}
	//QGraphicsLineItem::paint(painter,ot,w);
}

QRectF PortLink::boundingRect() const
{
	qreal extra = (pen().width() + 20) / 2.0;

	return QRectF(path().boundingRect())
		.normalized()
		.adjusted(-extra, -extra, extra, extra);
}

bool PortLink::updated() const
{
    switch(m_linkType)
    {
        case IDataType::DIRECT:
            return m_outputPort->executionID > m_inputPort->executionID;
        break;
        case IDataType::CONTAINS:
            return  m_inputPort->partsConnected>1 ? true : m_outputPort->executionID > m_inputPort->executionID;
        break;
        case IDataType::PART_OF:
            return m_inputPort->partsConnected != m_partIndex || m_outputPort->executionID > m_inputPort->executionID;
        break;
        default: return false;
    }
}

void PortLink::link()
{
    switch(m_linkType)
    {
        case IDataType::DIRECT:
            m_inputPort->dataType = m_outputPort->dataType;
            break;
        case IDataType::CONTAINS:
            m_inputPort->dataType->addPart(m_outputPort->dataType,
                                        QString(m_outputPort->description.c_str()));
            break;
        case IDataType::PART_OF:
            if(m_inputPort->dataType->setFromContainer(m_outputPort->dataType, m_partIndex-1))
                m_pIndexIcon->setDefaultTextColor(Qt::black);
            else
                m_pIndexIcon->setDefaultTextColor(Qt::red);

            m_pIndexIcon->setFocus();
            m_inputPort->partsConnected = m_partIndex;
            break;
        case IDataType::INVALID:
            //TODO: Signal Error
            break;
    }
    m_inputPort->executionID = m_outputPort->executionID;
}

void PortLink::portIndexChanged(const QString& text)
{
    bool ok = false;
    unsigned int portI = text.toInt(&ok);
    if(ok)
    {
        if(portI > 0 && portI != m_partIndex)
        {
            m_partIndex = portI;
            emit refreshLink();
        }
    }
    m_pIndexIcon->setPlainText(QString().setNum(m_partIndex));
}
