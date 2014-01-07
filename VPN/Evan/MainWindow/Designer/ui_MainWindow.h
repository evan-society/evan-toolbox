/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionDeleteSelection;
    QAction *actionRunNetwork;
    QAction *action_New;
    QAction *action_Open;
    QAction *action_Save;
    QAction *actionE_xit;
    QAction *actionCl_ose;
    QAction *actionClose_All;
    QAction *action_Tile;
    QAction *action_Cascade;
    QAction *action_Next;
    QAction *action_Previous;
    QAction *actionLog;
    QAction *emptyAction;
    QAction *actionLicense;
    QAction *actionAbout;
    QAction *actionRegister;
    QAction *actionWithout_Nodes_States;
    QAction *actionNew_vpn;
    QAction *actionWith_Nodes_States;
    QAction *actionForceRunNetwork;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *NodeBoxTabs;
    QWidget *dataTab;
    QWidget *utilTab;
    QWidget *gmmTab;
    QWidget *viewTab;
    QVBoxLayout *verticalLayout;
    QMdiArea *mdiArea;
    QTextEdit *m_logTextEdit;
    QStatusBar *mainStatusBar;
    QToolBar *toolBar;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *recentMenu;
    QMenu *menuSave_As;
    QMenu *menu_View;
    QMenu *menu_Window;
    QMenu *menuAbout;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 630);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/Evan.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        actionDeleteSelection = new QAction(MainWindow);
        actionDeleteSelection->setObjectName(QString::fromUtf8("actionDeleteSelection"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDeleteSelection->setIcon(icon1);
        actionDeleteSelection->setShortcutContext(Qt::ApplicationShortcut);
        actionRunNetwork = new QAction(MainWindow);
        actionRunNetwork->setObjectName(QString::fromUtf8("actionRunNetwork"));
        actionRunNetwork->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/play.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8(":/Images/pause.png"), QSize(), QIcon::Normal, QIcon::On);
        actionRunNetwork->setIcon(icon2);
        action_New = new QAction(MainWindow);
        action_New->setObjectName(QString::fromUtf8("action_New"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/I_New.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_New->setIcon(icon3);
        action_Open = new QAction(MainWindow);
        action_Open->setObjectName(QString::fromUtf8("action_Open"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/I_Open.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Open->setIcon(icon4);
        action_Save = new QAction(MainWindow);
        action_Save->setObjectName(QString::fromUtf8("action_Save"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/I_Save.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Save->setIcon(icon5);
        actionE_xit = new QAction(MainWindow);
        actionE_xit->setObjectName(QString::fromUtf8("actionE_xit"));
        actionCl_ose = new QAction(MainWindow);
        actionCl_ose->setObjectName(QString::fromUtf8("actionCl_ose"));
        actionClose_All = new QAction(MainWindow);
        actionClose_All->setObjectName(QString::fromUtf8("actionClose_All"));
        action_Tile = new QAction(MainWindow);
        action_Tile->setObjectName(QString::fromUtf8("action_Tile"));
        action_Cascade = new QAction(MainWindow);
        action_Cascade->setObjectName(QString::fromUtf8("action_Cascade"));
        action_Next = new QAction(MainWindow);
        action_Next->setObjectName(QString::fromUtf8("action_Next"));
        action_Previous = new QAction(MainWindow);
        action_Previous->setObjectName(QString::fromUtf8("action_Previous"));
        actionLog = new QAction(MainWindow);
        actionLog->setObjectName(QString::fromUtf8("actionLog"));
        actionLog->setCheckable(true);
        actionLog->setChecked(true);
        emptyAction = new QAction(MainWindow);
        emptyAction->setObjectName(QString::fromUtf8("emptyAction"));
        emptyAction->setEnabled(false);
        actionLicense = new QAction(MainWindow);
        actionLicense->setObjectName(QString::fromUtf8("actionLicense"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionRegister = new QAction(MainWindow);
        actionRegister->setObjectName(QString::fromUtf8("actionRegister"));
        actionWithout_Nodes_States = new QAction(MainWindow);
        actionWithout_Nodes_States->setObjectName(QString::fromUtf8("actionWithout_Nodes_States"));
        actionNew_vpn = new QAction(MainWindow);
        actionNew_vpn->setObjectName(QString::fromUtf8("actionNew_vpn"));
        actionWith_Nodes_States = new QAction(MainWindow);
        actionWith_Nodes_States->setObjectName(QString::fromUtf8("actionWith_Nodes_States"));
        actionForceRunNetwork = new QAction(MainWindow);
        actionForceRunNetwork->setObjectName(QString::fromUtf8("actionForceRunNetwork"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionForceRunNetwork->setIcon(icon6);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        NodeBoxTabs = new QTabWidget(centralwidget);
        NodeBoxTabs->setObjectName(QString::fromUtf8("NodeBoxTabs"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(NodeBoxTabs->sizePolicy().hasHeightForWidth());
        NodeBoxTabs->setSizePolicy(sizePolicy);
        NodeBoxTabs->setTabPosition(QTabWidget::West);
        NodeBoxTabs->setTabShape(QTabWidget::Triangular);
        dataTab = new QWidget();
        dataTab->setObjectName(QString::fromUtf8("dataTab"));
        NodeBoxTabs->addTab(dataTab, QString());
        utilTab = new QWidget();
        utilTab->setObjectName(QString::fromUtf8("utilTab"));
        NodeBoxTabs->addTab(utilTab, QString());
        gmmTab = new QWidget();
        gmmTab->setObjectName(QString::fromUtf8("gmmTab"));
        NodeBoxTabs->addTab(gmmTab, QString());
        viewTab = new QWidget();
        viewTab->setObjectName(QString::fromUtf8("viewTab"));
        NodeBoxTabs->addTab(viewTab, QString());

        horizontalLayout->addWidget(NodeBoxTabs);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        mdiArea = new QMdiArea(centralwidget);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mdiArea->sizePolicy().hasHeightForWidth());
        mdiArea->setSizePolicy(sizePolicy1);
        mdiArea->setViewMode(QMdiArea::TabbedView);
        mdiArea->setTabShape(QTabWidget::Triangular);

        verticalLayout->addWidget(mdiArea);

        m_logTextEdit = new QTextEdit(centralwidget);
        m_logTextEdit->setObjectName(QString::fromUtf8("m_logTextEdit"));
        m_logTextEdit->setReadOnly(true);

        verticalLayout->addWidget(m_logTextEdit);


        horizontalLayout->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralwidget);
        mainStatusBar = new QStatusBar(MainWindow);
        mainStatusBar->setObjectName(QString::fromUtf8("mainStatusBar"));
        MainWindow->setStatusBar(mainStatusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        recentMenu = new QMenu(menu_File);
        recentMenu->setObjectName(QString::fromUtf8("recentMenu"));
        menuSave_As = new QMenu(menu_File);
        menuSave_As->setObjectName(QString::fromUtf8("menuSave_As"));
        menu_View = new QMenu(menubar);
        menu_View->setObjectName(QString::fromUtf8("menu_View"));
        menu_Window = new QMenu(menubar);
        menu_Window->setObjectName(QString::fromUtf8("menu_Window"));
        menuAbout = new QMenu(menubar);
        menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
        MainWindow->setMenuBar(menubar);

        toolBar->addAction(action_New);
        toolBar->addAction(action_Open);
        toolBar->addAction(action_Save);
        toolBar->addSeparator();
        toolBar->addAction(actionDeleteSelection);
        toolBar->addAction(actionRunNetwork);
        toolBar->addAction(actionForceRunNetwork);
        toolBar->addSeparator();
        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_View->menuAction());
        menubar->addAction(menu_Window->menuAction());
        menubar->addAction(menuAbout->menuAction());
        menu_File->addAction(action_New);
        menu_File->addAction(action_Open);
        menu_File->addAction(action_Save);
        menu_File->addAction(menuSave_As->menuAction());
        menu_File->addSeparator();
        menu_File->addAction(recentMenu->menuAction());
        menu_File->addSeparator();
        menu_File->addSeparator();
        menu_File->addAction(actionE_xit);
        recentMenu->addAction(emptyAction);
        menuSave_As->addAction(actionWith_Nodes_States);
        menuSave_As->addAction(actionWithout_Nodes_States);
        menu_View->addAction(actionLog);
        menu_Window->addAction(actionCl_ose);
        menu_Window->addAction(actionClose_All);
        menu_Window->addSeparator();
        menu_Window->addAction(action_Tile);
        menu_Window->addAction(action_Cascade);
        menu_Window->addSeparator();
        menu_Window->addAction(action_Next);
        menu_Window->addAction(action_Previous);
        menuAbout->addAction(actionLicense);
        menuAbout->addAction(actionRegister);
        menuAbout->addAction(actionAbout);

        retranslateUi(MainWindow);
        QObject::connect(actionE_xit, SIGNAL(triggered()), MainWindow, SLOT(close()));
        QObject::connect(actionCl_ose, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));
        QObject::connect(actionClose_All, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));
        QObject::connect(action_Tile, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));
        QObject::connect(action_Cascade, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));
        QObject::connect(action_Next, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));
        QObject::connect(action_Previous, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));
        QObject::connect(actionLog, SIGNAL(toggled(bool)), m_logTextEdit, SLOT(setVisible(bool)));

        NodeBoxTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "EVAN Toolbox", 0, QApplication::UnicodeUTF8));
        actionDeleteSelection->setText(QApplication::translate("MainWindow", "DeleteSelection", 0, QApplication::UnicodeUTF8));
        actionDeleteSelection->setIconText(QApplication::translate("MainWindow", "Delete Selection", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionDeleteSelection->setToolTip(QApplication::translate("MainWindow", "Delete Selection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionDeleteSelection->setShortcut(QApplication::translate("MainWindow", "Del", 0, QApplication::UnicodeUTF8));
        actionRunNetwork->setText(QApplication::translate("MainWindow", "runNetwork", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionRunNetwork->setToolTip(QApplication::translate("MainWindow", "Start the VPN Network", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionRunNetwork->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", 0, QApplication::UnicodeUTF8));
        action_New->setText(QApplication::translate("MainWindow", "&New", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_New->setToolTip(QApplication::translate("MainWindow", "Create New Project File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_New->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0, QApplication::UnicodeUTF8));
        action_Open->setText(QApplication::translate("MainWindow", "&Open", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Open->setToolTip(QApplication::translate("MainWindow", "Open an Existing Project File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Open->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        action_Save->setText(QApplication::translate("MainWindow", "&Save", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Save->setToolTip(QApplication::translate("MainWindow", "Save this Project File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Save->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionE_xit->setText(QApplication::translate("MainWindow", "E&xit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionE_xit->setToolTip(QApplication::translate("MainWindow", "Exit the Application", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionE_xit->setShortcut(QApplication::translate("MainWindow", "Ctrl+X", 0, QApplication::UnicodeUTF8));
        actionCl_ose->setText(QApplication::translate("MainWindow", "Cl&ose", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCl_ose->setToolTip(QApplication::translate("MainWindow", "Close Active Window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCl_ose->setShortcut(QApplication::translate("MainWindow", "Ctrl+F4", 0, QApplication::UnicodeUTF8));
        actionClose_All->setText(QApplication::translate("MainWindow", "Close &All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionClose_All->setToolTip(QApplication::translate("MainWindow", "Close All Windows", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionClose_All->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+F4", 0, QApplication::UnicodeUTF8));
        action_Tile->setText(QApplication::translate("MainWindow", "&Tile", 0, QApplication::UnicodeUTF8));
        action_Cascade->setText(QApplication::translate("MainWindow", "&Cascade", 0, QApplication::UnicodeUTF8));
        action_Next->setText(QApplication::translate("MainWindow", "&Next", 0, QApplication::UnicodeUTF8));
        action_Previous->setText(QApplication::translate("MainWindow", "&Previous", 0, QApplication::UnicodeUTF8));
        actionLog->setText(QApplication::translate("MainWindow", "Log", 0, QApplication::UnicodeUTF8));
        actionLog->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+L", 0, QApplication::UnicodeUTF8));
        emptyAction->setText(QApplication::translate("MainWindow", "Empty", 0, QApplication::UnicodeUTF8));
        actionLicense->setText(QApplication::translate("MainWindow", "License", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        actionRegister->setText(QApplication::translate("MainWindow", "Register", 0, QApplication::UnicodeUTF8));
        actionWithout_Nodes_States->setText(QApplication::translate("MainWindow", "Without Nodes States", 0, QApplication::UnicodeUTF8));
        actionNew_vpn->setText(QApplication::translate("MainWindow", "New vpn", 0, QApplication::UnicodeUTF8));
        actionWith_Nodes_States->setText(QApplication::translate("MainWindow", "With Nodes States", 0, QApplication::UnicodeUTF8));
        actionForceRunNetwork->setText(QApplication::translate("MainWindow", "forceRunNetwork", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionForceRunNetwork->setToolTip(QApplication::translate("MainWindow", "Reset All Nodes", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        NodeBoxTabs->setTabText(NodeBoxTabs->indexOf(dataTab), QApplication::translate("MainWindow", "Data", 0, QApplication::UnicodeUTF8));
        NodeBoxTabs->setTabText(NodeBoxTabs->indexOf(utilTab), QApplication::translate("MainWindow", "Utilities", 0, QApplication::UnicodeUTF8));
        NodeBoxTabs->setTabText(NodeBoxTabs->indexOf(gmmTab), QApplication::translate("MainWindow", "GMM", 0, QApplication::UnicodeUTF8));
        NodeBoxTabs->setTabText(NodeBoxTabs->indexOf(viewTab), QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));
        m_logTextEdit->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        recentMenu->setTitle(QApplication::translate("MainWindow", "Open Recent ", 0, QApplication::UnicodeUTF8));
        menuSave_As->setTitle(QApplication::translate("MainWindow", "Save &As...", 0, QApplication::UnicodeUTF8));
        menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
        menu_Window->setTitle(QApplication::translate("MainWindow", "&Window", 0, QApplication::UnicodeUTF8));
        menuAbout->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
