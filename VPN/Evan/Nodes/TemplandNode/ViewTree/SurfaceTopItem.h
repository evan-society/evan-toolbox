#ifndef SURFACETOPITEM_H
#define SURFACETOPITEM_H

#include "FormItem.h"

class SurfaceItem;

class SurfaceTopItem : public ViewTreeItem
{
Q_OBJECT

private:
    bool                addSpecimentOptions;

    SurfaceTopItem(bool specimenOpt) : ViewTreeItem(SURFACES_ITEM), addSpecimentOptions(specimenOpt)
    {fillContextMenu();}

public:
    SurfaceTopItem(FormItem* parent, bool specimenOpt) :  ViewTreeItem(parent, SURFACES_ITEM), addSpecimentOptions(specimenOpt)
    {fillContextMenu();}

    void fillContextMenu()
    {
        if( addSpecimentOptions )
        {
            QAction* importSurface = new QAction("Import Surface", m_contextMenu);
            m_contextMenu->addAction(importSurface);
            connect(importSurface, SIGNAL(triggered()), this, SLOT(addSurface()));
        }
    }

public slots:
    void addSurface(const QString& id="", const QString& file="", bool loaded=false);
    void signalSurfaceColor(SurfaceItem* item, bool front) {emit surfaceColorChange(item, m_parentForm, front);}
    void signalDelete(SurfaceItem*);
	
	//error: cannot dynamic_cast 'item' (of type 'struct SurfaceItem*') to type 'class ViewTreeItem*' (source is a pointer to incomplete type)
    //void signalSurfaceFocus(SurfaceItem* item) { emit surfaceFocus(m_parentForm, dynamic_cast< ViewTreeItem* >( item ) ); }
	void signalSurfaceFocus(SurfaceItem* item) { emit surfaceFocus(m_parentForm, ( ViewTreeItem * )( item ) ); }

signals:
    void surfaceFocus(FormItem*, ViewTreeItem*);
    bool surfaceItemAdded(SurfaceItem*, FormItem*);
    void surfaceColorChange(SurfaceItem*, FormItem*, bool);
    void surfaceDeleted(FormItem*, int);
};
#endif
