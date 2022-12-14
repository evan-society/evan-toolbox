#ifndef TABLEAULAYOUT_H
#define TABLEAULAYOUT_H

#include <map>
#include <sstream>
#include <QMdiSubWindow>
#include "Designer/ui_Tab.h"
#include "FilmstripPlayerControls.h"
#include "ViewTree/LandmarkItem.h"
#include "ew/Dig3Space.h"
#include "ew/DataflowSpline3.h"
//#if TEMPLAND_TOOLKIT_BUILD //YN 9Nov2015: It does not build if the EMPLAND_TOOLKIT_BUILD is undefined
#include "tps.h"
#include "../../DataTypes/MatrixD.h"
//#endif


class CurvesTopItem;
class CurveItem;
class SurfaceTopItem;
class SurfaceItem;
class FormItem;

struct MappedLmk
{
    std::string templateID;
    std::string targetID;
    double tempX, tempY, tempZ;
    double tarX, tarY, tarZ;
};

class TableauLayout : public QWidget, public Ui::Form
{
Q_OBJECT
public:
    TableauLayout(QWidget *parent = 0, QGLWidget* share2 = 0);
    virtual ~TableauLayout();

	void fromString(const QString& data) {loadTableau(data, true);}

	QString toString() const {return m_tableauFile;}

	QString getCurrentFilename() { if( m_tableauFile.isEmpty() ) return "Untitled"; return m_tableauFile; }

	const ew::Dig3Space* getTargetDigSpace()
	{return m_dig3.get_spaces()[1];}

	const ew::Dig3Space* getTemplateDigSpace()
	{return m_dig3.get_spaces()[0];}

    const ew::Form3* getTemplateFormData()
    {return getTemplateDigSpace()->get_form_data();}

    const ew::Form3* getTargetFormData()
    {return getTargetDigSpace()->get_form_data();}

    QGLWidget* mainWidget()
    {return m_targetSliceView;}

    void selectPointsInViews( bool select ) { m_selectPointsInViews = select; }
    bool selectPointsInViews() { return m_selectPointsInViews; }
    void slideAll(int iterations, double eps, int pointsetIndex=-1);
    void gotoFrame(int n)
    {
        m_frameSlider->setValue( n + 1 );
        m_frameSBox->setValue( n + 1 );
        filmstripFrameNumberChanged( n + 1 );
    }
#if TEMPLAND_TOOLKIT_BUILD
    MatrixD* getSplineTemplate() { return m_splineTemplateLmks; }
    MatrixD* getSplineTarget() { return m_splineTargetLmks; }
#endif

protected:

