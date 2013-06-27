#ifndef SPECIMENSMODEL_H
#define SPECIMENSMODEL_H

#include <QAbstractTableModel>
#include <QTableView>
#include <QContextMenuEvent>
#include <QMenu>
#include "../../DataTypes/Specimens.h"

class SpecimensModel : public QAbstractTableModel
{
Q_OBJECT

private:
    Specimens* m_specimens;
    QSet<unsigned int> m_selectedSpecimens;
    QSet<unsigned int> m_unselectedLmks;

public:
    SpecimensModel(QObject *parent=0) : QAbstractTableModel(parent), m_specimens(NULL)
    {}
    SpecimensModel(Specimens* s, QObject *parent=0) : QAbstractTableModel(parent), m_specimens(s)
    {}

    void selectAllSpecimens()
    {
        if(m_specimens)
        {
            for(unsigned int i=0; i<m_specimens->getSize(); ++i)
                m_selectedSpecimens.insert(i);
        }
    }
    void unSelectAllSpecimens() {m_selectedSpecimens.clear();}
    void addSelection(QSet<unsigned int> s) {m_selectedSpecimens += s;}
    void setDataSource(Specimens* s)
    {
        m_selectedSpecimens.clear();
        m_unselectedLmks.clear();
        m_specimens = s;
        emit layoutChanged();
    }
    int rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return m_specimens ? m_specimens->getSize() : 0;
    }
    int columnCount(const QModelIndex &parent) const
    {
        return rowCount(parent) ? m_specimens->getSpecimenLabels(0)->size() : 0;
    }
    QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        if (index.row() >= (int)m_specimens->getSize() || index.row() < 0)
            return QVariant();

        if (role == Qt::DisplayRole)
        {
            QString labelName = headerData(index.column(),Qt::Horizontal,Qt::DisplayRole).toString();
            return m_specimens->getSpecimenLabelValue(index.row(), labelName);
        }
        else if (role == Qt::CheckStateRole && index.column() == 0)
            return m_selectedSpecimens.contains(index.row()) ? Qt::Checked : Qt::Unchecked;
        else if (role == Qt::BackgroundRole && !m_specimens->getLandmarkSet(index.row())->isComplete())
            return Qt::red;
        return QVariant();
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role != Qt::DisplayRole || !m_specimens->getSize())
            return QVariant();

        if (orientation == Qt::Horizontal)
        {
            Labels* specimenLabels = m_specimens->getSpecimenLabels(0);
            return specimenLabels->keys().at(section);
        }
        else if (orientation == Qt::Vertical)
            return section+1;
        return QVariant();
    }
    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        if(index.column() == 0)
            return  Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        else
            return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole)
    {
        if (role == Qt::CheckStateRole && index.column() == 0)
        {
            if(m_selectedSpecimens.contains(index.row()))
                m_selectedSpecimens.remove(index.row());
            else
                m_selectedSpecimens.insert(index.row());
            emit dataChanged(index,index);
        }
        return true;
    }
//    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
//    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

    const QSet<unsigned int>& getUnselectedLMKs()   const {return m_unselectedLmks;}
    const QSet<unsigned int>& getSelectedSpecimens()const {return m_selectedSpecimens;}

public slots:
    void checkLandmark(unsigned int row, bool checked)
    {
        if(checked)
            m_unselectedLmks.remove(row);
        else
            m_unselectedLmks.insert(row);
    }

};

class SpecimensView : public QTableView
{
Q_OBJECT

public:
    SpecimensView(QWidget* parent=0) : QTableView(parent)
    {}

protected:
    void contextMenuEvent(QContextMenuEvent* event)
    {
        QMenu* contextMenu = new QMenu();

        QAction *toggle = new QAction("Toggle", contextMenu);
        contextMenu->addAction(toggle);
        connect(toggle, SIGNAL(triggered()), this, SLOT(toggleSelected()));

        QAction *allAction = new QAction("Select All", contextMenu);
        contextMenu->addAction(allAction);
        connect(allAction, SIGNAL(triggered()), this, SLOT(markAll()));

        QAction *nallAction = new QAction("Unselect All", contextMenu);
        contextMenu->addAction(nallAction);
        connect(nallAction, SIGNAL(triggered()), this, SLOT(unmarkAll()));

        contextMenu->exec( event->globalPos() );
        delete contextMenu;
    }

protected slots:
    void currentChanged(const QModelIndex& current, const QModelIndex& previous)
    {
        if(previous.isValid())
            emit clicked(current);
        QTableView::currentChanged(current, previous);
    }

    void markAll()
    {
        if(model())
        {
            SpecimensModel* m = (SpecimensModel*)model();
            m->selectAllSpecimens();
            emit selectedAll();
        }
    }
    void unmarkAll()
    {
        if(model())
        {
            SpecimensModel* m = (SpecimensModel*)model();
            m->unSelectAllSpecimens();
        }
    }
    void toggleSelected()
    {
        if(model())
        {
            QVariant dummy;
            foreach(QModelIndex index, selectedIndexes())
                model()->setData(index, dummy, Qt::CheckStateRole);
        }
    }

signals:
    void selectedAll();
};

#endif
