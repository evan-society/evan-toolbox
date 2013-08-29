#include "GridTreeWidget.h"
#include <QMenu>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Material>
#include <QColorDialog>
#include <QList>

extern ofstream dbgfile;// debug



void GroupGridTreeItem::refreshName()
{
    for (int i=0;i<childCount();++i)
    {
        GridTreeItem* gridItem = dynamic_cast<GridTreeItem*>(child(i));
        if (gridItem)
        {
            if (gridItem->getGrid()->getRenderableName() != gridItem->text(0))
                gridItem->setText(0,gridItem->getGrid()->getRenderableName());
        }
        else
        {
            GroupGridTreeItem* groupitem = dynamic_cast<GroupGridTreeItem*>(child(i));
            if (groupitem)
            {
                if (groupitem->getGroupGrid()->getRenderableName() != groupitem->text(0))
                    groupitem->setText(0,groupitem->getGroupGrid()->getRenderableName());
                groupitem->refreshName();
            }
        }
    }
}

//void changeChildrenColor(GroupGridTreeItem* item, QColor c)
//{
//    for(int i=0; i<item->childCount(); ++i)
//    {
//        GridTreeItem* childItem = (GridTreeItem*)item->child(i);
//        if(!childItem->getGrid()->getOsgNode()->getStateSet())
//            childItem->setData(0, Qt::DecorationRole, c);
//    }
//}

///////////////////////////////////////////////

void GridTreeWidget::moveGridItem(GridTreeItem* gridItem, GroupWarpGrids* newGroupGrid)
{
    gridItem->getGrid()->getParent()->removeGrid(gridItem->getGrid());
    gridItem->getGrid()->setParent(newGroupGrid);
    gridItem->getGrid()->reInitializeGrid();
    newGroupGrid->addGrid(gridItem->getGrid());
    newGroupGrid->initialize(newGroupGrid->getRenderableName());
    emit membersUpdated();
}

void GridTreeWidget::moveGridItem(GridTreeItem* gridItem, GroupGridTreeItem* newParent)
{
    moveGridItem(gridItem, newParent->getGroupGrid());
}

QMimeData* GridTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const
{
        QMimeData *md = QTreeWidget::mimeData(items);
        // Create a QByteArray to store the data for the drag.
        QByteArray ba;
        // Create a data stream that operates on the binary data
        QDataStream ds(&ba, QIODevice::WriteOnly);
        // Add each item's text for col 0 to the stream
        if (items.size()>1)
        {
            return md;
        }

        if (items[0]->parent())
        {
			int a,b;
		
            a=indexOfTopLevelItem(items[0]->parent());
            if (a==-1)
            {
                return md;
            }
            b=items[0]->parent()->indexOfChild(items[0]);
            if (b==-1)
            {
                return md;
            }
            ds << a << b ;
            md->setData("myownmimetype/grid", ba);
        }
        return md;
}

void GridTreeWidget::dropEvent(QDropEvent *event)
 {
    GridTreeItem* drag = NULL;
    if (event->mimeData()->hasFormat("myownmimetype/grid"))
    {
        QByteArray itemData = event->mimeData()->data("myownmimetype/grid");
        QDataStream stream(&itemData, QIODevice::ReadOnly);

        int r, c;
        stream >> r >> c;
        drag = dynamic_cast<GridTreeItem*>(topLevelItem(r)->child(c));
        if (!drag)
            return;
    }
    else if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
    {
        QByteArray itemData = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&itemData, QIODevice::ReadOnly);

        int r, c;
        QMap<int, QVariant> v;
        stream >> r >> c >> v;
        QTreeWidgetItem *item = topLevelItem(r);
        if( item )
        {
            drag = dynamic_cast<GridTreeItem*>(item);
            if (!drag)
                return;
        }
    }
    else return;

    QTreeWidget::dropEvent(event);
    GroupWarpGrids* gwarpg = NULL;
    if (drag->parent())
    {
        GroupGridTreeItem* newParent = dynamic_cast<GroupGridTreeItem*>(drag->parent());

        if (newParent)
        {
            gwarpg = dynamic_cast<GroupWarpGrids*>(newParent->getGroupGrid());
            if(!gwarpg)
                return;
           // changeChildrenColor(newParent, gwarpg->getFrontMaterial());
        }
    }
    else
        gwarpg = m_groupWarpGrids;


    moveGridItem(drag, gwarpg);

    event->accept();
    return;
}

