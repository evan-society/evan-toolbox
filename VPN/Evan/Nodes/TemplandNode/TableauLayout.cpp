
#include "TableauLayout.h"
#include "TableauDialog.h"
#include "SlideDialog.h"

#include "../../Utilities/Logger.h"

#include "ViewTree/SurfaceItem.h"
#include "ViewTree/CurveItem.h"
#include "ViewTree/SliceItem.h"
#include "ViewTree/VolumeItem.h"
#include "ViewTree/GridItem.h"
#include "ViewTree/RefPlanesItem.h"
#include "ViewTree/SemiLandmarksTopItem.h"
#include "ViewTree/SemiLandmarkItem.h"

#include <QMessageBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QSettings>
#include <QTextStream>

#include "ew/View3Surface.h"
#include "ew/Surface3.h"
#include "ew/ErrorIO.h"
#include "ew/DataflowSurface3E.h"
#include "ew/DataflowCurve3E.h"

#include <limits>
#include <cmath>
#include <map>

using namespace std;

double project_point_to_line_segment(float *A, float *B, double *p, double *q)
{
    double d = 0;

    float AB[3];

    AB[0] = (B[0]-A[0]);
    AB[1] = (B[1]-A[1]);
    AB[2] = (B[2]-A[2]);

    float AB_squared = AB[0]*AB[0]+AB[1]*AB[1]+AB[2]*AB[2];
    if(AB_squared < 1e-10)
    {
        q[0] = A[0];
        q[1] = A[1];
        q[2] = A[2];
    }
    else
    {
		float Ap[3];
        Ap[0] = (p[0]-A[0]);
        Ap[1] = (p[1]-A[1]);
        Ap[2] = (p[2]-A[2]);

        double t = (Ap[0]*AB[0]+Ap[1]*AB[1]+Ap[2]*AB[2])/AB_squared;
        if (t < 0.0)
        {
            q[0] = A[0];
            q[1] = A[1];
            q[2] = A[2];
        }
        else if (t > 1.0)
        {
            q[0] = B[0];
            q[1] = B[1];
            q[2] = B[2];
        }
        else
        {
            q[0] = A[0] + t * AB[0];
            q[1] = A[1] + t * AB[1];
            q[2] = A[2] + t * AB[2];
        }
        d = std::sqrt((p[0]-q[0])*(p[0]-q[0])+(p[1]-q[1])*(p[1]-q[1])+(p[2]-q[2])*(p[2]-q[2]));
    }

    return d;
}

void projectOntoCurve(const ew::DataflowCurve3E * const * cur, int *edge, double *coeff, double *tangent, double *proj, double *p , int index)
{
    const ew::Curve3 *data = cur[index]->get_data();

    std::vector<float> points = data->points;
    std::vector<int> edges = data->edges;

    double mindist = std::numeric_limits<double>::infinity();
    for(unsigned int i=0;i<edges.size()/2;i++)
    {
        int e1 = edges[i*2];
        int e2 = edges[i*2+1];
        float *A = &points[e1*3];
        float *B = &points[e2*3];
        double q[3];

        double d = project_point_to_line_segment(A, B, p, q);
        if(d<mindist)
        {
            mindist = d;
            proj[0] = q[0];
            proj[1] = q[1];
            proj[2] = q[2];

            double d0 = (A[0]-B[0]);
            double d1 = (A[1]-B[1]);
            double d2 = (A[2]-B[2]);

            double div = std::sqrt(d0*d0+d1*d1+d2*d2);

            if(div > 1e-10)
            {
                tangent[0] = d0 / div;
                tangent[1] = d1 / div;
                tangent[2] = d2 / div;
            }
            else
            {
                tangent[0] = 1;
                tangent[1] = 0;
                tangent[2] = 0;
            }
        }
    }
}

const unsigned char* chooseColor(const unsigned char* oldCol)
{
    unsigned char* col = NULL;
    QColor result;
    result.setRgb(oldCol[0], oldCol[1], oldCol[2]);
    result = QColorDialog::getColor(result);

    if(result.isValid())
    {
        int ir, ig, ib;
        result.getRgb(&ir, &ig, &ib);
        col = new unsigned char[3];
        col[0] = (unsigned char)ir;
        col[1] = (unsigned char)ig;
        col[2] = (unsigned char)ib;
    }
    return (const unsigned char* )col;
}

TableauLayout::TableauLayout(QWidget *parent, QGLWidget* share2) : QWidget(parent), m_tps(0), m_dig3(&m_ewNetwork), m_newTableau(false),
                                                m_savedAtleastOnce(false), m_treesFull(false), m_tableauSaved(true), m_templateSaved(true),
                                                m_targetSaved(true), m_templateFile(""), m_targetFile(""), m_tableauFile(""), m_regular( false ), m_selectPointsInViews( true ), m_templateTopSurface(0), m_targetTopSurface(0)
{
	setupUi(this, &m_ewNetwork, share2, &m_dig3);

    ew::Transform3 trm;
    trm.set_to_identity();

    for(int i=0; i<m_dig3.get_n_views(); ++i)
        m_dig3.get_views()[i]->set_view_mapping(&trm);

    m_templateFormItem = new FormItem(m_templateItemTree, false );
    m_templateFormItem->setText(0, "Form ()");
    m_targetFormItem = new FormItem(m_targetItemTree, true);
    m_targetFormItem->setText(0, "Form ()");

    connect(m_frameSBox, SIGNAL(valueChanged(int)), this, SLOT(moveToFrame(int)));
    connect(m_frameSlider, SIGNAL(valueChanged(int)), this, SLOT(moveToFrame(int)));

    connect(m_targetItemTree, SIGNAL(viewItemChanged(ViewTree*, QTreeWidgetItem*, int)),
            this, SLOT(applyItemChange(ViewTree*, QTreeWidgetItem*, int)));
    connect(m_templateItemTree, SIGNAL(viewItemChanged(ViewTree*, QTreeWidgetItem*, int)),
            this, SLOT(applyItemChange(ViewTree*, QTreeWidgetItem*, int)));
    connect(m_targetItemTree, SIGNAL(viewItemSelected(ViewTree*, QTreeWidgetItem*)),
            this, SLOT(highlightItem(ViewTree*, QTreeWidgetItem*)));
    connect(m_templateItemTree, SIGNAL(viewItemSelected(ViewTree*, QTreeWidgetItem*)),
            this, SLOT(highlightItem(ViewTree*, QTreeWidgetItem*)));

    m_filmGrpBox->setVisible(false);
    m_srcGrpBox->setVisible(false);
    m_templateItemTree->setVisible(false);

    m_templateTopLandmarks = 0;
    m_targetTopLandmarks = 0;

    fillViewTrees(m_templateFormItem);
    fillViewTrees(m_targetFormItem);

    connect( m_targetView, SIGNAL(landmarkSelected(const QString&, int, int, int)), this, SLOT(landmarkSelected(const QString&, int, int, int)) );
    connect( m_targetSliceView, SIGNAL(landmarkSelected(const QString&, int, int, int)), this, SLOT(landmarkSelected(const QString&, int, int, int)) );
    connect( m_templateView, SIGNAL(landmarkSelected(const QString&, int, int, int)), this, SLOT(landmarkSelected(const QString&, int, int, int)) );
    connect( m_templateSliceView, SIGNAL(landmarkSelected(const QString&, int, int, int)), this, SLOT(landmarkSelected(const QString&, int, int, int)) );

    connect( m_targetView, SIGNAL(adjustLandmarkState()), this, SLOT(adjustLandmarkState()) );
    connect( m_targetSliceView, SIGNAL(adjustLandmarkState()), this, SLOT(adjustLandmarkState()) );

    connect( m_targetView, SIGNAL(landmarkMoved(const QString&, int)), this, SLOT(landmarkMoved(const QString&, int)) );
    connect( m_targetSliceView, SIGNAL(landmarkMoved(const QString&, int)), this, SLOT(landmarkMoved(const QString&, int)) );

    m_targetView->templateLandmarkTopItem( m_templateTopLandmarks );
    m_targetSliceView->templateLandmarkTopItem( m_templateTopLandmarks );
    m_targetView->targetLandmarkTopItem( m_targetTopLandmarks );
    m_targetSliceView->targetLandmarkTopItem( m_targetTopLandmarks );
//    m_targetView->curvesTopItem(m_targetTopCurve);
//    m_targetSliceView->curvesTopItem(m_targetTopCurve);

    m_templateView->IsTemplate( true );
    m_templateSliceView->IsTemplate( true );

    emit appendForm( false );
    emit replaceForm( false );
    emit deleteForm( false );
    emit swapPrevForm( false );
    emit swapNextForm( false );
    m_tblStart->setDisabled( true );
    m_tblEnd->setDisabled( true );
    m_tblLeftE->setDisabled( true );
    m_tblLeft->setDisabled( true );
    m_tblRight->setDisabled( true );
    m_tblListRightE->setDisabled( true );

    //m_consensusPB->setHidden( true );
    m_consensusPB->setDisabled( true );

    m_filmFirstFrame->setDisabled( true );
    m_filmPlayBackward->setDisabled( true );
    m_filmPlay->setDisabled( true );
    m_filmPause->setDisabled( true );
    m_filmStop->setDisabled( true );
    m_filmLastFrame->setDisabled( true );
    m_filmStepPrev->setDisabled( true );
    m_filmStepNext->setDisabled( true );
    QString v;
    v = v.setNum( m_filmstrip.secPerFrame() );
    m_secFrameTXT->setText( v );
    m_secFrameTXT->setDisabled( true );
    v = v.setNum( (int)m_filmstrip.stepPerFrame() );
    m_stepsFrameTXT->setText( v );
    m_stepsFrameTXT->setDisabled( true );


    m_splineTemplateLmks = new MatrixD;
    m_splineTargetLmks = new MatrixD;

    unsigned char col[3] = {0, 0, 127};

    m_templateView->get_slice_items()[2]->set_color(col);
    m_targetView->get_slice_items()[3]->set_color(col);
    m_templateSliceView->get_slice_items()[2]->set_color(col);
    m_targetSliceView->get_slice_items()[3]->set_color(col);
}

TableauLayout::~TableauLayout()
{
    for(int i = m_dig3.get_n_views() - 1; i >= 0; --i)
        delete m_dig3.get_views()[i];
    if(m_ewWarnings.str().length() > 0)
        QMessageBox::information(this, "Session Warnings", QString("EW library Warnings:\n") + m_ewWarnings.str().c_str());
}

void TableauLayout::fillViewTrees(FormItem* formItem)
{
    m_treesFull = false;

    connect( formItem==m_targetFormItem ? m_targetView : m_templateView, SIGNAL(viewChanged()),this, SLOT(tableauUpdated()));
    connect( formItem==m_targetFormItem ? m_targetSliceView : m_templateSliceView, SIGNAL(viewChanged()),this, SLOT(tableauUpdated()));

    if( formItem==m_targetFormItem )
    {
        connect( m_targetView, SIGNAL(viewChanged()),this, SLOT(targetFormUpdated()));
        connect( m_targetSliceView, SIGNAL(viewChanged()),this, SLOT(targetFormUpdated()));
    }

    connect(formItem, SIGNAL(formFocus(FormItem*, ViewTreeItem*)), this, SLOT(focusView(FormItem*, ViewTreeItem*)));
    if( formItem == m_targetFormItem )
    {
    connect(formItem, SIGNAL(formSaved(FormItem*)), this, SLOT(saveForm(FormItem*)));
    connect(formItem, SIGNAL(formSavedAs(FormItem*)), this, SLOT(saveFormAs(FormItem*)));
    connect(formItem, SIGNAL(formReseted(FormItem*)), this, SLOT(resetForm(FormItem*)));
    connect(formItem, SIGNAL(formExported(FormItem*)), this, SLOT(exportForm(FormItem*)));

    connect(formItem, SIGNAL(formAppend(FormItem*)), this, SLOT(tblAppend(FormItem*)));
    connect(formItem, SIGNAL(formInsert(FormItem*)), this, SLOT(tblInsert(FormItem*)));
    connect(formItem, SIGNAL(formReplace(FormItem*)), this, SLOT(tblReplace(FormItem*)));
    connect(formItem, SIGNAL(formDelete(FormItem*)), this, SLOT(tblDelete(FormItem*)));

    connect( this, SIGNAL(newForm(bool)), formItem, SLOT(newForm(bool)) );
    connect( this, SIGNAL(appendForm(bool)), formItem, SLOT(appendForm(bool)) );
    connect( this, SIGNAL(insertForm(bool)), formItem, SLOT(insertForm(bool)) );
    connect( this, SIGNAL(replaceForm(bool)), formItem, SLOT(replaceForm(bool)) );
    connect( this, SIGNAL(deleteForm(bool)), formItem, SLOT(deleteForm(bool)) );
    }

    connect(formItem, SIGNAL(formLoaded(FormItem*)), this, SLOT(loadForm(FormItem*)));

    connect( formItem, SIGNAL(formSwapPrev(FormItem*)), this, SLOT(tblSwapPrev()));
    connect( formItem, SIGNAL(formSwapNext(FormItem*)), this, SLOT(tblSwapNext()));

//    VolumeItem* targetVolume = new VolumeItem(formItem);
//    targetVolume->setText(0, "Volume");
//    targetVolume->setCheckState(1, Qt::Checked);
//    targetVolume->setCheckState(2, Qt::Checked);

    SurfaceTopItem* targetTopSurfaces = 0;
    if( formItem == m_targetFormItem )
    {
        targetTopSurfaces = m_targetTopSurface = new SurfaceTopItem(formItem, (formItem==m_targetFormItem) );
    }
    else
    {
        targetTopSurfaces = m_templateTopSurface = new SurfaceTopItem(formItem, (formItem==m_targetFormItem) );
    }
    targetTopSurfaces->setText(0, "Surfaces");
    targetTopSurfaces->setCheckState(1, Qt::Checked);
    targetTopSurfaces->setCheckState(2, Qt::Checked);
    connect(targetTopSurfaces, SIGNAL(surfaceFocus(FormItem*, ViewTreeItem*)), this, SLOT(focusView(FormItem*, ViewTreeItem*)));
    if( formItem == m_targetFormItem )
    {
        connect(targetTopSurfaces, SIGNAL(surfaceItemAdded(SurfaceItem*, FormItem*)),
                this, SLOT(addSurface(SurfaceItem*, FormItem*)));
    }
    connect(targetTopSurfaces, SIGNAL(surfaceColorChange(SurfaceItem*, FormItem*, bool)),
            this, SLOT(changeSurfaceColor(SurfaceItem*, FormItem*, bool)));
    connect(targetTopSurfaces, SIGNAL(surfaceDeleted(FormItem*, int)), this, SLOT(deleteSurface(FormItem*, int)));

    CurvesTopItem* targetTopCurve = 0;
    if( formItem == m_targetFormItem )
    {
        targetTopCurve = m_targetTopCurve = new CurvesTopItem(formItem, (formItem==m_targetFormItem) );
    }
    else
    {
        targetTopCurve = m_templateTopCurve = new CurvesTopItem(formItem, (formItem==m_targetFormItem) );
    }
    targetTopCurve->setText(0, "Curves");
    targetTopCurve->setCheckState(1, Qt::Checked);
    targetTopCurve->setCheckState(2, Qt::Checked);
    connect(targetTopCurve, SIGNAL(curveFocus(FormItem*, ViewTreeItem*)), this, SLOT(focusView(FormItem*, ViewTreeItem*)));
    if( formItem == m_targetFormItem )
    {
        connect(targetTopCurve, SIGNAL(curveItemAdded(CurveItem*, FormItem*)),
                this, SLOT(addCurve(CurveItem*, FormItem*)));
        connect(targetTopCurve, SIGNAL(curveItemCreated(CurveItem*, FormItem*)),
                this, SLOT(createCurve(CurveItem*, FormItem*)));
    }
    connect(targetTopCurve, SIGNAL(curveColorChange(CurveItem*, FormItem*)),
            this, SLOT(changeCurveColor(CurveItem*, FormItem*)));
    connect(targetTopCurve, SIGNAL(curveDeleted(FormItem*, int)), this, SLOT(deleteCurve(FormItem*, int)));

    LandmarksTopItem* lmkTopItem = 0;
    if( formItem == m_targetFormItem )
    {
        lmkTopItem = m_targetTopLandmarks = new LandmarksTopItem(formItem);
        m_targetTopLandmarks->templateTreeView( false );
        m_targetTopLandmarks->setForm(getTargetFormData());
        m_targetTopLandmarks->setDig3(&m_dig3);
    }
    else
    {
        lmkTopItem = m_templateTopLandmarks = new LandmarksTopItem(formItem);
        m_templateTopLandmarks->templateTreeView( true );
        m_templateTopLandmarks->setForm(getTemplateFormData());
        m_templateTopLandmarks->setDig3(&m_dig3);
    }
    lmkTopItem->setText(0, "Landmarks");
    lmkTopItem->setCheckState(1, Qt::Checked);
    lmkTopItem->setCheckState(2, Qt::Checked);
    connect( lmkTopItem, SIGNAL(lmkFocus(FormItem*, ViewTreeItem*)), this, SLOT(focusView(FormItem*, ViewTreeItem*)));
    connect( lmkTopItem, SIGNAL(lmkColorChanged(FormItem*)), this, SLOT(changeLmkColor(FormItem*)));
    connect( lmkTopItem, SIGNAL(lmkAdded(FormItem*)), this, SLOT(formUpdated(FormItem*)));
    connect( formItem==m_targetFormItem ? m_targetView : m_templateView, SIGNAL(landmarkAdded(const QString&)),
            lmkTopItem, SLOT(addLandmarkItem(const QString&)));
    connect( formItem==m_targetFormItem ? m_targetSliceView : m_templateSliceView, SIGNAL(landmarkAdded(const QString&)),
            lmkTopItem, SLOT(addLandmarkItem(const QString&)));
    connect( lmkTopItem, SIGNAL(lmkDeleted(FormItem*, int)), this, SLOT(deleteLmk(FormItem*, int)));
    connect( lmkTopItem, SIGNAL(lmkDeletedChild(FormItem*, ViewTreeItem*, int)), this, SLOT(deleteLmk(FormItem*, ViewTreeItem*, int)));
    connect( lmkTopItem, SIGNAL(lmkMoved(FormItem*, int)), this, SLOT(moveLmk(FormItem*, int)));
    connect( lmkTopItem, SIGNAL(lmkMovedChild(FormItem*, ViewTreeItem*, int)), this, SLOT(moveLmk(FormItem*, ViewTreeItem*, int)));
    connect( lmkTopItem, SIGNAL(lmkSizeChange(FormItem*,bool)), this, SLOT(changeLmkSize(FormItem*,bool)));
    connect( lmkTopItem, SIGNAL(lmkSymbolChanged(FormItem*, ew::View3Landmarks::symbol_t)),
                            this, SLOT(changeLmksSymbol(FormItem*, ew::View3Landmarks::symbol_t)));
    connect( lmkTopItem, SIGNAL(lmkDeleteAll(FormItem*, int)), this, SLOT(deleteAllLmk(FormItem*, int)));
    if( formItem == m_templateFormItem )
    {
        connect( lmkTopItem, SIGNAL(lmkMapped(FormItem*, int)), this, SLOT(mapLmk(FormItem*, int)) );
        connect( lmkTopItem, SIGNAL(lmkMappedAll(FormItem*, int)), this, SLOT(mapAllLmk(FormItem*, int)) );

        connect( lmkTopItem, SIGNAL(matchSpline()), this, SLOT(matchSpline()) );
        connect( lmkTopItem, SIGNAL(matchSplineWarp()), this, SLOT(matchSplineWarp()) );
    }
    else
    {
        connect( lmkTopItem, SIGNAL(lmkRegister(FormItem*, int)), this, SLOT(matchLmk(FormItem*, int)) );
        connect( lmkTopItem, SIGNAL(addLandmarkHere(const QString&, int)), m_targetView, SLOT(addLandmarkHere(const QString&, int)) );
        connect( lmkTopItem, SIGNAL(addLandmarkHere(const QString&, int)), m_targetSliceView, SLOT(addLandmarkHere(const QString&, int)) );
        connect( lmkTopItem, SIGNAL(lmkProject(FormItem*, ViewTreeItem*, int)), this, SLOT(lmkProject(FormItem*, ViewTreeItem*, int)) );
        connect( lmkTopItem, SIGNAL(lmkSlide(FormItem*, ViewTreeItem*, int, bool)), this, SLOT(lmkSlide(FormItem*, ViewTreeItem*, int, bool)) );
        connect( lmkTopItem, SIGNAL(lmkRename(FormItem*, ViewTreeItem*, int, const QString&)), this, SLOT(renameLmk(FormItem*, ViewTreeItem*, int, const QString&)) );
        connect( lmkTopItem, SIGNAL(lmkState(FormItem*, ViewTreeItem*, int, int)), this, SLOT(stateLmk(FormItem*, ViewTreeItem*, int, int)) );
        connect( lmkTopItem, SIGNAL(lmkExport(FormItem*)), this, SLOT(lmkExport(FormItem*)) );
        connect( lmkTopItem, SIGNAL(lmkExportAll(FormItem*)), this, SLOT(lmkExportAll(FormItem*)) );
        connect( lmkTopItem, SIGNAL(lmkSaveCurve(FormItem*, int)), this, SLOT(saveCurve(FormItem*, int)) );
    }

    SliceItem* targetSlice = new SliceItem(formItem==m_targetFormItem? m_targetItemTree: m_templateItemTree);
    targetSlice->setText(0, "Slice");
    targetSlice->setCheckState(1, Qt::Checked);
    targetSlice->setCheckState(2, Qt::Checked);
    connect(targetSlice, SIGNAL(sliceColorAction(ViewTree*)), this, SLOT(changeSliceColor(ViewTree*)));
    connect(targetSlice, SIGNAL(sliceFocus(FormItem*, ViewTreeItem*)), this, SLOT(focusView(FormItem*, ViewTreeItem*)));
    connect(targetSlice, SIGNAL(sliceClipAction(ViewTree*)), this, SLOT(changeSliceClipR(ViewTree*)));

//    GridItem* targetGrid = new GridItem(formItem==m_targetFormItem? m_targetItemTree: m_templateItemTree);
//    targetGrid->setText(0, "Grid");
//    targetGrid->setCheckState(1, Qt::Unchecked);
//    targetGrid->setCheckState(2, Qt::Unchecked);

//    RefPlanesItem* targetRefPlanes = new RefPlanesItem(formItem==m_targetFormItem? m_targetItemTree: m_templateItemTree);
//    targetRefPlanes->setText(0, "Reference Planes");
//    targetRefPlanes->setCheckState(1, Qt::Unchecked);
//    targetRefPlanes->setCheckState(2, Qt::Unchecked);

    formItem->setExpanded(true);
    m_treesFull = true;

    m_templateItemTree->resizeColumnToContents( 0 );
    m_templateItemTree->resizeColumnToContents( 1 );
    m_templateItemTree->resizeColumnToContents( 2 );

    m_targetItemTree->resizeColumnToContents( 0 );
    m_targetItemTree->resizeColumnToContents( 1 );
    m_targetItemTree->resizeColumnToContents( 2 );

    targetSlice->setCheckState(1, Qt::Unchecked);
    targetSlice->setCheckState(2, Qt::Unchecked);
}

