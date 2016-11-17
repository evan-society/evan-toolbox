#ifndef NODEGRAPHICSITEM_H
#define NODEGRAPHICSITEM_H

#include <QGraphicsRectItem>
#include <QString>
#include <QtGui>
#include <iostream>
#include <fstream>
using namespace std;

class INode;
class MyTitleGraphicItems;
class MyTextGraphicItems;


class NodeGraphicsItem : public QGraphicsRectItem
{
private:
    typedef QVector<QGraphicsEllipseItem*>::iterator portIterator;
	QVector<QGraphicsEllipseItem*> m_inputPorts;
	QVector<QGraphicsEllipseItem*> m_outputPorts;
	MyTitleGraphicItems* m_nodeName;

public:
    NodeGraphicsItem(const INode*, QGraphicsItem* parent = 0);
    int getOutputPortIndex(const QPointF&) const;
    int getInputPortIndex(const QPointF&) const;
    QGraphicsEllipseItem* getOutputPortShape(int i) const {return m_outputPorts[i];}
    QGraphicsEllipseItem* getInputPortShape(int i)  const {return m_inputPorts[i];}
    QString getAnnotation() const;
    void setAnnotation(const QString& annotation) const;
    bool sceneEvent (QEvent * event);
    void changeToAnnotation();
    bool isAnnotation() {return true;}
};

class MyTitleGraphicItems : public QGraphicsItem
{
    private:
    QGraphicsRectItem* nodeNameRect;
    QGraphicsTextItem* nodeName;
    MyTextGraphicItems* annotations;

    public:
    MyTitleGraphicItems(QGraphicsItem* parent = 0) : QGraphicsItem(parent){update();}

    void update();
    inline void setRect(qreal x, qreal y, qreal width, qreal height) const
    {
        nodeNameRect->setRect(x,y,width,height);
    }
    QRectF boundingRect() const{
        QRectF rect;
        rect.united(nodeNameRect->boundingRect());
        rect.united(nodeName->boundingRect());
        return rect;
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        nodeNameRect->paint(painter, option, widget);
    }
    void setTitle(const QString& text) const {nodeName->setHtml(text);}
    QString getText() const;
    void setText(const QString& annot) const;
    MyTextGraphicItems* getAnnotation() const {return annotations;};
};

class MyTextGraphicItems : public QGraphicsTextItem
{
    public:
    MyTextGraphicItems(const QString& text, QGraphicsItem* parent = 0) : QGraphicsTextItem(text,parent){}
    bool sceneEvent (QEvent* event);
};
#endif
