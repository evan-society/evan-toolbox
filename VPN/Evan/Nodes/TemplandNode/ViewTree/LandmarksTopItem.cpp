#include "LandmarksTopItem.h"
#include "LandmarkItem.h"
#include "SemiLandmarksTopItem.h"
#include "SemiLandmarkItem.h"
#include "../../Utilities/Logger.h"
#include <QTreeWidget>

void LandmarksTopItem::addLandmarkItem(const QString& id)
{
    if( !m_assignhere )
    {
        LandmarkItem* lmkChild = new LandmarkItem(this);
        lmkChild->setText(0, id);
        lmkChild->setLmkLabel( id );
        lmkChild->setLmkID( id );
        lmkChild->setLmkIndex(childCount()-1);
        setCheckState(1, Qt::Checked);
        setExpanded(true);
        treeWidget()->clearSelection();
        lmkChild->setSelected(true);
        if( m_templateTreeView )
        {
            lmkChild->addMapLmk();
            connect(lmkChild, SIGNAL(mapLmkAction(int)), this, SLOT(signalMap(int)));
        }
        else
        {
            lmkChild->deleteLmk();
            lmkChild->moveLmk();
            lmkChild->addRename();
            lmkChild->addProjectLmk();
            lmkChild->addState();
            connect(lmkChild, SIGNAL(projectLmkAction(FormItem*, ViewTreeItem*, int)), this, SLOT(project(FormItem*, ViewTreeItem*, int)));
            connect(lmkChild, SIGNAL(projectLmkCAction(FormItem*, ViewTreeItem*, int)), this, SLOT(projectOntoCurve(FormItem*, ViewTreeItem*, int)));
            connect(lmkChild, SIGNAL(renameLmkAction(FormItem*, ViewTreeItem*, int, const QString&)), this, SLOT(rename(FormItem*, ViewTreeItem*, int, const QString&)));
            connect(lmkChild, SIGNAL(stateAction(FormItem*, ViewTreeItem*, int, int)), this, SLOT(state(FormItem*, ViewTreeItem*, int, int)));
            connect(lmkChild, SIGNAL(slideLmkAction(FormItem*, ViewTreeItem*, int, bool)), this, SLOT(slide(FormItem*, ViewTreeItem*, int, bool)));
        }
        emit lmkAdded(m_parentForm);
        connect(lmkChild, SIGNAL(lmkDeleteAction(int)), this, SLOT(signalDelete(int)));
        connect(lmkChild, SIGNAL(lmkMoveAction(int)), this, SLOT(signalMove(int)));

        treeWidget()->resizeColumnToContents( 0 );
    }
    else
    {
        //SemiLandmarksTopItem* sl = (SemiLandmarksTopItem*)m_assignhere;
		SemiLandmarksTopItem* sl = dynamic_cast< SemiLandmarksTopItem* >( m_assignhere );
        sl->addSemiLandmarkItem();
    }
    m_moving = false;
    m_movingIndex = 0;
    m_movingSubIndex = 0;
}

void LandmarksTopItem::signalDelete(int index)
{
    emit lmkDeleted(m_parentForm,index);
    QTreeWidgetItem * childItem = child(index);
    removeChild(childItem);
    delete childItem;
    for(int i=0; i<childCount(); ++i)
    {
        //ViewTreeItem* vti = (ViewTreeItem*)child(i);
		ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( child(i) );
		
        if( vti->getType() == LANDMARK_ITEM )
        {
            //LandmarkItem* lmkItem = (LandmarkItem*)vti;
			LandmarkItem* lmkItem = dynamic_cast< LandmarkItem* >( vti );
			
            lmkItem->setLmkIndex(i);
        }
        else if( vti->getType() == SEMILANDMARKS_ITEM )
        {
            SemiLandmarksTopItem* lmkItem = dynamic_cast< SemiLandmarksTopItem* >( vti );
            lmkItem->setLmkIndex(i);
        }
    }
}

void LandmarksTopItem::signalDelete(FormItem* semipatch, ViewTreeItem* semilmk, int index)
{
    emit lmkDeletedChild(semipatch, semilmk, index);
}

void LandmarksTopItem::signalMove(int index)
{
	m_moving = true;
	m_movingIndex = index;
	m_movingSubIndex = 0;

    emit lmkMoved(m_parentForm,index);
}

void LandmarksTopItem::signalMove(FormItem* semipatch, ViewTreeItem* semilmk, int index)
{
	m_moving = true;
    m_movingIndex = dynamic_cast< SemiLandmarksTopItem* >( semipatch )->getLmkIndex();
	m_movingSubIndex = index;

    emit lmkMovedChild(semipatch, semilmk, index);
}

bool LandmarksTopItem::getMovingItem(int &index, int &subIndex)
{
	index = m_movingIndex;
	subIndex = m_movingSubIndex;
	return m_moving;
}

bool LandmarksTopItem::isMoving()
{
	return m_moving;
}
void LandmarksTopItem::setMoving(bool value)
{
	m_moving = value;
}

