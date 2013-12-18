#include <QtGui>
#include <QImage>
#include <QPixmap>
#include <QString>
#include "MainWindow.h"
#include "../Nodes/NodeButton.h"
#include "../Utilities/XMLParser.h"
#include "../Utilities/Logger.h"
#include "VPNLayout.h"

#ifdef ENCRYPT_EVAN_LICENSE_
#include "../Utilities/LicenseKeyUtils.h"
#endif

#ifdef USE_UNIX_LOCATIONS
extern QString libpath;
extern QString licensepath;
#endif

extern int IsRegistered;
extern bool IsRunning;
extern QString workingDirectory;

QString getVersionString()
{
    XMLParser parser(":MainWindow/About.xml");
    parser.gotoElement("Version");
    QString versionString = parser.getAttribute("ni");
    return versionString;
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), m_activeProject(NULL), m_logger(Logger::getInstance())
{
    setupUi(this);

    m_zoomSpin = new QSpinBox;

    connect(m_zoomSpin, SIGNAL(valueChanged(int)),this, SLOT(zoomChanged(int)));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(actionLicense, SIGNAL(triggered()), this, SLOT(showLicense()));
    connect(actionRegister, SIGNAL(triggered()), this, SLOT(showRegistration()));

    m_zoomSpin->setMinimum(0);
    m_zoomSpin->setMaximum(500);
    m_zoomSpin->setValue(100);
    m_zoomSpin->setSingleStep(10);
    m_zoomSpin->setSuffix("%");

    toolBar->addWidget(m_zoomSpin);

    // splitter for log and network
    QSplitter* page = new QSplitter(Qt::Vertical);
    verticalLayout->addWidget(page);
    page->addWidget(mdiArea);
    mdiArea->setParent(page);
    page->addWidget(m_logTextEdit);
    m_logTextEdit->setParent(page);

    // Recent file history
    for (int i = 0; i < 5; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    for (int i = 0; i < 5; ++i)
    {
        recentMenu->addAction(recentFileActs[i]);
    }

    loadNodeBox();
    connectActions();
    m_logger->setLog(m_logTextEdit);
    QString title = "EVAN Toolbox " + getVersionString();
    setWindowTitle(QApplication::translate("MainWindow", title.toStdString().c_str(), 0, QApplication::UnicodeUTF8));

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateActions()));

    actionRunNetwork->setChecked(false);
    actionRunNetwork->setDisabled(true);
    actionForceRunNetwork->setDisabled(true);
    actionDeleteSelection->setDisabled(true);
    action_Save->setDisabled(true);
    m_zoomSpin->setDisabled(true);

#ifdef ENCRYPT_EVAN_LICENSE_
    if(checkForLicense() == 0)
    {
        QDialog *qRegDialog = new QDialog(0,Qt::WindowStaysOnTopHint);
        registerInstuctDialog.setupUi(qRegDialog);
        qRegDialog->show();
        QObject::connect(registerInstuctDialog.pushButton, SIGNAL(clicked()), qRegDialog, SLOT(accept()));
    }
    else
    {
        IsRegistered = 1;
    }
#else
    menuAbout->removeAction(actionRegister);
    IsRegistered = 1;
#endif

/*
    QDate builddate(2012,4,28);
    QDate expdate(2012,10,1);

    if(QDate::currentDate() > expdate || QDate::currentDate() < builddate)
    {
        Logger::getInstance()->log("Trial period expired. Contact evan-toolboxsupport@univie.ac.at to buy a copy", Logger::WARNING);
    }
    else
    {
        Logger::getInstance()->log("Trial version, expires: " + expdate.toString());
        menuAbout->removeAction(actionRegister);
        IsRegistered = 1;
    }
*/
    IsRunning = false;
    workingDirectory = "";

    if(IsRegistered)
    {
        updateRecentFileActions();
    }

// add specific subfolder in which qt plugins shall be searched
//    Logger::getInstance()->log( QCoreApplication::applicationDirPath() );
//    Logger::getInstance()->log( QCoreApplication::applicationDirPath() + "/QtPlugins" );
//    Logger::getInstance()->log( QCoreApplication::applicationDirPath() + "/../QtPlugins" );

    QDir qd;
