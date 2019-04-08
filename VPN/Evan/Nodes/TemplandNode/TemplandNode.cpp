#include "TemplandNode.h"
#if TEMPLAND_TOOLKIT_BUILD
#include "../../Utilities/Logger.h"
#endif

#include "ViewTree/SurfaceItem.h"
#include "ViewTree/CurveItem.h"
#include "ViewTree/LandmarkItem.h"
#include "ViewTree/SliceItem.h"
#include "ViewTree/VolumeItem.h"
#include "ViewTree/GridItem.h"
#include "ViewTree/RefPlanesItem.h"
#include <QActionGroup>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QProgressDialog>

#include "SlideDialog.h"
#include "ew/View3Surface.h"
#include "ew/Dig3Tableau.h"

#include <iostream>

//extern int IsRegistered;
#include "../../Utilities/Settings.h" //YN 9Nov2015:extern globals are not clean, and are generating link errors with cmake

static const char *const help =
  "Left mouse button drag\t-> Tumble\n"
  "Right mouse button drag\t-> Zoom\n"
  "Right mouse button\t-> Selects a Landmark\n"
  "Middle mouse button drag\t-> Translate\n"
  "Middle mouse button scroll\t-> Push/Pull\n"
  "Left mouse double click\t-> Create lmk\n"
  "Arrow Keys\t\t-> 90 degree rotations\n"
  "r  R\t\t-> 120 degree rotation\n"
  "[  ]\t\t-> 90 degree twists\n"
  "+  -\t\t-> Jump Zoom\n"
  "Page Up/Down\t\t-> Jump Push/Pull\n"
  "M\t\t-> Move selected lmk\n"
  "C\t\t-> Center View\n"
  "Spacebar\t\t-> Focus at View\n\n"
  "Pressing and holding Ctrl along with keys/mouse navigation buttons will result in fine control of the movement.";