void TableauLayout::lmkProject(FormItem* form, ViewTreeItem* item, int index)
{
    if( form == m_templateFormItem )
        return;

    if( item->getType() == ViewTreeItem::LANDMARKS_ITEM )
    {
        projectAll( false );
    }
    else if( item->getType() == ViewTreeItem::LANDMARK_ITEM )
    {
        projectLmk( item, index, false );
    }
    else if( item->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
    {
        projectSemiLmk( dynamic_cast< SemiLandmarksTopItem* >( item )->getLmkID(), false );
    }
    else if( item->getType() == ViewTreeItem:: SEMILANDMARK_ITEM )
    {
    	SemiLandmarkItem* semilmk = dynamic_cast< SemiLandmarkItem* >( item );
    	projectSemiLmk( semilmk->getParent()->getLmkID(), false, true, index);
    }
    // add the landmark states
    updateTreeViewStates( 1 );

}

void TableauLayout::projectAll( bool checksurface, int target)
{
    int notprojected = 0;
    LandmarksTopItem* lmkTopItem = (target==1)?m_targetTopLandmarks:m_templateTopLandmarks;
    int count = lmkTopItem->childCount();
    for( int i = 0; i < count; ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( lmkTopItem->child( i ) );

        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
        {
            LandmarkItem *li = dynamic_cast< LandmarkItem* >( vti );
            if( projectLmk( vti, li->getLmkIndex(), checksurface, false, target ) == false )
            {
                ++notprojected;
            }
        }
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
        {
            if( projectSemiLmk( dynamic_cast< SemiLandmarksTopItem* >(vti)->getLmkID(), checksurface, false, target ) == false )
            {
                ++notprojected;
            }
        }
    }

    if(target==1)
    {
		QString msg; msg.setNum( count - notprojected );
		msg += " Landmarks have been projected.";
		if( notprojected > 0 )
		{
			QString ss; ss.setNum( notprojected );
			msg += " ";
			msg += ss;
			msg += " Landmarks were already projected.";
		}
		Logger::getInstance()->log( msg, Logger::INFO );
    }

    updateTreeViewStates(target);
}

bool TableauLayout::projectLmk( ViewTreeItem* item, int index, bool checksurface, bool showstatus, int target )
{
    if( m_dig3.get_spaces()[target]->get_form_data()->surfaces.size() > 0 )
    {
        return projectLmkOntoSurface(item, index, checksurface, showstatus, target );
    }
    else if( m_dig3.get_spaces()[target]->get_form_data()->curves.size() > 0 )
    {
        return projectLmkOntoCurve(item, index, checksurface, showstatus, target );
    }
    else
    {
        QMessageBox::information( this, "Error", "There is no object to project the landmark onto. Please import a surface or a curve." );
    }

    return false;
}

bool TableauLayout::projectLmkOntoSurface( ViewTreeItem* item, int index, bool checksurface, bool showstatus, int target  )
{
    std::string s = dynamic_cast< LandmarkItem* >( item )->getLmkID().toStdString();
    const ew::Form3* form = m_dig3.get_spaces()[target]->get_form_data();
    if( form != 0 )
        for(unsigned int j=0; j<form->pointsets.size(); ++j)
            if( form->pointsets[j].type == ew::Form3::TYPE_LANDMARK )
            {
                if( form->pointsets[j].id == s )
                {
                    if( form->pointsets[j].state == ew::Form3::STATE_WARPED ||
                        form->pointsets[j].state == ew::Form3::STATE_SET )
                    {
                        double p[3];
                        p[0] = form->pointsets[j].locations[0];
                        p[1] = form->pointsets[j].locations[1];
                        p[2] = form->pointsets[j].locations[2];

                        const ew::DataflowSurface3E * const * sur = m_dig3.get_spaces()[target]->get_surface_nodes();

                        int face[3];
                        double coeff[3];
                        double normal[3];
                        double proj[3];

                        (*sur)->project( face, coeff, normal, proj, p );

                        ew::Form3PointSet ps = form->pointsets[j];

                        ps.locations[0] = proj[0];
                        ps.locations[1] = proj[1];
                        ps.locations[2] = proj[2];

                        ps.state = ew::Form3::STATE_PROJECTED;

                        ew::Dig3Space *sp = m_dig3.get_spaces()[target];
                        bool b = false;
                        sp->set_form_pointset(&b, &ps);

                        // make sure to note form has been changed
                        m_targetSaved = false;

                        if( showstatus )
                        {
                            QString msg = s.c_str();
                            msg += " Landmark has been projected.";
                            Logger::getInstance()->log( msg, Logger::INFO );
                        }

                        return true;
                    }
                    else if( form->pointsets[j].state == ew::Form3::STATE_PROJECTED )
                    {
//                        if( checksurface )
//                            QMessageBox::critical( this, "Error", "Landmark already projected." );

                        QString msg; msg = s.c_str(); msg += " has already been projected.";
                        Logger::getInstance()->log( msg, Logger::WARNING );

                        return false;
                    }
                }
            }
    return false;
}

bool TableauLayout::projectLmkOntoCurve( ViewTreeItem* item, int index, bool checksurface, bool showstatus, int target )
{
    std::string s = dynamic_cast< LandmarkItem* >( item )->getLmkID().toStdString();
    const ew::Form3* form = m_dig3.get_spaces()[target]->get_form_data();
    if( form != 0 )
        for(unsigned int j=0; j<form->pointsets.size(); ++j)
            if( form->pointsets[j].type == ew::Form3::TYPE_LANDMARK )
            {
                if( form->pointsets[j].id == s )
                {
                    if( form->pointsets[j].state == ew::Form3::STATE_WARPED ||
                        form->pointsets[j].state == ew::Form3::STATE_SET )
                    {
                        double p[3];
                        p[0] = form->pointsets[j].locations[0];
                        p[1] = form->pointsets[j].locations[1];
                        p[2] = form->pointsets[j].locations[2];

                        const ew::DataflowCurve3E * const * cur = m_dig3.get_spaces()[target]->get_curve_nodes();

						int edge;
						double coeff[3];
						double normal[3];
						double proj[3];

						projectOntoCurve(cur, &edge, coeff, normal, proj, p, 0);

                        ew::Form3PointSet ps = form->pointsets[j];

                        ps.locations[0] = proj[0];
                        ps.locations[1] = proj[1];
                        ps.locations[2] = proj[2];

                        ps.state = ew::Form3::STATE_PROJECTED;

                        ew::Dig3Space *sp = m_dig3.get_spaces()[target];
                        bool b = false;
                        sp->set_form_pointset(&b, &ps);

                        // make sure to note form has been changed
                        m_targetSaved = false;

                        if( showstatus )
                        {
                            QString msg = s.c_str();
                            msg += " Landmark has been projected.";
                            Logger::getInstance()->log( msg, Logger::INFO );
                        }

                        return true;
                    }
                    else if( form->pointsets[j].state == ew::Form3::STATE_PROJECTED )
                    {
//                        if( checksurface )
//                            QMessageBox::critical( this, "Error", "Landmark already projected." );

                        QString msg; msg = s.c_str(); msg += " has already been projected.";
                        Logger::getInstance()->log( msg, Logger::WARNING );

                        return false;
                    }
                }
            }
    return false;
}

bool TableauLayout::projectToEmbedding(const ew::Form3* form, const std::string& semiLmkId,
										int surface_index, int curve_index, int semiLmkIndex,
										bool checksurface, bool showstatus, int target)
{
	for(unsigned int j=0; j<form->pointsets.size(); ++j)
		if(form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
		{
			if(form->pointsets[j].id == semiLmkId)
			{
				if( form->pointsets[j].state == ew::Form3::STATE_WARPED ||
					form->pointsets[j].state == ew::Form3::STATE_OPTIMIZED ||
					form->pointsets[j].state == ew::Form3::STATE_SET ||
					form->pointsets[j].state == ew::Form3::STATE_PROJECTED)
				{
					ew::Form3PointSet ps = form->pointsets[j];

					int n = (semiLmkIndex<0)?form->pointsets[j].n:1; //either project all or one semilandmark
					for(int i=0; i<n; ++i)
					{
						int pos = (semiLmkIndex<0)?i:semiLmkIndex;
						pos*=3;

						double p[3];
						double coeff[3];
						double normal[3];
						double proj[3];

						p[0] = form->pointsets[j].locations[pos];
						p[1] = form->pointsets[j].locations[pos+1];
						p[2] = form->pointsets[j].locations[pos+2];

						if(surface_index>=0)
						{
							int face[3];
							const ew::DataflowSurface3E * const * sur = m_dig3.get_spaces()[target]->get_surface_nodes();
							sur = &sur[surface_index];
							(*sur)->project(face, coeff, normal, proj, p);
						}
						else if(curve_index>=0)
						{
							int edge;
							const ew::DataflowCurve3E * const * cur = m_dig3.get_spaces()[target]->get_curve_nodes();
							projectOntoCurve(cur, &edge, coeff, normal, proj, p, curve_index);
						}

						ps.locations[pos] 	= proj[0];
						ps.locations[pos+1] = proj[1];
						ps.locations[pos+2] = proj[2];

						if(ps.relax_params.size()>=pos+3)
						{
							ps.relax_params[pos] = normal[0];
							ps.relax_params[pos+1] = normal[1];
							ps.relax_params[pos+2] = normal[2];
						}
						else
						{
							while(ps.relax_params.size()<pos)
								ps.relax_params.push_back(0); //fill with zeros until reaching the right index

							ps.relax_params.push_back( normal[0] );
							ps.relax_params.push_back( normal[1] );
							ps.relax_params.push_back( normal[2] );
						}
					}

					ps.state = ew::Form3::STATE_PROJECTED;

					ew::Dig3Space *sp = m_dig3.get_spaces()[target];
					bool b = false;
					sp->set_form_pointset(&b, &ps);

					// make sure to note form has been changed
					m_targetSaved = (target!=1);

					if( showstatus )
					{
						QString num; num.setNum( n );
						QString msg;
						msg = num;
						msg += " SemiLandmarks in ";
						msg += semiLmkId.c_str();
						msg += " patch have been projected.";
						Logger::getInstance()->log( msg, Logger::INFO );
					}
					return true;
				}
				/*else if( form->pointsets[j].state == ew::Form3::STATE_PROJECTED )
				{
					if( checksurface )
						QMessageBox::critical( this, "Error", "Landmark already projected." );

					QString msg; msg = semiLmkId.c_str(); msg += " has already been projected.";
					Logger::getInstance()->log( msg, Logger::WARNING );

					return false;
				}*/
			}
		}
	return true;
}

int TableauLayout::embeddedCurveIndex(const std::string& id, const ew::Form3* form)
{
	for(unsigned int j=0; j<form->curves.size(); ++j)
		if(form->curves[j].id == id)
			return j;
	return -1;
}
int TableauLayout::embeddedSurfaceIndex(const std::string& id, const ew::Form3* form)
{
	for(unsigned int j=0; j<form->surfaces.size(); ++j)
		if(form->surfaces[j].id == id)
			return j;
	return -1;
}

bool TableauLayout::projectSemiLmk(const QString& topId, bool checksurface, bool showstatus, int semiLmkIndex, int target)
{
    if( checksurface )
    {
        if( m_dig3.get_spaces()[target]->get_form_data()->surfaces.size() == 0 && m_dig3.get_spaces()[target]->get_form_data()->curves.size() == 0 )
        {
            QMessageBox::information( this, "Error", "There is no surface or curve to project the landmarks onto. Please import a surface or a curve." );
            return false;
        }
    }

    std::string semiLmkId = topId.toStdString();

    const ew::Form3* form = m_dig3.get_spaces()[target]->get_form_data();

    std::string embeddedItemId = "";

    const char* found_embedding = m_dig3.get_spaces()[target]->get_form_data()->search_superset(semiLmkId.c_str());

    if(found_embedding == 0)
    {
    	// if no embedded item found, use first
    	if(m_dig3.get_spaces()[target]->get_form_data()->surfaces.size() > 0)
    		embeddedItemId = m_dig3.get_spaces()[target]->get_form_data()->surfaces[0].id;
    	else if(m_dig3.get_spaces()[target]->get_form_data()->curves.size() > 0)
    		embeddedItemId = m_dig3.get_spaces()[target]->get_form_data()->curves[0].id;

    	ew::Form3Embedding fe;
    	fe.subset_id = semiLmkId;
    	fe.superset_id = embeddedItemId;
    	bool b = false;
    	ew::Dig3Space *sp = m_dig3.get_spaces()[target];
    	sp->set_form_embedding( &b, &fe );

    	Logger::getInstance()->log("No embedded object found for: " + QString(semiLmkId.c_str()) + ". Projecting onto first object.",Logger::WARNING);
    }
    else
    {
        embeddedItemId = std::string(found_embedding);
    }

    if( form != 0 )
    {
		int surface_index = embeddedSurfaceIndex(embeddedItemId, form);
		int curve_index = embeddedCurveIndex(embeddedItemId, form);
		if((surface_index==-1) && (curve_index==-1))
			return false;
		else
			return projectToEmbedding(form, semiLmkId, surface_index, curve_index, semiLmkIndex, checksurface, false, target);
    }

    return false;
}


void TableauLayout::lmkSlide( FormItem* form, ViewTreeItem* item, int index, bool slide)
{
    if( m_dig3.get_spaces()[1]->get_form_data()->surfaces.size() == 0 && m_dig3.get_spaces()[1]->get_form_data()->curves.size() == 0 )
    {
        QMessageBox::information( this, "Error", "There is no surface or curve to slide the landmarks on. Please import a surface or a curve." );
        return;
    }

    std::string semiLmkId = dynamic_cast< SemiLandmarksTopItem* >( item )->getLmkID().toStdString();
    const ew::Form3* frm = m_dig3.get_spaces()[1]->get_form_data();
    const char* found_embedding = m_dig3.get_spaces()[1]->get_form_data()->search_superset(semiLmkId.c_str());
	std::string embeddedItemId = std::string(found_embedding);

    if(embeddedItemId == "" || found_embedding == 0)
    {
    	// if no embedded item found, use first
    	if(m_dig3.get_spaces()[1]->get_form_data()->surfaces.size() > 0)
    		embeddedItemId = m_dig3.get_spaces()[1]->get_form_data()->surfaces[0].id;
    	else if(m_dig3.get_spaces()[1]->get_form_data()->curves.size() > 0)
    		embeddedItemId = m_dig3.get_spaces()[1]->get_form_data()->curves[0].id;

    	ew::Form3Embedding fe;
    	fe.subset_id = semiLmkId;
    	fe.superset_id = embeddedItemId;
    	bool b = false;
    	ew::Dig3Space *sp = m_dig3.get_spaces()[1];
    	sp->set_form_embedding( &b, &fe );
    }

    if( item->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
    {
        SlideDialog* slideDialog = new SlideDialog(this, false);
        if(slideDialog->exec())
        {
            for(unsigned int j=0; j<frm->pointsets.size(); ++j)
                    if( frm->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK &&  
                        frm->pointsets[j].id == semiLmkId)
                    {
                        if(frm->pointsets[j].state == ew::Form3::STATE_PROJECTED)
                            slideAll(slideDialog->getIterations(),slideDialog->getEpsilon(), j);
                        else
                            QMessageBox::information( this, "Error", "Landmarks should be projected before slid." );                            
                        break;
                    }
            
        }
        delete slideDialog;
    }
}

bool TableauLayout::isInTargetTreeView( const QString& id )
{
    int size = m_targetTopLandmarks->childCount();
    for( int i = 0; i < size; ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_targetTopLandmarks->child( i ) );
        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
        {
            if( dynamic_cast< LandmarkItem* >( vti )->getLmkID() == id )
            {
                return true;
            }
        }
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
        {
            if( dynamic_cast< SemiLandmarksTopItem* >( vti )->getLmkID() == id )
            {
                return true;
            }
        }
    }
    return false;
}

bool TableauLayout::checkSpline()
{
	const ew::DataflowSpline3* ds = m_dig3.get_spline_node();

	if( ds->get_n_lmks() < 4 || ds->get_energy() == -1)
	{
	   QMessageBox::information(this, "Error", "Compute spline before warping Template landmarks to Specimen.");
	   return false;
	}

	if( ds->get_nonsingular() == false )
	{
	   QMessageBox::information(this, "Error", "Spline is singular. Template landmarks can not be warped.");
	   return false;
	}
	return true;
}

bool TableauLayout::warpLandmarkItem(LandmarkItem* lmi)
{
	std::string tt = lmi->getLmkID().toStdString();

	double x = 0, y = 0, z = 0;
	int index;
	if(!getPointsetLocation( tt, 0, x, y, z, index ))
		return false;

	double pin[3];
	double pout[3];
	pin[0] = x;
	pin[1] = y;
	pin[2] = z;

	const ew::DataflowSpline3* ds = m_dig3.get_spline_node();
	ds->warp_points( pout, pin, 1 );

	m_targetTopLandmarks->assignLandmarksHere( 0 );
	// is it already available in target
	if( isInTargetTreeView( lmi->getLmkID() ) )
	{
		// update the existing point location
		std::string str = lmi->getLmkID().toStdString();
		const ew::Form3 * form = m_dig3.get_spaces()[1]->get_form_data();
		for( unsigned int i = 0; i < form->pointsets.size(); ++i )
		{
			if( form->pointsets[i].id == str )
			{
				ew::Form3PointSet ps = form->pointsets[i];
				ps.state = ew::Form3::STATE_WARPED;
				bool r = false;
				ps.locations[0] = pout[0];
				ps.locations[1] = pout[1];
				ps.locations[2] = pout[2];
				m_dig3.get_spaces()[1]->set_form_pointset(&r, &ps);
				break;
			}
		}
	}
	else
	{
		m_targetView->placePoint( pout[0], pout[1], pout[2], false, false, lmi->getLmkID(), ew::Form3::STATE_WARPED );
	}

	projectLmk(lmi, lmi->getLmkIndex(), false, false);
	// add the landmark states
	updateTreeViewStates( 1 );
	return true;
}

bool TableauLayout::warpSemilandmarkItem(SemiLandmarksTopItem* lmi)
{
	std::string tt = lmi->getLmkID().toStdString();
	double x, y, z;
	int index2 = 0;
	if(!getPointsetLocation( tt, 0, x, y, z, index2 ))
		return false;
	const ew::Form3 * form = m_dig3.get_spaces()[0]->get_form_data();
	int numSemi = form->pointsets[ index2 ].n;
	double* pin = new double[ 3 * numSemi ];
	double* pout = new double[ 3 * numSemi ];
	int count = 0;
	for( int c = 0; c < numSemi * 3; c += 3 )
	{
		pin[count] = form->pointsets[ index2 ].locations[ c ];
		pin[count+1] = form->pointsets[ index2 ].locations[ c + 1 ];
		pin[count+2] = form->pointsets[ index2 ].locations[ c + 2 ];
		pout[count] = 0.0;
		pout[count+1] = 0.0;
		pout[count+2] = 0.0;
		count+=3;
	}

	const ew::DataflowSpline3* ds = m_dig3.get_spline_node();
	ds->warp_points( pout, pin, numSemi );

	count = 0;
	// is it already available in target
	if( isInTargetTreeView( lmi->getLmkID() ) )
	{
		// update the existing point location
		std::string str = lmi->getLmkID().toStdString();
		const ew::Form3 * form = m_dig3.get_spaces()[1]->get_form_data();
		for( unsigned int i = 0; i < form->pointsets.size(); ++i )
		{
			if( form->pointsets[i].id == str )
			{
				ew::Form3PointSet ps = form->pointsets[i];
				ps.state = ew::Form3::STATE_WARPED;
				bool r = false;
				m_dig3.get_spaces()[1]->set_form_pointset(&r, &ps);
				for( int c = 0; c < ps.n; ++c )
				{
					double ptemp[3];
					ptemp[0] = pout[count];
					ptemp[1] = pout[count+1];
					ptemp[2] = pout[count+2];
					m_dig3.get_spaces()[1]->set_form_pointset_location( i, c * 3, ptemp );

					projectSemiLmk(lmi->getLmkID(), false, false, c);

					count+=3;
				}

				if( ps.n < numSemi )
				{
					// activate the semilandmark patch
					for( int c = 0; c < m_targetTopLandmarks->childCount(); ++c )
					{
						ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_targetTopLandmarks->child( c ) );
						if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
						{
							if( dynamic_cast< LandmarkItem* >( vti )->getLmkID() == lmi->getLmkID() )
							{
								m_targetTopLandmarks->assignLandmarksHere( vti );
								break;
							}
						}
						else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
						{
							if( dynamic_cast< SemiLandmarksTopItem* >( vti )->getLmkID() == lmi->getLmkID() )
							{
								m_targetTopLandmarks->assignLandmarksHere( vti );
								break;
							}
						}
					}
					for( int i = ps.n; i < numSemi; ++i )
					{
						m_targetView->placePoint( pout[count], pout[count+1], pout[count+2], false, false, QString(ps.id.c_str())+i, ew::Form3::STATE_WARPED );
						count+=3;
					}
					m_targetTopLandmarks->assignLandmarksHere( 0 );
				}
			} // end if check id
		} // for loop
	}// end if existing semilandmark patch
	else
	{
		m_targetTopLandmarks->addSemiLandmark( tt.c_str() );
		//m_targetTopLandmarks->assignLandmarksHere( (ViewTreeItem*)m_targetTopLandmarks->child( m_targetTopLandmarks->childCount() - 1 ) );
		m_targetTopLandmarks->assignLandmarksHere( dynamic_cast< ViewTreeItem* >( m_targetTopLandmarks->child( m_targetTopLandmarks->childCount() - 1 ) ) );

		std::string embeddedItemId = lmi->getEmbeddedItemID().toStdString();
		if(embeddedItemId != "")
		{
			ew::Form3Embedding fe;
			fe.subset_id = tt;
			fe.superset_id = embeddedItemId;
			bool b = false;
			m_dig3.get_spaces()[1]->set_form_embedding( &b, &fe );

			//((SemiLandmarksTopItem*)m_targetTopLandmarks->child( m_targetTopLandmarks->childCount() - 1))->setEmbeddedItemID(embeddedItemId.c_str());
			dynamic_cast< SemiLandmarksTopItem* >( m_targetTopLandmarks->child( m_targetTopLandmarks->childCount() - 1) )->setEmbeddedItemID(embeddedItemId.c_str());
		}

		for( int c = 0; c < numSemi; ++c )
		{
			m_targetView->placePoint( pout[count], pout[count+1], pout[count+2], false, false, QString(tt.c_str())+c, ew::Form3::STATE_WARPED );
			count += 3;
		}
		m_targetTopLandmarks->assignLandmarksHere( 0 );
	}

	delete [] pout; pout = 0;
	delete [] pin; pin = 0;

	// add the landmark states
	updateTreeViewStates( 1 );

	return true;
}

void TableauLayout::mapLmk(FormItem* item, int index)
{
	if(!checkSpline())
		return;
    try
    {
        // get the template landmark
        int size = m_templateTopLandmarks->childCount();
        for( int i = 0; i < size; ++i )
        {
            ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_templateTopLandmarks->child( i ) );
            if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
            {
                LandmarkItem* lmi = dynamic_cast< LandmarkItem* >( m_templateTopLandmarks->child( i ) );
                if( lmi->getLmkIndex() == index )
                {
                	if(warpLandmarkItem(lmi))
                		break;
                }
            }
            else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
            {
                SemiLandmarksTopItem* lmi = dynamic_cast< SemiLandmarksTopItem* >( m_templateTopLandmarks->child( i ) );
                if( lmi->getLmkIndex() == index )
                {
                	if(warpSemilandmarkItem(lmi))
						break;
                }
            }
        } // end for loop
    }
    catch( ... )
    {
    }
}

