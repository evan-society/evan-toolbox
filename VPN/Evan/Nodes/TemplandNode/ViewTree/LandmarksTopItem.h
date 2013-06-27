#ifndef LANDMARKSTOPITEM_H
#define LANDMARKSTOPITEM_H

#include "FormItem.h"
#include "ew/View3Landmarks.h"
#include "ew/Form3.h"
#include "ew/Dig3.h"
#include "ew/Dig3Space.h"
#include "../IdDialog.h"
#include "../SemiLmkIdDialog.h"
#include "ew/Dig3.h"

class SemiLandmarksTopItem;

class LandmarksTopItem : public ViewTreeItem
{
Q_OBJECT

private:
    bool m_templateTreeView;
    QAction* m_mapAllAction;
    ViewTreeItem* m_assignhere;
    QAction* m_projectAction;
    QAction* m_fixAction;
    QAction* m_deleteAllAction;
    QAction* m_exportAction;
    QAction* m_exportAllAction;
    QAction* m_saveCurveAction;

    LandmarksTopItem() : ViewTreeItem(LANDMARKS_ITEM), m_templateTreeView(false), m_mapAllAction(0), m_assignhere(0), m_projectAction(0), m_fixAction(0)
    {fillContextMenu();}

    const ew::Form3 * m_form;
    ew::Dig3* m_dig3;

    bool m_moving;
    int m_movingIndex;
    int m_movingSubIndex;
public:

    void setForm(const ew::Form3* form)
    {
        m_form = form;
    }

    void setDig3(ew::Dig3* dig3)
    {
        m_dig3 = dig3;
    }


    LandmarksTopItem(FormItem* parent) :  ViewTreeItem(parent, LANDMARKS_ITEM), m_templateTreeView(false), m_mapAllAction(0), m_assignhere(0)/*, addAction(0)//, m_splineAction(0)*/, m_projectAction(0), m_fixAction(0)
    {
        fillContextMenu();
        m_moving = false;
    }

    void fillContextMenu()
    {
        QAction* focusAction = new QAction("Focus View", m_contextMenu);
        m_contextMenu->addAction(focusAction);
        connect(focusAction, SIGNAL(triggered()), this, SLOT(focusView()));
        m_contextMenu->addSeparator();

        QAction* colorAction = new QAction("Colour", m_contextMenu);
        m_contextMenu->addAction(colorAction);
        connect(colorAction, SIGNAL(triggered()), this, SLOT(signalColorChange()));

//        QAction* labelsAction = new QAction("Labels", m_contextMenu);
//        labelsAction->setCheckable(true);
//        m_contextMenu->addAction(labelsAction);
//        QAction* projectToSur = new QAction("Project Semi-Landmarks", m_contextMenu);
//        m_contextMenu->addAction(projectToSur);
//        QAction* orientAction = new QAction("Orient and Relax Semi-Landmarks", m_contextMenu);
//        m_contextMenu->addAction(orientAction);
//        QAction* optimizeAction = new QAction("Optimize", m_contextMenu);
//        m_contextMenu->addAction(optimizeAction);

        QMenu* symbolMenu = new QMenu("Symbol");
        QAction* crossAction = new QAction("Cross", symbolMenu);
        crossAction->setCheckable(true);
        crossAction->setChecked(true);
        connect(crossAction, SIGNAL(triggered()), this, SLOT(signalXSymbol()));
        symbolMenu->addAction(crossAction);

        QAction* circleAction = new QAction("Circle", symbolMenu);
        circleAction->setCheckable(true);
        connect(circleAction, SIGNAL(triggered()), this, SLOT(signalCircleSymbol()));
        symbolMenu->addAction(circleAction);

        QAction* dotAction = new QAction("Dot", symbolMenu);
        dotAction->setCheckable(true);
        symbolMenu->addAction(dotAction);
        connect(dotAction, SIGNAL(triggered()), this, SLOT(signalDotSymbol()));
        m_contextMenu->addMenu(symbolMenu);

        QActionGroup* symbolsGroup = new QActionGroup(m_contextMenu);
        symbolsGroup->addAction(crossAction);
        symbolsGroup->addAction(circleAction);
        symbolsGroup->addAction(dotAction);
    }

    void enableMapChild( int index, bool enable );

    void enableRegChild( int index, bool enable );