#if TEMPLAND_TOOLKIT_BUILD
TemplandNode::TemplandNode(QWidget* parent) :   QMainWindow(parent),  INode(), m_tableauIndex(0)
{
	m_targetLmks = new LandmarkSet;
	m_targetSurfaces = new SurfaceVector;
	m_templateLmks = new LandmarkSet;
	m_templateSurfaces = new SurfaceVector;
	m_templateLmksSpline = new LandmarkSet;
	m_targetLmksSpline = new LandmarkSet;
	m_specimens = new Specimens;
	m_open = false;

#else
TemplandNode::TemplandNode(QWidget* parent) :   QMainWindow(parent), m_tableauIndex(0)
{
#endif
    m_share = 0;

	setupUi((QMainWindow*)this);
	setWhatsThis(tr(help));

    connectActions();

    actionAdd_Tableau->setDisabled( true );
    action_Replace_Tableau->setDisabled( true );
    actionDelete_Tableau->setDisabled( true );

    actionMove_Tableau_Up->setDisabled( true );
    actionMove_Tableau_Down->setDisabled( true );
}

TemplandNode::~TemplandNode()
{
}

#if TEMPLAND_TOOLKIT_BUILD
void TemplandNode::process()
{
    // do nothing
}

QString TemplandNode::toString() const
{
    QString tableaus = "";
    QList<QMdiSubWindow *> cwindows =  mdiArea->subWindowList();

    if ( cwindows.size() > 0)
    {
        QMdiSubWindow * cwin = cwindows[0];
        if( cwin != 0 )
        {
            TableauLayout* tableau = dynamic_cast< TableauLayout* >( cwin->widget() );
            tableaus += tableau->getCurrentFilename();
        }
        for (int i=1;i<cwindows.size();i++)
        {
            cwin = cwindows[i];
            if( cwin != 0 )
            {
                TableauLayout* tableau = dynamic_cast< TableauLayout* >( cwin->widget() );
                tableaus += "$$" + tableau->getCurrentFilename();
            }
        }
    }

    return tableaus;
}

void TemplandNode::fromString(const QString& params)
{
    if (params != "")
    {
        showGUI();
        QStringList tableaus = params.split("$$");

        for(int i=0;i<tableaus.size();i++)
        {
            QDir path;
            QString fileName = tableaus[i];
            if(path.exists(fileName))
            {
                openTableau(fileName);
            }
            else
            {
                Logger::getInstance()->log("[Templand Node] File not found: " + fileName, Logger::WARNING);
            }
        }
        hide();
    }
}

#endif

void TemplandNode::connectActions()
{
	QActionGroup* moveRestrictGroup = new QActionGroup(this);
    moveRestrictGroup->addAction(actionFree_Movement);
    moveRestrictGroup->addAction(actionHRestrict);
    moveRestrictGroup->addAction(actionVRestrict);
    moveRestrictGroup->addAction(actionTwist);
}



TableauLayout* TemplandNode::newTableau( bool createNewTab )
{

    bool first = getCurrentTableauFile() == 0;
    TableauLayout* newProject = new TableauLayout(0, first ? 0 : m_share );

    newProject->setWindowTitle(QString("Untitled %1.ltb*").arg(mdiArea->subWindowList().count()+1));
    mdiArea->setActiveSubWindow(mdiArea->addSubWindow(newProject));
    connect(newProject, SIGNAL(	status( QString) ), this, SLOT( status( QString) ) );
    connect(newProject, SIGNAL(	status( QString, int) ), this, SLOT( status( QString, int) ) );
    connect(newProject, SIGNAL(	calcConsensus() ), this, SLOT( createConsensus() ) );
    connect(newProject, SIGNAL(	slideAllSignal() ), this, SLOT( slideAll() ) );

    connect( mdiArea, SIGNAL(subWindowActivated (QMdiSubWindow*)), this, SLOT(subWindowChanged(QMdiSubWindow*)) );

    connect(actionHRestrict, SIGNAL(toggled(bool)), newProject, SLOT(restTargetY(bool) ) );
    connect(actionVRestrict, SIGNAL(toggled(bool)), newProject, SLOT(restTargetX(bool) ) );
    connect(actionTwist, SIGNAL(toggled(bool)), newProject, SLOT(restTargetTwist(bool) ) );
    connect(actionHRestrict, SIGNAL(toggled(bool)), newProject, SLOT(restTargetSliceY(bool) ) );
    connect(actionVRestrict, SIGNAL(toggled(bool)), newProject, SLOT(restTargetSliceX(bool) ) );
    connect(actionTwist, SIGNAL(toggled(bool)), newProject, SLOT(restTargetSliceTwist(bool) ) );

    connect(actionHRestrict, SIGNAL(toggled(bool)), newProject, SLOT(restTempY(bool) ) );
    connect(actionVRestrict, SIGNAL(toggled(bool)), newProject, SLOT(restTempX(bool) ) );
    connect(actionTwist, SIGNAL(toggled(bool)), newProject, SLOT(restTempTwist(bool) ) );
    connect(actionHRestrict, SIGNAL(toggled(bool)), newProject, SLOT(restTempSliceY(bool) ) );
    connect(actionVRestrict, SIGNAL(toggled(bool)), newProject, SLOT(restTempSliceX(bool) ) );
    connect(actionTwist, SIGNAL(toggled(bool)), newProject, SLOT(restTempSliceTwist(bool) ) );

    connect( newProject, SIGNAL(filenameChanges(QString)), this, SLOT(filenameChanged(QString)) );

    if( createNewTab )
        newProject->newTableau();

    // get the gobal widget to share
    if( first )
    {
        m_share = newProject->mainWidget();
    }

    newProject->showMaximized();
    return newProject;
}

void TemplandNode::openTableau( const QString& cmd_filename )
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    if(!Globals::getInstance()->isRegistered())
    {
        QMessageBox::information( this, "Warning", QString("Open Tableau option is only allowed for registered users.") );
        return;
    }

    bool fromCMD = true;
    QString fileName;
    if( cmd_filename == "" || cmd_filename.isEmpty() )
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open a Tableau File"),
                                                        fsLastDir,
                                                        "Landmarking tableau files (*.ltb)");
        if(fileName == "" || fileName.isEmpty())
            return;

        fromCMD = false;

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
    else
    {
        fileName = cmd_filename;
        // NOTE: EW seems to not like the path containing \, so replace \ with /
        fileName.replace('\\', '/' );
    }

    TableauLayout* tl = newTableau( false );
    if( !tl->loadTableau( fileName ) )
    {
        mdiArea->closeActiveSubWindow();
        delete tl;

        if( fromCMD )
        {
            QApplication::quit();
        }
    }
    else
    {
        QString str = "Templand - ";
        str += fileName.right(fileName.length() - (fileName.lastIndexOf(QRegExp("[\\/]"))+1));
        setWindowTitle( str );
    }
}