void TableauLayout::slideAll(int iterations, double eps, int pointsetIndex)
{
	ew::Dig3Space* sp = m_dig3.get_spaces()[1];
	const ew::Form3* frm = sp->get_form_data();
	if(!checkSpline() || !frm)
		return;
	const ew::DataflowSpline3* ds = m_dig3.get_spline_node();
	int nIterations = 0;

	emit status("Sliding semi-landmarks, Please wait...");
	QApplication::setOverrideCursor(Qt::WaitCursor);

    map<int,int> slidLMKs;
    int n = (pointsetIndex<0)?frm->pointsets.size():pointsetIndex+1; //slide all or just one pointset
    pointsetIndex = (pointsetIndex<0)?0:pointsetIndex;

	while(true)
	{
		double error = 0;
		for(int j=pointsetIndex; j<n; ++j)
		{
			ew::Form3PointSet ps = frm->pointsets[j];
			if( ps.type != ew::Form3::TYPE_SEMI_LANDMARK || ps.state != ew::Form3::STATE_PROJECTED)
				continue;
			string embeddedItemId = frm->search_superset(ps.id.c_str());
			if(embeddedItemId.empty())
				continue;
			int relaxDims = embeddedCurveIndex(embeddedItemId, frm)>=0 ? 1:2;
			for( size_t i = 0; i < ps.relax_dims.size(); ++i )
				ps.relax_dims[i] = relaxDims;
			while( (int)ps.relax_dims.size() < ps.n )
				ps.relax_dims.push_back( relaxDims );
			bool b = false;
			sp->set_form_pointset(&b, &ps);
			slidLMKs[j] = frm->pointsets[j].n;
		}

		try
		{
			const double* points = ds->get_optimized_lmk_images();
			if( points )
			{
				for(map<int,int>::iterator it = slidLMKs.begin(); it!=slidLMKs.end(); ++it)
				{
					ew::Form3PointSet ps = frm->pointsets[it->first];
					for(unsigned int i=0; i<it->second; ++i )
					{
						int index = ds->lmk_index( 1, it->first, i );
						error += abs(ps.locations[i*3]-points[index*3]);
						error += abs(ps.locations[i*3+1]-points[index*3+1]);
						error += abs(ps.locations[i*3+2]-points[index*3+2]);

						ps.locations[i * 3    ] = points[index * 3    ];
						ps.locations[i * 3 + 1] = points[index * 3 + 1];
                        ps.locations[i * 3 + 2] = points[index * 3 + 2];
					}

					error/=it->second;
					ps.state = ew::Form3::STATE_OPTIMIZED;
					bool b = false;
					sp->set_form_pointset(&b, &ps);
					projectSemiLmk(ps.id.c_str(), false, false);
				}
			}
		}
		catch( std::exception& ex )
		{QMessageBox::information( this, "Error", ex.what() );}
        nIterations++;
		if(iterations>0)
		{
			if(nIterations==iterations)
				break;
		}
		else if (error-eps<=0)
			break;
        else if (nIterations>=100)
        {
            QString errStr; errStr = errStr.setNum(error);
            QMessageBox::information( this, "Could not reach specified error", 
                                    QString("Reached sliding error : ")+errStr );
            break;
        }
	}

	updateTreeViewStates( 1 );
	QApplication::restoreOverrideCursor();
	emit status("Semi-landmarks sliding complete.");
}

void TableauLayout::mapAllLmk(FormItem* item, int index)
{
	if(!checkSpline())
		return;
    try
    {
    	SlideDialog* slideDialog = new SlideDialog(this);
    	if(slideDialog->exec())
    	{
            QApplication::setOverrideCursor(Qt::WaitCursor);
			// get the template landmark
			int size = m_templateTopLandmarks->childCount();
			for( int i = 0; i < size; ++i )
			{
				ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_templateTopLandmarks->child( i ) );
				if( vti->getType() != ViewTreeItem::LANDMARK_ITEM && vti->getType() != ViewTreeItem::SEMILANDMARKS_ITEM )
					continue;

				if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
					warpLandmarkItem(dynamic_cast< LandmarkItem* >( vti ));
				else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
					warpSemilandmarkItem(dynamic_cast< SemiLandmarksTopItem* >( vti ));
			}
			emit status( "Warped all landmarks and semi-landmarks" );

			if(slideDialog->performSliding())
				slideAll(slideDialog->getIterations(),slideDialog->getEpsilon());
            QApplication::restoreOverrideCursor();
    	}
		delete slideDialog;
    }
    catch( ... )
    {
        QApplication::restoreOverrideCursor();
    }
}

void TableauLayout::adjustLandmarkState()
{
    // add the landmark states
    updateTreeViewStates( 1 );
}

void TableauLayout::matchLmk(FormItem* item, int index)
{
    int size = m_templateTopLandmarks->childCount();
    QStringList sl;
    for( int i = 0; i < size; ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_templateTopLandmarks->child( i ) );
        if( vti->getType() != ViewTreeItem::LANDMARK_ITEM )
            continue;
        sl.push_back( dynamic_cast< LandmarkItem* >( m_templateTopLandmarks->child( i ) )->getLmkID() );
    }

    if( size == 0 )
        return;

    if( sl.size() )
    {
            // get the template landmark
            int size = m_targetTopLandmarks->childCount();
            for( int i = 0; i < size; ++i )
            {
                ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_targetTopLandmarks->child( i ) );
                if( vti->getType() != ViewTreeItem::LANDMARK_ITEM )
                    continue;
                LandmarkItem* lmi = dynamic_cast< LandmarkItem* >( m_targetTopLandmarks->child( i ) );
                if( lmi->getLmkIndex() == index )
                {
                    std::string tt = lmi->getLmkID().toStdString();

                    if( !sl.contains( lmi->getLmkID() ) )
                    {
                        QMessageBox::information( this, "Error", "Specimen Landmark name does not match any Template Landmark." );
                        return;
                    }

                    double x1 = 0, y1 = 0, z1 = 0;
                    int targetindex = 0, templateindex = 0;
                    bool r1 = getPointsetLocation( tt, 1, x1, y1, z1, targetindex );
                    double x2 = 0, y2 = 0, z2 = 0;
                    bool r2 = getPointsetLocation( tt, 0, x2, y2, z2, templateindex );
                    if( r1 && r2 )
                    {
                        const ew::DataflowSpline3* ds = m_dig3.get_spline_node();
                        int result1 = ds->lmk_index( 1, targetindex, 0 );
                        int result0 = ds->lmk_index( 0, templateindex, 0 );
                        if( result1 != -1 && result0 != -1 )
                        {
                        // mapped successfully
                        QString msg;
                        msg = "Template: ";
                        msg += lmi->getLmkID();
                        msg += " and Specimen: ";
                        msg += lmi->getLmkID();
                        msg += " Landmarks have been matched.";
                        emit status( msg );

                        m_targetTopLandmarks->enableRegChild( index, false );
                        }
                        else
                        {
                        // not mapped successfully
                        QString msg;
                        msg = "Template: ";
                        msg += lmi->getLmkID();
                        msg += " and Specimen: ";
                        msg += lmi->getLmkID();
                        msg += " Landmarks has not been matched.";
                        emit status( msg );
                        }
                    }
                    break;
                }
            } // end for loop
    }
    else
    {
        QMessageBox::information(this, "Error", "Add some Specimen landmarks before registered.");
    }
}