    void closeEvent( QCloseEvent* event );

public slots:
    void changeBckgndColor();
    void resetForm(FormItem*);
    void loadForm(FormItem*, bool loaded=false, bool focus = true, bool resetLmkViewState = true );
    void saveFormAs(FormItem*);
    bool saveForm(FormItem*);
    bool saveTargetForm()         {return saveForm(m_targetFormItem);}
    bool saveTemplateForm()       {return saveForm(m_templateFormItem);}
    void newTableau();
    bool loadTableau(QString f="", bool loaded=false);
    void saveTableauAs();
    bool saveTableau();
    void pauseFilmstip();
    void stopFilmstip();
    void firstFrameFilmstrip();
    void forwardFilmstip();
    void backwardFilmstip();
    void lastFrameFilmstrip();
    void filmstripStatus( int status );
    void addTableau( int index = -1 );
    void replaceTableau();
    void deleteTableau();
    void moveTableauToPrev();
    void moveTableauToNext();
    void changeSliceColor(ViewTree*);
    void changeSliceClipR(ViewTree*);
    void changeLmkColor(FormItem*);
    bool addSurface(SurfaceItem*, FormItem*);
    void changeSurfaceColor(SurfaceItem*, FormItem*, bool);
    bool addCurve(CurveItem*, FormItem*);
    void changeCurveColor(CurveItem* surItem, FormItem* itemForm);
    void applyItemChange(ViewTree*, QTreeWidgetItem*, int);
    void highlightItem(ViewTree*, QTreeWidgetItem*);
    void changeLmkSize(FormItem*, bool);
    void changeLmksSymbol(FormItem*, ew::View3Landmarks::symbol_t);
    void moveToFrame(int);
    void focusView(FormItem*,ViewTreeItem*);
    void deleteLmk(FormItem* itemForm, int i);
    void deleteLmk(FormItem* patch, ViewTreeItem* semi, int index );
    void moveLmk(FormItem* itemForm, int i);
    void moveLmk(FormItem* patch, ViewTreeItem* semi, int index ); 
    void deleteAllLmk(FormItem* itemForm, int i);
    void deleteSurface(FormItem* itemForm, int i)
    {
        m_dig3.get_spaces()[itemForm==m_targetFormItem ? 1:0]->remove_form_surface(i);
        formUpdated(itemForm);
    }
    void deleteCurve(FormItem* itemForm, int i)
    {
        m_dig3.get_spaces()[itemForm==m_targetFormItem ? 1:0]->remove_form_curve(i);
        formUpdated(itemForm);
    }
    void tableauUpdated()
    {
        if(m_tableauSaved)
        {
            m_tableauSaved = false;
            QString str = windowTitle();
            if( !str.contains( '*' ) )
                setWindowTitle(windowTitle() + "*");
        }
    }
    void restTargetY(bool v ) { m_targetView->restrictAlongY(v); }
    void restTargetX(bool v ) { m_targetView->restrictAlongX(v); }
    void restTargetTwist(bool v ) { m_targetView->toggleTwist(v); }
    void restTargetSliceY(bool v ) { m_targetSliceView->restrictAlongY(v); }
    void restTargetSliceX(bool v ) { m_targetSliceView->restrictAlongX(v); }
    void restTargetSliceTwist(bool v ) { m_targetSliceView->toggleTwist(v); }

    void restTempY(bool v ) { m_targetView->restrictAlongY(v); }
    void restTempX(bool v ) { m_targetView->restrictAlongX(v); }
    void restTempTwist(bool v ) { m_targetView->toggleTwist(v); }
    void restTempSliceY(bool v ) { m_targetSliceView->restrictAlongY(v); }
    void restTempSliceX(bool v ) { m_targetSliceView->restrictAlongX(v); }
    void restTempSliceTwist(bool v ) { m_targetSliceView->toggleTwist(v); }

    void tblMoveToStart();
    void tblMoveToEnd();
    void tblMoveLeftE();
    void tblMoveLeft();
    void tblMoveRight();
    void tblMoveRightE();

    void tblSwapPrev();
    void tblSwapNext();

    void createConsensus()  {emit calcConsensus();}
    void slideAll()         {emit slideAllSignal();}

    void pointType( int type );

    void secPerFrame( const QString& text );
    void stepPerFrame( const QString& text );
    void stepPrevFrameFilmstrip();
    void stepNextFrameFilmstrip();

//    void tblNew( FormItem* item );
    void tblAppend( FormItem* item );
    void tblInsert( FormItem* item );
    void tblReplace( FormItem* item );
    void tblDelete( FormItem* item );

    void templateHide()
    {
        bool v = m_srcGrpBox->isVisible();
        m_srcGrpBox->setVisible(!v);
        m_templateItemTree->setVisible(!v);
    }

    void projectAll( bool checksurface = true, int target=1 );
    bool projectLmk( ViewTreeItem* item, int index, bool checksurface = true, bool showstatus = true, int target=1 );
    bool projectLmkOntoSurface( ViewTreeItem* item, int index, bool checksurface = true, bool showstatus = true, int target=1 );
    bool projectLmkOntoCurve( ViewTreeItem* item, int index, bool checksurface = true, bool showstatus = true , int target=1);
    bool projectSemiLmk(const QString& topId, bool checksurface = true, bool showstatus = true, int index=-1, int target=1);
    void lmkProject(FormItem* form, ViewTreeItem* item, int index);
    void lmkSlide( FormItem* form, ViewTreeItem* item, int index, bool slide);
    void lmkExport(FormItem* form);
    void lmkExportAll(FormItem* form);
//    void updateTemplate(FormItem* form);
    void saveCurve(FormItem* form, int index);
    const QString& getTargetFormFileName(){return m_targetFile;}
    const QString& getTemplateFormFileName(){return m_templateFile;}

