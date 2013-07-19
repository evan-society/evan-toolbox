#ifndef GRIDTREEWIDGET_H
#define GRIDTREEWIDGET_H

#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include "../../DataTypes/WarpGrid.h"

class GridTreeItem : public QTreeWidgetItem
{

private:
    WarpGrid* m_warpGrid;

public:
    GridTreeItem() :
        QTreeWidgetItem(QTreeWidgetItem::UserType),
        m_warpGrid(NULL)
    { setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled); }

    GridTreeItem(QTreeWidget* parent) :
        QTreeWidgetItem(parent, QTreeWidgetItem::UserType),
        m_warpGrid(NULL)
    { setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled); }

    void setGrid(WarpGrid* warpg){m_warpGrid=warpg;}
    WarpGrid* getGrid() {return m_warpGrid;}
};

class GroupGridTreeItem : public QTreeWidgetItem
{

private:
    GroupWarpGrids* m_groupWarpGrid;
    QColor m_color;

public:
    GroupGridTreeItem() : QTreeWidgetItem(QTreeWidgetItem::UserType), m_groupWarpGrid(new GroupWarpGrids()), m_color(Qt::transparent)
    {setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);}
    GroupGridTreeItem(QTreeWidget* parent) : QTreeWidgetItem(parent, QTreeWidgetItem::UserType),m_groupWarpGrid(new GroupWarpGrids()), m_color(Qt::transparent)
    {setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);}

    void setGroupGrid(GroupWarpGrids* newGroupGrid) {m_groupWarpGrid=newGroupGrid;}
    GroupWarpGrids* getGroupGrid() {return m_groupWarpGrid;}
    void refreshName();
    const QColor& getColor() const {return m_color;}
    void setColor(const QColor& c) {m_color = c;}
};

class GridTreeWidget : public QTreeWidget
{
Q_OBJECT
protected:
    void contextMenuEvent(QContextMenuEvent* event);
    GroupWarpGrids* m_groupWarpGrids;

public:
    GridTreeWidget(QWidget * parent = 0) :
        QTreeWidget(parent),
        m_groupWarpGrids(NULL)
    {
        headerItem()->setText(0, "Name");
        setDragEnabled(true);
        setDragDropMode(QAbstractItemView::InternalMove);
        viewport()->setAcceptDrops(true);
    }

    bool isTopLevel(QTreeWidgetItem* item) const
    {
        for(int i=0; i<topLevelItemCount(); ++i)
            if(item == topLevelItem(i))
                return true;
        return false;
    }

    void setGroupWarpGrids(GroupWarpGrids* gwarpg){m_groupWarpGrids=gwarpg;}
    void addNewGroup();
    virtual void dropEvent(QDropEvent * event);
    virtual QMimeData* mimeData(const QList<QTreeWidgetItem *> items) const;
    void updateName();

    void moveGridItem(GridTreeItem* gridItem, GroupWarpGrids* newGroupGrid);
    void moveGridItem(GridTreeItem* gridItem, GroupGridTreeItem* newParent);

public slots:
    void deleteGrid();
    void createGrid();
    void cloneGrid();
    void parallelGrid();
    void perpendicularGrid();
    void propertyGrid();
    void createGroup();
    void deleteGroupGrid();

signals:
    void membersUpdated();
    void showCreateGridDialog();
    void signalCloneGrid(WarpGrid*);
    void parallelGrid(WarpGrid*);
    void perpendicularGrid(WarpGrid*);
    void propertyGrid(GridTreeItem*);
};

#endif
