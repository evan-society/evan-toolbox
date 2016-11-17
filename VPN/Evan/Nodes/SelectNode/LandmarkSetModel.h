#ifndef LANDMARKSETMODEL_H
#define LANDMARKSETMODEL_H

#include <QAbstractTableModel>
#include <QTableView>
#include <QContextMenuEvent>
#include <QMenu>
#include "../../DataTypes/LandmarkSet.h"

class LandmarkSetView : public QTableView
{
Q_OBJECT

public:
    LandmarkSetView(QWidget* parent=0) : QTableView(parent)
    {}

protected:
    void contextMenuEvent(QContextMenuEvent* event)
    {
        QMenu* contextMenu = new QMenu();

        QAction *toggle = new QAction("Toggle", contextMenu);
        contextMenu->addAction(toggle);
        connect(toggle, SIGNAL(triggered()), this, SLOT(toggleSelected()));
        contextMenu->exec( event->globalPos() );

        delete contextMenu;
    }

protected slots:
    void toggleSelected()
    {
        if(model())
        {
            QVariant dummy;
            foreach(QModelIndex index, selectedIndexes())
                model()->setData(index, dummy, Qt::CheckStateRole);
        }
    }
};

class LandmarkSetModel : public QAbstractTableModel
{
Q_OBJECT

private:
    LandmarkSet* m_landmarkSet;
    QSet<unsigned int> m_unselectedLmks;

public:
    LandmarkSetModel(QObject *parent=0) : QAbstractTableModel(parent), m_landmarkSet(NULL)
    {}
    LandmarkSetModel(LandmarkSet* l, QObject *parent=0) : QAbstractTableModel(parent), m_landmarkSet(l)
    {}

    void setDataSource(LandmarkSet* l, QSet<unsigned int> unselectedLmks)
    {
        m_landmarkSet = l;
        m_unselectedLmks = unselectedLmks;
        if(m_landmarkSet)
        {
            m_unselectedLmks += m_landmarkSet->getMissingLandmarks();
            foreach(unsigned int i, m_landmarkSet->getMissingLandmarks())
                emit toggleLandmark(i, false);
        }
        emit layoutChanged();
    }
    int rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return m_landmarkSet ? m_landmarkSet->GetRows() : 0;
    }
    int columnCount(const QModelIndex &parent) const
    {
        return m_landmarkSet ? m_landmarkSet->GetCols() : 0;
    }
    QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid() || !m_landmarkSet)
            return QVariant();

        if (index.row() >= (int)m_landmarkSet->GetRows() || index.row() < 0
        ||  index.column() >= (int)m_landmarkSet->GetCols() || index.column() < 0)
            return QVariant();

        if (role == Qt::DisplayRole)
            return m_landmarkSet->get(index.row(), index.column());
        else if (role == Qt::CheckStateRole && index.column() == 0)
            return m_unselectedLmks.contains(index.row()) ? Qt::Unchecked : Qt::Checked;
        else if (role == Qt::BackgroundRole && m_landmarkSet->isMissing(index.row()))
            return Qt::red;

        return QVariant();
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Horizontal)
        {
            switch(section)
            {
                case 0: return "x";
                case 1: return "y";
                case 2: return "z";
                case 3: return "log(centroid size)";
                default: return "";
            }
        }
        else if (orientation == Qt::Vertical)
            return section+1;
        return QVariant();
    }
    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        if(index.column() == 0)
            return  Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
        else
            return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole)
    {
        if (role == Qt::CheckStateRole && index.column() == 0)
        {
            if(m_unselectedLmks.contains(index.row()))
            {
                m_unselectedLmks.remove(index.row());
                emit toggleLandmark(index.row(), true);
            }
            else
            {
                m_unselectedLmks.insert(index.row());
                emit toggleLandmark(index.row(), false);
            }
        }
        return true;
    }

signals:
    void toggleLandmark(unsigned int, bool);
};

#endif
