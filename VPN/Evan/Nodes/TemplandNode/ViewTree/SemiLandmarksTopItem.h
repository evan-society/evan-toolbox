#ifndef SEMILANDMARKSTOPITEM_H
#define SEMILANDMARKSTOPITEM_H

#include "FormItem.h"
#include "ew/View3Landmarks.h"
#include "LandmarksTopItem.h"
#include "../../Utilities/Logger.h"

#include <QInputDialog>

//#include <cassert> // for assert()
#include <typeinfo> // for typeid()
#include <cxxabi.h> // for human readable name of typeid

class LandmarksTopItem;

class SemiLandmarksTopItem : public ViewTreeItem
{
Q_OBJECT

public:
    enum SemiLandmarkItemTypes  {SURFACE_SEMILANDMARK_ITEM, CURVE_SEMILANDMARK_ITEM};

private:
    bool m_templateTreeView;
    SemiLandmarkItemTypes m_semitype;
    QAction* addAction;
    QAction* slideAction;
    QAction* projAction;
    QAction* m_mapAction;
    int m_lmkIndex;
    QString m_lmkID;
    QAction* m_renAction;
    QAction* m_fixAction;

    SemiLandmarksTopItem() : ViewTreeItem(SEMILANDMARKS_ITEM), m_templateTreeView(false), m_semitype(SURFACE_SEMILANDMARK_ITEM), addAction(0), slideAction(0), projAction(0), m_mapAction(0), m_lmkIndex(-1), m_renAction(0), m_fixAction(0)
    {fillContextMenu();}

    QString createAlphaNumeric( unsigned int idnum );
    QString m_embeddedItemID;

public:
    SemiLandmarksTopItem(LandmarksTopItem* parent) :  ViewTreeItem(parent, SEMILANDMARKS_ITEM), m_templateTreeView(false), m_semitype(SURFACE_SEMILANDMARK_ITEM), addAction(0), slideAction(0), projAction(0), m_mapAction(0), m_lmkIndex(-1), m_renAction(0), m_fixAction(0)
    {fillContextMenu();}

    void fillContextMenu()
    {
    }

    void mapEnable( bool enable )
    {
        if( m_mapAction )
        {
            m_mapAction->setEnabled( enable );
        }
    }

    void setLmkIndex(int i) {m_lmkIndex = i;}
    int getLmkIndex() const {return m_lmkIndex;}

    void deleteLmk()
    {
        QAction* deleteAction = new QAction("Delete", m_contextMenu);
        m_contextMenu->addAction(deleteAction);
        connect(deleteAction, SIGNAL(triggered()), this, SLOT(lmkDeleted()));
    }

    void projectLmk()
    {
        projAction = new QAction("Project", m_contextMenu);
        m_contextMenu->addAction(projAction);
        connect(projAction, SIGNAL(triggered()), this, SLOT(lmkProject()));
    }

    void addRename()
    {
        m_renAction = new QAction("Rename", m_contextMenu);
        m_contextMenu->addAction(m_renAction);
        connect(m_renAction, SIGNAL(triggered()), this, SLOT(renameLmk()));
    }

    void mapLmk()
    {
        m_mapAction = new QAction("Warp to Specimen", m_contextMenu);
        m_contextMenu->addAction(m_mapAction);
        connect(m_mapAction, SIGNAL(triggered()), this, SLOT(lmkMap()));
        mapEnable( true );
    }

    void fixLmk()
    {
        m_fixAction = new QAction("Fix SemilLandmark", m_contextMenu);
        m_contextMenu->addAction(m_fixAction);
        connect(m_fixAction, SIGNAL(triggered()), this, SLOT(fix()));
    }

    void saveLmkAsCurve()
    {
//    	m_contextMenu->addSeparator();
//        m_saveCurveAction = new QAction("Save as Curve", m_contextMenu);
//        m_contextMenu->addAction(m_saveCurveAction);
//        connect(m_saveCurveAction, SIGNAL(triggered()), this, SLOT(saveCurve()));
    }

    void projectEnable( bool enable )
    {
        if( projAction )
        {
            projAction->setEnabled( enable );
        }
    }

    void addLmksHere(bool checked = false)
    {
        if( !m_templateTreeView )
        {
            addHere(true);
/*
            if( !addAction )
            {
                addAction = new QAction("Current", m_contextMenu);
                addAction->setCheckable( true );
                addAction->setChecked( checked );
                m_contextMenu->addAction(addAction);
                connect(addAction, SIGNAL(toggled(bool)), this, SLOT(addHere(bool)));
            }
*/
        }
    }

