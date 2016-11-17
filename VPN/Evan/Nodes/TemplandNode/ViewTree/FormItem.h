#ifndef FORMITEM_H
#define FORMITEM_H

#include "ViewTreeItem.h"

class FormItem : public ViewTreeItem
{
Q_OBJECT

private:
    FormItem() : ViewTreeItem(FORM_ITEM)
    {fillContextMenu();}

    bool                addSpecimentOptions;

    QAction*            newAction;
    QAction*            appendAction;
    QAction*            insertAction;
    QAction*            replaceAction;
    QAction*            deleteAction;
    QAction*            swapPrevAction;
    QAction*            swapNextAction;

public:
    FormItem(ViewTree* parent, bool specimenOpt) :
        ViewTreeItem(parent, FORM_ITEM),
        addSpecimentOptions(specimenOpt),
        newAction(0),
        appendAction(0),
        insertAction(0),
        replaceAction(0),
        deleteAction(0)
    {fillContextMenu();}

    void fillContextMenu()
    {
        QAction* focusAction = new QAction("Focus View", m_contextMenu);
        m_contextMenu->addAction(focusAction);
        connect(focusAction, SIGNAL(triggered()), this, SLOT(focusView()));
        m_contextMenu->addSeparator();

        if( addSpecimentOptions )
        {
        newAction = new QAction("New", m_contextMenu);
        connect(newAction, SIGNAL(triggered()), this, SLOT(newForm()));
        m_contextMenu->addAction(newAction);

        appendAction = new QAction("Append", m_contextMenu);
        connect(appendAction, SIGNAL(triggered()), this, SLOT(appendForm()));
        m_contextMenu->addAction(appendAction);

        insertAction = new QAction("Insert ...", m_contextMenu);
        connect(insertAction, SIGNAL(triggered()), this, SLOT(insertForm()));
        m_contextMenu->addAction(insertAction);

        replaceAction = new QAction("Replace", m_contextMenu);
        connect(replaceAction, SIGNAL(triggered()), this, SLOT(replaceForm()));
        m_contextMenu->addAction(replaceAction);

        deleteAction = new QAction("Delete", m_contextMenu);
        connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteForm()));
        m_contextMenu->addAction(deleteAction);

        swapPrevAction = new QAction("Swap with previous", m_contextMenu);
        connect(swapPrevAction, SIGNAL(triggered()), this, SLOT(swapPrevForm()));
        m_contextMenu->addAction(swapPrevAction);

        swapNextAction = new QAction("Swap with next", m_contextMenu);
        connect(swapNextAction, SIGNAL(triggered()), this, SLOT(swapNextForm()));
        m_contextMenu->addAction(swapNextAction);

        m_contextMenu->addSeparator();
        QAction* saveAction = new QAction("Save", m_contextMenu);
        m_contextMenu->addAction(saveAction);
        connect(saveAction, SIGNAL(triggered()), this, SLOT(saveForm()));

        QAction* saveAsAction = new QAction("Save As", m_contextMenu);
        m_contextMenu->addAction(saveAsAction);
        connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveFormAs()));
        }

        QAction* loadAction = new QAction("Load", m_contextMenu);
        m_contextMenu->addAction(loadAction);
        connect(loadAction, SIGNAL(triggered()), this, SLOT(loadForm()));

        if( addSpecimentOptions )
        {
        QAction* resetAction = new QAction("Reset", m_contextMenu);
        m_contextMenu->addAction(resetAction);
        connect(resetAction, SIGNAL(triggered()), this, SLOT(resetForm()));
        }

        if( !addSpecimentOptions)
        {
            QAction* updateAction = new QAction("Update Template", m_contextMenu);
            m_contextMenu->addAction(updateAction);
            connect(updateAction, SIGNAL(triggered()), this, SLOT(updateTemplate()));
        }
    }

public slots:
    void focusView()    {emit formFocus(this, this);}
    void saveForm()     {emit formSaved(this);}
    void saveFormAs()   {emit formSavedAs(this);}
    void loadForm()     {emit formLoaded(this);}
    void resetForm()    {emit formReseted(this);}
    void updateTemplate() {emit templateUpdated(this);}

    void newForm()      {emit formNew(this);}
    void appendForm()      {emit formAppend(this);}
    void insertForm()   {emit formInsert(this);}
    void replaceForm()  {emit formReplace(this);}
    void deleteForm()   {emit formDelete(this);}
    void swapPrevForm()   {emit formSwapPrev(this);}
    void swapNextForm()   {emit formSwapNext(this);}

    void newForm( bool enable ) { newAction->setEnabled( enable ); }
    void appendForm( bool enable ) { appendAction->setEnabled( enable ); }
    void insertForm( bool enable ) { insertAction->setEnabled( enable ); }
    void replaceForm( bool enable ) { replaceAction->setEnabled( enable ); }
    void deleteForm( bool enable ) { deleteAction->setEnabled( enable ); }
    void swapPrevForm( bool enable ) { swapPrevAction->setEnabled( enable ); }
    void swapNextForm( bool enable ) { swapNextAction->setEnabled( enable ); }

signals:
    void formFocus(FormItem*, ViewTreeItem*);
    void formSaved(FormItem*);
    void formSavedAs(FormItem*);
    void formLoaded(FormItem*);
    void formReseted(FormItem*);
    void templateUpdated(FormItem*);

    void formNew(FormItem*);
    void formAppend(FormItem*);
    void formInsert(FormItem*);
    void formReplace(FormItem*);
    void formDelete(FormItem*);
    void formSwapPrev(FormItem*);
    void formSwapNext(FormItem*);
};
#endif