void TemplandNode::saveAsTableau()
{
    if(!Globals::getInstance()->isRegistered())
    {
        QMessageBox::information( this, "Warning", QString("Save As Tableau option is only allowed for registered users.") );
        return;
    }
    TableauLayout* tlw = getCurrentTableauFile();

    if( tlw != 0 )
        tlw->saveTableauAs();
}

void TemplandNode::saveTableau()
{
    if(!Globals::getInstance()->isRegistered())
    {
        QMessageBox::information( this, "Warning", QString("Save Tableau option is only allowed for registered users.") );
        return;
    }
    TableauLayout* tlw = getCurrentTableauFile();

    if( tlw != 0 )
        tlw->saveTableau();
}

void TemplandNode::changeBckgndColor()
{
    TableauLayout* tlw = getCurrentTableauFile();

    if( tlw != 0 )
        tlw->changeBckgndColor();
}

void TemplandNode::templateHide()
{
    TableauLayout* tlw = getCurrentTableauFile();

    if( tlw != 0 )
        tlw->templateHide();
}

void TemplandNode::filenameChanged( QString filename )
{
    setWindowTitle( QString( "Templand - " ) + filename );
}

void TemplandNode::typeLandmark()
{
//    actionLandmark->setChecked( true );
//    actionSemi_Landmark_2->setChecked( false );
//    actionNon_Landmark->setChecked( false );

    TableauLayout* tlw = getCurrentTableauFile();
    if( tlw != 0 )
        tlw->pointType( ew::Form3::TYPE_LANDMARK );
}

void TemplandNode::typeSemiLandmark()
{
//    actionLandmark->setChecked( false );
//    actionSemi_Landmark_2->setChecked( true );
//    actionNon_Landmark->setChecked( false );

    TableauLayout* tlw = getCurrentTableauFile();
    if( tlw != 0 )
        tlw->pointType( ew::Form3::TYPE_SEMI_LANDMARK );
}

void TemplandNode::typeNonLandmark()
{
//    actionLandmark->setChecked( false );
//    actionSemi_Landmark_2->setChecked( false );
//    actionNon_Landmark->setChecked( true );

    TableauLayout* tlw = getCurrentTableauFile();
    if( tlw != 0 )
        tlw->pointType( ew::Form3::TYPE_POINT );
}

void TemplandNode::subWindowChanged( QMdiSubWindow * window )
{
    QString str = "Templand";

    if( window == 0 )
    {
        window = mdiArea->currentSubWindow();
    }

    if( window != 0 )
    {
        QWidget* w = window->widget();
        if( w != 0 )
        {
            str += " - ";
            TableauLayout* tbl = dynamic_cast< TableauLayout* >( w );
            str += tbl->getCurrentFilename();
        }
    }
    setWindowTitle( str );
}

void TemplandNode::closeEvent(QCloseEvent *event)
{
#if TEMPLAND_TOOLKIT_BUILD
    m_open = false;
    hide();
#else
     mdiArea->closeAllSubWindows();
     (mdiArea->currentSubWindow()) ? event->ignore() : event->accept();
#endif
}

void TemplandNode::selectPoints( bool tog )
{
    QList< QMdiSubWindow* > list = mdiArea->subWindowList();

    for( int i = 0; i < list.size(); ++i )
    {
        QMdiSubWindow* msw = list[i];

        if( msw == 0 )
            continue;

        TableauLayout* tl = dynamic_cast< TableauLayout* >( msw->widget() );
        tl->selectPointsInViews( tog );
    }
}