    void templateTreeView( bool temp )
    {
        m_templateTreeView = temp;

        if( m_templateTreeView )
        {
            m_contextMenu->addSeparator();

            m_mapAllAction = new QAction("Warp all unmatched landmarks", m_contextMenu);
            m_contextMenu->addAction(m_mapAllAction);
            connect(m_mapAllAction, SIGNAL(triggered()), this, SLOT(mapAllLmk()));
        }
        else
        {
            m_contextMenu->addSeparator();
            QAction* semiAction = new QAction("Add Semi-Landmark", m_contextMenu);
            m_contextMenu->addAction(semiAction);
            connect(semiAction, SIGNAL(triggered()), this, SLOT(addSemiLandmark()));
            m_contextMenu->addSeparator();
            m_projectAction = new QAction("Project", m_contextMenu);
            m_contextMenu->addAction(m_projectAction);
            connect(m_projectAction, SIGNAL(triggered()), this, SLOT(project()));
            m_fixAction = new QAction("Fix all Landmarks", m_contextMenu);
            m_contextMenu->addAction(m_fixAction);
            connect(m_fixAction, SIGNAL(triggered()), this, SLOT(fix()));
            m_deleteAllAction = new QAction("Delete all Landmarks", m_contextMenu);
            m_contextMenu->addAction(m_deleteAllAction);
            connect(m_deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAll()));
            m_contextMenu->addSeparator();
            m_exportAction = new QAction("Export Landmarks for this Specimen", m_contextMenu);
            m_contextMenu->addAction(m_exportAction);
            m_exportAllAction = new QAction("Export Landmarks for all Specimens", m_contextMenu);
            m_contextMenu->addAction(m_exportAllAction);
            connect(m_exportAction, SIGNAL(triggered()), this, SLOT(exportLandmarks()));
            connect(m_exportAllAction, SIGNAL(triggered()), this, SLOT(exportAllLandmarks()));
            m_saveCurveAction = new QAction("Save as Curve", m_contextMenu);
            m_contextMenu->addAction(m_saveCurveAction);
            connect(m_saveCurveAction, SIGNAL(triggered()), this, SLOT(saveAsCurve()));
        }
    }

    void select( const QString& parentid, const QString& childid = "" );

    bool doesChildExist( const QString& parentid, const QString& childid );

    void mapAllEnable( bool enable )
    {
        if( m_mapAllAction )
        {
            m_mapAllAction->setEnabled( enable );
        }
    }

    void projectEnable( bool enable )
    {
        if( m_projectAction )
        {
            m_projectAction->setEnabled( enable );
        }
    }

    QString getSelectedLandmarkID();
    void selectNextLandmark( const QString& parentid, const QString& childid = "" );

    bool isMoving();
    bool getMovingItem(int &index, int &subIndex);
    void setMoving(bool value);


public slots:

    void focusView() {emit lmkFocus(m_parentForm, this);}
    void addSemiLandmark(const QString& suggestedName = "")
    {
        addSemiLandmarkSet( 0, suggestedName );
    }

    void addLandmarkItem(const QString& lmkId);
    void signalDelete(int index);
    void signalDelete(FormItem* semipatch, ViewTreeItem* semilmk, int index);
    void signalMove(int index);
    void signalMove(FormItem* semipatch, ViewTreeItem* semilmk, int index);
    void signalMap(int index);
    void signalMapAll(int index);
    void signalReg(int index);
    void signalXSymbol() {emit lmkSymbolChanged(m_parentForm, ew::View3Landmarks::SYMBOL_CROSS);}
    void signalCircleSymbol() {emit lmkSymbolChanged(m_parentForm, ew::View3Landmarks::SYMBOL_CIRCLE);}
    void signalDotSymbol() {emit lmkSymbolChanged(m_parentForm, ew::View3Landmarks::SYMBOL_DOT);}
    void signalColorChange() {emit lmkColorChanged(m_parentForm);}
    void mapAllLmk() {emit lmkMappedAll(m_parentForm, -1);}
    void assignLandmarksHere( ViewTreeItem* vti );
    void project() {emit lmkProject(m_parentForm, this, 0);}
    void fix() { emit lmkState( m_parentForm, this, -1, ew::Form3::STATE_SET ); }
    void deleteAll()
    {
    	int n = childCount()-1;
    	for(int i=n; i>=0; i--)
        {
            emit lmkDeleted(m_parentForm,i);
            QTreeWidgetItem * childItem = child(i);
            removeChild(childItem);
            delete childItem;
    	}
    }

    void rename(FormItem* form, ViewTreeItem* item, int index, const QString& newID)
    {
        emit lmkRename( form, item, index, newID );
    }
    void state( FormItem* form, ViewTreeItem* item, int index, int state )
    {
        emit lmkState( form, item, index, state );
    }
    void exportLandmarks() {emit lmkExport(m_parentForm);};
    void exportAllLandmarks() {emit lmkExportAll(m_parentForm);};
    void saveCurve(FormItem* form, int index) { emit lmkSaveCurve(form, index);}
    void saveAsCurve() { emit lmkSaveCurve(m_parentForm, 0);}


private slots:
    void project(FormItem* form, ViewTreeItem* item, int index) { emit lmkProject( form, item, index ); }
    void slide(FormItem* form, ViewTreeItem* item, int index, bool slide) { emit lmkSlide( form, item, index, slide); }

signals:
    void lmkFocus(FormItem*, ViewTreeItem*);
    void lmkDeleted(FormItem*, int);
    void lmkMoved(FormItem*, int);
    void lmkDeletedChild(FormItem* semipatch, ViewTreeItem* semilmk, int index);
    void lmkMovedChild(FormItem* semipatch, ViewTreeItem* semilmk, int index);
    void lmkMapped(FormItem*, int);
    void lmkMappedAll(FormItem*, int);
    void lmkRegister(FormItem*, int);
    void lmkAdded(FormItem*);
    void lmkSymbolChanged(FormItem*, ew::View3Landmarks::symbol_t);
    void lmkColorChanged(FormItem*);
    void addLandmarkHere( const QString& id, int semitype );
    void lmkProject(FormItem*, ViewTreeItem*, int);
    void lmkSlide( FormItem*, ViewTreeItem*, int, bool );
    void lmkRename(FormItem*, ViewTreeItem*, int, const QString&);
    void lmkState( FormItem*, ViewTreeItem*, int, int );
    void lmkExport(FormItem*);
    void lmkExportAll(FormItem*);
    void lmkSaveCurve(FormItem*, int);
    void lmkDeleteAll(FormItem*, int);

private:
    void addSemiLandmarkSet( int t, const QString& suggestedName = "" );
};
#endif
