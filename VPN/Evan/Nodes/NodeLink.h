#ifndef NODELINK_H
#define NODELINK_H

#include <QGraphicsPathItem>
#include <QKeyEvent>
#include "NodePort.h"

class INode;

class PortIndexItem : public QGraphicsTextItem
{
Q_OBJECT

public:
    PortIndexItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0) : QGraphicsTextItem(parent,scene)
    {
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsMovable);
    }
    int type() const {return UserType + 3;}

protected:
    void keyPressEvent(QKeyEvent* event)
    {
        if(event->key() == Qt::Key_Return)
        {
            setSelected (false);
            clearFocus();
            emit textChanged(toPlainText());
        }
        else QGraphicsTextItem::keyPressEvent(event);
    }
    QVariant itemChange(GraphicsItemChange change, const QVariant& value)
    {
        QGraphicsTextItem::itemChange(change, value);
        emit textChanged(toPlainText());
        return value;
    }
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
    {
        if (textInteractionFlags() == Qt::NoTextInteraction)
            setTextInteractionFlags(Qt::TextEditorInteraction);
        QGraphicsTextItem::mouseDoubleClickEvent(event);
    }

signals:
    void textChanged(const QString&);
};

class PortLink : public QObject, public QGraphicsPathItem
{
Q_OBJECT

private:
    NodePort* m_outputPort;
    NodePort* m_inputPort;
    QGraphicsItem* m_startPortIcon;
    QGraphicsItem* m_endPortIcon;
    PortIndexItem* m_pIndexIcon;

    QPointF m_curveCPoint1;
    QPointF m_curveCPoint2;

    bool m_delete;
    int m_outputIndex;
    int m_inputIndex;
    unsigned int m_partIndex;
    IDataType::RelationType m_linkType;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

public:
	PortLink(QGraphicsItem *parent = 0);
	PortLink(const QPainterPath& path, QGraphicsItem * parent = 0 );
    ~PortLink();

    void setStartPortIcon(QGraphicsItem* i) {m_startPortIcon = i;}
	void setEndPortIcon(QGraphicsItem* o)   {m_endPortIcon = o;}
	void setCurveCPoint1(const QPointF& p1) {m_curveCPoint1 = p1;}
	void setCurveCPoint2(const QPointF& p2) {m_curveCPoint2 = p2;}

    void setOutputPort(NodePort* op, int i) {m_outputPort=op;m_outputIndex=i;m_outputPort->connected = NodePort::CONNECTED;}
    void setInputPort (NodePort* ip, int i)
    {
        m_inputPort=ip;m_inputIndex=i;
        m_linkType = m_inputPort->dataType->getRelationType(m_outputPort->dataType.getPtr());
    }
    void setPartIndex(unsigned int i )
    {
        m_partIndex = i;
        if(!m_pIndexIcon && m_linkType==IDataType::PART_OF)
        {
            m_pIndexIcon = new PortIndexItem(this, scene());
            m_pIndexIcon->setPlainText(QString().setNum(m_partIndex));
            connect(m_pIndexIcon, SIGNAL(textChanged(const QString&)), this, SLOT(portIndexChanged(const QString&)));
        }
    }

    IDataType::RelationType getLinkType()  const {return m_linkType;}
    const NodePort* getOutputPort() const {return m_outputPort;}
    const NodePort* getInputPort()  const {return m_inputPort;}
    int getOutputPortIndex()        const {return m_outputIndex;}
    int getInputPortIndex()         const {return m_inputIndex;}
    unsigned int getPartIndex()     const {return m_partIndex;}

    bool dataAvailable()const {return (m_outputPort->dataType->isValid() && m_outputPort->executionID > 0);}
    bool updated()      const;// {return (m_inputPort->connected==NodePort::PART_CONNECTED && m_inputPort->partsConnected>1)
                              //      ? true : m_outputPort->executionID > m_inputPort->executionID; }
    void link();

    void flagDelete() {m_delete=true;}
    bool toBeDeleted() const {return m_delete;}
	QString toString() const {return QString("%1:%2:%3:%4|%5:%6|%7")
                                    .arg(m_outputIndex).arg(m_inputIndex).arg(m_partIndex-1)
                                    .arg(m_curveCPoint1.x()).arg(m_curveCPoint1.y())
                                    .arg(m_curveCPoint2.x()).arg(m_curveCPoint2.y());}
	void fromString(const QString&) {}

	QRectF boundingRect() const;
//	QRegion boundingRegion(const QTransform& itemToDeviceTransform) const
//	{return path().toFillPolygon(itemToDeviceTransform).toPolygon();}

private slots:
    void portIndexChanged(const QString&);

signals:
    void refreshLink();
};

struct NodeLink
{
    INode* startNode;
    INode* endNode;
    QList<PortLink*> portLinks;
    NodeLink() : startNode(NULL), endNode(NULL)
    {}
    ~NodeLink()
    {
        for(int i=0; i<portLinks.size(); ++i)
            delete portLinks[i];
        portLinks.clear();
    }
};

#endif