    void slideLmks()
    {
        if( !m_templateTreeView )
        {
            if( !slideAction )
            {
                slideAction = new QAction("Slide", m_contextMenu);
                m_contextMenu->addAction(slideAction);
                connect(slideAction, SIGNAL(triggered()), this, SLOT(slide()));
            }
        }
    }

    void toggleAdd(bool tog)
    {
        if( addAction )
            addAction->setChecked( tog );
    }

    void templateTreeView( bool temp ) {m_templateTreeView = temp; }

    void setSemiType( SemiLandmarkItemTypes t ) {m_semitype = t;}
    SemiLandmarkItemTypes getSemiType() const {return m_semitype;}

    void setLmkID( const QString& lmkID ) { m_lmkID = lmkID; }
    QString getLmkID() {return m_lmkID;}

    void setEmbeddedItemID( const QString& embeddedItemID )
    {
        m_embeddedItemID = embeddedItemID;
        setToolTip(0,"Embedded object: " + embeddedItemID);
    }
    QString getEmbeddedItemID() {return m_embeddedItemID;}

public slots:
    void addHere(bool tog)
    {
        if ( m_parentForm == NULL ) {
            Logger::getInstance()->log( "parentForm is NULL" );
        }

        LandmarksTopItem *pLti = dynamic_cast< LandmarksTopItem* >( m_parentForm );
        //if( pLti == NULL )
        //assert( pLti != NULL );
        if( pLti != NULL ) {
            pLti->assignLandmarksHere( tog ? this : 0 );
        } else {
            #if !defined(NDEBUG)
            //Logger::getInstance()->log( "failed to cast type of parentForm to LandmarksTopItem* => falling back to original code..." );
            //Logger::getInstance()->log( QString( "actual type = " ) + QString( typeid(*m_parentForm).name() ) );

            int status;
            char *realname = abi::__cxa_demangle(typeid(*m_parentForm).name(), 0, 0, &status);
            printf( "class name = %s\n", realname );
            free(realname);
            #endif
            // [husky] that was the original code which did not check for any errors!
			// better than nothing?
			((LandmarksTopItem*)m_parentForm)->assignLandmarksHere( tog ? this : 0 );
        }
    }

    void slide( bool tog = true )
    {
        emit lmkSlide( m_parentForm, this, 0, tog );
    }
    void lmkDeleted()
    {
        dynamic_cast< LandmarksTopItem* >( m_parentForm )->assignLandmarksHere( 0 );
        emit lmkDeleteAction(m_lmkIndex);
    }
    void lmkMoved()
    {
        dynamic_cast< LandmarksTopItem* >( m_parentForm )->assignLandmarksHere( 0 );
        emit lmkMoveAction(m_lmkIndex);
    }
    void lmkMovedChild( FormItem* semipatch, ViewTreeItem* semi, int  semiID );
    void lmkDeletedChild( FormItem* semipatch, ViewTreeItem* semi, int  semiID );
    void addSemiLandmarkItem();
    void lmkProject() { emit lmkProject(m_parentForm, this, 0); }
    void lmkMap() { emit lmkMap( m_lmkIndex ); }
    void renameLmk()
    {
        bool ok;
        QString title = QInputDialog::getText( NULL, "SemiLandmark", "Enter SemiLandmark new name", QLineEdit::Normal, getLmkID(), &ok);
        if (ok)
        {
            emit lmkRenameAction( m_parentForm, this, m_lmkIndex, title );
        }
    }
    void fix() { emit lmkState( m_parentForm, this, m_lmkIndex, ew::Form3::STATE_SET ); }
    void saveCurve() { emit lmkSaveCurve( m_parentForm , m_lmkIndex); }

signals:
    void lmkDeleteAction(int);
    void lmkDeleteAction( FormItem*, ViewTreeItem*, int );
    void lmkMoveAction(int);
    void lmkMoveAction( FormItem*, ViewTreeItem*, int );
    void lmkProject(FormItem*, ViewTreeItem*, int);
    void lmkMap(int);
	void lmkSlide( FormItem*, ViewTreeItem*, int, bool );
    void lmkRenameAction(FormItem*, ViewTreeItem*, int, const QString& newID );
    void lmkState( FormItem*, ViewTreeItem*, int, int );
    void lmkSaveCurve( FormItem* , int );
};

#endif // SEMILANDMARKSTOPITEM_H