void GridTreeWidget::deleteGrid()
{
    GridTreeItem* selected = dynamic_cast< GridTreeItem* >( selectedItems().front() );
    if (selected)
    {
        selected->getGrid()->getParent()->deleteGrid(selected->getGrid());
        delete selected;
        emit membersUpdated();
    }
}

void GridTreeWidget::deleteGroupGrid()
{
    GroupGridTreeItem* selected = dynamic_cast< GroupGridTreeItem* >( selectedItems().front() );
    if (selected)
    {
        m_groupWarpGrids->deleteGroupGrid(selected->getGroupGrid());
        delete selected;
        emit membersUpdated();
    }
}

void GridTreeWidget::createGrid()
{
    if (!selectedItems().isEmpty())
    {
        GroupGridTreeItem* selected = dynamic_cast<GroupGridTreeItem*>(selectedItems().front());

        if (selected)
        {
            emit showCreateGridDialog();
            GridTreeItem* drag = dynamic_cast<GridTreeItem*>(topLevelItem(topLevelItemCount()-1));
            if (!drag)
                return;
            selected->addChild(takeTopLevelItem(topLevelItemCount()-1));
            moveGridItem(drag,selected);
        }
        else // the selected items  is a GridTreeItem
        {
            GridTreeItem* gridItem = dynamic_cast< GridTreeItem* >( selectedItems().front() );
            if (!gridItem->parent())
                emit showCreateGridDialog();
            else
            {
                GroupGridTreeItem* parent = dynamic_cast<GroupGridTreeItem*>(gridItem->parent());
                if (parent)
                {
                    emit showCreateGridDialog();
                    GridTreeItem* drag = dynamic_cast<GridTreeItem*>(topLevelItem(topLevelItemCount()-1));
                    if (!drag)
                        return;
                    parent->addChild(takeTopLevelItem(topLevelItemCount()-1));
                    moveGridItem(drag,parent);
                }
            }
        }
    }
    else
        emit showCreateGridDialog();
}

void GridTreeWidget::cloneGrid()
{
    GridTreeItem* selected = dynamic_cast< GridTreeItem* >( selectedItems().front() );
    if (selected)
    {
        //GroupGridTreeItem* parent;
		GroupGridTreeItem* parent = static_cast< GroupGridTreeItem* >( 0 );
		
		// CppCheck:  Found duplicate branches for 'if' and 'else'.
        if (selected->parent())
            parent = dynamic_cast<GroupGridTreeItem*>(selected->parent());
        //else
        //    parent = dynamic_cast<GroupGridTreeItem*>(selected->parent());
			
        if (parent)
        {
            emit signalCloneGrid(selected->getGrid()->asWarpGrid());
            GridTreeItem* drag = dynamic_cast<GridTreeItem*>(topLevelItem(topLevelItemCount()-1));
            if (!drag)
                return;
            parent->addChild(takeTopLevelItem(topLevelItemCount()-1));
            moveGridItem(drag,parent);
        }
        else
            emit signalCloneGrid(selected->getGrid()->asWarpGrid());
    }
}