void TemplandNode::refreshOutputs()
{
    QMdiSubWindow * cwin = mdiArea->currentSubWindow();

    if( cwin == 0 )
        return;
    emit status(QString("Refreshing Output Ports, Please wait..."));
    TableauLayout* tlw = dynamic_cast< TableauLayout* >( cwin->widget() );

	const ew::Form3 * form =  tlw->getTargetFormData();
    unsigned int lmkCount = 0;

    if(isOutputConnected(0))
    {
        if(fillLandmarkSet(m_targetLmks,form))
        {
            QString file = "";
            if(getCurrentTableauFile() != 0)
            {
                file = getCurrentTableauFile()->getTargetFormFileName();

                if(file.contains('/'))
                {
                    file = file.mid(file.lastIndexOf('/')+1,-1);
                }
                else if(file.contains('\\'))
                {
                    file = file.mid(file.lastIndexOf('\\')+1,-1);
                }
                if(file.contains('.'))
                {
                    file = file.left(file.lastIndexOf('.'));
                }
            }
            if(file == "" || file.isEmpty())
            {
                file = "Templand Specimen Landmark Set";
            }
            m_targetLmks->initialize(file);
            setOutputPort0(m_targetLmks);

            Logger::getInstance()->log("[Templand Node] Specimen Landmarks updated");
        }
    }

    if(isOutputConnected(1))
    {
        if(fillSurfaces(m_targetSurfaces,form))
        {
            m_targetSurfaces->initialize(form->surfaces[0].id.c_str());
            setOutputPort1(m_targetSurfaces);

            Logger::getInstance()->log("[Templand Node] Specimen Surface updated");
        }
    }

    // template landmarks and surfaces
    const ew::Form3 * form2 =  tlw->getTemplateFormData();

    if(isOutputConnected(2))
    {
        if(fillLandmarkSet(m_templateLmks,form2))
        {
            QString file = "";
            if(getCurrentTableauFile() != 0)
            {
                file = getCurrentTableauFile()->getTemplateFormFileName();

                if(file.contains('/'))
                {
                    file = file.mid(file.lastIndexOf('/')+1,-1);
                }
                else if(file.contains('\\'))
                {
                    file = file.mid(file.lastIndexOf('\\')+1,-1);
                }
                if(file.contains('.'))
                {
                    file = file.left(file.lastIndexOf('.'));
                }
            }
            if(file == "" || file.isEmpty())
            {
                file = "Templand Template Landmark Set";
            }
            m_templateLmks->initialize(file);
            setOutputPort2(m_templateLmks);

            Logger::getInstance()->log("[Templand Node] Template Landmarks updated");
        }
    }

    if(isOutputConnected(3))
    {
        if(fillSurfaces(m_templateSurfaces,form2))
        {
            m_templateSurfaces->initialize(form2->surfaces[0].id.c_str());
            setOutputPort3(m_templateSurfaces);

            Logger::getInstance()->log("[Templand Node] Template Surface updated");
        }
    }

    if(isOutputConnected(4) || isOutputConnected(5))
    {
        MatrixD * templateMat = tlw->getSplineTemplate();
        MatrixD * targetMat = tlw->getSplineTarget();

        if(templateMat != 0 && targetMat !=0)
        {
            m_templateLmksSpline->ReSize(templateMat->GetRows(),3);
            m_targetLmksSpline->ReSize(targetMat->GetRows(),3);
            for (unsigned int i = 0;i< templateMat->GetRows();i++)
            {
                m_templateLmksSpline->set(i, 0, (*templateMat)[i][0]);
                m_templateLmksSpline->set(i, 1, -(*templateMat)[i][2]);
                m_templateLmksSpline->set(i, 2, (*templateMat)[i][1]);
            }
            for (unsigned int i = 0;i< targetMat->GetRows();i++)
            {
                m_targetLmksSpline->set(i, 0, (*targetMat)[i][0]);
                m_targetLmksSpline->set(i, 1, -(*targetMat)[i][2]);
                m_targetLmksSpline->set(i, 2, (*targetMat)[i][1]);
            }
            Logger::getInstance()->log("[Templand Node] Source Landmarks for Spline updated");
            Logger::getInstance()->log("[Templand Node] Target Landmarks for Spline updated");

            m_templateLmksSpline->initialize("Templand Spline Source");
            m_targetLmksSpline->initialize("Templand Spline Target");
            setOutputPort4(m_templateLmksSpline);
            setOutputPort5(m_targetLmksSpline);
        }
    }

    if(isOutputConnected(6))
    {
        GetSpecimens();
        setOutputPort6(m_specimens);
        Logger::getInstance()->log("[Templand Node] Specimens updated");
    }

    emit status(QString("Output Ports Updated"));
}