void TableauLayout::matchAllLmk()
{
    std::vector<double> template_x;
    std::vector<double> template_y;
    std::vector<double> template_z;
    std::vector<double> target_x;
    std::vector<double> target_y;
    std::vector<double> target_z;

    int size = m_templateTopLandmarks->childCount();
    QStringList sl;
    for( int i = 0; i < size; ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_templateTopLandmarks->child( i ) );
        if( vti->getType() != ViewTreeItem::LANDMARK_ITEM )
            continue;
        sl.push_back( dynamic_cast< LandmarkItem* >( m_templateTopLandmarks->child( i ) )->getLmkID() );
    }

    if( size == 0 )
        return;

    int counter = 0;
    for( int i = 0; i < m_targetTopLandmarks->childCount(); ++i )
    {
        LandmarkItem* lti = dynamic_cast< LandmarkItem* >( m_targetTopLandmarks->child( i ) );
        if( lti->getType() != ViewTreeItem::LANDMARK_ITEM )
            continue;
        QString lbl = lti->getLmkID();

        if( sl.contains( lbl ) )
        {
            double x1, y1, z1;
            double x2, y2, z2;
            int targetindex = 0, templateindex = 0;
            bool r1 = getPointsetLocation( lbl.toStdString(), 0, x1, y1, z1, templateindex );
            bool r2 = getPointsetLocation( lbl.toStdString(), 1, x2, y2, z2, targetindex );


            if( r1 && r2 )
            {
                // now make sure the target landmark is FIXED for it to be matched
                if( m_dig3.get_spaces()[1]->get_form_data()->pointsets[targetindex].state != ew::Form3::STATE_SET )
                    continue;

                const ew::DataflowSpline3* ds = m_dig3.get_spline_node();
                int result0 = ds->lmk_index( 0, templateindex, 0 );
                int result1 = ds->lmk_index( 1, targetindex, 0 );

                if( result1 >= 0 && result0 >= 0 )
                {
                    template_x.push_back(x1);
                    template_y.push_back(y1);
                    template_z.push_back(z1);
                    target_x.push_back(x2);
                    target_y.push_back(y2);
                    target_z.push_back(z2);

                    ++counter;
                }
                else
                {
                }
            }
        } // end if contains
    }

    if (counter > 0)
    {
        m_splineTemplateLmks->ReSize(counter,3);
        m_splineTargetLmks->ReSize(counter,3);

        for (int i=0;i<counter;i++)
        {
            (*m_splineTemplateLmks)[i][0] = template_x[i];
            (*m_splineTemplateLmks)[i][1] = template_y[i];
            (*m_splineTemplateLmks)[i][2] = template_z[i];

            (*m_splineTargetLmks)[i][0] = target_x[i];
            (*m_splineTargetLmks)[i][1] = target_y[i];
            (*m_splineTargetLmks)[i][2] = target_z[i];
        }
    }

    if( counter >= 4 )
    {
        QString num; num = num.setNum( counter );
        emit status( num + " landmarks have been matched." );
    }
    else if( counter == 0 )
    {
        emit status( "No landmarks have been matched." );
    }
}

bool TableauLayout::getPointsetLocation( const std::string& id, int space, double &x, double& y, double& z, int& index )
{
    bool result = false;

    const ew::Form3* form = m_dig3.get_spaces()[space]->get_form_data();
    if( form != 0 )
        for(unsigned int j=0; j<form->pointsets.size() && !result; ++j)
            if(form->pointsets[j].type == ew::Form3::TYPE_LANDMARK || form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
            {
                if( form->pointsets[j].id == id )
                {
                    x = form->pointsets[j].locations[0];
                    y = form->pointsets[j].locations[1];
                    z = form->pointsets[j].locations[2];
                    index = j;
                    result = true;
                }
            }

    return result;
}

void TableauLayout::matchSpline()
{
    matchAllLmk();
}

void TableauLayout::matchSplineWarp()
{
    matchAllLmk();
    mapAllLmk( 0, -1 );
}

void TableauLayout::pointType( int type)
{
    m_templateView->pointType( type );
    m_templateSliceView->pointType( type );
    m_targetView->pointType( type );
    m_targetSliceView->pointType( type );
}

void TableauLayout::changeBckgndColor()
{
    const unsigned char* rgb = m_targetView->get_background_color();
    const unsigned char* col = chooseColor(rgb);
    if(col && m_targetView)
    {
        m_targetView->set_background_color(col);
        m_targetSliceView->set_background_color(col);
        m_templateView->set_background_color(col);
        m_templateSliceView->set_background_color(col);

        delete[] col;

        tableauUpdated();
    }
}

void TableauLayout::loadViewTreeItems(const FormItem* formTreeItem, const ew::Form3& form)
{
    m_treesFull = false;
    for(int i=0; i<formTreeItem->childCount(); ++i)
    {
        ViewTreeItem* treeItem = dynamic_cast< ViewTreeItem* >( formTreeItem->child(i) );

        switch(treeItem->getType())
        {
            case ViewTreeItem::SURFACES_ITEM:
            {
                SurfaceTopItem* surTopItem = dynamic_cast< SurfaceTopItem* >( treeItem );
                for(unsigned int j=0; j<form.surfaces.size(); ++j)
                {
                    surTopItem->addSurface(form.surfaces[j].id.c_str(), form.surfaces[j].file.c_str(), true);
                }
            }
            break;
            case ViewTreeItem::CURVES_ITEM:
            {
                CurvesTopItem* curTopItem = dynamic_cast< CurvesTopItem* >( treeItem );
                for(unsigned int j=0; j<form.curves.size(); ++j)
                {
                    curTopItem->addCurve(form.curves[j].id.c_str(), form.curves[j].file.c_str(), true);
                }
            }
            break;
            case ViewTreeItem::LANDMARKS_ITEM:
            {
                LandmarksTopItem* lmkTopItem = dynamic_cast< LandmarksTopItem* >( treeItem );

                for(unsigned int j=0; j<form.pointsets.size(); ++j)
                {
                    if( form.pointsets[j].type == ew::Form3::TYPE_LANDMARK )
                    {
                        lmkTopItem->assignLandmarksHere( 0 );
                        lmkTopItem->addLandmarkItem(QString(form.pointsets[j].id.c_str()));
                    }
                    else if( form.pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK )
                    {
                        lmkTopItem->addSemiLandmark( form.pointsets[j].id.c_str() );
                        lmkTopItem->assignLandmarksHere( dynamic_cast< ViewTreeItem* >( lmkTopItem->child( lmkTopItem->childCount() - 1 ) ) );

                        unsigned int count = form.pointsets[j].locations.size();

                        for( unsigned int k = 0; k < count/3; ++k )
                        {
                            dynamic_cast< SemiLandmarksTopItem* >( lmkTopItem->child( lmkTopItem->childCount() - 1 ) )->addSemiLandmarkItem();
                        }
                    }
                } // end for loop
            }
            break;
            default: break;
        }
    }

    m_treesFull = true;
}

void TableauLayout::targetFormUpdated()
{
    formUpdated( m_targetFormItem );
}

void TableauLayout::formUpdated(FormItem* formTreeItem)
{
    if(formTreeItem==m_targetFormItem && m_targetSaved)
    {
        m_targetSaved = false;
        m_trgtGrpBox->setTitle(m_trgtGrpBox->title()+"*");
        formTreeItem->setText(0, formTreeItem->text(0)+"*");
    }
    else if(formTreeItem==m_templateFormItem && m_templateSaved)
    {
        m_templateSaved = false;
        m_srcGrpBox->setTitle(m_srcGrpBox->title()+"*");
        formTreeItem->setText(0, formTreeItem->text(0)+"*");
    }
}

void TableauLayout::filmstripFrameNumberChanged( float number )
{
    QString str;
    str.setNum( number );
    m_frameCountLBL->setText( str );

    // update the form tree view with template and target forms
    int num = (int)number;
    m_templateFile = m_tableauList[num-1].space[0].form_filename.c_str();
    m_targetFile = m_tableauList[num-1].space[1].form_filename.c_str();
    loadForm( m_templateFormItem, true, false, false );
    loadForm( m_targetFormItem, true, false, false );

    // update the landmark views checkbox
    bool v = m_tableauList[num-1].space[1].show_lmks_in_main;
    bool s = m_tableauList[num-1].space[1].show_lmks_in_slice;
    m_targetView->get_landmarks_item()->set_state(v);
    m_targetSliceView->get_landmarks_item()->set_state(s);
    m_targetTopLandmarks->setCheckState( 1, v ? Qt::Checked : Qt::Unchecked );
    m_targetTopLandmarks->setCheckState( 2, s ? Qt::Checked : Qt::Unchecked );
    // do same for template
    v = m_tableauList[num-1].space[0].show_lmks_in_main;
    s = m_tableauList[num-1].space[0].show_lmks_in_slice;
    m_templateView->get_landmarks_item()->set_state(v);
    m_templateSliceView->get_landmarks_item()->set_state(s);
    m_templateTopLandmarks->setCheckState( 1, v ? Qt::Checked : Qt::Unchecked );
    m_templateTopLandmarks->setCheckState( 2, s ? Qt::Checked : Qt::Unchecked );

    // update the surface view checkbox
    v = s = true;
    if( m_tableauList[num-1].space[1].surface_settings.size() > 0 )
    {
        v = m_tableauList[num-1].space[1].surface_settings[0].show_in_main;
        s = m_tableauList[num-1].space[1].surface_settings[0].show_in_slice;
    }
    m_targetTopSurface->setCheckState( 1, v ? Qt::Checked : Qt::Unchecked );
    m_targetTopSurface->setCheckState( 2, s ? Qt::Checked : Qt::Unchecked );
    // do the same for target
    v = s = true;
    if( m_tableauList[num-1].space[0].surface_settings.size() > 0 )
    {
        v = m_tableauList[num-1].space[0].surface_settings[0].show_in_main;
        s = m_tableauList[num-1].space[0].surface_settings[0].show_in_slice;
    }
    m_templateTopSurface->setCheckState( 1, v ? Qt::Checked : Qt::Unchecked );
    m_templateTopSurface->setCheckState( 2, s ? Qt::Checked : Qt::Unchecked );
}

void TableauLayout::resetForm(FormItem* formTreeItem)
{
    int spaceIndex = formTreeItem==m_targetFormItem ? 1:0;

    m_dig3.get_spaces()[spaceIndex]->reset_form();

    m_treesFull = false;
    for(int i=0; i<formTreeItem->childCount(); ++i)
    {
        foreach(QTreeWidgetItem* item, formTreeItem->child(i)->takeChildren())
            delete item;
    }

    // reset the form text
    formTreeItem->setText( 0, "Form" );

    // reset the name in the specimen group or template group
    if( spaceIndex == 0 )
    {
        m_srcGrpBox->setTitle( "Template" );
        m_templateFile = "";
    }
    else
    {
        m_trgtGrpBox->setTitle( "Specimen" );
        m_targetFile = "";
    }

    formUpdated(formTreeItem);
    m_treesFull = true;
}

void TableauLayout::showFilmstrip( bool tog )
{
    m_filmShow->setText( tog ? "-" : "+" );
}

void TableauLayout::loadForm(FormItem* formTreeItem, bool loaded, bool focus, bool resetLmkViewState )
{
//    QSettings settings("Evan", "Templand");
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName;
    if( loaded )
    {
        fileName = ( formTreeItem == m_targetFormItem ? m_targetFile : m_templateFile );
    }
    else
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open a Form File"), fsLastDir, tr("Form files (*.frm)"));

        if (!fileName.isEmpty())
        {
            if(fileName.contains("/"))
            {
                QString dirName = fileName.left(fileName.lastIndexOf("/"));
                settings.setValue("lastDirectory",dirName);
            }
            else if(fileName.contains("\\"))
            {
                QString dirName = fileName.left(fileName.lastIndexOf("\\"));
                settings.setValue("lastDirectory",dirName);
            }
        }

/*
        QFileDialog fileDialog(this, tr("Open a Form File"), fsLastDir, tr("Form files (*.frm)"));
        if( fileDialog.exec() )
        {
            fileName = fileDialog.selectedFiles()[0];
            QDir dir = fileDialog.directory();
            settings.setValue("lastDirectory",dir.absolutePath());
        }
*/
    }

    if(fileName == "" || fileName.isEmpty())
        return;

    QFile file(fileName);
    if(!file.exists())
    {
#if TEMPLAND_TOOLKIT_BUILD
        Logger::getInstance()->log(QString("[Templand Node] Failed to load form! File '%1' does not exist!").arg(fileName), Logger::RUN_ERROR);
#else
        QMessageBox::information( this, "Error", QString("[Templand Node] Failed to load form! File '%1' does not exist!").arg(fileName) );
#endif
        return;
    }
    int spaceIndex = formTreeItem==m_targetFormItem ? 1:0;

#ifdef _WIN32
        // for windows change the path slashes from forward to backward
        QString str = fileName.replace( '/', '\\' );
#else
        QString str = fileName.replace( '\\', '/' );
#endif

    ew::Form3 form;
    try
    {
        resetForm(formTreeItem);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        emit status(QString("Loading Form, Please wait..."));
        form.read_file(fileName.toStdString().c_str());
        m_dig3.get_spaces()[spaceIndex]->set_form_data(&form);

        loadViewTreeItems(formTreeItem, form);

        // set embedded objects
        for(int i=0; i<formTreeItem->childCount();i++)
        {
            ViewTreeItem* treeItem = dynamic_cast< ViewTreeItem* >( formTreeItem->child(i) );
            if(treeItem->getType() == ViewTreeItem::LANDMARKS_ITEM)
            {
                for(int j=0; j<treeItem->childCount();j++)
                {
                    ViewTreeItem* lmkItem = dynamic_cast< ViewTreeItem* >( treeItem->child(j) );
                    if(lmkItem->getType() == ViewTreeItem::SEMILANDMARKS_ITEM)
                    {
                        std::vector<ew::Form3Embedding> embeddings = form.embeddings;
                        for (unsigned int k = 0u; k<embeddings.size(); k++)
                        {
                            if(embeddings[k].subset_id == dynamic_cast< SemiLandmarksTopItem* >( lmkItem )->getLmkID().toStdString())
                            {
                                //((SemiLandmarksTopItem*)lmkItem)->setEmbeddedItemID(embeddings[k].superset_id.c_str());
								( dynamic_cast< SemiLandmarksTopItem* >( lmkItem ) )->setEmbeddedItemID(embeddings[k].superset_id.c_str());
                                break;
                            }
                        }
                    }
                }
            }
        }

        // focus view for the first surface only
        if (m_dig3.get_spaces()[spaceIndex]->get_form_data()->surfaces.size() > 0) //Focus on first surface
        {
            bool found = false;
            for(int i=0; i<formTreeItem->childCount() || !found; ++i)
            {
                ViewTreeItem* treeItem = dynamic_cast< ViewTreeItem* >( formTreeItem->child(i) );
                switch(treeItem->getType())
                {
                    case ViewTreeItem::SURFACES_ITEM:
                    {
                        SurfaceTopItem* surTopItem = dynamic_cast< SurfaceTopItem* >( treeItem );

                        if( surTopItem->childCount() <= 0 )
                            break;

                        SurfaceItem* sur = dynamic_cast< SurfaceItem* >( surTopItem->child( 0 ) );

                        sur->setSurfaceIndex(0);
                        if( focus )
                            focusView(formTreeItem, sur);

                        found = true;
                    }
                    break;
                    default: break;
                }
            }
        }
        else if (m_dig3.get_spaces()[spaceIndex]->get_form_data()->curves.size() > 0) //Focus on first curve
        {
            bool found = false;
            for(int i=0; i<formTreeItem->childCount() || !found; ++i)
            {
                ViewTreeItem* treeItem = dynamic_cast< ViewTreeItem* >( formTreeItem->child(i) );
                switch(treeItem->getType())
                {
                    case ViewTreeItem::CURVES_ITEM:
                    {
                        CurvesTopItem* curTopItem = dynamic_cast< CurvesTopItem* >( treeItem );

                        if( curTopItem->childCount() <= 0 )
                            break;

                        CurveItem* cur = dynamic_cast< CurveItem* >( curTopItem->child( 0 ) );
                        cur->setCurveIndex(0);

                        if( focus )
                            focusView(formTreeItem, cur);

                        found = true;
                    }
                    break;
                    default: break;
                }
            }
        }

        View3Qt* surfaceWidget = formTreeItem==m_targetFormItem? m_targetView : m_templateView;
        View3Qt* sliceWidget = formTreeItem==m_targetFormItem? m_targetSliceView: m_templateSliceView;
        if(surfaceWidget && sliceWidget)   // set curve colour to red
        {
            int nc = m_dig3.get_spaces()[spaceIndex]->get_form_data()->curves.size();
            if (nc>0)
            {
                unsigned char col[3] = {255, 0, 0};
                for(int i=0;i<nc;i++)
                {
                    surfaceWidget->get_curve_items()[i]->set_color(col);
                    sliceWidget->get_curve_items()[i]->set_color(col);
                }
            }
        }

        emit status(QString("Form '%1' Loaded").arg(fileName), 2000);
        QString formID = fileName;
        if(spaceIndex)
        {
            m_targetFile = fileName;
            m_targetSaved = true;
            m_trgtGrpBox->setTitle(QString("Specimen - (%1)").arg(m_targetFile));
        }
        else
        {
            m_templateFile = fileName;
            m_templateSaved = true;
            m_srcGrpBox->setTitle(QString("Template - (%1)").arg(m_templateFile));
        }
        formTreeItem->setText(0, QString("Form (%1)").arg(formID));

        // add the landmark states
        updateTreeViewStates( spaceIndex );

        // refresh the views
        if( resetLmkViewState )
        {
            if( formTreeItem == m_targetFormItem )
            {
                m_targetView->get_landmarks_item()->set_state(false);
                m_targetSliceView->get_landmarks_item()->set_state(false);
                m_targetView->get_landmarks_item()->set_state(true);
                m_targetSliceView->get_landmarks_item()->set_state(true);

                m_targetTopSurface->setCheckState( 1, Qt::Checked );
                m_targetTopSurface->setCheckState( 2, Qt::Checked );

                m_targetTopLandmarks->setCheckState( 1, Qt::Checked );
                m_targetTopLandmarks->setCheckState( 2, Qt::Checked );

                int n = m_dig3.get_spaces()[spaceIndex]->get_form_data()->curves.size();

                if (n>0)
                {
                    unsigned char col[3] = {255, 0, 0};
                    for(int i=0;i<n;i++)
                    {
                        m_targetView->get_curve_items()[i]->set_color(col);
                        m_targetSliceView->get_curve_items()[i]->set_color(col);
                    }
                }
            }
            else
            {
                m_templateView->get_landmarks_item()->set_state(false);
                m_templateSliceView->get_landmarks_item()->set_state(false);
                m_templateView->get_landmarks_item()->set_state(true);
                m_templateSliceView->get_landmarks_item()->set_state(true);

                m_templateTopLandmarks->setCheckState( 1, Qt::Checked );
                m_templateTopLandmarks->setCheckState( 2, Qt::Checked );

                m_templateTopSurface->setCheckState( 1, Qt::Checked );
                m_templateTopSurface->setCheckState( 2, Qt::Checked );

                int n = m_dig3.get_spaces()[spaceIndex]->get_form_data()->curves.size();

                if (n>0)
                {
                    unsigned char col[3] = {255, 0, 0};
                    for(int i=0;i<n;i++)
                    {
                        m_templateView->get_curve_items()[i]->set_color(col);
                        m_templateSliceView->get_curve_items()[i]->set_color(col);
                    }
                }
            }
        }
    }
    catch(ew::ErrorIO &ex)
    {
        QMessageBox::information(this, "IO Exception", QString("Failed to load form!\n- %1)").arg(ex.what()));
    }
    catch(std::exception &ex)
    {
        QMessageBox::information(this, "IO Exception", QString("Failed to load form!\n- %1)").arg(ex.what()));
    }
    QApplication::restoreOverrideCursor();
}

