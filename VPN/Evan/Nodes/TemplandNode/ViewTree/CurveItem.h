#ifndef CURVEITEM_H
#define CURVEITEM_H

#include "CurvesTopItem.h"


class CurveItem : public ViewTreeItem
{

Q_OBJECT

private:
    QString m_curveID;
    QString m_curveFile;
    int m_curveIndex;

public:
    CurveItem() :
        ViewTreeItem(CURVE_ITEM),
        m_curveID(""),
        m_curveFile(""),
        m_curveIndex(-1)
    {fillContextMenu();}
    CurveItem(CurvesTopItem* parent) :   ViewTreeItem(parent, CURVE_ITEM),
                                            m_curveID(""),
                                            m_curveFile(""),
                                            m_curveIndex(-1)
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

        QAction* colorAction = new QAction("Colour", m_contextMenu);
        m_contextMenu->addAction(colorAction);
        connect(colorAction, SIGNAL(triggered()), this, SLOT(signalColorChange()));

        QAction* deleteAction = new QAction("Delete", m_contextMenu);
        m_contextMenu->addAction(deleteAction);
        connect(deleteAction, SIGNAL(triggered()), this, SLOT(signalDelete()));
    }

    void setCurveID(const QString& id) {m_curveID = id;}
    QString getCurveID() const {return m_curveID;}
    void setCurveFile(const QString& path)
    {
        m_curveFile = path;
    }
    QString getCurveFile() const {return m_curveFile;}
    void setCurveIndex(int i) {m_curveIndex = i;}
    int getCurveIndex() const {return m_curveIndex;}

public slots:
    void focusView()
    {emit curveFocus(this);}
    void signalColorChange()
    {emit curveColorAction(this);}
    void signalDelete()
    {emit curveDeleteAction(this);}

signals:
    void curveFocus(CurveItem*);
    void curveColorAction(CurveItem*);
    void curveDeleteAction(CurveItem*);
};
#endif