int TemplandNode::fillLandmarkSet(LandmarkSet* lmkset, const ew::Form3* form, bool flipAxis)
{
	//Fill the Landmarks Output
	int lmkCount = 0;
	for(unsigned int j=0; j<form->pointsets.size(); ++j)
	{
		if(form->pointsets[j].type == ew::Form3::TYPE_LANDMARK)
			lmkCount++;
		else if(form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
			lmkCount += form->pointsets[j].n;
	}

	if(lmkCount)
	{
		lmkset->ReSize(lmkCount, 3);
		unsigned int lmkIndex = 0;
		for(unsigned int j=0; j<form->pointsets.size(); ++j)
		{
			if(form->pointsets[j].type == ew::Form3::TYPE_LANDMARK)
			{
				lmkset->set(lmkIndex, 0, form->pointsets[j].locations[0]);
				lmkset->set(lmkIndex, 1, flipAxis?-form->pointsets[j].locations[2]:form->pointsets[j].locations[1]);
				lmkset->set(lmkIndex, 2, flipAxis?form->pointsets[j].locations[1]:form->pointsets[j].locations[2]);
				lmkIndex++;
			}
			else if (form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
			{
				for ( int k = 0; k < form->pointsets[j].n; k++ )
				{
					lmkset->set(lmkIndex, 0, form->pointsets[j].locations[3*k]);
					lmkset->set(lmkIndex, 1, flipAxis?-form->pointsets[j].locations[3*k+2]:form->pointsets[j].locations[3*k+1]);
					lmkset->set(lmkIndex, 2, flipAxis?form->pointsets[j].locations[3*k+1]:form->pointsets[j].locations[3*k+2]);
					lmkIndex++;
				}
			}
		}
	}

	return lmkCount;
}

bool TemplandNode::fillSurfaces(SurfaceVector* sv, const ew::Form3* form)
{
	//Fill the Surfaces Output
    for(unsigned int j=0; j<form->surfaces.size(); ++j)
	{
        bool replaceSurface = (j<sv->getSize());
        if (replaceSurface && sv->getSurface(j)->getRenderableName() == form->surfaces[j].id.c_str())
			continue;
        Surface* outSurface = new Surface;
        if(!outSurface->initialize(form->surfaces[j].id.c_str(), form->surfaces[j].file.c_str()))
        {
            Logger::getInstance()->log(QString("[Templand Node] Failed to initialize surface '%1'!").arg(form->surfaces[j].file.c_str()),
                                        Logger::RUN_ERROR);
            delete outSurface;
            return false;
        }
        outSurface->refreshGeometry(true);
		if (replaceSurface)
            sv->setSurface(j, outSurface);
		else
			sv->addSurface(outSurface);
	}

	return sv->getSize()>0;
}

void TemplandNode::GetSpecimens(bool flipAxis)
{
    m_specimens->clear();
	std::vector<std::vector<double> > rawdata;

    QString fileName = getCurrentTableauFile()->toString();

    if(fileName !="")
    {
    	//ew::Dig3Tableau::read_file(&tableauList, fileName.toStdString().c_str());
    	QMdiSubWindow * cwin = mdiArea->currentSubWindow();
    	TableauLayout* tlw = dynamic_cast< TableauLayout* >( cwin->widget() );
    	const std::vector<ew::Dig3Tableau>& tableauList = tlw->getTableauList();

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
				LandmarkSet *lmkset = new LandmarkSet;
				int lmks = fillLandmarkSet(lmkset,&form,flipAxis);
				if (landmarks == 0)
					landmarks = lmks;
				else if (landmarks != (int) lmks)
				{
					if(!warned)
					{
						Logger::getInstance()->log("[Templand Node] Number of landmarks are not the same for all specimens", Logger::WARNING);
						warned = true;
					}
				}
				m_specimens->addLandmarkSet(lmkset);

				QString value;
				value.setNum(i+1);
				m_specimens->setLabelValue(i, "ID", value);
			}
		}

		SpecimenGroup* defaultGroup = new SpecimenGroup;
		for (unsigned int k=0;k<m_specimens->getSize();k++)
		{
			 defaultGroup->addMember(m_specimens->getLandmarkSet(k), m_specimens->getSpecimenLabelValue(k, "ID"));
		}
		m_specimens->addGroup("Ungrouped", defaultGroup);
		m_specimens->initialize("Templand Specimens");
	}
}