//    Logger::getInstance()->log( qd.dirName() );
//    Logger::getInstance()->log( qd.absolutePath() );
//    Logger::getInstance()->log( qd.dirName() + "/QtPlugins" );
//    Logger::getInstance()->log( qd.absolutePath() + "/QtPlugins" );

// basically the first path should do, but it does not hurt to specify further (~identical) search pathes
    QCoreApplication::addLibraryPath( qd.absolutePath() + "/QtPlugins" );
    QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() + "/QtPlugins" );
    QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() + "/../QtPlugins" ); // when launched from C::B the path is /debug or /release, so we have to '..' once
    QCoreApplication::addLibraryPath( qd.dirName() + "/QtPlugins" );



    crashBugHack(); // only once at startup
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadNetwork(action->data().toString());
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings("Evan", "Evan Toolbox");
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), 5);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < 5; ++j)
        recentFileActs[j]->setVisible(false);

    if (numRecentFiles>0)
        emptyAction->setVisible(false);
}

MainWindow::~MainWindow()
{
    for (nodeButtonIterator i=m_nodeButtons.begin(); i!=m_nodeButtons.end(); ++i)
        delete (*i);
    m_nodeButtons.clear();
    delete m_logger;
}

void MainWindow::zoomChanged(int scale)
{
    if (mdiArea->activeSubWindow())
    {
        QGraphicsView* projectView = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() )->getProjectView();
        m_activeProject = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() )->getProjectArea();
        if (projectView)
        {
            QMatrix oldMatrix = projectView->matrix();
            double newScale = scale/100.0;
            projectView->resetMatrix();
            projectView->translate(oldMatrix.dx(), oldMatrix.dy());
            projectView->scale(newScale, newScale);
        }
        if(m_activeProject)
        {
            m_activeProject->setZoom(scale);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    (mdiArea->currentSubWindow()) ? event->ignore() : event->accept();
}

void MainWindow::loadNodeBox()
{
#ifdef USE_UNIX_LOCATIONS
    XMLParser nodeParser(libpath + "Nodes/Nodes.xml");
#else

#ifdef USE_R_
    XMLParser nodeParser("Nodes/NodesR.xml");
#else
    XMLParser nodeParser("Nodes/Nodes.xml");
#endif

#endif


    if (nodeParser.gotoElement("DataTab"))
    {
        QVBoxLayout* verticalLayout = new QVBoxLayout(dataTab);
        loadTabNodes(nodeParser,verticalLayout);
        verticalLayout->addStretch();
    }
    nodeParser.resetCursor();
    if (nodeParser.gotoElement("UtilTab"))
    {
        QVBoxLayout* verticalLayout = new QVBoxLayout(utilTab);
        loadTabNodes(nodeParser,verticalLayout);
        verticalLayout->addStretch();
    }
    nodeParser.resetCursor();
    if (nodeParser.gotoElement("GMMTab"))
    {
        QVBoxLayout* verticalLayout = new QVBoxLayout(gmmTab);
        loadTabNodes(nodeParser,verticalLayout);
        verticalLayout->addStretch();
    }
    nodeParser.resetCursor();
    if (nodeParser.gotoElement("ViewTab"))
    {
        QVBoxLayout* verticalLayout = new QVBoxLayout(viewTab);
        loadTabNodes(nodeParser,verticalLayout);
        verticalLayout->addStretch();
    }
}

void MainWindow::connectActions()
{
    connect(actionRunNetwork, SIGNAL(toggled(bool)), this, SLOT(runNetwork(bool)));
    connect(actionForceRunNetwork, SIGNAL(triggered(bool)), this, SLOT(forceRunNetwork(bool)));

    connect(action_New, SIGNAL(triggered()), this, SLOT(createNewProject()));
    connect(action_Save, SIGNAL(triggered()), this, SLOT(saveNetwork()));
    connect(actionWith_Nodes_States, SIGNAL(triggered()), this, SLOT(saveNetworkWithStates()));
    connect(actionWithout_Nodes_States, SIGNAL(triggered()), this, SLOT(saveNetworkWithoutStates()));
    connect(action_Open, SIGNAL(triggered()), this, SLOT(loadNetworkFromFilename()));
}

