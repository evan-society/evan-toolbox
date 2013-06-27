#ifndef PLOTSYMBOLCOMBO_H
#define PLOTSYMBOLCOMBO_H

#include <QComboBox>
#include<QTreeWidgetItem>

class PlotSymbolCombo : public QComboBox
{
Q_OBJECT
private:
    QTreeWidgetItem* m_item;
    int m_previousIndex;

public:
    PlotSymbolCombo(QWidget* parent, QTreeWidgetItem* i) : QComboBox(parent), m_item(i), m_previousIndex(-1)
    {
        addItem("Ellipse");
        addItem("Rectangle");
        addItem("Diamond");
        addItem("Triangle");
        addItem("Cross");
        addItem("X-Cross");
        addItem("Horizontal Line");
        addItem("Vertical Line");
        addItem("Star1");
        addItem("Star2");
        addItem("Hexagon");
        connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
        connect(this, SIGNAL(highlighted(int)), this, SLOT(cacheIndex(int)));
        //connect(this, SIGNAL(highlighted(int)), this, SLOT(selectItem(int)));
    }
protected:
    void mousePressEvent(QMouseEvent* event)
    {
        selectItem(0);
        QComboBox::mousePressEvent(event);
    }

private slots:
    void cacheIndex(int index)
    {m_previousIndex = currentIndex();}
    void indexChanged(int index)
    {emit symbolChanged(m_previousIndex, index, m_item);}
    void selectItem(int index)
    {
        if(m_item)
        {
            m_item->treeWidget()->clearSelection();
            m_item->setSelected(true);
        }
    }

signals:
    void symbolChanged(int,int,QTreeWidgetItem*);
};

#endif
