#ifndef REFPLANESITEM_H
#define REFPLANESITEM_H

#include "ViewTreeItem.h"

class RefPlanesItem : public ViewTreeItem
{
private:
    RefPlanesItem() : ViewTreeItem(REF_PLANES_ITEM)
    {fillContextMenu();}

public:
    RefPlanesItem(ViewTree* parent) :  ViewTreeItem(parent, REF_PLANES_ITEM)
    {fillContextMenu();}

    void fillContextMenu()
    {
//        QAction* centerAction = new QAction("Center", m_contextMenu);
//        m_contextMenu->addAction(centerAction);
//        QAction* sliceAction = new QAction("Set at Slice", m_contextMenu);
//        m_contextMenu->addAction(sliceAction);
//        QAction* resAction = new QAction("Resolution", m_contextMenu);
//        m_contextMenu->addAction(resAction);
    }
};
#endif