VPNLayout* MainWindow::createNewProject()
{
    VPNLayout* newProject = new VPNLayout();
    newProject->setWindowTitle(QString("Untitled Network%1.vpn*").arg(mdiArea->subWindowList().count()+1));
    mdiArea->setActiveSubWindow(mdiArea->addSubWindow(newProject));
    newProject->showMaximized();
    m_activeProject = newProject->getProjectArea();
    connectVPN();
    return newProject;
}

void MainWindow::saveNetwork()
{
    if(IsRegistered == 0)
    {
        Logger::getInstance()->log("Save option is only allowed for registered users.",Logger::WARNING);
        return;
    }
    if (mdiArea->activeSubWindow())
    {
        VPNLayout* vpnNetwork = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() );
        if (!vpnNetwork)
            return;
        if (!vpnNetwork->isSaved())
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            mainStatusBar->showMessage("Saving Network, Please wait...");
            vpnNetwork->save();
            mainStatusBar->showMessage(QString("Network %1 Saved").arg(vpnNetwork->getFilePath()), 2000);
            QApplication::restoreOverrideCursor();
        }
    }
}

void MainWindow::saveNetworkAs()
{
    if(IsRegistered == 0)
    {
        Logger::getInstance()->log("Save As option is only allowed for registered users.",Logger::WARNING);
        return;
    }
    if (mdiArea->activeSubWindow())
    {
        VPNLayout* vpnNetwork = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() );
        if (!vpnNetwork)
            return;
        QApplication::setOverrideCursor(Qt::WaitCursor);
        mainStatusBar->showMessage("Saving Network, Please wait...");
        vpnNetwork->saveAs();
        mainStatusBar->showMessage(QString("Network %1 Saved").arg(vpnNetwork->getFilePath()), 2000);
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::saveNetworkWithStates()
{
    if(IsRegistered == 0)
    {
        Logger::getInstance()->log("Save with states option is only allowed for registered users.",Logger::WARNING);
        return;
    }
    if (mdiArea->activeSubWindow())
    {
        VPNLayout* vpnNetwork = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() );
        if (!vpnNetwork)
            return;
        QApplication::setOverrideCursor(Qt::WaitCursor);
        mainStatusBar->showMessage("Saving Network, Please wait...");
        vpnNetwork->saveAs();
        mainStatusBar->showMessage(QString("Network %1 Saved").arg(vpnNetwork->getFilePath()), 2000);
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::saveNetworkWithoutStates()
{
    if (mdiArea->activeSubWindow())
    {
        VPNLayout* vpnNetwork = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() );
        if (!vpnNetwork)
            return;
        QApplication::setOverrideCursor(Qt::WaitCursor);
        mainStatusBar->showMessage("Saving Network, Please wait...");
        vpnNetwork->saveWithoutStates();
        mainStatusBar->showMessage(QString("Network %1 Saved").arg(vpnNetwork->getFilePath()), 2000);
        QApplication::restoreOverrideCursor();
    }
}


void MainWindow::crashBugHack()
{
    //### ugly fix for initial crash bug ###//
    // simply create a minimal new project with a single viewer node in it
    {
        //VPNLayout* MainWindow::createNewProject()
    //{
        VPNLayout* newProject = new VPNLayout();
        newProject->setWindowTitle(QString("Untitled Network%1.vpn*").arg(mdiArea->subWindowList().count()+1));
        mdiArea->setActiveSubWindow(mdiArea->addSubWindow(newProject));
        newProject->showMaximized();
        m_activeProject = newProject->getProjectArea();
        connectVPN();
        //return newProject;
    //}
        QPointF pos(0.0f,0.0f);
        QString annotation("");
        m_activeProject = newProject->getProjectArea();
        m_activeProject->setSelectedNode( QString("ViewerNode::3D Viewer::All viewable objects:Renderables:0,::") );
        m_activeProject->insertNode(pos, annotation);
        delete newProject;
    }
    //### ugly fix for initial crash bug ###//

}


void MainWindow::loadNetworkFromFilename()
{
    if(IsRegistered == 0)
    {
        Logger::getInstance()->log("Open option is only allowed for registered users.",Logger::WARNING);
        return;
    }

    //crashBugHack(); // every time a new .VPN file is loaded


    QSettings settings("Evan");//, "Evan Toolbox");
    QStringList files = settings.value("lastDirectory").toStringList();
    QString fileName;
    if (files.size()>0)
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open a Network File"),
                                           files[0],
                                           tr("VPN Network (*.vpn)"));

    }
    else
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open a Network File"),
                                           QString("Examples"),
                                           tr("VPN Network (*.vpn)"));
    }

    if (fileName.isEmpty())
        return;

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

    loadNetwork(fileName);
}