    void replaceTargetForm(const QString& f)
    {
    	m_targetFile = f;
    	loadForm(m_targetFormItem, true);
    }
    void replaceTemplateForm(const QString& f)
    {
		m_templateFile = f;
		loadForm(m_templateFormItem, true);
	}
    const std::vector<ew::Dig3Tableau>& getTableauList() const {return m_tableauList;}
    void setTableau(size_t i, const ew::Dig3Tableau& tbl)
    {
    	if(i<m_tableauList.size())
    		m_tableauList[i] = tbl;
    }
signals:
	void status( QString msg );
	void status( QString msg, int val );
	void filenameChanges( QString filename );

    void newForm( bool enable );
    void appendForm( bool enable );
    void insertForm( bool enable );
    void replaceForm( bool enable );
    void deleteForm( bool enable );
    void swapPrevForm( bool enable );
    void swapNextForm( bool enable );
    void calcConsensus();
    void slideAllSignal();

private slots:
    void formUpdated(FormItem*);
    void targetFormUpdated();
    void mapLmk(FormItem* item, int index);
    void mapAllLmk(FormItem* item, int index);
    void matchLmk(FormItem* item, int index);
    void matchAllLmk();
    void matchSpline();
    void matchSplineWarp();
    void landmarkSelected( const QString& id, int index, int indexWithin, int space );
    void renameLmk(FormItem* form, ViewTreeItem* item, int index, const QString& newID);
    void adjustLandmarkState();
    void stateLmk(FormItem* form, ViewTreeItem* item, int index, int newState );
    void landmarkMoved( const QString& id, int space );
    void showFilmstrip( bool tog );
    void filmstripFrameNumberChanged( float num );
private:

    FormItem* m_templateFormItem;
    FormItem* m_targetFormItem;
    FilmstripPlayerControls m_filmstrip;


//#if TEMPLAND_TOOLKIT_BUILD //YN 9Nov2015: It does not build if the EMPLAND_TOOLKIT_BUILD is undefined
    TPS* m_tps;
    MatrixD* m_splineTemplateLmks;
    MatrixD* m_splineTargetLmks;
//#endif
	ew::DataflowNetwork m_ewNetwork;
    ew::Dig3 m_dig3;

    bool m_newTableau;
    bool m_savedAtleastOnce;
    bool m_treesFull;
    bool m_tableauSaved;
    bool m_templateSaved;
    bool m_targetSaved;
    QString m_templateFile;
    QString m_targetFile;
    QString m_tableauFile;
    std::stringstream m_ewWarnings;
    std::vector<ew::Dig3Tableau> m_tableauList;
    std::vector<MappedLmk> m_regLmks;
    std::vector<std::string> m_mappedLmks;
    LandmarksTopItem* m_templateTopLandmarks;
    LandmarksTopItem* m_targetTopLandmarks;
    bool m_regular;
    bool m_selectPointsInViews;
    SurfaceTopItem* m_templateTopSurface;
    SurfaceTopItem* m_targetTopSurface;
    CurvesTopItem* m_templateTopCurve;
    CurvesTopItem* m_targetTopCurve;

    void fillViewTrees(FormItem*);
    void loadViewTreeItems(const FormItem*, const ew::Form3&);
    bool getPointsetLocation( const std::string& id, int space, double &x, double& y, double& z, int& index );
    QString getNewFormFilename();
    bool isInTargetTreeView( const QString& id );
    void updateTreeViewStates( int space );
    bool hasStateLabel( const QString& lbl, QString& lblWithoutState );
    QString stateLabel( int state );
    void changeState( const std::string& id, int state );

    int embeddedCurveIndex(const std::string& id, const ew::Form3* form);
    int embeddedSurfaceIndex(const std::string& id, const ew::Form3* form);
    bool projectToEmbedding(const ew::Form3* form, const std::string& semiLmkId,
    						int surface_index, int curve_index, int semiLmkIndex=-1,
    						bool checksurface=true, bool showstatus=true, int target=1);
    bool checkSpline();
    bool warpLandmarkItem(LandmarkItem* lmi);
    bool warpSemilandmarkItem(SemiLandmarksTopItem* lmi);

    QString m_lastDirectory;
};

#endif // TABLEAULAYOUT_H