void LandmarksTopItem::enableMapChild( int index, bool enable )
{
    int size = childCount();
    for( int i = 0; i < size; ++i )
    {
        LandmarkItem* lmi = dynamic_cast< LandmarkItem* >( child( i ) );
        if( lmi->getLmkIndex() == index )
        {
            lmi->mapEnable( enable );
        } // end if
    }// end for loop
}

void LandmarksTopItem::enableRegChild( int index, bool enable )
{
    int size = childCount();
    for( int i = 0; i < size; ++i )
    {
        LandmarkItem* lmi = dynamic_cast< LandmarkItem* >( child( i ) );
        if( lmi->getLmkIndex() == index )
        {
            lmi->regEnable( enable );
        } // end if
    }// end for loop
}

void LandmarksTopItem::signalMap(int index)
{
    emit lmkMapped(m_parentForm,index);
}

void LandmarksTopItem::signalMapAll(int index)
{
    emit lmkMappedAll(m_parentForm, index);
}

void LandmarksTopItem::signalReg(int index)
{
    emit lmkRegister(m_parentForm,index);
}

QString LandmarksTopItem::getSelectedLandmarkID()
{
    if( m_templateTreeView )
    {
        for( int i = 0; i < childCount(); ++i )
        {
            ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( child(i) );
            if( vti->getType() == LANDMARK_ITEM && vti->isSelected() )
            {
                LandmarkItem* lti = dynamic_cast< LandmarkItem* >( vti );
                return lti->getLmkID();
            }
        }
    }

    return "";
}

void LandmarksTopItem::selectNextLandmark( const QString& parentid, const QString& childid )
{
Logger::getInstance()->log("select this");
Logger::getInstance()->log(parentid);
    // is the child available?
    if( doesChildExist( parentid, childid) == false )
        return;
Logger::getInstance()->log("2");
    blockSignals( true );

    // deselect the parent if it is selected.
    setSelected( false );

    for( int i = 0; i < childCount(); ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( child(i) );
        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
        {
            LandmarkItem* li = dynamic_cast< LandmarkItem* >( vti );
            child(i)->setSelected( li->getLmkID() == parentid );
        }
        // deselect the semilandmark children
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
        {
            SemiLandmarksTopItem* sli = dynamic_cast< SemiLandmarksTopItem* >( child(i) );
            if( sli->getLmkID() == parentid )
            {
                sli->setSelected( (childid == "") );

                for( int c = 0; c < sli->childCount(); ++c )
                {
                    sli->child( c )->setSelected( sli->child( c )->text(0) == childid );
                }
            }
            else
            {
                sli->setSelected( false );

                for( int c = 0; c < sli->childCount(); ++c )
                {
                    sli->child( c )->setSelected( false );
                }
            }
        }
    }
    blockSignals( false );
}

bool LandmarksTopItem::doesChildExist( const QString& parentid, const QString& childid )
{
    bool found = false;
    // first check if the item exists in the tree
    for( int i = 0; i < childCount() && !found; ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( child(i) );
        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
        {
            LandmarkItem* li = dynamic_cast< LandmarkItem* >( vti );
            found = ( li->getLmkID() == parentid );
        }
        // deselect the semilandmark children
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
        {
            SemiLandmarksTopItem* sli = dynamic_cast< SemiLandmarksTopItem* >( child(i) );
            if( sli->getLmkID() == parentid )
            {
                for( int c = 0; c < sli->childCount() && !found; ++c )
                {
                    found = ( sli->child( c )->text(0) == childid );
                }
            }
        }
    }
    return found;
}

void LandmarksTopItem::select( const QString& parentid, const QString& childid )
{
    // is the child available?
    if( doesChildExist( parentid, childid) == false )
        return;

    blockSignals( true );

    // deselect the parent if it is selected.
    setSelected( false );

    for( int i = 0; i < childCount(); ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( child(i) );
        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
        {
            LandmarkItem* li = dynamic_cast< LandmarkItem* >( vti );
            child(i)->setSelected( li->getLmkID() == parentid );
        }
        // deselect the semilandmark children
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
        {
            SemiLandmarksTopItem* sli = dynamic_cast< SemiLandmarksTopItem* >( child(i) );
            if( sli->getLmkID() == parentid )
            {
                sli->setSelected( (childid == "") );

                for( int c = 0; c < sli->childCount(); ++c )
                {
                    sli->child( c )->setSelected( sli->child( c )->text(0) == childid );
                }
            }
            else
            {
                sli->setSelected( false );

                for( int c = 0; c < sli->childCount(); ++c )
                {
                    sli->child( c )->setSelected( false );
                }
            }
        }
    }
    blockSignals( false );
}

