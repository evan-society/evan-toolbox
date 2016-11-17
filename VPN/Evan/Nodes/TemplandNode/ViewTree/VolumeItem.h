#ifndef VOLUMEITEM_H
#define VOLUMEITEM_H

#include "ViewTreeItem.h"

class VolumeItem : public ViewTreeItem
{
private:
    VolumeItem() : ViewTreeItem(VOLUME_ITEM)
    {fillContextMenu();}

public:
    VolumeItem(FormItem * parent) :  ViewTreeItem(parent, VOLUME_ITEM)
    {fillContextMenu();}

    void fillContextMenu()
    {
//        QAction* loadAction = new QAction("Load", m_contextMenu);
//        m_contextMenu->addAction(loadAction);
    }
};
#endif