void MainWindow::loadNetwork(QString fileName)
{
    if (fileName == "" || fileName.isEmpty())
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    mainStatusBar->showMessage("Loading Network, Please wait...");

    VPNLayout* newProject = createNewProject();
    newProject->setFilePath(fileName);
    m_activeProject = newProject->getProjectArea();

    XMLParser parser(fileName);
    while (parser.gotoElement("Node"))
    {
        QString selectedNode = "";
//        QPixmap selectedNodeIcon;
        for (int i=0; i< m_nodeNames.size(); ++i)
        {
            if ( m_nodeNames[i].startsWith(parser.getAttribute("type")) )
            {
                selectedNode = m_nodeNames[i];
//                selectedNodeIcon = m_nodeButtons[i]->icon().pixmap(64);
                break;
            }
        }
        m_activeProject->setSelectedNode(selectedNode);
//        m_activeProject->setSelectedNodeIcon(selectedNodeIcon);
        m_activeProject->setSelectedNodeParams(parser.getAttribute("params"));
        m_activeProject->insertNode(QPointF(parser.getAttribute("x").toFloat(),
                                            parser.getAttribute("y").toFloat()),parser.getAttribute("annotation"));
    }

    while (parser.gotoElement("Link"))
    {
        m_activeProject->insertLink(parser.getAttribute("sNode").toInt(),
                                    parser.getAttribute("eNode").toInt(),
                                    parser.getAttribute("ports"));
    }

    while (parser.gotoElement("Settings"))
    {
        m_activeProject->setZoom(parser.getAttribute("zoom").toInt());
        m_zoomSpin->setValue(parser.getAttribute("zoom").toInt());
    }

    mainStatusBar->showMessage(QString("Network %1 Loaded").arg(fileName), 2000);
    Logger::getInstance()->log("<b>VPN "+ fileName +" Opened</b>");
    QApplication::restoreOverrideCursor();
    newProject->updated(false);

    //VD update recent history
    QSettings settings("Evan", "Evan Toolbox");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > 5)
        files.removeLast();

    settings.setValue("recentFileList", files);
    updateRecentFileActions();
}

void MainWindow::connectVPN()
{
    if (mdiArea->activeSubWindow())
    {
        //m_activeProject = ((VPNLayout*)mdiArea->activeSubWindow())->getProjectArea();
        m_activeProject = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() )->getProjectArea();

//        connect(actionConnectMode, SIGNAL(toggled(bool)), m_activeProject, SLOT(enableConnect(bool)));
//        connect(actionSelectMode, SIGNAL(toggled(bool)), m_activeProject, SLOT(enableMove(bool)));
        connect(actionDeleteSelection, SIGNAL(triggered()), m_activeProject, SLOT(deleteSelection()));
        connect(m_activeProject, SIGNAL(nodeInserted()), this, SLOT(nodeInsertDone()));
        connect(m_activeProject, SIGNAL(linkCreated()), this, SLOT(linkCreateDone()));
        connect(m_activeProject, SIGNAL(setStatusBar(QString,int)), mainStatusBar, SLOT(showMessage(QString,int)));
    }
}

