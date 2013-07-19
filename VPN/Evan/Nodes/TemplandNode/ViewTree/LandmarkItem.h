#ifndef LANDMARKITEM_H
#define LANDMARKITEM_H

#include "LandmarksTopItem.h"
#include "../Utilities/Logger.h"

#include <QInputDialog>

#include "ew/Form3.h"

class LandmarkItem : public ViewTreeItem
{
Q_OBJECT

private:
    int m_lmkIndex;
    QString m_lmkLabel;
    QString m_lmkID;
    QAction* m_regAction;
    QAction* m_mapAction;
    QAction* m_projAction;
    QAction* m_projActionC;
    QAction* m_slideAction;
    QAction* m_renAction;

    LandmarkItem() :
        ViewTreeItem(LANDMARK_ITEM),
        m_lmkIndex(-1),
        m_lmkLabel(),
        m_lmkID(),
        m_regAction(NULL),
        m_mapAction(NULL),
        m_projAction(NULL),
        m_projActionC(NULL),
        m_slideAction(NULL),
        m_renAction(NULL)
    {
    }

public:
    LandmarkItem(LandmarksTopItem* parent) :
        ViewTreeItem(parent, LANDMARK_ITEM),
        m_lmkIndex(-1),
        m_lmkLabel(),
        m_lmkID(),
        m_regAction(NULL),
        m_mapAction(NULL),
        m_projAction(NULL),
        m_projActionC(NULL),
        m_slideAction(NULL),
        m_renAction(NULL)
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

    void addMapLmk()
    {
        m_mapAction = new QAction("Warp to Specimen", m_contextMenu);
        m_contextMenu->addAction(m_mapAction);
        connect(m_mapAction, SIGNAL(triggered()), this, SLOT(mapLmk()));
        mapEnable( true );
    }

    void addProjectLmk()
    {
        m_projAction = new QAction("Project", m_contextMenu);
        m_contextMenu->addAction(m_projAction);
        connect(m_projAction, SIGNAL(triggered()), this, SLOT(projLmk()));
    }

    void addRename()
    {
        m_renAction = new QAction("Rename", m_contextMenu);
        m_contextMenu->addAction(m_renAction);
        connect(m_renAction, SIGNAL(triggered()), this, SLOT(renameLmk()));
    }

    void addState()
    {
        QMenu* stateMenu = new QMenu("State");
        QAction* fixAction = new QAction("[F] Fixed", stateMenu);
        connect(fixAction, SIGNAL(triggered()), this, SLOT(stateFix()));
        stateMenu->addAction(fixAction);

        QAction* warpAction = new QAction("[W] Warped", stateMenu);
        connect(warpAction, SIGNAL(triggered()), this, SLOT(stateWarp()));
        stateMenu->addAction(warpAction);

        QAction* projectAction = new QAction("[P] Projected", stateMenu);
        connect(projectAction, SIGNAL(triggered()), this, SLOT(stateProject()));
        stateMenu->addAction(projectAction);
        m_contextMenu->addMenu(stateMenu);

        QActionGroup* stateGroup = new QActionGroup(m_contextMenu);
        stateGroup->addAction(fixAction);
        stateGroup->addAction(warpAction);
        stateGroup->addAction(projectAction);
    }

    void setLmkIndex(int i) {m_lmkIndex = i;}
    int getLmkIndex() const {return m_lmkIndex;}

    void setLmkLabel( const QString& lmkLBL )
    {
        m_lmkLabel = lmkLBL;
    }
    QString getLmkLabel() {return m_lmkLabel;}

    void setLmkID( const QString& lmkID ) { m_lmkID = lmkID; }
    QString getLmkID() {return m_lmkID;}

    void mapEnable( bool enable )
    {
        if( m_mapAction )
        {
            m_mapAction->setEnabled( enable );
        }
    }

    void regEnable( bool enable )
    {
        if( m_regAction )
        {
            m_regAction->setEnabled( enable );
        }
    }

    void projectEnable( bool enable )
    {
        if( m_projAction )
        {
            m_projAction->setEnabled( enable );
        }
    }

public slots:
    void lmkDeleted()
    {emit lmkDeleteAction(m_lmkIndex);}
    void lmkMoved()
    {	emit lmkMoveAction(m_lmkIndex);}
    void mapLmk()
    { emit mapLmkAction(m_lmkIndex); }
    void projLmk()
    { emit projectLmkAction( m_parentForm, this, m_lmkIndex); }
    void slide( bool tog )
    { emit slideLmkAction( m_parentForm, this, m_lmkIndex, tog ); }
    void renameLmk()
    {
        bool ok;
        QString title = QInputDialog::getText( NULL, "Landmark", "Enter Landmark new name", QLineEdit::Normal, getLmkID(), &ok);
        if (ok)
        {
            emit renameLmkAction( m_parentForm, this, m_lmkIndex, title );
        }
    }
    void stateFix()
    {
        emit stateAction( m_parentForm, this, m_lmkIndex, ew::Form3::STATE_SET );
    }
    void stateWarp()
    {
        emit stateAction( m_parentForm, this, m_lmkIndex, ew::Form3::STATE_WARPED );
    }
    void stateProject()
    {
        emit stateAction( m_parentForm, this, m_lmkIndex, ew::Form3::STATE_PROJECTED );
    }

signals:
    void lmkDeleteAction(int);
    void lmkMoveAction(int);
    void mapLmkAction(int);
    void projectLmkAction(FormItem*, ViewTreeItem*, int);
    void slideLmkAction( FormItem*, ViewTreeItem*, int, bool );
    void renameLmkAction(FormItem*, ViewTreeItem*, int, const QString& newID );
    void stateAction( FormItem*, ViewTreeItem*, int, int );
};
#endif