void TableauLayout::saveFormAs(FormItem* formTreeItem)
{
    QString oldFileName = formTreeItem==m_targetFormItem ? m_targetFile : m_templateFile;
    if(formTreeItem == m_targetFormItem) m_targetFile = "";
    else               m_templateFile = "";

    if(!saveForm(formTreeItem))
    {
        if(formTreeItem == m_targetFormItem) m_targetFile = oldFileName;
        else               m_templateFile = oldFileName;
    }
}

bool TableauLayout::saveForm(FormItem* formTreeItem)
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    int spaceIndex = formTreeItem==m_targetFormItem ? 1:0;

    QString fileName = spaceIndex? m_targetFile : m_templateFile;

    if(fileName == "" || fileName.isEmpty())
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Form"),
                                                            fsLastDir,
                                                             "Form files (*.frm)");
        if(fileName == "" || fileName.isEmpty())
            return false;

        if (!fileName.isEmpty())
        {
            if(fileName.contains("/"))
            {
                QString dirName = fileName.left(fileName.lastIndexOf("/"));
                settings.setValue("lastDirectory",dirName);
            }
            else if(fileName.contains("\\"))
            {
                QString dirName = fileName.left(fileName.lastIndexOf("\\"));
                settings.setValue("lastDirectory",dirName);
            }
        }
    }

    try
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        emit status(QString("Saving Form, Please wait..."));

#ifdef _WIN32
        // for windows change the path slashes from forward to backward
        QString str = fileName.replace( '/', '\\' );
#else
        QString str = fileName.replace( '\\', '/' );
#endif

        m_dig3.get_spaces()[spaceIndex]->get_form_data()->write_file( str.toStdString().c_str(), 0 );
        emit status(QString("Form %1 Saved").arg(fileName), 2000);
        if(spaceIndex)
        {
            m_targetFile = fileName;
            m_targetSaved = true;
            m_trgtGrpBox->setTitle(QString("Specimen - (%1)").arg(m_targetFile));
        }
        else
        {
            m_templateFile = fileName;
            m_templateSaved = true;
            m_srcGrpBox->setTitle(QString("Template - (%1)").arg(m_templateFile));
        }
        QString formID = fileName;
        formTreeItem->setText(0, QString("Form (%1)").arg(formID));
    }
    catch(ew::ErrorIO &ex)
    {
        QMessageBox::information(this, "IO Exception", QString("Failed to save form!\n- %1)").arg(ex.what()));
        QApplication::restoreOverrideCursor();
        return false;
    }
    catch(std::exception &ex)
    {
        QMessageBox::information(this, "IO Exception", QString("Failed to save form!\n- %1)").arg(ex.what()));
        QApplication::restoreOverrideCursor();
        return false;
    }

    QApplication::restoreOverrideCursor();

    emit appendForm( true );

    return true;
}

void TableauLayout::newTableau()
{
    if( m_newTableau )
        return;

    m_newTableau = true;

    QDir d;

    QString s = d.absolutePath();

    s += "/Untitled.ltb";

    QObject::connect(&m_filmstrip, SIGNAL(frameIndexChange(int)), m_frameSlider, SLOT(setValue(int)));
    QObject::connect(&m_filmstrip, SIGNAL(frameIndexChange(int)), m_frameSBox, SLOT(setValue(int)));
    QObject::connect(&m_filmstrip, SIGNAL(frameNumberChange(float)), this, SLOT(filmstripFrameNumberChanged(float)));
    QObject::connect(m_frameSlider, SIGNAL(sliderMoved(int)), &m_filmstrip, SLOT(moveToFrame(int)));
    QObject::connect(m_frameSBox, SIGNAL(valueChanged(int)), &m_filmstrip, SLOT(moveToFrame(int)));
    QObject::connect(&m_filmstrip, SIGNAL(statusChanged(int)), this, SLOT(filmstripStatus(int)));

    emit appendForm( true );
}

bool TableauLayout::loadTableau(QString loadedFile, bool loaded)
{
    bool result = true;
    QString fileName = loadedFile;

    QFile file(fileName);
    if(!file.exists())
    {
#if TEMPLAND_TOOLKIT_BUILD
        Logger::getInstance()->log(QString("[Templand Node] Failed to load tableau! File '%1' does not exist!").arg(fileName), Logger::RUN_ERROR);
#else
        QMessageBox::information( this, "Error", QString("[Templand Node] Failed to load tableau! File '%1' does not exist!").arg(fileName) );
#endif
        result = false;
        return result;
    }

    try
    {
#ifdef _WIN32
        // for windows change the path slashes from forward to backward
        fileName = fileName.replace( '/', '\\' );
#else
        fileName = fileName.replace( '\\', '/' );
#endif
        std::vector<ew::Dig3Tableau> tempTableaus;
        ew::Dig3Tableau::read_file(&tempTableaus, fileName.toStdString().c_str());
        m_tableauList.clear();
        m_tableauList = tempTableaus;

        TableauDialog* loadDialog = new TableauDialog(this, fileName, tempTableaus.size());
        if(loaded ? true : loadDialog->exec())
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            emit status(QString("Loading Tableau, Please wait..."));

            if( tempTableaus.size() > 0 )
            {
                if(loadDialog->loadTemplate())
                {
                    m_templateFile = tempTableaus[0].space[0].form_filename.c_str();
                    loadForm(m_templateFormItem, true);

                    // check if the template has surface or landmarks. If so then display the template views
                    const ew::Form3* form = m_dig3.get_spaces()[0]->get_form_data();
                    if( form->surfaces.size() || form->pointsets.size() || form->curves.size())
                    {
                        m_srcGrpBox->setVisible(true);
                        m_templateItemTree->setVisible(true);
                    }
                }
                if(loadDialog->loadTarget())
                {
                    m_targetFile = tempTableaus[0].space[1].form_filename.c_str();
                    loadForm(m_targetFormItem, true);
                }
                if(loadDialog->loadSettings())
                {
                    m_dig3.load_tableau(&tempTableaus[0], 0, ew::Dig3::TABLEAU_SETTINGS );
                    m_dig3.load_tableau(&tempTableaus[0], 1, ew::Dig3::TABLEAU_SETTINGS );
                    m_dig3.load_tableau(&tempTableaus[0], 2, ew::Dig3::TABLEAU_SETTINGS );
                    m_dig3.load_tableau(&tempTableaus[0], 3, ew::Dig3::TABLEAU_SETTINGS );
                }
                if(loadDialog->loadView())
                {
                    m_dig3.load_tableau(&tempTableaus[0], 0, ew::Dig3::TABLEAU_VIEW );
                    m_dig3.load_tableau(&tempTableaus[0], 1, ew::Dig3::TABLEAU_VIEW );
                    m_dig3.load_tableau(&tempTableaus[0], 2, ew::Dig3::TABLEAU_VIEW );
                    m_dig3.load_tableau(&tempTableaus[0], 3, ew::Dig3::TABLEAU_VIEW );
                }
            }

            m_tableauFile = fileName;
            if(tempTableaus.size())
            {
                m_frameSlider->setMinimum(1);
                m_frameSlider->setMaximum(tempTableaus.size() > 0 ? tempTableaus.size() : 1);
                m_frameSBox->setMinimum(1);
                m_frameSBox->setMaximum(tempTableaus.size() > 0 ? tempTableaus.size() : 1);

                filmstripFrameNumberChanged( 1.0f );
            }
            setWindowTitle(QString("Templand - ") + m_tableauFile);
            m_tableauSaved = true;
            emit status(QString("File %1 Loaded").arg(m_tableauFile), 2000);

            if( tempTableaus.size() > 0 )
            {
                m_filmstrip.AssignContent( &m_dig3, tempTableaus );
            }

            QObject::connect(&m_filmstrip, SIGNAL(frameIndexChange(int)), m_frameSlider, SLOT(setValue(int)));
            QObject::connect(&m_filmstrip, SIGNAL(frameIndexChange(int)), m_frameSBox, SLOT(setValue(int)));
            QObject::connect(&m_filmstrip, SIGNAL(frameNumberChange(float)), this, SLOT(filmstripFrameNumberChanged(float)));
            QObject::connect(m_frameSlider, SIGNAL(sliderMoved(int)), &m_filmstrip, SLOT(moveToFrame(int)));
            QObject::connect(m_frameSBox, SIGNAL(valueChanged(int)), &m_filmstrip, SLOT(moveToFrame(int)));
            QObject::connect(&m_filmstrip, SIGNAL(statusChanged(int)), this, SLOT(filmstripStatus(int)));

            emit appendForm( true );
            emit replaceForm( tempTableaus.size() );
            emit deleteForm( tempTableaus.size() );
            emit swapPrevForm( false );
            emit swapNextForm( tempTableaus.size() > 1 );
            m_tblStart->setDisabled( tempTableaus.size() < 2 );
            m_tblEnd->setDisabled( tempTableaus.size() < 2 );
            m_tblLeftE->setDisabled( tempTableaus.size() < 2 );
            m_tblLeft->setDisabled( tempTableaus.size() < 2 );
            m_tblRight->setDisabled( tempTableaus.size() < 2 );
            m_tblListRightE->setDisabled( tempTableaus.size() < 2 );

            m_consensusPB->setDisabled( tempTableaus.size() < 2 );

            m_filmFirstFrame->setDisabled( tempTableaus.size() < 2 );
            m_filmPlayBackward->setDisabled( tempTableaus.size() < 2 );
            m_filmPlay->setDisabled( tempTableaus.size() < 2 );
            m_filmPause->setDisabled( tempTableaus.size() < 2 );
            m_filmStop->setDisabled( tempTableaus.size() < 2 );
            m_filmLastFrame->setDisabled( tempTableaus.size() < 2 );

            m_filmStepPrev->setDisabled( tempTableaus.size() < 2 );
            m_filmStepNext->setDisabled( tempTableaus.size() < 2 );
            m_secFrameTXT->setDisabled( tempTableaus.size() < 2 );
            m_stepsFrameTXT->setDisabled( tempTableaus.size() < 2 );

            m_savedAtleastOnce = true;
        }
        else
            result = false;
        delete loadDialog;
    }
    catch(ew::ErrorIO &ex)
    {
        QMessageBox::information(this, "IO Exception", QString("Failed to load tableau!\n- %1)").arg(ex.what()));
        result = false;
    }
    catch(std::exception &ex)
    {
        QMessageBox::information(this, "IO Exception", QString("Failed to load tableau!\n- %1)").arg(ex.what()));
        result = false;
    }
    QApplication::restoreOverrideCursor();
    return result;
}

void TableauLayout::saveTableauAs()
{
    QString oldFile = m_tableauFile;
    m_tableauFile = "";
    if(!saveTableau())
        m_tableauFile = oldFile;
}

