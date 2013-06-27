#ifndef SURFACEITEM_H
#define SURFACEITEM_H

#include "SurfaceTopItem.h"

class SurfaceItem : public ViewTreeItem
{
Q_OBJECT

private:
    QString m_surfaceID;
    QString m_surfaceFile;
    int m_surfaceIndex;

public:
    SurfaceItem() : ViewTreeItem(SURFACE_ITEM), m_surfaceID(""), m_surfaceFile(""), m_surfaceIndex(-1)
    {fillContextMenu();}
    SurfaceItem(SurfaceTopItem* parent) :   ViewTreeItem(parent, SURFACE_ITEM),
                                            m_surfaceID(""),
                                            m_surfaceFile(""),
                                            m_surfaceIndex(-1)
    {fillContextMenu();}

    void fillContextMenu()
    {
    }

    void addSpecimentOptions()
    {

        QAction* focusAction = new QAction("Focus View", m_contextMenu);
        m_contextMenu->addAction(focusAction);
        connect(focusAction, SIGNAL(triggered()), this, SLOT(focusView()));
        m_contextMenu->addSeparator();

        QAction* fColor = new QAction("Front color", m_contextMenu);
        m_contextMenu->addAction(fColor);
        connect(fColor, SIGNAL(triggered()), this, SLOT(signalFColorChange()));

        QAction* bColor = new QAction("Back color", m_contextMenu);
        m_contextMenu->addAction(bColor);
        connect(bColor, SIGNAL(triggered()), this, SLOT(signalBColorChange()));

        QAction* deleteAction = new QAction("Delete", m_contextMenu);
        m_contextMenu->addAction(deleteAction);
        connect(deleteAction, SIGNAL(triggered()), this, SLOT(signalDelete()));
    }

    void setSurfaceID(const QString& id) {m_surfaceID = id;}
    QString getSurfaceID() const {return m_surfaceID;}
    void setSurfaceFile(const QString& path)
    {
        m_surfaceFile = path;
    }
    QString getSurfaceFile() const {return m_surfaceFile;}
    void setSurfaceIndex(int i) {m_surfaceIndex = i;}
    int getSurfaceIndex() const {return m_surfaceIndex;}

public slots:
    void focusView()
    {emit surfaceFocus(this);}
    void signalFColorChange()
    {emit surfaceColorAction(this, true);}
    void signalBColorChange()
    {emit surfaceColorAction(this, false);}
    void signalDelete()
    {emit surfaceDeleteAction(this);}

signals:
    void surfaceFocus(SurfaceItem*);
    void surfaceColorAction(SurfaceItem*, bool);
    void surfaceDeleteAction(SurfaceItem*);
};
#endif