void MainWindow::loadTabNodes(XMLParser& nodeParser, QVBoxLayout* verticalLayout)
{
    nodeParser.goDown(1);
    do
    {
        QString nodeIcon = nodeParser.getAttribute("icon").trimmed();
        QString nodeType = nodeParser.getAttribute("type").trimmed();
        QString nodeName = nodeParser.getAttribute("name").trimmed();
        QString nodeToolTip = nodeParser.getAttribute("tooltip").trimmed();
        QString nodeInputs = "";
        QString nodeOutputs = "";

        if (nodeParser.gotoElement("ports"))
        {
            while (nodeParser.gotoElement("output"))
                nodeOutputs +=  nodeParser.getAttribute("description").trimmed() + ":" +
                                nodeParser.getAttribute("type").trimmed() + ",";
            while (nodeParser.gotoElement("input"))
                nodeInputs +=   nodeParser.getAttribute("description").trimmed() + ":" +
                                nodeParser.getAttribute("type").trimmed() + ":" +
                                nodeParser.getAttribute("optional").trimmed() + ",";
        }

        m_nodeNames.push_back( nodeType + "::" + nodeName + "::"  + nodeInputs + "::" + nodeOutputs );

        NodeButton* nodeButton = new NodeButton();
        nodeButton->setIcon(QIcon(nodeIcon));
        nodeButton->setIconSize(QSize(64,64));
        nodeButton->setFlat(true);
        nodeButton->setCheckable(true);
        nodeButton->setToolTip(nodeToolTip);
        m_nodeButtons.push_back(nodeButton);

        QObject::connect(   nodeButton, SIGNAL(nodeButtonClicked(QAbstractButton*)),
                            this, SLOT(nodeButtonChecked(QAbstractButton*)));

        QObject::connect(   nodeButton, SIGNAL(clicked(bool)),
                            this, SLOT(nodeButtonClicked(bool)));

        verticalLayout->addWidget(nodeButton);
        nodeParser.goUp(2);
    }
    while (nodeParser.next());
}

void MainWindow::nodeButtonChecked(QAbstractButton* button)
{
    QString selectedNode = "";
//	QPixmap selectedNodeIcon;
    for (int i=0; i< m_nodeButtons.size(); ++i)
    {
        if ( m_nodeButtons[i] != button)
            m_nodeButtons[i]->setChecked(false);
        else
        {
            selectedNode = m_nodeNames[i];
//            selectedNodeIcon = m_nodeButtons[i]->icon().pixmap(64);
        }
    }

//	actionConnectMode->setChecked(false);
//	actionSelectMode->setChecked(true);
    if (mdiArea->activeSubWindow())
    {
        m_activeProject = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() )->getProjectArea();
        if (m_activeProject)
        {
            m_activeProject->setMode(DropArea::NODE_INSERT);
            m_activeProject->setSelectedNode(selectedNode);
//            m_activeProject->setSelectedNodeIcon(selectedNodeIcon);
        }
    }
}

void MainWindow::nodeButtonClicked(bool checked)
{
    if (!checked)
    {
//        actionConnectMode->setChecked(false);
//        actionSelectMode->setChecked(true);
        if (mdiArea->activeSubWindow())
        {
            m_activeProject = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() )->getProjectArea();
            if (m_activeProject)
            {
                m_activeProject->setMode(DropArea::NODE_MOVE);
                m_activeProject->setSelectedNode("");
            }
        }
    }
}

void MainWindow::nodeInsertDone()
{
    for (int i=0; i< m_nodeButtons.size(); ++i)
        m_nodeButtons[i]->setChecked(false);
}

void MainWindow::linkCreateDone()
{
//	actionConnectMode->setChecked(false);
//    actionSelectMode->setChecked(true);
    if (mdiArea->activeSubWindow())
    {
        m_activeProject = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() )->getProjectArea();
        if (m_activeProject)
            m_activeProject->setMode(DropArea::NODE_MOVE);
    }
}

void MainWindow::runNetwork(bool run)
{
	IsRunning = run;

    actionDeleteSelection->setEnabled(!run);
    action_New->setEnabled(!run);
    action_Open->setEnabled(!run);
    action_Save->setEnabled(!run);
    actionCl_ose->setEnabled(!run);
    actionClose_All->setEnabled(!run);
    action_Next->setEnabled(!run);
    action_Previous->setEnabled(!run);
    actionWith_Nodes_States->setEnabled(!run);
    actionWithout_Nodes_States->setEnabled(!run);
    NodeBoxTabs->setEnabled(!run);
    menubar->setEnabled(!run);

    if (mdiArea->activeSubWindow())
    {
        m_activeProject = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() )->getProjectArea();
        if (m_activeProject)
        {
            if (run)
            {
                m_activeProject->setMode(DropArea::VPN_RUN);
                m_activeProject->runNetwork();
            }
            else
            {
                nodeInsertDone();
                m_activeProject->setMode(DropArea::NODE_MOVE);
                m_activeProject->stopNetwork();
            }
        }
    }
}