bool TableauLayout::saveTableau()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    if( !m_savedAtleastOnce )
        m_tableauFile = "";

    if(m_tableauFile == "" || m_tableauFile.isEmpty())
    {
        m_tableauFile = QFileDialog::getSaveFileName(this, tr("Save Tableau"),
                                                            fsLastDir,
                                                             "Landmarking tableau files (*.ltb)");
        if(m_tableauFile == "" || m_tableauFile.isEmpty())
            return false;

        if(m_tableauFile.contains("/"))
        {
            QString dirName = m_tableauFile.left(m_tableauFile.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(m_tableauFile.contains("\\"))
        {
            QString dirName = m_tableauFile.left(m_tableauFile.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }

    try
    {
        emit status(QString("Saving Tableau, Please wait..."));

        QString fileID = m_tableauFile.left(m_tableauFile.lastIndexOf('.'));
        if(m_templateFile == "" || m_templateFile.isEmpty())
            m_templateFile = QString(fileID+"_0.frm");
        if(!saveForm(m_templateFormItem))
            return false;
        if(m_targetFile == "" || m_targetFile.isEmpty())
            m_targetFile = QString(fileID+"_1.frm");
        if(!saveForm(m_targetFormItem))
            return false;

        QString templatePath = m_templateFile;

        QString targetPath = m_targetFile;

        QApplication::setOverrideCursor(Qt::WaitCursor);

#ifdef _WIN32
        // for windows change the path slashes from forward to backward
        m_tableauFile = m_tableauFile.replace( '/', '\\' );
#else
        m_tableauFile = m_tableauFile.replace( '\\', '/' );
#endif

        m_savedAtleastOnce = true;
        if(m_tableauList.size()>0)
        {
            int v = m_frameSBox->value() - 1;
            if(v>=0)
            {
                m_tableauList[v].space[0].form_filename = templatePath.toStdString().c_str();
                m_tableauList[v].space[1].form_filename = targetPath.toStdString().c_str();
            }
        }

        ew::Dig3Tableau::write_file(m_tableauFile.toStdString().c_str(), false, &m_tableauList);


        setWindowTitle(QString("Templand - ") + m_tableauFile);
        m_tableauSaved = true;
        emit status(QString("File %1 Saved").arg(m_tableauFile), 2000);
    }
    catch(ew::ErrorIO &ex)
    {
        QMessageBox::information(this, "IO Exception", QString("Failed to save tableau!\n- %1)").arg(ex.what()));
        QApplication::restoreOverrideCursor();
        return false;
    }
    catch(std::exception &ex)
    {
        QMessageBox::information(this, "IO Exception", QString("Failed to save tableau!\n- %1)").arg(ex.what()));
        QApplication::restoreOverrideCursor();
        return false;
    }

    QApplication::restoreOverrideCursor();
    return true;
}

void TableauLayout::pauseFilmstip()
{
    m_filmstrip.pause();

    m_filmPlayBackward->setDisabled( false );
    m_filmPlay->setDisabled( false );
}

void TableauLayout::stopFilmstip()
{
    m_filmstrip.stop();

    emit swapPrevForm( false );
    emit swapNextForm( true );

    m_filmPlayBackward->setDisabled( false );
    m_filmPlay->setDisabled( false );
}

void TableauLayout::forwardFilmstip()
{
    m_filmstrip.playForward();
    m_filmstrip.play();

    m_filmPlayBackward->setDisabled( true );
    m_filmPlay->setDisabled( true );
    m_filmPause->setDisabled( false );
}

void TableauLayout::backwardFilmstip()
{
    m_filmstrip.playBackward();
    m_filmstrip.play();

    m_filmPlayBackward->setDisabled( true );
    m_filmPlay->setDisabled( true );
    m_filmPause->setDisabled( false );
}

void TableauLayout::firstFrameFilmstrip()
{
    m_filmstrip.prevFrame();

    m_filmPlayBackward->setDisabled( false );
    m_filmPlay->setDisabled( false );
    m_filmPause->setDisabled( true );
}

void TableauLayout::lastFrameFilmstrip()
{
    m_filmstrip.nextFrame();

    m_filmPlayBackward->setDisabled( false );
    m_filmPlay->setDisabled( false );
    m_filmPause->setDisabled( true );
}

void TableauLayout::filmstripStatus( int status )
{
    if( status == FilmstripPlayerControls::PLAYING )
    {
        emit appendForm( false );
        emit replaceForm( false );
        emit deleteForm( false );

        emit swapPrevForm( false );
        emit swapNextForm( false );
    }
    else if( status == FilmstripPlayerControls::STOPPED )
    {
        emit appendForm( true );
        emit replaceForm( true );
        emit deleteForm( true );

        emit swapPrevForm( true );
        emit swapNextForm( true );
    }
    else if( status == FilmstripPlayerControls::PAUSED )
    {
        emit appendForm( true );
        emit replaceForm( true );
        emit deleteForm( true );

        emit swapPrevForm( true );
        emit swapNextForm( true );
    }
}

void TableauLayout::addTableau( int index )
{
    if( !m_savedAtleastOnce )
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Save?"),
                            tr("Tableau file needs to be saved before adding Tableau frames.\nDo you want to save your changes?"),
                            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            m_tableauFile = "";
            if( saveTableau() )
            {
                QObject::connect(&m_filmstrip, SIGNAL(frameIndexChange(int)), m_frameSlider, SLOT(setValue(int)));
                QObject::connect(&m_filmstrip, SIGNAL(frameIndexChange(int)), m_frameSBox, SLOT(setValue(int)));
                QObject::connect(&m_filmstrip, SIGNAL(frameNumberChange(float)), this, SLOT(filmstripFrameNumberChanged(float)));
                QObject::connect(m_frameSlider, SIGNAL(sliderMoved(int)), &m_filmstrip, SLOT(moveToFrame(int)));
                QObject::connect(m_frameSBox, SIGNAL(valueChanged(int)), &m_filmstrip, SLOT(moveToFrame(int)));
                QObject::connect(&m_filmstrip, SIGNAL(statusChanged(int)), this, SLOT(filmstripStatus(int)));
            }
            else
            {
                return;
            }
        }
        else if (ret == QMessageBox::Cancel || ret == QMessageBox::Discard )
        {
            return;
        }
    }

    ew::Dig3Tableau tabl;
    m_dig3.save_tableau(&tabl, 1, 2, 0, 3);

    int n = 0;
    if( index == -1 )
    {
        m_tableauList.push_back( tabl );
        n = m_tableauList.size() - 1;
    }
    else
    {
         m_tableauList.insert( m_tableauList.begin() + index, tabl );
         n = index;
    }
    int size = m_tableauList.size();

    QString templatePath = m_templateFile;

    QString targetPath = m_targetFile;

    m_tableauList[n].space[0].form_filename = templatePath.toStdString().c_str();
    m_tableauList[n].space[1].form_filename = targetPath.toStdString().c_str();

    m_filmstrip.AssignContent( &m_dig3, m_tableauList  );

    if(size > 1)
    {
        m_frameSlider->setMinimum(1);
        m_frameSlider->setMaximum(size);
        m_frameSBox->setMinimum(1);
        m_frameSBox->setMaximum(size);
    }
    else
    {
        m_frameSlider->setMinimum(0);
        m_frameSlider->setMaximum(0);
        m_frameSBox->setMinimum(1);
        m_frameSBox->setMaximum(1);
    }

    m_frameSlider->setValue( n + 1 );
    m_frameSBox->setValue( n + 1 );
    filmstripFrameNumberChanged( n + 1 );

    // activate the filmstrip controls
    m_filmFirstFrame->setDisabled( m_tableauList.size() < 2 );
    m_filmPlayBackward->setDisabled( m_tableauList.size() < 2 );
    m_filmPlay->setDisabled( m_tableauList.size() < 2 );
    m_filmPause->setDisabled( m_tableauList.size() < 2 );
    m_filmStop->setDisabled( m_tableauList.size() < 2 );
    m_filmLastFrame->setDisabled( m_tableauList.size() < 2 );

    m_filmStepPrev->setDisabled( m_tableauList.size() < 2 );
    m_filmStepNext->setDisabled( m_tableauList.size() < 2 );
    m_secFrameTXT->setDisabled( m_tableauList.size() < 2 );
    m_stepsFrameTXT->setDisabled( m_tableauList.size() < 2 );

    emit replaceForm( true );
    emit deleteForm( true );

    emit swapPrevForm( false );
    emit swapNextForm( m_tableauList.size() > 1 );

    m_tblStart->setDisabled( m_tableauList.size() < 2 );
    m_tblEnd->setDisabled( m_tableauList.size() < 2 );
    m_tblLeftE->setDisabled( m_tableauList.size() < 2 );
    m_tblLeft->setDisabled( m_tableauList.size() < 2 );
    m_tblRight->setDisabled( m_tableauList.size() < 2 );
    m_tblListRightE->setDisabled( m_tableauList.size() < 2 );

    m_consensusPB->setDisabled( m_tableauList.size() < 2 );
}

void TableauLayout::replaceTableau()
{
    int v = m_frameSBox->value() - 1;

    m_dig3.save_tableau( &m_tableauList[v], 0, 1, 2, 3);

    QString templatePath = m_templateFile;

    QString targetPath = m_targetFile;

    m_tableauList[v].space[0].form_filename = templatePath.toStdString().c_str();
    m_tableauList[v].space[1].form_filename = targetPath.toStdString().c_str();

    m_filmstrip.AssignContent( &m_dig3, m_tableauList );

    // activate the filmstrip controls
    m_filmFirstFrame->setDisabled( ( m_tableauList.size() < 2 ) );
    m_filmPlayBackward->setDisabled( ( m_tableauList.size() < 2 ) );
    m_filmPlay->setDisabled( ( m_tableauList.size() < 2 ) );
    m_filmPause->setDisabled( ( m_tableauList.size() < 2 ) );
    m_filmStop->setDisabled( ( m_tableauList.size() < 2 ) );
    m_filmLastFrame->setDisabled( ( m_tableauList.size() < 2 ) );
}

void TableauLayout::deleteTableau()
{
    int v = m_frameSlider->value() - 1;

    m_tableauList.erase( m_tableauList.begin() + v );

    m_filmstrip.AssignContent( &m_dig3, m_tableauList);

    if(m_tableauList.size())
    {
        m_frameSlider->setMinimum(1);
        m_frameSlider->setMaximum(m_tableauList.size());
        m_frameSBox->setMinimum(1);
        m_frameSBox->setMaximum(m_tableauList.size());
    }

    m_frameSlider->setValue( 0 );
    m_frameSBox->setValue( 0 );

    if( m_tableauList.size() > 1 )
    {
        // activate the filmstrip controls
        m_filmFirstFrame->setDisabled( false );
        m_filmPlayBackward->setDisabled( false );
        m_filmPlay->setDisabled( false );
        m_filmPause->setDisabled( false );
        m_filmStop->setDisabled( false);
        m_filmLastFrame->setDisabled( false );

        m_tblStart->setDisabled( false );
        m_tblEnd->setDisabled( false );
        m_tblLeftE->setDisabled( false );
        m_tblLeft->setDisabled( false );
        m_tblRight->setDisabled( false );
        m_tblListRightE->setDisabled( false );
    }
    else
    {
        // activate the filmstrip controls
        m_filmFirstFrame->setDisabled( true );
        m_filmPlayBackward->setDisabled( true );
        m_filmPlay->setDisabled( true );
        m_filmPause->setDisabled( true );
        m_filmStop->setDisabled( true );
        m_filmLastFrame->setDisabled( true );

        emit deleteForm( false );

        m_tblStart->setDisabled( true );
        m_tblEnd->setDisabled( true );
        m_tblLeftE->setDisabled( true );
        m_tblLeft->setDisabled( true );
        m_tblRight->setDisabled( true );
        m_tblListRightE->setDisabled( true );
    }

    m_consensusPB->setDisabled( m_tableauList.size() < 2 );
}

void TableauLayout::moveTableauToPrev()
{
    // move right
    int v = m_frameSlider->value() - 1;

    if( v == 0 )
        return;

    ew::Dig3Tableau replacement = m_tableauList[v - 1];
    m_tableauList[v - 1] = m_tableauList[v];
    m_tableauList[v] = replacement;

    m_filmstrip.AssignContent( &m_dig3, m_tableauList );

    m_frameSlider->setValue( v );
    m_frameSBox->setValue( v );
    filmstripFrameNumberChanged( v );
}

void TableauLayout::moveTableauToNext()
{
    // move left
    int v = m_frameSlider->value() - 1;

    if( v == (int)m_tableauList.size() - 1 )
        return;

    ew::Dig3Tableau replacement = m_tableauList[v + 1];
    m_tableauList[v + 1] = m_tableauList[v];
    m_tableauList[v] = replacement;

    m_filmstrip.AssignContent( &m_dig3, m_tableauList );

    m_frameSlider->setValue( v + 2 );
    m_frameSBox->setValue( v + 2 );
    filmstripFrameNumberChanged( v + 2 );
}

void TableauLayout::changeSliceColor(ViewTree* itemTree)
{
    const ew::Dig3View* surfaceView = itemTree==m_targetItemTree ?  m_targetView : m_templateView;
    const ew::Dig3View* sliceView = itemTree==m_targetItemTree ?  m_targetSliceView : m_templateSliceView;
    int sliceIndex = itemTree==m_targetItemTree ? 3:2;
    if(!surfaceView->get_slice_items() || !sliceView->get_slice_items())
        return;

    const unsigned char* rgb = surfaceView->get_slice_items()[sliceIndex]->get_color();
    const unsigned char* col = chooseColor(rgb);
    if(col)
    {
        surfaceView->get_slice_items()[sliceIndex]->set_color(col);
        sliceView->get_slice_items()[sliceIndex]->set_color(col);
        delete [] col;

        tableauUpdated();
    }
}

void TableauLayout::changeSliceClipR(ViewTree* itemTree)
{
    QString itemData = m_tableauFile;

    if(m_frameSlider->value() >0 && m_frameSlider->value()<=(int)m_tableauList.size())
    {
        bool ok = false;
        double clipRatio = m_tableauList[m_frameSlider->value()-1].slice_clip_ratio;
        clipRatio = QInputDialog::getDouble(this, tr("Slice Clip Ratio"),
                                            tr("Enter Slice Clip Ratio: "), clipRatio, 0.001, 100, 3, &ok);
        if(ok)
            m_tableauList[m_frameSlider->value()-1].slice_clip_ratio = clipRatio;
    }
}
void TableauLayout::changeLmkSize(FormItem* itemForm, bool increase)
{
    View3Qt* surfaceWidget = itemForm==m_targetFormItem ?  m_targetView : m_templateView;
    View3Qt* sliceWidget = itemForm==m_targetFormItem ?  m_targetSliceView: m_templateSliceView;

    if(!surfaceWidget->get_landmarks_item() || !sliceWidget->get_landmarks_item())
        return;
    surfaceWidget->get_landmarks_item()->change_size(increase);
    sliceWidget->get_landmarks_item()->change_size(increase);

    tableauUpdated();
}

void TableauLayout::changeLmkColor(FormItem* itemForm)
{
    View3Qt* surfaceWidget = itemForm==m_targetFormItem ?  m_targetView : m_templateView;
    View3Qt* sliceWidget = itemForm==m_targetFormItem ?  m_targetSliceView: m_templateSliceView;

    if(!surfaceWidget->get_landmarks_item() || !sliceWidget->get_landmarks_item())
        return;

    const unsigned char* rgb = surfaceWidget->get_landmarks_item()->get_color();
    const unsigned char* col = chooseColor(rgb);
    if(col)
    {
        surfaceWidget->get_landmarks_item()->set_color(col);
        sliceWidget->get_landmarks_item()->set_color(col);

        //Asssign the higlight color to the inverse
        QColor result;
        result.setRgb(255-col[0], 255-col[1], 255-col[2]);
        int ir, ig, ib;
        result.getRgb(&ir, &ig, &ib);
        unsigned char hCol[3];
        hCol[0] = (unsigned char)ir;
        hCol[1] = (unsigned char)ig;
        hCol[2] = (unsigned char)ib;
        surfaceWidget->set_highlight_color(hCol);
        sliceWidget->set_highlight_color(hCol);

        delete[] col;

        tableauUpdated();
    }
}

bool TableauLayout::addSurface(SurfaceItem* treeItem, FormItem* itemForm)
{
    bool result = false;
    try
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        int space = itemForm==m_targetFormItem ? 1 : 0;
        QString formFile = itemForm==m_targetFormItem ? m_targetFile : m_templateFile;

        QString surfacePath = treeItem->getSurfaceFile();

#ifdef _WIN32
        // for windows change the path slashes from forward to backward
        surfacePath = surfacePath.replace( '/', '\\' );
#else
        surfacePath = surfacePath.replace( '\\', '/' );
#endif

        ew::Form3Surface fs;
        fs.id = treeItem->getSurfaceID().toStdString().c_str();
        fs.file = surfacePath.toStdString().c_str();
        bool replaced;
        m_dig3.get_spaces()[space]->set_form_surface(&replaced, &fs);

        if (m_dig3.get_spaces()[space]->get_form_data()->surfaces.size() == 1) //Focus on first surface
        {
            treeItem->setSurfaceIndex(0);
            focusView(itemForm, treeItem);
        }

        formUpdated(itemForm);
        QApplication::restoreOverrideCursor();

        emit appendForm( true );

        result = true;
    }
    catch(ew::ErrorIO &ex)
    {
        QMessageBox::critical(this, "IO Exception", QString("Failed to load surface!\n- %1)").arg(ex.what()));
        result = false;
    }
    QApplication::restoreOverrideCursor();

    return result;
}

void TableauLayout::changeSurfaceColor(SurfaceItem* surItem, FormItem* itemForm, bool front)
{
    const ew::Dig3View* surfaceView = itemForm==m_targetFormItem ?  m_targetView : m_templateView;
    const ew::Dig3View* sliceView = itemForm==m_targetFormItem ?  m_targetSliceView : m_templateSliceView;

    int surIndex = surItem->getSurfaceIndex();
    if(!surfaceView->get_surface_items() || !sliceView->get_surface_items())
        return;

    const unsigned char *rgb = front? surfaceView->get_surface_items()[surIndex]->get_front_color() :
                                      surfaceView->get_surface_items()[surIndex]->get_back_color() ;
    const unsigned char* col = chooseColor(rgb);
    if(col)
    {
        if(front)
        {
            surfaceView->get_surface_items()[surIndex]->set_front_color(col);
            sliceView->get_surface_items()[surIndex]->set_front_color(col);
        }
        else
        {
            surfaceView->get_surface_items()[surIndex]->set_back_color(col);
            sliceView->get_surface_items()[surIndex]->set_back_color(col);
        }
        delete[] col;

        tableauUpdated();
    }
}

void TableauLayout::changeCurveColor(CurveItem* curItem, FormItem* itemForm)
{
    const ew::Dig3View* curveView = itemForm==m_targetFormItem ?  m_targetView : m_templateView;
    const ew::Dig3View* sliceView = itemForm==m_targetFormItem ?  m_targetSliceView : m_templateSliceView;

    int curIndex = curItem->getCurveIndex();
    if(!curveView->get_curve_items() || !sliceView->get_curve_items())
        return;

    const unsigned char *rgb = curveView->get_curve_items()[curIndex]->get_color();
    const unsigned char* col = chooseColor(rgb);
    if(col)
    {
    	curveView->get_curve_items()[curIndex]->set_color(col);
        sliceView->get_curve_items()[curIndex]->set_color(col);
        delete[] col;
    }
    tableauUpdated();
}

bool TableauLayout::addCurve(CurveItem* treeItem, FormItem* itemForm)
{
    bool result = false;

    try
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        int space = itemForm==m_targetFormItem ? 1 : 0;
        QString formFile = itemForm==m_targetFormItem ? m_targetFile : m_templateFile;

        QString curvePath = treeItem->getCurveFile();

#ifdef _WIN32
        // for windows change the path slashes from forward to backward
        curvePath = curvePath.replace( '/', '\\' );
#else
        curvePath = curvePath.replace( '\\', '/' );
#endif

        ew::Form3Curve fs;
        fs.id = treeItem->getCurveID().toStdString().c_str();
        fs.file = curvePath.toStdString().c_str();
        bool replaced;
        m_dig3.get_spaces()[space]->set_form_curve(&replaced, &fs);

        int n = m_dig3.get_spaces()[space]->get_form_data()->curves.size();
        if (n>0)
        {
            const ew::Dig3View* curveView = itemForm==m_targetFormItem ?  m_targetView : m_templateView;
            const ew::Dig3View* sliceView = itemForm==m_targetFormItem ?  m_targetSliceView : m_templateSliceView;

            unsigned char col[3] = {255, 0, 0};
            curveView->get_curve_items()[n-1]->set_color(col);
            sliceView->get_curve_items()[n-1]->set_color(col);
        }

        if (n == 1) //Focus on first curve
        {
            treeItem->setCurveIndex(0);
            focusView(itemForm, treeItem);
        }

        formUpdated(itemForm);
        QApplication::restoreOverrideCursor();

        emit appendForm( true );

        result = true;
    }
    catch(ew::ErrorIO &ex)
    {
        QMessageBox::critical(this, "IO Exception", QString("Failed to load curve!\n- %1)").arg(ex.what()));
        result = false;
    }
    QApplication::restoreOverrideCursor();

    return result;
}

void TableauLayout::applyItemChange(ViewTree* itemTree, QTreeWidgetItem* treeItem, int column)
{
    if(!m_treesFull)
        return;

    bool on = treeItem->checkState(column) != Qt::Unchecked;
    const ew::Dig3View* surfaceView = itemTree==m_targetItemTree ?  m_targetView : m_templateView;
    const ew::Dig3View* sliceView = itemTree==m_targetItemTree ?  m_targetSliceView : m_templateSliceView;

    switch( dynamic_cast< ViewTreeItem* >( treeItem )->getType() )
    {
        case ViewTreeItem::SURFACES_ITEM:
            for(int i=0; i<treeItem->childCount(); ++i)
            {
                if( column == 1 )
                    surfaceView->get_surface_items()[i]->set_state(on);
                else
                    sliceView->get_surface_items()[i]->set_state(on);
                treeItem->child(i)->setCheckState(column, treeItem->checkState(column));
            }
        break;
        case ViewTreeItem::SURFACE_ITEM:
        {
            SurfaceItem* surItem = dynamic_cast< SurfaceItem* >( treeItem );
            int surIndex = surItem->getSurfaceIndex();
            if(surIndex != -1)
            {
                if( column == 1 )
                    surfaceView->get_surface_items()[surIndex]->set_state(on);
                else
                    sliceView->get_surface_items()[surIndex]->set_state(on);
            }
        }
        break;
        case ViewTreeItem::CURVES_ITEM:
            for(int i=0; i<treeItem->childCount(); ++i)
            {
                if( column == 1 )
                    surfaceView->get_curve_items()[i]->set_state(on);
                else
                    sliceView->get_curve_items()[i]->set_state(on);
                treeItem->child(i)->setCheckState(column, treeItem->checkState(column));
            }
        break;
        case ViewTreeItem::CURVE_ITEM:
        {
            CurveItem* curItem = dynamic_cast< CurveItem* >( treeItem );
            int curIndex = curItem->getCurveIndex();
            if(curIndex != -1)
            {
                if( column == 1 )
                    surfaceView->get_curve_items()[curIndex]->set_state(on);
                else
                    sliceView->get_curve_items()[curIndex]->set_state(on);
            }
        }
        break;
        case ViewTreeItem::SLICE_ITEM:
        {
            if( column == 1 )
            {
                if(surfaceView->get_slice_items())
                    surfaceView->get_slice_items()[itemTree==m_targetItemTree? 3:2]->set_state(on);
            }
            else
            {
                if( sliceView->get_slice_items() )
                    sliceView->get_slice_items()[itemTree==m_targetItemTree ? 3 : 2 ]->set_state( on );
            }
        }
        break;
        case ViewTreeItem::LANDMARKS_ITEM:
        {
            if( column == 1 )
                surfaceView->get_landmarks_item()->set_state(on);
            else
                sliceView->get_landmarks_item()->set_state(on);
        }
        break;
        case ViewTreeItem::SEMILANDMARKS_ITEM:
        {
            if( column == 1 )
                surfaceView->get_landmarks_item()->set_state(on);
            else
                sliceView->get_landmarks_item()->set_state(on);
        }
        break;
        default: break;
    }
}