#include"../GPANode/GPANode.h"

void TemplandNode::createConsensus()
{
    if(QMessageBox::question(this, "Create consensus", "This will first perform a GPA over all the specimens in the tableau, "
                                                    "then replace the current template with the GPA mean shape (consensus), "
                                                    "and slide all specimens against the consensus.",
                                   QMessageBox::Ok|QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    status("Creating Consensus...");
	QApplication::setOverrideCursor(Qt::WaitCursor);

	//1- Perform GPA
	QMdiSubWindow * cwin = mdiArea->currentSubWindow();
	TableauLayout* tlw = dynamic_cast< TableauLayout* >( cwin->widget() );
	const ew::Form3 * templateForm =  tlw->getTemplateFormData();
    QString cfname = tlw->getTemplateFormFileName();
    bool firstConsensus = !cfname.endsWith("_consensus.frm");

	int lmkCount=fillLandmarkSet(m_templateLmks,templateForm,false);
	GetSpecimens(false);

    const int individuals = m_specimens->getSize() + 1;
	const int landmarks = m_specimens->getLandmarkCount();
	const int dimensions = m_specimens->getLandmarkDimensions(); //should always be 3
	if (lmkCount != landmarks)
		Logger::getInstance()->log("[Templand Node] Template and target(s) do not have the same number of landmarks!", Logger::WARNING);
	Matrix<double> data(landmarks * individuals, dimensions);

	for (int i=0;i<individuals;i++) //Fill target landmarks
	{
        LandmarkSet *lands = i<(individuals-1)?m_specimens->getLandmarkSet(i):m_templateLmks;
		for (int j=0;j<landmarks;j++)
			for (int k=0;k<dimensions;k++)
				data[i*landmarks+j][k] = (*lands)[j][k];
	}

    GPANode gpaNode(this);
    if(!gpaNode.exec())
    {
        status("Consensus cancelled!");
        QApplication::restoreOverrideCursor();
        return;
    }
    GPA* gpa = gpaNode.getGPARef();
    gpa->LoadData(&data, individuals, landmarks, dimensions);
    gpaNode.CalculateGPA();

	//2- Get mean shape, multiply mean shape by mean centroid size
	LandmarkSet meanSpecimen;
    gpa->GetProcrustesMean(&meanSpecimen);
	MatrixD transMat;
    gpa->GetTransformationMatrix(individuals-1,&transMat); //Template is the last specimen
	meanSpecimen.transform(!transMat);
    if(firstConsensus) tlw->replaceTargetForm(tlw->getTemplateFormFileName());

	//Set mean shape as the new template
	ew::Form3 consensusForm(*templateForm);
	int lmkIndex = 0;
	QVector<QString> supersets;
	QVector<QString> subsets;
	for(unsigned int j=0; j<templateForm->pointsets.size(); ++j)
	{
		if(consensusForm.pointsets[j].type == ew::Form3::TYPE_LANDMARK)
		{
			consensusForm.pointsets[j].locations[0] = meanSpecimen[lmkIndex][0];
			consensusForm.pointsets[j].locations[1] = meanSpecimen[lmkIndex][1];
			consensusForm.pointsets[j].locations[2] = meanSpecimen[lmkIndex][2];
			lmkIndex++;
		}
		else if (consensusForm.pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
		{
			for ( int k = 0; k < consensusForm.pointsets[j].n; k++ )
			{
				consensusForm.pointsets[j].locations[3*k] = meanSpecimen[lmkIndex][0];
				consensusForm.pointsets[j].locations[3*k+1] = meanSpecimen[lmkIndex][1];
				consensusForm.pointsets[j].locations[3*k+2] = meanSpecimen[lmkIndex][2];
				lmkIndex++;
			}
			consensusForm.pointsets[j].state = ew::Form3::STATE_SET;
			//consensusForm.pointsets[j].relax_dims.clear();
			std::fill(consensusForm.pointsets[j].relax_params.begin(), consensusForm.pointsets[j].relax_params.end(),0);
			subsets.push_back(consensusForm.pointsets[j].id.c_str());
			supersets.push_back(consensusForm.search_superset(subsets.back().toStdString().c_str()));
		}
	}

	//3- Generate consensus surface
	status("Warping template surface(s) to mean shape...");

	TPS tps(m_templateLmks->GetMatrix(),meanSpecimen.GetMatrix(),meanSpecimen.GetRows(),3);
	tps.PerformTPS();

	const ew::Dig3Space* templateSpace = tlw->getTemplateDigSpace();
	for(int i=0; i<templateSpace->get_n_surface_nodes(); ++i)
	{
		const ew::Surface3* currentSurface = templateSpace->get_surface_nodes()[i]->get_data();
		Matrix<double>* pointInOutMat = new Matrix<double>(currentSurface->points.size()/3,3);
		Matrix<unsigned int>* facesMat= new Matrix<unsigned int>(currentSurface->faces.size()/3,3);
		for(size_t j=0; j<pointInOutMat->GetRows(); ++j)
		{
			(*pointInOutMat)[j][0] = currentSurface->points[3*j];
			(*pointInOutMat)[j][1] = currentSurface->points[3*j+1];
			(*pointInOutMat)[j][2] = currentSurface->points[3*j+2];
		}
		for(size_t j=0; j<facesMat->GetRows(); ++j)
		{
			(*facesMat)[j][0] = currentSurface->faces[3*j];
			(*facesMat)[j][1] = currentSurface->faces[3*j+1];
			(*facesMat)[j][2] = currentSurface->faces[3*j+2];
		}
		tps.WarpPoints(pointInOutMat->GetMatrix(), pointInOutMat, pointInOutMat->GetRows());

        QString newSurfaceId = QString(consensusForm.surfaces[i].id.c_str());
        if(firstConsensus) newSurfaceId += "_consensus";
        QString newSurfaceFile = consensusForm.surfaces[i].file.c_str();
        if(firstConsensus) newSurfaceFile = newSurfaceFile.left( newSurfaceFile.lastIndexOf(".obj") )+ "_consensus.obj";

		Surface* inSurface = new Surface;
		inSurface->setVertices(pointInOutMat);
		inSurface->setIndices(facesMat);
		inSurface->initialize(templateForm->surfaces[i].id.c_str());
		inSurface->saveToFile(newSurfaceFile);

		int embedIndex = supersets.lastIndexOf(consensusForm.surfaces[i].id.c_str());
		if(embedIndex>=0)
			consensusForm.set_superset(subsets[embedIndex].toStdString().c_str(), newSurfaceId.toStdString().c_str());
		consensusForm.surfaces[i].file = newSurfaceFile.toStdString();
		consensusForm.surfaces[i].id = newSurfaceId.toStdString();
	}
	// Also warp curves
	status("Warping template curves(s) to mean shape...");
	for(int i=0; i<templateSpace->get_n_curve_nodes(); ++i)
	{
		const ew::Curve3* currentCurve = templateSpace->get_curve_nodes()[i]->get_data();
		Matrix<double> pointInOutMat(currentCurve->points.size()/3,3);
		for(size_t j=0; j<pointInOutMat.GetRows(); ++j)
		{
			pointInOutMat[j][0] = currentCurve->points[3*j];
			pointInOutMat[j][1] = currentCurve->points[3*j+1];
			pointInOutMat[j][2] = currentCurve->points[3*j+2];
		}
		tps.WarpPoints(pointInOutMat.GetMatrix(), &pointInOutMat, pointInOutMat.GetRows());

		/*for(size_t j=0; j<pointInOutMat.GetRows(); ++j)
		{
			double temp = pointInOutMat[j][1];
			pointInOutMat[j][1] = pointInOutMat[j][2];
			pointInOutMat[j][2] = -temp;
		}*/

		std::vector<double> warpedPoints;
		warpedPoints.assign(pointInOutMat.GetSinglePtr(), pointInOutMat.GetSinglePtr()+currentCurve->points.size());
		ew::Curve3 warpedCurve(*currentCurve);
		warpedCurve.read_points(warpedPoints);

		QString newCurveFile = consensusForm.curves[i].file.c_str();
        if(firstConsensus) newCurveFile = newCurveFile.left( newCurveFile.lastIndexOf(".obj") )+ "_consensus.obj";

		warpedCurve.write_file_txt(newCurveFile.toStdString().c_str());
		consensusForm.curves[i].file = newCurveFile.toStdString();
	}

	// Now save consensus form to disk
	status("Saving consensus to disk...");
    if(firstConsensus) cfname = cfname.left( cfname.lastIndexOf(".frm") ) + "_consensus.frm";
	consensusForm.write_file(cfname.toStdString().c_str(),false);

	tlw->replaceTemplateForm(cfname);
    if(firstConsensus)
    {
        tlw->addTableau();
        // Project all template landmarks and semi-landmarks to their embedding (just in case)
        tlw->projectAll(true,0);

        //4- Replace templates of all targets with the consensus
        status("Replacing template with consensus...");
        const std::vector<ew::Dig3Tableau>& tableauList = tlw->getTableauList();
        for(size_t i=0; i<tableauList.size(); ++i)
        {
            ew::Dig3Tableau currentTbl = tableauList[i];
            currentTbl.space[0].form_filename = cfname.toStdString();
            tlw->setTableau(i, currentTbl);
        }
    }
    saveTableau();

    //5- Finally, slide all spcimens against consensus
    status("Sliding all specimens against consensus...");
    slideAll();
    status("Consensus done!", 2);

	QApplication::restoreOverrideCursor();
}

void TemplandNode::slideAll()
{
    QMdiSubWindow * cwin = mdiArea->currentSubWindow();
    TableauLayout* tlw = dynamic_cast< TableauLayout* >( cwin->widget() );
    const std::vector<ew::Dig3Tableau>& tableauList = tlw->getTableauList();

    SlideDialog slideDialog(this, false);
    slideDialog.setWindowTitle("Set slide all options");
    if(slideDialog.exec())
    {
        status("Sliding tableau...");
        QApplication::setOverrideCursor(Qt::WaitCursor);
        //5- Now slide all targets against the consensus
        QProgressDialog progress("Sliding all targets on template...", "Abort", 0, tableauList.size()-1, this);
        progress.setMinimumDuration(0);
        progress.setWindowModality(Qt::WindowModal);
        progress.setModal(true);
        progress.setAutoClose(false);
        progress.setAutoReset(false);
        progress.setCancelButton(0);
        status("Sliding target semi-landmarks against current template...");
        for(size_t i=0; i<tableauList.size(); ++i)
        {
            //tlw->moveToFrame(i+1);
            tlw->slideAll(slideDialog.getIterations(),slideDialog.getEpsilon());
            tlw->moveTableauToNext();
            progress.setValue(i);
        }
        progress.setCancelButton(new QPushButton("Done"));
        if(progress.exec() == 0)
        {
            status("Slide all done!");
            QApplication::restoreOverrideCursor();
        }
    }
}
