#ifndef SEMILANDMARKITEM_H
#define SEMILANDMARKITEM_H

#include "SemiLandmarksTopItem.h"
#include "../../Utilities/Logger.h"
class SemiLandmarkItem : public ViewTreeItem
{
Q_OBJECT

private:
    int m_lmkIndex;
    QString m_lmkLabel;
    SemiLandmarkItem() : ViewTreeItem(SEMILANDMARK_ITEM), m_lmkIndex(-1)
    {
        fillContextMenu();
    }

public:
    SemiLandmarkItem(SemiLandmarksTopItem* parent) :  ViewTreeItem(parent, SEMILANDMARK_ITEM), m_lmkIndex(-1)
    {
        fillContextMenu();
    }

    void fillContextMenu()
    {
    }

    void deleteLmk()
    {
        QAction* deleteAction = new QAction("Delete", m_contextMenu);
        m_contextMenu->addAction(deleteAction);
        connect(deleteAction, SIGNAL(triggered()), this, SLOT(lmkDeleted()));
    }

    void moveLmk()
    {
        QAction* moveAction = new QAction("Move", m_contextMenu);
        m_contextMenu->addAction(moveAction);
        connect(moveAction, SIGNAL(triggered()), this, SLOT(lmkMoved()));
    }

    void setLmkIndex(int i) {m_lmkIndex = i;}
    int getLmkIndex() const {return m_lmkIndex;}

    void setLmkLabel( const QString& lmkLBL )
    {
        m_lmkLabel = lmkLBL;
    }
    QString getLmkLabel() {return m_lmkLabel;}

public slots:
    void lmkDeleted()
    {
        emit lmkDeleteAction( m_parentForm, this, m_lmkIndex );
    }
    void lmkMoved()
	{
		emit lmkMoveAction( m_parentForm, this, m_lmkIndex );
	}

signals:
    void lmkDeleteAction( FormItem*, ViewTreeItem*, int );
    void lmkMoveAction( FormItem*, ViewTreeItem*, int );

};
#endif