void MainWindow::forceRunNetwork(bool run)
{
    if (mdiArea->activeSubWindow())
    {
        m_activeProject = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() )->getProjectArea();
        if (m_activeProject)
        {
            m_activeProject->setMode(DropArea::VPN_RUN);
            m_activeProject->forceRunNetwork();
            m_activeProject->setMode(DropArea::NODE_MOVE);
        }
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::showAbout()
{
    QDialog *Form = new QDialog(this);
    QVBoxLayout *verticalLayout_2;

    XMLParser parser(":MainWindow/About.xml");

    if (Form->objectName().isEmpty())
        Form->setObjectName(QString::fromUtf8("Form"));

    QLabel *frame = new QLabel(Form);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    QImage logo(":Images/About.png");
    frame->setPixmap(QPixmap::fromImage(logo));

    verticalLayout_2 = new QVBoxLayout(Form);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    verticalLayout_2->addWidget(frame);

    QLabel *label = new QLabel(Form);
    label->setObjectName(QString::fromUtf8("label"));
    QLabel *version = new QLabel(Form);
    version->setObjectName(QString::fromUtf8("version"));
    label->setAlignment(Qt::AlignCenter);
    version->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    version->setWordWrap(true);

    QLabel *qtversion = new QLabel(Form);
    qtversion->setObjectName(QString::fromUtf8("qtversion"));
    qtversion->setAlignment(Qt::AlignCenter);

    QLabel *revision = new QLabel(Form);
    revision->setObjectName(QString::fromUtf8("revision"));
    revision->setAlignment(Qt::AlignCenter);

    QString qtversionstring(qVersion());
    qtversionstring = "Built with Qt " + qtversionstring;
    qtversion->setText(qtversionstring);

    QLabel *build = new QLabel(Form);
    build->setObjectName(QString::fromUtf8("build"));
    build->setAlignment(Qt::AlignCenter);

    QLabel *website = new QLabel(Form);
    website->setObjectName(QString::fromUtf8("website"));
    website->setAlignment(Qt::AlignCenter);

    QString authorString("EVAN Toolbox Project Development Team:\n");
    int i=0;
    while (parser.gotoElement("Author"))
    {
        if (i!=0)
            authorString.append("\n");
        authorString.append(parser.getAttribute("name"));
        i++;
    }

    parser.gotoElement("Version");
    QString versionString;
    versionString.append("Version: ");
    versionString.append(parser.getAttribute("ni"));

    Form->setWindowTitle("About Evan Toolbox");
    label->setText(authorString);
#ifdef WIN32
    version->setText(versionString + " for Windows");
#elif defined(__APPLE__)
    version->setText(versionString + " for Mac OS X");
#else
    version->setText(versionString + " for Linux");
#endif

	QString dateString;
	dateString.append( parser.getAttribute("month") );
	dateString.append( "-" );
	dateString.append( parser.getAttribute("day") );
	dateString.append( "-" );
	dateString.append( parser.getAttribute("year") );

    QString buildString("Build Date: ");
    parser.gotoElement("Website");
    website->setText(parser.getAttribute("path"));

    //parser.gotoElement("SVN");
    //QString revisionString = parser.getAttribute("revision");
    //QString dateString = parser.getAttribute("date");

    //! parser.gotoElement("GIT");
    //! QString revisionString = parser.getAttribute("revision");
    //! QString dateString = parser.getAttribute("date");

    //buildString.append(dateString.left(30));
	buildString.append( dateString.left( dateString.length() ) );

    //revisionString = "Revision: " + revisionString;
    //revisionString = "git revision: " + revisionString;
    //revision->setText(revisionString);

    build->setText(buildString);

    verticalLayout_2->addWidget(label);
    verticalLayout_2->addWidget(version);
    verticalLayout_2->addWidget(revision);
    verticalLayout_2->addWidget(build);
    verticalLayout_2->addWidget(qtversion);
    verticalLayout_2->addWidget(website);

    verticalLayout_2->setSizeConstraint( QLayout::SetFixedSize );
    Form->setLayout(verticalLayout_2);

    QRect screen = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
    QPoint center(screen.center().x()-logo.size().width()/2, screen.center().y()-logo.size().height());
    Form->move(center);
    Form->show();
}

void MainWindow::updateActions()
{
    if (mdiArea->activeSubWindow())
    {
        actionRunNetwork->setDisabled(false);
        actionForceRunNetwork->setDisabled(false);
        actionDeleteSelection->setDisabled(false);
        action_Save->setDisabled(false);
        m_zoomSpin->setDisabled(false);

        m_activeProject = dynamic_cast<VPNLayout *>( mdiArea->activeSubWindow() )->getProjectArea();
        if (m_activeProject)
        {
            if (m_activeProject->isRunning())
            {
                actionRunNetwork->setChecked(true);
            }
            else
            {
                actionRunNetwork->setChecked(false);
            }
            int zoom = m_activeProject->getZoom();
            m_zoomSpin->setValue(zoom);
        }
    }
    else
    {
        actionRunNetwork->setChecked(false);
        actionRunNetwork->setDisabled(true);
        actionForceRunNetwork->setDisabled(true);
        actionDeleteSelection->setDisabled(true);
        action_Save->setDisabled(true);
        m_zoomSpin->setValue(100);
        m_zoomSpin->setDisabled(true);
    }
}

void MainWindow::showLicense()
{
    QDialog *licenseDialog = new QDialog();
    Ui::LicenseDialog *Form = new Ui::LicenseDialog();
    Form->setupUi(licenseDialog);
    licenseDialog->show();
}

void MainWindow::showRegistration()
{
#ifdef ENCRYPT_EVAN_LICENSE_
    if(IsRegistered == 0)
    {
        QDialog *registerDialog = new QDialog();

        Form.setupUi(registerDialog);
        registerDialog->show();
        QStringList qmaclist = GetMACaddresses();
        QString mactext;

        if(qmaclist.count() > 0)
        {
            mactext = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                      "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                      "p, li { white-space: pre-wrap; }\n"
                      "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n";
                        for(int i=0;i<qmaclist.size();i++)
                        {
                            mactext.append("<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:12pt; font-weight:600;\">");
                            mactext.append(qmaclist[i]);
                            mactext.append("</span></p>");
                        }
                        mactext.append("</body></html>");
        }
        else
        {
            mactext = "No hardware address found. Cannot register software.\n";
        }
        Form.textEdit->setHtml(QApplication::translate("RegisterDialog", mactext.toStdString().c_str(), 0, QApplication::UnicodeUTF8));

        QObject::connect(Form.textEdit_2, SIGNAL(textChanged ()), this, SLOT(enterLicenseKey( )));
        QObject::connect(Form.buttonBox, SIGNAL(accepted()), this, SLOT(validateLicenseKey()));
    }
    else
    {
        QMessageBox::information(0, "Evan Toolbox", QString("This is a Registered Copy"));
    }
#endif
}

void MainWindow::enterLicenseKey()
{
    m_licenseText = Form.textEdit_2->toPlainText();
}

bool MainWindow::validateLicenseKey()
{
#ifdef ENCRYPT_EVAN_LICENSE_
    if(IsRegistered == 1)
    {
        Logger::getInstance()->log("This copy is already registered.");
        return true;
    }
    QStringList keys = m_licenseText.split("\n");
    QString timekey;

    bool isTrialLicense;
    QStringList validKeys;
    IsRegistered = parseLicense( keys, validKeys, timekey, isTrialLicense );

    if(IsRegistered == 1)
    {
#ifdef USE_UNIX_LOCATIONS
        ofstream licenseFile(licensepath.toUtf8().data());
#else
        ofstream licenseFile("license.dat");
#endif
        for (int i=0;i<validKeys.size();i++)
        {
            licenseFile << validKeys[i].toStdString() << endl;
        }
        licenseFile << timekey.toStdString() << endl;

        if ( isTrialLicense ) {
            licenseFile << "trial" << endl;
        }

        Logger::getInstance()->log("Valid License Key entered. This copy is now registered.");

        return true;
    }
    if(m_licenseText != "")
    {
        Logger::getInstance()->log("Invalid License Key entered.", Logger::RUN_ERROR);
    }
    return false;
#else
    return true;
#endif
}

void MainWindow::setDeleteActionDisabled(bool on)
{
    actionDeleteSelection->setDisabled(on);
}
