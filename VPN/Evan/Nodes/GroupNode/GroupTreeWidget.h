#ifndef GROUPTREEWIDGET_H
#define GROUPTREEWIDGET_H

#include <QTreeWidgetItem>

class QContextMenuEvent;


class GroupTreeItem : public QTreeWidgetItem
{
private:
    bool m_hasMean;

public:
    GroupTreeItem() : QTreeWidgetItem(QTreeWidgetItem::UserType), m_hasMean(false)
    {}
    GroupTreeItem(QTreeWidget* parent)    :  QTreeWidgetItem(parent, QTreeWidgetItem::UserType), m_hasMean(false)
    {}

    bool hasMean() const {return m_hasMean;}
    void setHasMean(bool flag) {m_hasMean = flag;}
};

class MemberTreeItem : public QTreeWidgetItem
{
private:
    unsigned int m_specimenIndex;
    QColor m_color;

public:
    MemberTreeItem() : 
		QTreeWidgetItem(QTreeWidgetItem::UserType), 
		m_specimenIndex( 0u ),
		m_color(Qt::transparent)
    {}
    MemberTreeItem(QTreeWidgetItem* parent)    :  
		QTreeWidgetItem(parent, QTreeWidgetItem::UserType), 
		m_specimenIndex( 0u ),
		m_color(Qt::transparent)
    {}

    unsigned int getSpecimenIndex() const {return m_specimenIndex;}
    void setSpecimenIndex(unsigned int i) {m_specimenIndex = i;}
    const QColor& getColor() const {return m_color;}
    void setColor(const QColor& c) {m_color = c;}
};

class GroupTreeWidget : public QTreeWidget
{
Q_OBJECT

private:
    float m_groupItemSize;

protected:
    void contextMenuEvent(QContextMenuEvent* event);

public:
    GroupTreeWidget(QWidget * parent = 0) : QTreeWidget(parent)
    {
        connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(checkItemSize(QTreeWidgetItem*, int)));
        connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(saveItemSize(QTreeWidgetItem*, int)));
    }

    bool isTopLevel(QTreeWidgetItem* item) const
    {
        for(int i=0; i<topLevelItemCount(); ++i)
            if(item == topLevelItem(i))
                return true;
        return false;
    }
    bool isAChildOf(QTreeWidgetItem* parent, unsigned int index)
    {
        for(int i=0; i<parent->childCount(); ++i)
        {
            MemberTreeItem* groupMemberItem = dynamic_cast<MemberTreeItem *>( parent->child( i ) );
            if(groupMemberItem->getSpecimenIndex() == index)
                return true;
        }
        return false;
    }

public slots:
    void addNewGroup();
    void changeGroupColor();
    void changeGroupPSymbol(int,int,QTreeWidgetItem*);
    void changeGroupSize(QTreeWidgetItem* item=0, float oldSize=0.0f, float newSize=0.0f, bool menu=false );
    void checkItemSize(QTreeWidgetItem*, int);
    void saveItemSize(QTreeWidgetItem*, int);

signals:
    void membersUpdated();
    void colorChanged(QColor);
    void sizeChanged(float);
    void symbolChanged(int);
};

#endif