void GridTreeWidget::parallelGrid()
{
    GridTreeItem* selected = dynamic_cast< GridTreeItem* >( selectedItems().front() );
    if (selected)
    {
        //GroupGridTreeItem* parent;
		GroupGridTreeItem* parent = static_cast< GroupGridTreeItem* >( 0 );
		
		// CppCheck:  Found duplicate branches for 'if' and 'else'.
        if (selected->parent())
            parent = dynamic_cast<GroupGridTreeItem*>(selected->parent());
        //else
        //    parent = dynamic_cast<GroupGridTreeItem*>(selected->parent());
			
        if (parent)
        {
            emit parallelGrid(selected->getGrid()->asWarpGrid());
            GridTreeItem* drag = dynamic_cast<GridTreeItem*>(topLevelItem(topLevelItemCount()-1));
            if (!drag)
                return;
            parent->addChild(takeTopLevelItem(topLevelItemCount()-1));
            moveGridItem(drag,parent);
        }
        else
           emit parallelGrid(selected->getGrid()->asWarpGrid());
    }
}

void GridTreeWidget::perpendicularGrid()
{
    GridTreeItem* selected = dynamic_cast< GridTreeItem* >( selectedItems().front() );
    if (selected)
    {
        //GroupGridTreeItem* parent;
		GroupGridTreeItem* parent = static_cast< GroupGridTreeItem* >( 0 );
		
		// CppCheck:  Found duplicate branches for 'if' and 'else'.
        if (selected->parent())
            parent = dynamic_cast<GroupGridTreeItem*>(selected->parent());
        //else
        //    parent = dynamic_cast<GroupGridTreeItem*>(selected->parent());
			
        if (parent)
        {
            emit perpendicularGrid(selected->getGrid()->asWarpGrid());
            GridTreeItem* drag = dynamic_cast<GridTreeItem*>(topLevelItem(topLevelItemCount()-1));
            if (!drag)
                return;
            parent->addChild(takeTopLevelItem(topLevelItemCount()-1));
            moveGridItem(drag,parent);
        }
        else
            emit perpendicularGrid(selected->getGrid()->asWarpGrid());
    }
}

void GridTreeWidget::propertyGrid()
{
    GridTreeItem* selected = dynamic_cast< GridTreeItem* >( selectedItems().front() );
    if (selected)
    {
        emit propertyGrid(selected);
    }
}

//
//
//void GridTreeWidget::colorGrid()
//{
//    GridTreeItem* treeItem = ((GridTreeItem*)selectedItems().front());
//    osg::ref_ptr<osg::Geode> node = treeItem->getGrid()->getOsgNode();
//    osg::ref_ptr<osg::StateSet> nodess = node->getOrCreateStateSet();
//    osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>
//                                            (nodess->getAttribute(osg::StateAttribute::MATERIAL));
//    QColor result;
//    if(material.valid())
//    {
//        osg::Vec4 diffuse = material->getDiffuse(osg::Material::FRONT);
//        result.setRgbF(diffuse.x(),diffuse.y(),diffuse.z(),diffuse.w());
//    }
//    else
//        material = new osg::Material;
//
//    bool ok = false;
//    result.setRgba(QColorDialog::getRgba(result.rgba(),&ok));
//    if(ok)
//    {
//        treeItem->getGrid()->setFrontMaterial(result, true);
//        treeItem->setData(0, Qt::DecorationRole, result);
//       // changeChildrenColor(treeItem, result);
//    }
//    emit membersUpdated();
//}
//
//void GridTreeWidget::colorGroupGrid()
//{
//    GroupGridTreeItem* treeItem = ((GroupGridTreeItem*)selectedItems().front());
//    osg::ref_ptr<osg::Geode> node = treeItem->getGroupGrid()->getOsgNode();
//    osg::ref_ptr<osg::StateSet> nodess = node->getOrCreateStateSet();
//    osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>
//                                            (nodess->getAttribute(osg::StateAttribute::MATERIAL));
//    QColor result;
//    if(material.valid())
//    {
//        osg::Vec4 diffuse = material->getDiffuse(osg::Material::FRONT);
//        result.setRgbF(diffuse.x(),diffuse.y(),diffuse.z(),diffuse.w());
//    }
//    else
//        material = new osg::Material;
//
//    bool ok = false;
//    result.setRgba(QColorDialog::getRgba(result.rgba(),&ok));
//    if(ok)
//    {
//        treeItem->getGroupGrid()->setFrontMaterial(result, true);
//        treeItem->setData(0, Qt::DecorationRole, result);
//        changeChildrenColor(treeItem, result);
//    }
//    emit membersUpdated();
//}