void TableauLayout::deleteLmk(FormItem* itemForm, int i)
{
    std::string id;
    ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( (( itemForm==m_targetFormItem )?m_targetTopLandmarks:m_templateTopLandmarks)->child( i ) );
    if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
        id = dynamic_cast< LandmarkItem* >( vti )->getLmkID().toStdString();
    else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
        id = dynamic_cast< SemiLandmarksTopItem* >( vti )->getLmkID().toStdString();

    if( id.empty() )
        return;
    const ew::Form3* form = m_dig3.get_spaces()[ (itemForm==m_targetFormItem) ? 1 : 0 ]->get_form_data();
    int index = i;
    bool found = false;
    if( form != 0 )
        for(unsigned int j=0; j<form->pointsets.size() && !found; ++j)
            if(form->pointsets[j].type == ew::Form3::TYPE_LANDMARK || form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
            {
                if( form->pointsets[j].id == id )
                {
                    index = j;
                    found = true;
                }
            }

    if( !found )
        return;
    m_dig3.get_spaces()[itemForm==m_targetFormItem ? 1:0]->remove_form_pointset(index);
    formUpdated(itemForm);
}

void TableauLayout::deleteLmk(FormItem* patch, ViewTreeItem* semi, int index )
{
    std::string id = dynamic_cast< SemiLandmarkItem* >( semi )->getParent()->getLmkID().toStdString();
    int foundIndex = -1;
    const ew::Form3* form = m_dig3.get_spaces()[ 1 ]->get_form_data();
    if( form != 0 )
        for(unsigned int j=0; j<form->pointsets.size(); ++j)
            if( form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK && form->pointsets[j].id == id )
            {
                foundIndex = j;
                break;
            }
    if(foundIndex>=0)
    {
        ew::Form3PointSet ps = form->pointsets[foundIndex];
        // delete the x y z
        size_t psIndex = index*3;
        ps.locations.erase( ps.locations.begin() + psIndex,  ps.locations.begin() + psIndex + 3);
        // delet the relax dims
        if( (int)ps.relax_dims.size() > index )
            ps.relax_dims.erase( ps.relax_dims.begin() + index );

        // delete the relax params
        if( (int)ps.relax_params.size() > psIndex + 3 )
            ps.relax_params.erase( ps.relax_params.begin() + psIndex,  ps.relax_params.begin() + psIndex + 3);
        // decrease the pointset count
        --ps.n;
        bool b = false;
        m_dig3.get_spaces()[ 1 ]->set_form_pointset(&b, &ps);
    }
}

void TableauLayout::deleteAllLmk(FormItem* itemForm, int i)
{

    std::string id;
    if( itemForm==m_targetFormItem )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_targetTopLandmarks->child( i ) );
        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
            id = dynamic_cast< LandmarkItem* >( vti )->getLmkID().toStdString();
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
            id = dynamic_cast< SemiLandmarksTopItem* >( vti )->getLmkID().toStdString();
    }
    else if( itemForm==m_templateFormItem )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_templateTopLandmarks->child( i ) );
        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
            id = dynamic_cast< LandmarkItem* >( vti )->getLmkID().toStdString();
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
            id = dynamic_cast< SemiLandmarksTopItem* >( vti )->getLmkID().toStdString();
    }

    if( id.empty() )
        return;

    const ew::Form3* form = m_dig3.get_spaces()[ (itemForm==m_targetFormItem) ? 1 : 0 ]->get_form_data();
    int index = i;
    bool found = false;
    if( form != 0 )
        for(unsigned int j=0; j<form->pointsets.size() && !found; ++j)
            if(form->pointsets[j].type == ew::Form3::TYPE_LANDMARK || form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
            {
                if( form->pointsets[j].id == id )
                {
                    index = j;
                    found = true;
                }
            }

    if( !found )
        return;

    m_dig3.get_spaces()[itemForm==m_targetFormItem ? 1:0]->remove_form_pointset(index);
    formUpdated(itemForm);
}

void TableauLayout::moveLmk(FormItem* itemForm, int i)
{
	m_targetTopLandmarks->setMoving(true);
}

void TableauLayout::moveLmk(FormItem* patch, ViewTreeItem* semi, int index )
{
	m_targetTopLandmarks->setMoving(true);
}

