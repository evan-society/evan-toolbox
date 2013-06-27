#ifndef CURVESTOPITEM_H
#define CURVESTOPITEM_H


#include "FormItem.h"
#include "ew/Form3.h"


class CurveItem;

class CurvesTopItem : public ViewTreeItem
{
Q_OBJECT

private:
    bool addSpecimentOptions;
    bool m_creating;

    const ew::Form3 * m_form;


    CurvesTopItem(bool specimenOpt) : ViewTreeItem(CURVES_ITEM), addSpecimentOptions(specimenOpt)
    {fillContextMenu();}

public:
    CurvesTopItem(FormItem* parent, bool specimenOpt) :  ViewTreeItem(parent, CURVES_ITEM), addSpecimentOptions(specimenOpt)
    {fillContextMenu();}

    void setForm(const ew::Form3* form)
    {
        m_form = form;
    }

    void fillContextMenu()
    {
        if( addSpecimentOptions )
        {
            QAction* importCurve = new QAction("Import Curve", m_contextMenu);
            m_contextMenu->addAction(importCurve);
            connect(importCurve, SIGNAL(triggered()), this, SLOT(addCurve()));

//            QAction* createCurveA = new QAction("Create Curve", m_contextMenu);
//            m_contextMenu->addAction(createCurveA);
//            connect(createCurveA, SIGNAL(triggered()), this, SLOT(createCurve()));
        }
    }

    bool creatingCurve() {return m_creating;}
    void setCreatingCurve( bool val) {m_creating = val;}

public slots:
    void addCurve(const QString& id="", const QString& file="", bool loaded=false);
    void createCurve(const QString& id="", const QString& file="", bool loaded=false);
    void signalCurveColor(CurveItem* item) {emit curveColorChange(item, m_parentForm);}
    void signalDelete(CurveItem*);
    void signalCurveFocus(CurveItem* item) {emit curveFocus(m_parentForm, (ViewTreeItem*)item);}

signals:
    void curveFocus(FormItem*, ViewTreeItem*);
    bool curveItemAdded(CurveItem*, FormItem*);
    void curveColorChange(CurveItem*, FormItem*);
    void curveDeleted(FormItem*, int);
};
#endif
