#ifndef SLICEITEM_H
#define SLICEITEM_H

#include "FormItem.h"

class SliceItem : public ViewTreeItem
{
Q_OBJECT

private:
    SliceItem() : ViewTreeItem(SLICE_ITEM)
    {fillContextMenu();}

public:
    SliceItem(ViewTree* parent) :  ViewTreeItem(parent, SLICE_ITEM)
    {fillContextMenu();}

    void fillContextMenu()
    {
        QAction* focusAction = new QAction("Focus View", m_contextMenu);
        m_contextMenu->addAction(focusAction);
        connect(focusAction, SIGNAL(triggered()), this, SLOT(focusView()));
        m_contextMenu->addSeparator();

        QAction* colorAction = new QAction("Colour", m_contextMenu);
        m_contextMenu->addAction(colorAction);
        connect(colorAction, SIGNAL(triggered()), this, SLOT(signalColorChange()));

//        QAction* clipAction = new QAction("Clip Ratio", m_contextMenu);
//        m_contextMenu->addAction(clipAction);
//        connect(clipAction, SIGNAL(triggered()), this, SLOT(signalClipRatio()));

//        QAction* saveIAction = new QAction("Save as Image", m_contextMenu);
//        m_contextMenu->addAction(saveIAction);
//        QAction* saveAction = new QAction("Save", m_contextMenu);
//        m_contextMenu->addAction(saveAction);
//        QAction* restoreAction = new QAction("Restore", m_contextMenu);
//        m_contextMenu->addAction(restoreAction);
//        QAction* tripleAction = new QAction("Triple Slice View", m_contextMenu);
//        tripleAction->setCheckable(true);
//        m_contextMenu->addAction(tripleAction);
//        QAction* moveAction = new QAction("Move to Landmark(s)", m_contextMenu);
//        m_contextMenu->addAction(moveAction);
//        QAction* fixAction = new QAction("Fix in View", m_contextMenu);
//        m_contextMenu->addAction(fixAction);
//        QAction* crossAction = new QAction("Cross Hairs", m_contextMenu);
//        crossAction->setCheckable(true);
//        m_contextMenu->addAction(crossAction);
    }

public slots:
    void focusView()
    {emit sliceFocus(m_parentForm, this);}
    void signalColorChange()
    {emit sliceColorAction(m_parentTree);}
    void signalClipRatio()
    {emit sliceClipAction(m_parentTree);}

signals:
    void sliceFocus(FormItem*, ViewTreeItem*);
    void sliceColorAction(ViewTree*);
    void sliceClipAction(ViewTree*);
};
#endif
