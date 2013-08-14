#ifndef VIEWTREEITEM_H
#define VIEWTREEITEM_H

#include <QTreeWidgetItem>
#include <QMenu>

class FormItem;
//#include "FormItem.h"

class ViewTree;

class ViewTreeItem : public QObject, public QTreeWidgetItem
{
public:
    enum TreeItemTypes  {NONE, FORM_ITEM, SURFACES_ITEM, SURFACE_ITEM, SLICE_ITEM, REF_PLANES_ITEM,
                            LANDMARKS_ITEM, LANDMARK_ITEM, SEMILANDMARKS_ITEM, SEMILANDMARK_ITEM, GRID_ITEM, CURVES_ITEM, CURVE_ITEM, VOLUME_ITEM};

    ViewTreeItem(TreeItemTypes type = NONE) : QTreeWidgetItem(QTreeWidgetItem::UserType),
                                              m_contextMenu(new QMenu()),
                                              m_parentForm(0),
                                              m_parentTree(0),
                                              m_type(type)
    {}
    ViewTreeItem(ViewTree* parent, TreeItemTypes type = NONE) :  QTreeWidgetItem((QTreeWidget*)parent, QTreeWidgetItem::UserType),
                                                            m_contextMenu(new QMenu()),
                                                            m_parentForm(0),
                                                            m_parentTree(parent),
                                                            m_type(type)
    {}
    ViewTreeItem(ViewTreeItem* parent, TreeItemTypes type = NONE) : QTreeWidgetItem(parent, QTreeWidgetItem::UserType),
                                                                    m_contextMenu(new QMenu()),
                                                                    //m_parentForm( dynamic_cast<FormItem *>( parent ) ),
                                                                    m_parentForm( (FormItem *)( parent ) ),
                                                                    m_parentTree(0),
                                                                    m_type(type)
    {}

    virtual ~ViewTreeItem()
    {delete m_contextMenu;}

    const ViewTree* getParentTree() const {return m_parentTree;}
    TreeItemTypes getType() const {return m_type;}
    QMenu* getContextMenu() const {return m_contextMenu;}

protected:
    QMenu* m_contextMenu;
    FormItem* m_parentForm;
    ViewTree* m_parentTree;
    TreeItemTypes m_type;
};
#endif