void GridTreeWidget::createGroup()
{
    GroupGridTreeItem* groupItem = new GroupGridTreeItem(this);
    QString name("Group ");
    name.append(QString::number(m_groupWarpGrids->getNumGroupGrid()+1));
    groupItem->setText(0, name);
//    groupItem->setData(0, Qt::DecorationRole, Qt::white);
    groupItem->getGroupGrid()->initialize(name);
    m_groupWarpGrids->addGroupGrid(groupItem->getGroupGrid());
    insertTopLevelItem(0,groupItem);
    expandAll();
    emit membersUpdated();
}

void GridTreeWidget::updateName()
{
    for (int i=0;i<topLevelItemCount();++i)
    {
        GridTreeItem* gridItem = dynamic_cast<GridTreeItem*>(topLevelItem(i));
        if (gridItem)
        {
            if (gridItem->getGrid()->getRenderableName() != gridItem->text(0))
                gridItem->setText(0,gridItem->getGrid()->getRenderableName());
        }
        else
        {
            GroupGridTreeItem* group = dynamic_cast< GroupGridTreeItem* >( topLevelItem(i) );
            if (group)
            {
                group->setText(0,group->getGroupGrid()->getRenderableName());
                group->refreshName();
            }
        }
    }
}

void GridTreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* contextMenu = new QMenu();
    GridTreeItem* item = dynamic_cast<GridTreeItem*>(itemAt(event->pos()));
    GroupGridTreeItem* item2 = dynamic_cast<GroupGridTreeItem*>(itemAt(event->pos()));

    QAction* createGrid = new QAction("Create a new grid", contextMenu);
    contextMenu->addAction(createGrid);
    connect(createGrid, SIGNAL(triggered()),this, SLOT(createGrid()));

    QAction* createGroup = new QAction("Create a new group", contextMenu);
    contextMenu->addAction(createGroup);
    connect(createGroup, SIGNAL(triggered()),this, SLOT(createGroup()));

    contextMenu->addSeparator();

    if(item)
    {
        clearSelection();
        item->setSelected(true);
        if(selectedItems().count() && dynamic_cast<GridTreeItem*>(item))
        {
            QAction* deleteGrid = new QAction("Delete", contextMenu);
            contextMenu->addAction(deleteGrid);
            connect(deleteGrid, SIGNAL(triggered()),this, SLOT(deleteGrid()));

            QAction* cloneGrid = new QAction("Clone", contextMenu);
            contextMenu->addAction(cloneGrid);
            connect(cloneGrid, SIGNAL(triggered()),this, SLOT(cloneGrid()));

            QAction* parallelGrid = new QAction("Parallel Grid", contextMenu);
            connect(parallelGrid, SIGNAL(triggered()),this, SLOT(parallelGrid()));
            contextMenu->addAction(parallelGrid);

            QAction* perpendicularGrid = new QAction("Perpendicular Grid", contextMenu);
            connect(perpendicularGrid, SIGNAL(triggered()),this, SLOT(perpendicularGrid()));
            contextMenu->addAction(perpendicularGrid);


            QAction* propertyGrid = new QAction("Property", contextMenu);
            connect(propertyGrid, SIGNAL(triggered()),this, SLOT(propertyGrid()));
            contextMenu->addAction(propertyGrid);
        }
    }
    else if (item2)
    {
        clearSelection();
        item2->setSelected(true);

        QAction* deleteGroupGrid = new QAction("Delete", contextMenu);
        contextMenu->addAction(deleteGroupGrid);
        connect(deleteGroupGrid, SIGNAL(triggered()),this, SLOT(deleteGroupGrid()));
    }
    else
    {
        clearSelection();
    }

    contextMenu->exec( event->globalPos() );
    delete contextMenu;
}
