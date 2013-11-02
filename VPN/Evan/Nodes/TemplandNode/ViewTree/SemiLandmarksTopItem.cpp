#include "SemiLandmarksTopItem.h"

#include "SemiLandmarkItem.h"

void SemiLandmarksTopItem::addSemiLandmarkItem()
{
    SemiLandmarkItem* lmkChild = new SemiLandmarkItem(this);
    QString id = "";
    unsigned int startingID = 1;
    bool foundUnique = false;
    while( foundUnique == false )
    {
        foundUnique = true;
        id = createAlphaNumeric( startingID );
        for( int i = 0; i < childCount(); ++i )
        {
            SemiLandmarkItem* s = dynamic_cast< SemiLandmarkItem* >( child(i) );
            if( s->text(0) == id )
                foundUnique = false;
        }
        ++startingID;
    }

    lmkChild->setText(0, id);
    lmkChild->setLmkLabel( id );
    lmkChild->setLmkIndex(childCount()-1);

    if( !m_templateTreeView )
    {
        lmkChild->deleteLmk();
        connect( lmkChild, SIGNAL(lmkDeleteAction(FormItem*, ViewTreeItem*, int)), this, SLOT(lmkDeletedChild(FormItem*, ViewTreeItem*, int)) );
        lmkChild->moveLmk();
        connect( lmkChild, SIGNAL(lmkMoveAction(FormItem*, ViewTreeItem*, int)), this, SLOT(lmkMovedChild(FormItem*, ViewTreeItem*, int)) );
    }

    setExpanded(true);
    treeWidget()->clearSelection();
    lmkChild->setSelected(true);
}

QString SemiLandmarksTopItem::createAlphaNumeric( unsigned int idnum )
{
    QString num; num = num.setNum( idnum );
    QString base = "SM";
    //if( idnum < 99999  && idnum <= 9999)
	if( idnum < 99999  && idnum >= 10000 )
        base += "0";
    else
    {
        base += num;
        return base;
    }
    //if( idnum < 9999  && idnum <= 999)
	if( idnum < 9999  && idnum >= 1000 )
        base += "0";
    else
    {
        base += num;
        return base;
    }
    //if( idnum < 999  && idnum <= 99)
	if( idnum < 999  && idnum >= 100 )
        base += "0";
    else
    {
        base += num;
        return base;
    }
    //if( idnum < 99  && idnum <= 9)
	if( idnum < 99  && idnum >= 10 )
        base += "0";
    else
    {
        base += num;
        return base;
    }

    //if( idnum < 9 )
    //{
        base += num;
        return base;
    //}
}


void SemiLandmarksTopItem::lmkDeletedChild( FormItem* semipatch, ViewTreeItem* semi, int  semiID )
{
    emit lmkDeleteAction( semipatch, semi, semiID );

    QTreeWidgetItem * childItem = child(semiID);
    removeChild(childItem);
    delete childItem;
    for(int i=0; i<childCount(); ++i)
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( child(i) );
        SemiLandmarkItem* lmkItem = dynamic_cast< SemiLandmarkItem* >( vti );
        lmkItem->setLmkIndex(i);
    }
}

void SemiLandmarksTopItem::lmkMovedChild( FormItem* semipatch, ViewTreeItem* semi, int  semiID )
{
    emit lmkMoveAction( semipatch, semi, semiID );
}