void TableauLayout::stateLmk(FormItem* form, ViewTreeItem* item, int index, int newState )
{
    if( item->getType() == ViewTreeItem::LANDMARKS_ITEM )
    {
        // loop through all the Landmark children and change state to fixed
        for( int i = 0; i < item->childCount(); ++i )
        {
            ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( item->child(i) );
            if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
            {
                LandmarkItem* li = dynamic_cast< LandmarkItem* >( vti );
                QString id = li->getLmkID();

                changeState( id.toStdString(), newState );
            }
        }

        //update the tree
        updateTreeViewStates( 1 );
    }
    else if( item->getType() == ViewTreeItem::LANDMARK_ITEM )
    {
        LandmarkItem* li = dynamic_cast< LandmarkItem* >( item );
        QString id = li->getLmkID();

        changeState( id.toStdString(), newState );

        //update the tree
        updateTreeViewStates( 1 );
    }
    else if( item->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
    {
        SemiLandmarksTopItem* sti = dynamic_cast< SemiLandmarksTopItem* >( item );
        QString id = sti->getLmkID();

        changeState( id.toStdString(), newState );

        //update the tree
        updateTreeViewStates( 1 );
    }
}

void TableauLayout::changeState( const std::string& id, int state )
{
    for( unsigned int i = 0; i < m_dig3.get_spaces()[ 1 ]->get_form_data()->pointsets.size(); ++i )
    {
        if( m_dig3.get_spaces()[ 1 ]->get_form_data()->pointsets[i].id == id )
        {
            // take copy of the pointset
            ew::Form3PointSet ps = m_dig3.get_spaces()[ 1 ]->get_form_data()->pointsets[i];

            // change the state to new state
            ps.state = state;

            // insert pointset with new state
            bool b = false;
            m_dig3.get_spaces()[1]->set_form_pointset(&b, &ps);

            break;
        }
    }
}

void TableauLayout::landmarkMoved( const QString& id, int space )
{
    //update the tree
    updateTreeViewStates( space );
}

void TableauLayout::renameLmk(FormItem* form, ViewTreeItem* item, int index, const QString& newID)
{
    std::string oldID;
    // get the old name
    if( item->getType() == ViewTreeItem::LANDMARK_ITEM )
    {
        LandmarkItem* li = dynamic_cast< LandmarkItem* >( item );
        oldID = li->getLmkID().toStdString();
    }
    else if( item->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
    {
        SemiLandmarksTopItem* sti = dynamic_cast< SemiLandmarksTopItem* >( item );
        oldID = sti->getLmkID().toStdString();
    }

    // check existing landmark if new names exists
    bool exists = false;
    for( int i = 0; i < m_targetTopLandmarks->childCount() && !exists; ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_targetTopLandmarks->child( i ) );

        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
        {
            LandmarkItem* li = dynamic_cast< LandmarkItem* >( vti );
            exists = ( li->getLmkID() == newID );
        }
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
        {
            SemiLandmarksTopItem* sti = dynamic_cast< SemiLandmarksTopItem* >( vti );
            exists = ( sti->getLmkID() == newID );
        }
    }

    // display duplication error message and quit
    if( exists )
    {
        QString msg = newID;
        QMessageBox::critical( this, "Error", msg + " name already exists. Please choose another name." );
        return;
    }

    // now rename the pointsets if unique
    for( unsigned int i = 0; i < m_dig3.get_spaces()[ 1 ]->get_form_data()->pointsets.size(); ++i )
    {
        if( m_dig3.get_spaces()[ 1 ]->get_form_data()->pointsets[i].id == oldID )
        {
            // take copy of the pointset
            ew::Form3PointSet ps = m_dig3.get_spaces()[ 1 ]->get_form_data()->pointsets[i];

            ps.id = newID.toStdString();

            // delete the old pointset with the old name
            m_dig3.get_spaces()[1]->remove_form_pointset(i);

            // insert pointset with new name
            bool b = false;
            m_dig3.get_spaces()[1]->set_form_pointset(&b, &ps);

            // update the UI text and internal ID
            if( item->getType() == ViewTreeItem::LANDMARK_ITEM )
            {
                LandmarkItem* li = dynamic_cast< LandmarkItem* >( item );
                li->setLmkID( newID );
                QString state = stateLabel( ps.state );
                state += " ";
                state += newID;
                li->setText( 0, state );
            }
            else if( item->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
            {
                SemiLandmarksTopItem* sti = dynamic_cast< SemiLandmarksTopItem* >( item );
                sti->setLmkID( newID );
                QString state = stateLabel( ps.state );
                state += " ";
                state += newID;
                sti->setText( 0, state );
            }

            // update landmark index after pointset deletion and insertion
            for( int i = 0; i < m_targetTopLandmarks->childCount(); ++i )
            {
                ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( m_targetTopLandmarks->child( i ) );

                if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
                {
                    double x, y, z;
                    int index = -1;
                    LandmarkItem* li = dynamic_cast< LandmarkItem* >( vti );
                    if( getPointsetLocation( li->getLmkID().toStdString(), 1, x, y, z, index ) )
                        li->setLmkIndex( index );
                }
                else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
                {
                    double x, y, z;
                    int index = -1;
                    SemiLandmarksTopItem* sti = dynamic_cast< SemiLandmarksTopItem* >( vti );
                    if( getPointsetLocation( sti->getLmkID().toStdString(), 1, x, y, z, index ) )
                        sti->setLmkIndex( index );

                    //update embeddings
                    std::vector<ew::Form3Embedding> embeddings = m_dig3.get_spaces()[1]->get_form_data()->embeddings;
                    for (unsigned int k = 0; k<embeddings.size(); k++)
                        if(embeddings[k].subset_id == oldID)
                        {
                            bool replaced = false;
                            embeddings[k].subset_id = newID.toStdString();
                            m_dig3.get_spaces()[1]->remove_form_embedding(k);
                            m_dig3.get_spaces()[1]->set_form_embedding(&replaced, &embeddings[k]);
                            break;
                        }
                }
            }

            // finally informt he end user
            QString msg = oldID.c_str();
            msg += " Landmark has been renamed to: ";
            msg += newID;
            emit status( msg );
        }
    }
}

void TableauLayout::highlightItem(ViewTree* itemTree, QTreeWidgetItem* treeItem)
{
    if(!m_treesFull)
        return;

    const ew::Dig3View* surfaceView = itemTree==m_targetItemTree ?  m_targetView : m_templateView;
    const ew::Dig3View* sliceView = itemTree==m_targetItemTree ?  m_targetSliceView : m_templateSliceView;

    ((ew::Dig3View*)surfaceView)->clear_highlight();
    ((ew::Dig3View*)sliceView)->clear_highlight();

    LandmarksTopItem* other = itemTree==m_targetItemTree ? m_templateTopLandmarks : m_targetTopLandmarks;
    LandmarksTopItem* same = itemTree==m_targetItemTree ? m_targetTopLandmarks : m_templateTopLandmarks;

    switch( dynamic_cast< ViewTreeItem* >( treeItem )->getType() )
    {
        case ViewTreeItem::LANDMARK_ITEM:
        {
            LandmarkItem* lmkItem = dynamic_cast< LandmarkItem* >( treeItem );
            int index = lmkItem->getLmkIndex();

            std::string id = lmkItem->getLmkID().toStdString();
            const ew::Form3* form = m_dig3.get_spaces()[ (itemTree==m_targetItemTree) ? 1 : 0 ]->get_form_data();
            if( form != 0 )
                for(unsigned int j=0; j<form->pointsets.size(); ++j)
                    if(form->pointsets[j].type == ew::Form3::TYPE_LANDMARK)
                    {
                        if( form->pointsets[j].id == id )
                        {
                            index = j;
                            break;
                        }
                    }

            surfaceView->get_landmarks_item()->set_highlight(index, 0);
            sliceView->get_landmarks_item()->set_highlight(index, 0);

            // finally informt he end user
            QString msg = lmkItem->getLmkID();
            msg += " has been selected.";
            emit status( msg );

            if( other->childCount() && other->doesChildExist( lmkItem->getLmkID(), "" ) == true && m_selectPointsInViews == true )
            {
                // now highlight the landmark IF exists in other tree view
                other->select( lmkItem->getLmkID() );
            }
            else
            {
                // now highlight the landmark IF exists in other tree view
                same->select( lmkItem->getLmkID() );
            }

            // add new landmarks as top level landmarks
            if(itemTree==m_targetItemTree)
            {
                m_targetTopLandmarks->assignLandmarksHere( 0 );
            }
            else
            {
                m_templateTopLandmarks->assignLandmarksHere( 0 );
            }
        }
        break;
        case ViewTreeItem::SEMILANDMARKS_ITEM:
        {
            SemiLandmarksTopItem* lmkItem = dynamic_cast< SemiLandmarksTopItem* >( treeItem );
            int index = lmkItem->getLmkIndex();

            std::string id = lmkItem->getLmkID().toStdString();

            const ew::Form3* form = m_dig3.get_spaces()[ (itemTree==m_targetItemTree) ? 1 : 0 ]->get_form_data();
            if( form != 0 )
                for(unsigned int j=0; j<form->pointsets.size(); ++j)
                    if(form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
                    {
                        if( form->pointsets[j].id == id )
                        {
                            index = j;
                            break;
                        }
                    }

            surfaceView->get_landmarks_item()->set_highlight(index, 0);
            sliceView->get_landmarks_item()->set_highlight(index, 0);

            // finally informt he end user
            QString msg = lmkItem->getLmkID();
            msg += " has been selected.";
            emit status( msg );

            if( other->childCount() && other->doesChildExist( lmkItem->getLmkID(), "" ) == true && m_selectPointsInViews == true )
            {
                // now highlight the semilandmark patch IF exists in other tree view
                other->select( lmkItem->getLmkID() );
            }
            else
            {
                // now highlight the semilandmark patch IF exists in other tree view
                same->select( lmkItem->getLmkID() );
            }

            // add new landmarks to the selected semilandmark item
            lmkItem->addHere(true);
        }
        break;
        case ViewTreeItem::SEMILANDMARK_ITEM:
        {
            SemiLandmarkItem* lmkItem = dynamic_cast< SemiLandmarkItem* >( treeItem );
            QTreeWidgetItem* parent = dynamic_cast< QTreeWidgetItem* >( lmkItem )->parent();
            int index = dynamic_cast< SemiLandmarksTopItem* >( parent )->getLmkIndex();
            int within = lmkItem->getLmkIndex();

            //std::string id = ((SemiLandmarksTopItem*)parent)->getLmkID().toStdString();
			std::string id = dynamic_cast< SemiLandmarksTopItem* >( parent )->getLmkID().toStdString();

            const ew::Form3* form = m_dig3.get_spaces()[ (itemTree==m_targetItemTree) ? 1 : 0 ]->get_form_data();
            if( form != 0 )
                for(unsigned int j=0; j<form->pointsets.size(); ++j)
                    if(form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
                    {
                        if( form->pointsets[j].id == id )
                        {
                            index = j;
                            break;
                        }
                    }

            surfaceView->get_landmarks_item()->set_highlight(index, within * 3 );
            sliceView->get_landmarks_item()->set_highlight(index, within * 3 );

            // finally informt he end user
            QString msg = lmkItem->text( 0 );
            msg += " in ";
            msg += id.c_str();
            msg += " has been selected.";
            emit status( msg );

            if( other->childCount() && other->doesChildExist(id.c_str(), lmkItem->text(0)) == true && m_selectPointsInViews == true )
            {
                // now highlight the semilandmark patch IF exists in other tree view
                other->select( id.c_str(), lmkItem->text(0) );
            }
            else
            {
                // now highlight the semilandmark patch IF exists in other tree view
                same->select( id.c_str(), lmkItem->text(0) );
            }

            // add new landmarks to the selected semilandmark item
            dynamic_cast< SemiLandmarksTopItem* >( parent )->addHere(true);

        }
        break;
        default:
            // add new landmarks as top level landmarks
            if(itemTree==m_targetItemTree)
            {
                m_targetTopLandmarks->assignLandmarksHere( 0 );
            }
            else
            {
                m_templateTopLandmarks->assignLandmarksHere( 0 );
            }
        break;
    }
}

void TableauLayout::landmarkSelected( const QString& id, int index, int indexWithin, int space )
{
    LandmarksTopItem* lti = (space == 0) ? m_templateTopLandmarks : m_targetTopLandmarks;
    ViewTree* itemTree = (space == 0) ? m_templateItemTree : m_targetItemTree;

    for(int i = 0; i < lti->childCount(); ++i )
    {
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( lti->child(i) );
        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
        {
            if( dynamic_cast< LandmarkItem* >( vti )->getLmkID() == id )
            {
                highlightItem( itemTree, vti );
                break;
            }
        }
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
        {
            if( dynamic_cast< SemiLandmarksTopItem* >( vti )->getLmkID() == id )
            {
                highlightItem( itemTree, vti->child( indexWithin ) );

                break;
            }
        }
    }
}

bool TableauLayout::hasStateLabel( const QString& lbl, QString& lblWithoutState )
{
    if( lbl.contains( "[" ) && lbl.contains( "]" ) )
    {
        lblWithoutState = lbl.right( lbl.length() - 4 );
        return true;
    }
    return false;
}

QString TableauLayout::stateLabel( int state )
{
    if( state == ew::Form3::STATE_SET )
    {
        return "[F]";
    }
    else if( state == ew::Form3::STATE_PROJECTED )
    {
        return "[P]";
    }
    else if( state == ew::Form3::STATE_WARPED )
    {
        return "[W]";
    }
    else if( state == ew::Form3::STATE_OPTIMIZED )
    {
        return "[S]";
    }
    return "[-]";
}

void TableauLayout::updateTreeViewStates( int space )
{
    LandmarksTopItem* lti = ( space == 0 ) ? m_templateTopLandmarks : m_targetTopLandmarks;

    for( int i = 0; i < lti->childCount(); ++i )
    {
        QString lbl;
        ViewTreeItem* vti = dynamic_cast< ViewTreeItem* >( lti->child(i) );
        if( vti->getType() == ViewTreeItem::LANDMARK_ITEM )
            lbl = dynamic_cast< LandmarkItem* >( vti )->getLmkID();
        else if( vti->getType() == ViewTreeItem::SEMILANDMARKS_ITEM )
            lbl = dynamic_cast< SemiLandmarksTopItem* >( vti )->getLmkID();

        QString lblWithout;

        if( hasStateLabel( lbl, lblWithout ) == false )
            lblWithout = lbl;

        int index = -1;
        double x = 0.0, y = 0.0, z = 0.0;

        if( getPointsetLocation( lblWithout.toStdString(), space, x, y, z, index ) )
        {
            int state = m_dig3.get_spaces()[space]->get_form_data()->pointsets[index].state;

            QString finalLBL = stateLabel( state );
            finalLBL += " ";
            finalLBL += lblWithout;
            lti->child(i)->setText( 0, finalLBL );
        } // end if
    } // end for loop
}

void TableauLayout::changeLmksSymbol(FormItem* itemForm, ew::View3Landmarks::symbol_t sym)
{
    const ew::Dig3View* surfaceView = itemForm==m_targetFormItem ?  m_targetView : m_templateView;
    const ew::Dig3View* sliceView = itemForm==m_targetFormItem ?  m_targetSliceView : m_templateSliceView;

    surfaceView->get_landmarks_item()->set_symbol(sym);
    sliceView->get_landmarks_item()->set_symbol(sym);
    tableauUpdated();
}

void TableauLayout::moveToFrame(int frameIndex)
{
    if(frameIndex > 0 && frameIndex <= (int)m_tableauList.size())
    {
        m_dig3.load_tableau(&m_tableauList[frameIndex-1], 0, ew::Dig3::TABLEAU_VIEW );
        m_dig3.load_tableau(&m_tableauList[frameIndex-1], 1, ew::Dig3::TABLEAU_VIEW );
        m_dig3.load_tableau(&m_tableauList[frameIndex-1], 2, ew::Dig3::TABLEAU_VIEW );
        m_dig3.load_tableau(&m_tableauList[frameIndex-1], 3, ew::Dig3::TABLEAU_VIEW );
    }

    if( !m_filmstrip.IsPlaying() )
    {
        if( frameIndex == 1 )
        {
            emit swapPrevForm( false );
            emit swapNextForm( false );
        }
        else if( frameIndex == (int)m_tableauList.size() )
        {
            emit swapPrevForm( true );
            emit swapNextForm( false );
        }
        else
        {
            emit swapPrevForm( true );
            emit swapNextForm( true );
        }
    }

}

void TableauLayout::focusView(FormItem* itemForm, ViewTreeItem* treeItem)
{
    View3Qt* surfaceWidget = itemForm==m_targetFormItem ?  m_targetView : m_templateView;
    View3Qt* sliceWidget = itemForm==m_targetFormItem ?  m_targetSliceView : m_templateSliceView;
    ew::Transform3 tr;

    switch(treeItem->getType())
    {
        case ViewTreeItem::FORM_ITEM:
        {
            ew::Bbox3 b;
            m_dig3.get_spaces()[itemForm==m_targetFormItem ? 1 : 0]->get_bbox(&b);
            if (surfaceWidget->get_bbox_tr(&tr, &b, true, true))
                surfaceWidget->view_post_compose(&tr);
            if (sliceWidget->get_bbox_tr(&tr, &b, true, true))
                sliceWidget->view_post_compose(&tr);
        }
        break;
        case ViewTreeItem::SLICE_ITEM:
        {
            surfaceWidget = treeItem->getParentTree()==m_targetItemTree ?  m_targetView : m_templateView;
            sliceWidget = treeItem->getParentTree()==m_targetItemTree ?  m_targetSliceView : m_templateSliceView;
            const ew::Bbox3* b = surfaceWidget->get_slice_items()[treeItem->getParentTree()==m_targetItemTree ? 3:2]->get_bbox();
            if (surfaceWidget->get_bbox_tr(&tr, b, true, true))
                surfaceWidget->view_post_compose(&tr);
            if (sliceWidget->get_bbox_tr(&tr, b, true, true))
                sliceWidget->view_post_compose(&tr);
        }
        break;
        case ViewTreeItem::LANDMARKS_ITEM:
        {
            const ew::Bbox3* b = surfaceWidget->get_landmarks_item()->get_bbox();
            if (surfaceWidget->get_bbox_tr(&tr, b, true, true))
                surfaceWidget->view_post_compose(&tr);
            if (sliceWidget->get_bbox_tr(&tr, b, true, true))
                sliceWidget->view_post_compose(&tr);
        }
        break;
        case ViewTreeItem::SURFACE_ITEM:
        {
            SurfaceItem* surItem = dynamic_cast< SurfaceItem* >( treeItem );
            ew::Dig3Space *sp = m_dig3.get_spaces()[itemForm==m_targetFormItem ? 1 : 0];
            if (sp->get_form_data()->surfaces.size() && surItem->getSurfaceIndex()<(int)sp->get_form_data()->surfaces.size())
            {
                const ew::Bbox3 *b = surfaceWidget->get_surface_items()[surItem->getSurfaceIndex()]->get_bbox();
                if (surfaceWidget->get_bbox_tr(&tr, b, true, true))
                    surfaceWidget->view_post_compose(&tr);
                if (sliceWidget->get_bbox_tr(&tr, b, true, true))
                    sliceWidget->view_post_compose(&tr);
            }
        }
        break;
        case ViewTreeItem::CURVE_ITEM:
        {
            CurveItem* curItem = dynamic_cast< CurveItem* >( treeItem );
            ew::Dig3Space *sp = m_dig3.get_spaces()[itemForm==m_targetFormItem ? 1 : 0];
            if (sp->get_form_data()->curves.size() && curItem->getCurveIndex()<(int)sp->get_form_data()->curves.size())
            {
                const ew::Bbox3 *b = surfaceWidget->get_curve_items()[curItem->getCurveIndex()]->get_bbox();
                if (surfaceWidget->get_bbox_tr(&tr, b, true, true))
                    surfaceWidget->view_post_compose(&tr);
                if (sliceWidget->get_bbox_tr(&tr, b, true, true))
                    sliceWidget->view_post_compose(&tr);
            }
        }
        break;
        default: break;
    }

    tableauUpdated();
}

void TableauLayout::secPerFrame( const QString& text )
{
    if( text.isEmpty () )
        return;
    bool ok;
    float sec = text.toFloat( &ok );
    if( ok )
        m_filmstrip.secPerFrame( sec );
    else
        QMessageBox::critical( this, "Format Error", "Invalid input should be blank or positive Real number." );
}

void TableauLayout::stepPerFrame( const QString& text )
{
    if( text.isEmpty () )
        return;
    bool ok;
    int step = text.toInt( &ok );
    if( ok )
        m_filmstrip.stepPerFrame( (float)step );
    else
        QMessageBox::critical( this, "Format Error", "Invalid input should be blank or positive integer." );
}

void TableauLayout::stepPrevFrameFilmstrip()
{
    m_filmstrip.prevFrameStep();
}

void TableauLayout::stepNextFrameFilmstrip()
{
    m_filmstrip.nextFrameStep();
}

void TableauLayout::closeEvent(QCloseEvent *event)
{
    if(!m_tableauSaved)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Save?"),
                            tr("The landmark tableau file <b>'%1'</b> has been modified.\nDo you want to save your changes?")
                            .arg(m_tableauFile),
                            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            saveTableau();
        else if (ret == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }

    }
    if(!m_targetSaved)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Save?"),
                            tr("The specimen form <b>'%1'</b> has been modified.\nDo you want to save your changes?")
                            .arg(m_targetFile),
                            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            saveForm(m_targetFormItem);
        else if (ret == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
    }
    if(!m_templateSaved)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Save?"),
                            tr("The template form <b>'%1'</b> has been modified.\nDo you want to save your changes?")
                            .arg(m_templateFile),
                            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            saveForm(m_templateFormItem);
        else if (ret == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
    }
    event->accept();
}

void TableauLayout::tblMoveToStart()
{
    if( m_tableauList.size() > 1 )
        m_filmstrip.startFrame();
}

void TableauLayout::tblMoveToEnd()
{
    if( m_tableauList.size() > 1 )
        m_filmstrip.endFrame();
}

void TableauLayout::tblMoveLeftE()
{
    if( m_tableauList.size() > 1 )
    {
        m_filmstrip.prevFrame();
        m_filmstrip.prevFrame();
    }
}

void TableauLayout::tblMoveLeft()
{
    if( m_tableauList.size() > 1 )
        m_filmstrip.prevFrame();
}

void TableauLayout::tblMoveRight()
{
    if( m_tableauList.size() > 1 )
        m_filmstrip.nextFrame();
}

void TableauLayout::tblMoveRightE()
{
    if( m_tableauList.size() > 1 )
    {
        m_filmstrip.nextFrame();
        m_filmstrip.nextFrame();
    }
}

QString TableauLayout::getNewFormFilename()
{
    QString file = m_targetFile;
    /*if( m_tableauList.size() > 0 )
        file = m_tableauList[ m_tableauList.size()-1 ].space[1].form_filename.c_str();*/

    if( file.contains( "_c" ))
    {
        QString b = file.left( file.lastIndexOf("_c") );
        QString a = file.right( file.length() - file.lastIndexOf("_c") - 2 );
        QString c = a.left( a.lastIndexOf(".frm") );

        bool ok = false;
        int count = c.toInt( &ok );

        bool exists = true;
        QString finalname;
        while( exists )
        {
            count++;
            finalname = b + "_c";
            QString num; num.setNum( count );
            finalname += num;
            finalname += ".frm";
            exists = QFile::exists(finalname);
        }
        return finalname;
    }
    else
    {
        QString b = file.left( file.lastIndexOf(".frm") );

        int count = 0;

        bool exists = true;
        QString finalname;
        while( exists )
        {
            count++;
            finalname = b + "_c";
            QString num; num.setNum( count );
            finalname += num;
            finalname += ".frm";
            exists = QFile::exists(finalname);
        }
        return finalname;
    }
    return "";
}

void TableauLayout::tblAppend( FormItem* item )
{
    if( m_targetFile != "" && !m_targetSaved )
    {
        QString f = getNewFormFilename();
        if( f != "" )
        {
            m_targetFile = f;
        }
        saveForm( m_targetFormItem );
    }

    addTableau();
    // now save the tableau file
    saveTableau();
}

void TableauLayout::tblInsert( FormItem* item )
{
    if( m_targetFile != "" && !m_targetSaved)
    {
        QString f = getNewFormFilename();
        if( f != "" )
        {
            m_targetFile = f;
        }
        saveForm( m_targetFormItem );
    }

    bool ok;
    int max = m_tableauList.size() ? m_tableauList.size() : 1;
    int num = QInputDialog::getInteger( NULL, "Insert Tableau", "Specify index to insert tableau", 1, 1, max, 1, &ok);
    if (ok)
    {
        addTableau( num - 1 );
    }
    // now save the tableau file
    saveTableau();
}

void TableauLayout::tblReplace( FormItem* item )
{
    if( m_targetFile != "" && !m_targetSaved)
    {
        QString f = getNewFormFilename();
        if( f != "" )
        {
            m_targetFile = f;
        }
        saveForm( m_targetFormItem );
    }

    // replace
    replaceTableau();
    // now save the tableau ifle
    saveTableau();
}

void TableauLayout::tblSwapPrev()
{
    moveTableauToPrev();
    // now save the tableau file
    saveTableau();
}

void TableauLayout::tblSwapNext()
{
    moveTableauToNext();
    // now save the tableau file
    saveTableau();
}

void TableauLayout::tblDelete( FormItem* item )
{
    // delete
    deleteTableau();
    // now save the tablea file
    saveTableau();
}

void TableauLayout::lmkExport(FormItem* formTreeItem)
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];


    QString fileName = QFileDialog::getSaveFileName(0, tr("Export landmarks"),
                                                    fsLastDir,"*.txt");

    if(fileName != "")
    {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);

        int spaceIndex = formTreeItem==m_targetFormItem ? 1:0;
        const ew::Form3* form = m_dig3.get_spaces()[spaceIndex]->get_form_data();

        if( form != 0 )
        {
            for(unsigned int j=0; j<form->pointsets.size(); ++j)
            {
                if( form->pointsets[j].type == ew::Form3::TYPE_LANDMARK  )
                {
                    out<<form->pointsets[j].locations[0]<<"\t"
                    <<form->pointsets[j].locations[1]<<"\t"
                    <<form->pointsets[j].locations[2]<<"\n";
                }
                if( form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK  )
                {
                    int n = form->pointsets[j].n;

                    for( int i = 0; i < n; ++i )
                    {
                        out<<form->pointsets[j].locations[i * 3]<<"\t"
                        <<form->pointsets[j].locations[i * 3 + 1]<<"\t"
                        <<form->pointsets[j].locations[i * 3 + 2]<<"\n";
                    }
                }
            }
        }

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}

void TableauLayout::saveCurve(FormItem* formTreeItem, int index)
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

	QString fileName = QFileDialog::getSaveFileName(0, tr("Save as Curve"),
                                                fsLastDir,"*.obj");
	if(fileName != "")
	{
		QFile file(fileName);
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream out(&file);

        const ew::Form3* form = m_dig3.get_spaces()[1]->get_form_data();

        if( form != 0 )
		{
        	int num = 0;
			for(unsigned int j=0; j<form->pointsets.size(); ++j)
			{
				if( form->pointsets[j].type == ew::Form3::TYPE_LANDMARK )
				{
                	out<<"v "
					<<form->pointsets[j].locations[0]<<" "
					<<form->pointsets[j].locations[1]<<" "
					<<form->pointsets[j].locations[2]<<endl;
					num++;
				}
			}
            for( int i = 1; i < num; ++i )
            {
            	out<<"e "<<i<<" "<<i+1<<endl;
            }
		}
		file.close();

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
	}
}

void TableauLayout::lmkExportAll(FormItem* formTreeItem)
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString exportFileName = QFileDialog::getSaveFileName(0, tr("Export all landmarks"),
                                                    fsLastDir,"*.txt");

    if(exportFileName != "")
    {
        QFile exportFile(exportFileName);
        exportFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&exportFile);

        std::vector<std::vector<double> > rawdata;

        // std::vector<ew::Dig3Tableau> tempTableaus;
        std::vector<ew::Dig3Tableau> tableauList;

        if(m_tableauFile != "")
        {
            QString fileName = m_tableauFile;

            ew::Dig3Tableau::read_file(&tableauList, fileName.toStdString().c_str());

            int landmarks = 0;

            bool warned = false;

            for (unsigned int i=0;i<tableauList.size();i++)
            {
                QString formFile = tableauList[i].space[1].form_filename.c_str();
                if (!formFile.contains('\\') && !formFile.contains('/')) //Relative form path
                    formFile = fileName.left(fileName.lastIndexOf(QRegExp("[\\/]"))+1) + formFile;

                QFile file(formFile);
                if (!file.exists())
                {
                    Logger::getInstance()->log(QString("[Templand Node] Failed to load form! File '%1' does not exist!").arg(formFile), Logger::RUN_ERROR);
                    return;
                }
                else
                {
                    Logger::getInstance()->log(QString("\tLoading form: %1").arg(formFile));
                }

    #ifdef _WIN32
                // for windows change the path slashes from forward to backward
                QString str = formFile.replace( '/', '\\' );
    #else
                QString str = formFile.replace( '\\', '/' );
    #endif

                ew::Form3 form;
                form.read_file(str.toStdString().c_str());
                rawdata.clear();

                if (form.pointsets.size()>0)
                {
                    unsigned int lmks = 0;
                    for (unsigned int j=0; j<form.pointsets.size(); ++j)
                    {
                        std::vector<double> tempvec(3);
                        if (form.pointsets[j].type == ew::Form3::TYPE_LANDMARK)
                        {
                            tempvec[0] = form.pointsets[j].locations[0];
                            tempvec[1] = form.pointsets[j].locations[1];
                            tempvec[2] = form.pointsets[j].locations[2];

                            rawdata.push_back(tempvec);

                            lmks++;
                        }
                        else if (form.pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
                        {
                            // fill in the semi landmarks location in array
                            for ( int k = 0; k < form.pointsets[j].n; k++ )
                            {
                                tempvec[0] = form.pointsets[j].locations[3*k];
                                tempvec[1] = form.pointsets[j].locations[3*k+1];
                                tempvec[2] = form.pointsets[j].locations[3*k+2];

                                rawdata.push_back(tempvec);
                                lmks++;
                            }
                        }
                    }
                    if (landmarks == 0)
                    {
                        landmarks = lmks;
                    }
                    else if (landmarks != (int) lmks)
                    {
                        if(!warned)
                        {
                            Logger::getInstance()->log("[Templand Node] Number of landmarks are not the same for all specimens", Logger::WARNING);
                            warned = true;
                        }
                    }

                    out << "'" << formFile << endl;
                    for (unsigned int j=0;j<lmks;j++)
                    {
                    	//YN: I do not know why the order for the lmk locations was x,-z,y, but Cinzia reported the bug (issue #5)
                    	//out<<rawdata[j][0] << " " << -rawdata[j][2] << " " << rawdata[j][1] << endl;
                    	out<<rawdata[j][0] << " " << rawdata[j][1] << " " << rawdata[j][2] << endl;
                    }
                }
            }

            if(fileName.contains("/"))
            {
                QString dirName = fileName.left(fileName.lastIndexOf("/"));
                settings.setValue("lastDirectory",dirName);
            }
            else if(fileName.contains("\\"))
            {
                QString dirName = fileName.left(fileName.lastIndexOf("\\"));
                settings.setValue("lastDirectory",dirName);
            }
        }
    }
}
