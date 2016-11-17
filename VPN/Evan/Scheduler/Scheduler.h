#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QList>
#include "../Nodes/NodeLink.h"

class Scheduler : public QObject
{
Q_OBJECT

private:
    bool m_running;
    typedef QList<NodeLink*>::iterator linkIterator;
    typedef QList<PortLink*>::iterator portLinkIterator;
	QList<NodeLink*> m_links;
	QList<INode*> m_nodes;

public:
    Scheduler() : m_running(false) {}
    virtual ~Scheduler();

    void addPortLink(INode*, INode*, PortLink* l);
    void validateLinks();
    void flagLink(PortLink*);
    void flagLinks(INode*);
    void removeLinks();
    int findLink(INode*, INode*);
    bool findPortLink(INode*, INode*, int, int);

    void setRunning(bool on) {m_running = on;}
    const QList<NodeLink*>& links() const {return m_links;}
    void addNode(INode*);
    void removeNode(INode*);

public slots:
    bool start();
    bool forceStart();
    //void refreshLinks(QObject*);
signals:
    void inputMissing(INode*, int, const QColor&);
    void outputMissing(INode*, int, const QColor&);
};

#endif
