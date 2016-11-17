#ifndef GRIDITEM_H
#define GRIDITEM_H

#include "ViewTreeItem.h"

class GridItem : public ViewTreeItem
{
private:
    GridItem() : ViewTreeItem(GRID_ITEM)
    {fillContextMenu();}

public:
    GridItem(ViewTree* parent) :  ViewTreeItem(parent, GRID_ITEM)
    {fillContextMenu();}

    void fillContextMenu()
    {
//        QAction* loadAction = new QAction("Load", m_contextMenu);
//        m_contextMenu->addAction(loadAction);
    }
};
#endif
