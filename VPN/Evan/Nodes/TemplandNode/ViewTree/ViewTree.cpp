#include "ViewTree.h"
#include <QContextMenuEvent>

void ViewTree::contextMenuEvent(QContextMenuEvent* event)
{
    ViewTreeItem* item = (ViewTreeItem*)itemAt(event->pos());
    if(item)
    {
        clearSelection();
        item->setSelected(true);
        QMenu* contextMenu = item->getContextMenu();

        contextMenu->exec( event->globalPos() );
    }
}
