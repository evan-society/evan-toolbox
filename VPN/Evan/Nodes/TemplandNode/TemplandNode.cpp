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

    bool surfaceAdded = false;
	const ew::Form3 * form =  tlw->getTargetFormData();
    unsigned int lmkCount = 0;

    if(isOutputConnected(0))
    {
        //Fill the landmarks Output
        for(unsigned int j=0; j<form->pointsets.size(); ++j)
        {
            if(form->pointsets[j].type == ew::Form3::TYPE_LANDMARK)
                lmkCount++;
            else if (form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
                lmkCount += form->pointsets[j].n;
        }

        if(lmkCount)
        {
            m_targetLmks->ReSize(lmkCount, 3);
            unsigned int lmkIndex = 0;
            for(unsigned int j=0; j<form->pointsets.size(); ++j)
            {
                if(form->pointsets[j].type == ew::Form3::TYPE_LANDMARK)
                {

                    m_targetLmks->set(lmkIndex, 0, form->pointsets[j].locations[0]);
                    m_targetLmks->set(lmkIndex, 1, -form->pointsets[j].locations[2]);
                    m_targetLmks->set(lmkIndex, 2, form->pointsets[j].locations[1]);

                    //m_targetLmks->set(lmkIndex, 0, form->pointsets[j].locations[0]);
                    //m_targetLmks->set(lmkIndex, 1/*2*/, form->pointsets[j].locations[1]);
                    //m_targetLmks->set(lmkIndex, 2/*1*/, form->pointsets[j].locations[2]);
                    lmkIndex++;
                }
                else if (form->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
                {
                    for ( int k = 0; k < form->pointsets[j].n; k++ )
                    {

                        m_targetLmks->set(lmkIndex, 0, form->pointsets[j].locations[3*k]);
                        m_targetLmks->set(lmkIndex, 1, -form->pointsets[j].locations[3*k+2]);
                        m_targetLmks->set(lmkIndex, 2, form->pointsets[j].locations[3*k+1]);

                        //m_targetLmks->set(lmkIndex, 0, form->pointsets[j].locations[3*k]);
                        //m_targetLmks->set(lmkIndex, 1/*2*/, form->pointsets[j].locations[3*k+1]);
                        //m_targetLmks->set(lmkIndex, 2/*1*/, form->pointsets[j].locations[3*k+2]);
                        lmkIndex++;
                    }
                }
            }

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
                file = "Templand Specimen Landmark Set";
            }
            m_targetLmks->initialize(file);
            setOutputPort0(m_targetLmks);

            Logger::getInstance()->log("[Templand Node] Specimen Landmarks updated");
        }
    }

    if(isOutputConnected(1))
    {
        //Fill the Surfaces Output
        for(unsigned int j=0; j<form->surfaces.size(); ++j)
        {
            if(j < m_targetSurfaces->getSize())
            {
                if(m_targetSurfaces->getSurface(j)->getRenderableName() == form->surfaces[j].id.c_str())
                    continue;
                else
                {
                    Surface* outSurface = new Surface;
                    outSurface->initialize(form->surfaces[j].id.c_str(), form->surfaces[j].file.c_str());
                    m_targetSurfaces->setSurface(j, outSurface);
                    surfaceAdded = true;
                }
            }
            else
            {
                Surface* outSurface = new Surface;
                outSurface->initialize(form->surfaces[j].id.c_str(), form->surfaces[j].file.c_str());
                m_targetSurfaces->addSurface(outSurface);
                surfaceAdded = true;
            }
        }

        if(surfaceAdded)
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
        //Fill the Landmarks Output
        lmkCount = 0;
        for(unsigned int j=0; j<form2->pointsets.size(); ++j)
        {
            if(form2->pointsets[j].type == ew::Form3::TYPE_LANDMARK)
                lmkCount++;
            else if(form2->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
                lmkCount += form2->pointsets[j].n;
        }

        if(lmkCount)
        {
            m_templateLmks->ReSize(lmkCount, 3);
            unsigned int lmkIndex = 0;
            for(unsigned int j=0; j<form2->pointsets.size(); ++j)
            {
                if(form2->pointsets[j].type == ew::Form3::TYPE_LANDMARK)
                {
                    m_templateLmks->set(lmkIndex, 0, form2->pointsets[j].locations[0]);
                    m_templateLmks->set(lmkIndex, 1, -form2->pointsets[j].locations[2]);
                    m_templateLmks->set(lmkIndex, 2, form2->pointsets[j].locations[1]);
                    lmkIndex++;
                }
                else if (form2->pointsets[j].type == ew::Form3::TYPE_SEMI_LANDMARK)
                {
                    for ( int k = 0; k < form2->pointsets[j].n; k++ )
                    {
                        m_templateLmks->set(lmkIndex, 0, form2->pointsets[j].locations[3*k]);
                        m_templateLmks->set(lmkIndex, 1, -form2->pointsets[j].locations[3*k+2]);
                        m_templateLmks->set(lmkIndex, 2, form2->pointsets[j].locations[3*k+1]);
                        lmkIndex++;
                    }
                }
            }

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
        surfaceAdded = false;
        //Fill the Surfaces Output
        for(unsigned int j=0; j<form2->surfaces.size(); ++j)
        {
            if(j < m_templateSurfaces->getSize())
            {
                if(m_templateSurfaces->getSurface(j)->getRenderableName() == form2->surfaces[j].id.c_str())
                    continue;
                else
                {
                    Surface* outSurface = new Surface;
                    outSurface->initialize(form2->surfaces[j].id.c_str(), form2->surfaces[j].file.c_str());
                    m_templateSurfaces->setSurface(j, outSurface);
                    surfaceAdded = true;
                }
            }
            else
            {
                Surface* outSurface = new Surface;
                outSurface->initialize(form2->surfaces[j].id.c_str(), form2->surfaces[j].file.c_str());
                m_templateSurfaces->addSurface(outSurface);
                surfaceAdded = true;
            }
        }

        if(surfaceAdded)
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

void TemplandNode::GetSpecimens()
{
	m_specimens->clear();

	std::vector<std::vector<double> > rawdata;

    // std::vector<ew::Dig3Tableau> tempTableaus;
    std::vector<ew::Dig3Tableau> tableauList;

    QString fileName = getCurrentTableauFile()->toString();

    if(fileName !="")
    {
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
    			Logger::getInstance()->log(QString("[Import Node] Failed to load form! File '%1' does not exist!").arg(formFile), Logger::RUN_ERROR);
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

				LandmarkSet *lmkset = new LandmarkSet(lmks,3);
				for (unsigned int j=0;j<lmks;j++)
				{
					(*lmkset)[j][0] = rawdata[j][0];
					(*lmkset)[j][1] = -rawdata[j][2];
					(*lmkset)[j][2] = rawdata[j][1];
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


