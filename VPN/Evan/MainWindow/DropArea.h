#ifndef DROPAREA_H
#define DROPAREA_H

#include <QGraphicsScene>
#include <QList>
#include "../Scheduler/Scheduler.h"
#include "NodeGraphicsItem.h"

class QDropEvent;
class QDragEnterEvent;
class INode;

class DropArea : public QGraphicsScene
{
	Q_OBJECT

public:
	enum AreaMode {NODE_INSERT, NODE_MOVE, NODE_CONNECT, LINK_DRAW, VPN_RUN};

private:
    AreaMode m_mode;
    Scheduler m_scheduler;

    int m_startNodeIndex;
    int m_endNodeIndex;

	typedef QList<INode*>::iterator NodeIterator;
	typedef QList<NodeGraphicsItem*>::iterator NodeIconIterator;
	QList<INode*> m_sceneNodes;
	QList<NodeGraphicsItem*> m_sceneNodeIcons;

    QString m_selectedNode;
    QString m_selectedNodeParams;
//    QPixmap m_selectedNodeIcon;

    PortLink* m_link;
    QPointF m_linkStartPos;
    int m_zoom;

	int getSelectedNode(const QGraphicsItem*);
	int getNodeIcon(const INode*);
	bool flagListItems(const QGraphicsItem*);
	void validateNodes();
	bool canConnect(int,int) const;
	void refreshView();
	void markIncompatibleInputs();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

public:
	DropArea(QWidget *parent = 0);
	~DropArea();

	void setMode(AreaMode mode){m_mode = mode;}
	AreaMode getMode() const {return m_mode;}
	bool isRunning() {return m_mode == VPN_RUN;}
    int getZoom() const {return m_zoom;}
    void setZoom(int zoom) {m_zoom = zoom;}

	void setSelectedNode(const QString& Node){m_selectedNode = Node;}
	void setSelectedNodeParams(const QString& params){m_selectedNodeParams = params;}
	QString getSelectedNode() {return m_selectedNode;}

//	void setSelectedNodeIcon(const QPixmap& icon) {m_selectedNodeIcon = icon;}
	const QList<INode*>* nodes() const {return &m_sceneNodes;}
    QPointF getNodePos(int index)const {return m_sceneNodeIcons.at(index)->pos();}
    QString getNodeAnnotations(int index)const {return m_sceneNodeIcons.at(index)->getAnnotation();}
    void insertNode(const QPointF&, const QString&);
    void insertLink(int,int,const QString&);

    const QList<NodeLink*>& links() const {return m_scheduler.links();}

    void insertPortLink(int,int,bool openVPN=false, unsigned int partIndex=0, const QPointF& p1=QPointF(), const QPointF& p2=QPointF());

public slots:
	void enableConnect(bool enable){if(enable)setMode(NODE_CONNECT);}
	void enableMove(bool enable){if(enable)setMode(NODE_MOVE);}
    void deleteSelection();
    void runNetwork();
    void forceRunNetwork();
    void stopNetwork() {m_scheduler.setRunning(false); refreshView();}
    void colorInputPort(INode*, int, const QColor&);
    void colorOutputPort(INode*, int, const QColor&);
    void setNodeText();

signals:
	void nodeInserted();
	void linkCreated();
	void setStatusBar(const QString&,int);
	void nodesMoved();
};

#endif
