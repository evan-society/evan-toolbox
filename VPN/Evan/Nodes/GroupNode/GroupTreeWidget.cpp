#include "GroupTreeWidget.h"
#include "PlotSymbolCombo.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QColorDialog>
#include <QInputDialog>

//extern bool IsRunning;
#include "../../Utilities/Settings.h" //YN 9Nov2015:extern globals are not clean, and are generating link errors with cmake

void GroupTreeWidget::addNewGroup()
{
    GroupTreeItem* groupItem = new GroupTreeItem(this);
    groupItem->setText(0, QString("Untitled Group %1").arg(topLevelItemCount()-1));
    groupItem->setData(0, Qt::DecorationRole, Qt::white);
    groupItem->setText(1, "3");
    groupItem->setTextAlignment(1, Qt::AlignCenter);
    PlotSymbolCombo* grpSymCmb = new PlotSymbolCombo(this, groupItem);
    setItemWidget(groupItem, 2, grpSymCmb);
    connect(grpSymCmb, SIGNAL(symbolChanged(int,int,QTreeWidgetItem*)), this, SLOT(changeGroupPSymbol(int,int,QTreeWidgetItem*)));
    groupItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    resizeColumnToContents(2);
}

void GroupTreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* contextMenu = new QMenu();
    QTreeWidgetItem* item = itemAt(event->pos());
    if(item)
    {
        //clearSelection();
        item->setSelected(true);
        if(selectedItems().count())
        {
            QAction *colorGroup = new QAction("Colour", contextMenu);
            contextMenu->addAction(colorGroup);
            connect(colorGroup, SIGNAL(triggered()), this, SLOT(changeGroupColor()));

            QAction *sizeGroup = new QAction("Size", contextMenu);
            contextMenu->addAction(sizeGroup);
            connect(sizeGroup, SIGNAL(triggered()), this, SLOT(changeGroupSize()));

            QAction *symbolGroup = new QAction("Symbol", contextMenu);
            contextMenu->addAction(symbolGroup);
            connect(symbolGroup, SIGNAL(triggered()), this, SLOT(changeGroupPSymbol()));

            if(Globals::getInstance()->isRunning())
            {
            	colorGroup->setEnabled(false);
            	sizeGroup->setEnabled(false);
            }

        }
    }
    else
    {
        QAction *addGroup = new QAction("New Group", contextMenu);
        contextMenu->addAction(addGroup);
        connect(addGroup, SIGNAL(triggered()), this, SLOT(addNewGroup()));
    }
    contextMenu->exec( event->globalPos() );
    delete contextMenu;
}

void GroupTreeWidget::changeGroupPSymbol()
{
    PlotSymbolCombo grpSymCmb(this, 0);
    QStringList itemsInComboBox;
    for (int index=0; index<grpSymCmb.count(); index++)
        itemsInComboBox << grpSymCmb.itemText(index);
    bool ok = false;
    QString symbol = QInputDialog::getItem(this, "Choose symbol", "Symbols",
                                           itemsInComboBox, 0, false, &ok);
    if(ok)
    {
        int index = itemsInComboBox.indexOf(symbol);
        bool emitted = false;
        foreach( QTreeWidgetItem* selected, selectedItems() )
        {
            PlotSymbolCombo* symbolCombo = dynamic_cast< PlotSymbolCombo* >( itemWidget(selected,2) );
            if(selected->childCount())
            {
                changeGroupPSymbol(symbolCombo->currentIndex(), index, selected);
                emitted = true;
            }
            symbolCombo->setCurrentIndex(index);
        }
        if(!emitted)
            emit symbolChanged(index);
    }
}

void GroupTreeWidget::changeGroupPSymbol(int oldIndex, int index, QTreeWidgetItem* groupItem)
{
    if(oldIndex == -1 || index == -1 || !groupItem)
        return;
    for(int i=0; i<groupItem->childCount(); ++i)
    {
        MemberTreeItem* memberItem = dynamic_cast<MemberTreeItem*>(groupItem->child(i));
        if(memberItem)
        {
            if( dynamic_cast< PlotSymbolCombo* >( itemWidget(memberItem,2) )->currentIndex() == oldIndex )
                dynamic_cast< PlotSymbolCombo* >( itemWidget(memberItem,2) )->setCurrentIndex(index);
        }
    }
    emit membersUpdated();
    emit symbolChanged(index);
}

void GroupTreeWidget::changeGroupColor()
{

    QTreeWidgetItem* firstItem = selectedItems().front();
    QColor result = firstItem->data(0, Qt::DecorationRole).value<QColor>();

    bool ok = false;
    result.setRgba(QColorDialog::getRgba(result.rgba(),&ok));

    if(ok)
    {
        foreach(QTreeWidgetItem* groupItem, selectedItems())
        {
            groupItem->setData(0, Qt::DecorationRole, result);
            for(int i=0; i<groupItem->childCount(); ++i)
            {
                MemberTreeItem* memberItem = dynamic_cast<MemberTreeItem*>(groupItem->child(i));
                if(memberItem)
                {
                    if(memberItem->getColor() == Qt::transparent)
                        memberItem->setData(0, Qt::DecorationRole, result);
                }
            }
            MemberTreeItem* memberItem = dynamic_cast<MemberTreeItem*>(groupItem);
            if(memberItem)
                memberItem->setColor(result);
        }
        emit membersUpdated();
        emit colorChanged(result);
    }

}

void GroupTreeWidget::changeGroupSize(QTreeWidgetItem* item, float savedSize, float newSize, bool menu)
{
    QTreeWidgetItem* firstItem = menu ? item : selectedItems().front();
    float oldSize = menu ? savedSize : firstItem->text(1).toFloat();

    bool ok = menu;
    float result = menu ? newSize : QInputDialog::getDouble(this,tr("Landmark Size"),

                                                            tr("Enter landmark size: "), oldSize, 0.001, 100, 2, &ok);
    if(ok && oldSize!=result)
    {
        foreach(QTreeWidgetItem* groupItem, selectedItems())
        {
            if(menu)
                groupItem = item;
            groupItem->setText(1, QString().setNum(result));
            for(int i=0; i<groupItem->childCount(); ++i)
            {
                MemberTreeItem* memberItem = dynamic_cast<MemberTreeItem*>(groupItem->child(i));
                if(memberItem)
                {
                    if(memberItem->text(1).toFloat() == oldSize) {
                        // memberItem->setText(1, QString().setNum(result));
                        memberItem->setText(1, QString::number( result ) );

                    }
                }
            }
            if(menu)
                break;
        }
        emit membersUpdated();
        emit sizeChanged(result);
    }
}

void GroupTreeWidget::saveItemSize(QTreeWidgetItem* item, int column)
{
    if(column == 1)
        m_groupItemSize = item->text(1).toFloat();
}

void GroupTreeWidget::checkItemSize(QTreeWidgetItem* item, int column)
{
    if(column == 1)
        changeGroupSize(item, m_groupItemSize, item->text(1).toFloat(), true);
}