void LandmarksTopItem::addSemiLandmarkSet( int t, const QString& suggestedName )
{
    int count = 1;
    for( int i = 0; i < childCount(); ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( child( i ) );
        if( vti->getType() == SEMILANDMARKS_ITEM )
            count++;
    }
    QString num; num.setNum( count );
    QString lmkID = "SemiLandmark ";
    lmkID += num;
    SemiLmkIdDialog* detailsDialog = new SemiLmkIdDialog(0, lmkID, lmkID);

    if (m_form !=0)
    {
        QStringList list;
        for(unsigned int i =0;i<m_form->surfaces.size();i++)
        {
            list.append(m_form->surfaces[i].id.c_str());
        }
        for(unsigned int i =0;i<m_form->curves.size();i++)
        {
            list.append(m_form->curves[i].id.c_str());
        }
        detailsDialog->fillComboBox(list);
    }

    detailsDialog->hideLabel();
    QString id = suggestedName;
    QString embeddedItemId = "";

    if( id.isEmpty() )
    {
        if( !detailsDialog->exec() )
            return;
        id = detailsDialog->getID();

        embeddedItemId = detailsDialog->getEmbeddedObjectName();

        // set new form embedding
        ew::Form3Embedding fe;
        fe.subset_id = id.toStdString();
        fe.superset_id = embeddedItemId.toStdString();

        bool b = false;
        ew::Dig3Space *sp = m_dig3->get_spaces()[1];
        sp->set_form_embedding( &b, &fe );
    }

    SemiLandmarksTopItem* sti = new SemiLandmarksTopItem(this);
    sti->setLmkIndex(childCount()-1);

    sti->templateTreeView( m_templateTreeView );
    sti->setText(0, id);
    sti->setLmkID( id );
    sti->setEmbeddedItemID(embeddedItemId);
    sti->addLmksHere(true);
    setExpanded(true);
    treeWidget()->clearSelection();
    sti->setSelected(true);

    if( !m_templateTreeView )
    {
        sti->deleteLmk();
        sti->addRename();
        sti->projectLmk();
        sti->slideLmks();
        sti->fixLmk();
        sti->saveLmkAsCurve();
    }
    else
    {
        sti->mapLmk();
        connect(sti, SIGNAL(lmkMap(int)), this, SLOT(signalMap(int)));
    }

    connect(sti, SIGNAL(lmkDeleteAction(int)), this, SLOT(signalDelete(int)));
    connect(sti, SIGNAL(lmkDeleteAction(FormItem* , ViewTreeItem* , int)), this, SLOT(signalDelete(FormItem* , ViewTreeItem* , int)));
    connect(sti, SIGNAL(lmkMoveAction(int)), this, SLOT(signalMove(int)));
    connect(sti, SIGNAL(lmkMoveAction(FormItem* , ViewTreeItem* , int)), this, SLOT(signalMove(FormItem* , ViewTreeItem* , int)));
    connect(sti, SIGNAL(lmkProject(FormItem*, ViewTreeItem*, int)), this, SLOT(project(FormItem*, ViewTreeItem*, int)));
    connect(sti, SIGNAL(lmkRenameAction(FormItem*, ViewTreeItem*, int, const QString&)), this, SLOT(rename(FormItem*, ViewTreeItem*, int, const QString&)));
    connect(sti, SIGNAL(lmkSlide(FormItem*, ViewTreeItem*, int, bool)), this, SLOT(slide(FormItem*, ViewTreeItem*, int, bool)));
    connect(sti, SIGNAL(lmkState(FormItem*, ViewTreeItem*, int, int)), this, SLOT(state(FormItem*, ViewTreeItem*, int, int)));
    connect(sti, SIGNAL(lmkSaveCurve(FormItem*, int)), this, SLOT(saveCurve(FormItem*, int)));

    treeWidget()->resizeColumnToContents( 0 );
    assignLandmarksHere( sti );
    assignLandmarksHere( sti );
}

void LandmarksTopItem::assignLandmarksHere( ViewTreeItem* vti )
{
    m_assignhere = vti;
    QString id = "";
    int semitype = -1;
    if( vti == 0 )
    {
        for( int i = 0; i < childCount(); ++i )
        {
            ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( child( i ) );
            if( vti->getType() == SEMILANDMARKS_ITEM )
            {
                SemiLandmarksTopItem* s = dynamic_cast< SemiLandmarksTopItem* >( vti );
                s->toggleAdd( false );
            }
        }
    }
    else
    {
        for( int i = 0; i < childCount(); ++i )
        {
            ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( child( i ) );
            if( vti->getType() == SEMILANDMARKS_ITEM )
            {
                SemiLandmarksTopItem* s = dynamic_cast< SemiLandmarksTopItem* >( vti );
                if( s != m_assignhere )
                    s->toggleAdd( false );
                else
                {
                    id = s->getLmkID();
                    s->toggleAdd( true );
                    semitype = s->getSemiType() == SemiLandmarksTopItem::SURFACE_SEMILANDMARK_ITEM ? 0 : 1;
                }
            }
        }
    }

    emit addLandmarkHere( id, semitype );
}
