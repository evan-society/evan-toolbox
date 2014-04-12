#ifndef VIEWTREE_H
#define VIEWTREE_H

#include "ViewTreeItem.h"

#include "../../Utilities/Logger.h"

class ViewTree : public QTreeWidget
{
Q_OBJECT

protected:
    void contextMenuEvent(QContextMenuEvent* event);

public:
    ViewTree(QWidget * parent = 0) : QTreeWidget(parent)
    {
        connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),this, SLOT(signalChange(QTreeWidgetItem*,int)));
        connect(this, SIGNAL(itemSelectionChanged()),this, SLOT(signalSelectionChange()));
    }

public slots:
    void signalChange(QTreeWidgetItem* item, int column)
    {
    	emit viewItemChanged(this, item, column);
    }
    void signalSelectionChange()
    {
        //printf( "signalSelectionChange! selectedItems().count() = %d\n", selectedItems().count() );
        for ( int i = selectedItems().count() - 1; i >= 0; --i ) {
            if ( selectedItems().at( i )->isSelected() ) {
                emit viewItemSelected( this, selectedItems().at( i ) );
                break;
            }
        }

    }

signals:
    void viewItemChanged(ViewTree*, QTreeWidgetItem*, int);
    void viewItemSelected(ViewTree*, QTreeWidgetItem*);
};

#endif

