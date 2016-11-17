/********************************************************************************
** Form generated from reading ui file 'TemplandWindow.ui'
**
** Created: Mon 15. Mar 11:23:24 2010
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEMPLANDWINDOW_H
#define UI_TEMPLANDWINDOW_H

#include <QDesktopWidget>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TemplandWindow
{
public:
    QAction *actionShow_template;
    QAction *action_Open_File;
    QAction *actionOpen_Filmstrip;
    QAction *action_Save;
    QAction *actionSave_As;
    QAction *actionE_xit;
    QAction *actionFree_Movement;
    QAction *actionHRestrict;
    QAction *actionVRestrict;
    QAction *actionTwist;
    QAction *actionTem_plate_View;
    QAction *actionTemplate_Sli_ce;
    QAction *action_Target_View;
    QAction *actionTarget_Slice;
    QAction *actionBack_ground;
    QAction *actionDelete_Tableau;
    QAction *actionAdd_Tableau;
    QAction *action_Replace_Tableau;
    QAction *actionMove_Tableau_Up;
    QAction *actionMove_Tableau_Down;
    QAction *action_New;
    QAction *actionClose;
    QAction *actionClose_All;
    QAction *actionTile;
    QAction *actionCascade;
    QAction *actionNext;
    QAction *actionPrevious;
    QAction *actionOpen_Filmstrip_2;
    QAction *actionCompute_Spline;
    QAction *actionMap_all_landmarks;
    QAction *actionMap_single_Landmark;
    //QAction *actionLandmark;
    //QAction *actionNon_Landmark;
    //QAction *actionSemi_Landmark_2;
    //QAction *actionSelect_points;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QMdiArea *mdiArea;
    QMenuBar *menubar;
    QMenu *menuF_ile;
    QMenu *menu_View;
    QMenu *menuWindow;
    //QMenu *menu_Landmark;
    //QMenu *menuPoint_type;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *TemplandWindow)
    {
    if (TemplandWindow->objectName().isEmpty())
        TemplandWindow->setObjectName(QString::fromUtf8("TemplandWindow"));
    TemplandWindow->resize(778, 710);
    actionShow_template = new QAction(TemplandWindow);
    actionShow_template->setObjectName(QString::fromUtf8("actionShow_template"));
    actionShow_template->setCheckable(false);
    actionShow_template->setChecked(false);
    action_Open_File = new QAction(TemplandWindow);
    action_Open_File->setObjectName(QString::fromUtf8("action_Open_File"));
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/Images/I_Open.png")), QIcon::Normal, QIcon::Off);
    action_Open_File->setIcon(icon);
    actionOpen_Filmstrip = new QAction(TemplandWindow);
    actionOpen_Filmstrip->setObjectName(QString::fromUtf8("actionOpen_Filmstrip"));
    QIcon icon1;
    icon1.addPixmap(QPixmap(QString::fromUtf8(":/Images/I_OpenFilm.png")), QIcon::Normal, QIcon::Off);
    actionOpen_Filmstrip->setIcon(icon1);
    action_Save = new QAction(TemplandWindow);
    action_Save->setObjectName(QString::fromUtf8("action_Save"));
    QIcon icon2;
    icon2.addPixmap(QPixmap(QString::fromUtf8(":/Images/I_Save.png")), QIcon::Normal, QIcon::Off);
    action_Save->setIcon(icon2);
    actionSave_As = new QAction(TemplandWindow);
    actionSave_As->setObjectName(QString::fromUtf8("actionSave_As"));
    actionE_xit = new QAction(TemplandWindow);
    actionE_xit->setObjectName(QString::fromUtf8("actionE_xit"));
    actionFree_Movement = new QAction(TemplandWindow);
    actionFree_Movement->setObjectName(QString::fromUtf8("actionFree_Movement"));
    actionFree_Movement->setCheckable(true);
    actionFree_Movement->setChecked(true);
    QIcon icon3;
    icon3.addPixmap(QPixmap(QString::fromUtf8(":/Images/FreeMove.png")), QIcon::Normal, QIcon::Off);
    actionFree_Movement->setIcon(icon3);
    actionHRestrict = new QAction(TemplandWindow);
    actionHRestrict->setObjectName(QString::fromUtf8("actionHRestrict"));
    actionHRestrict->setCheckable(true);
    QIcon icon4;
    icon4.addPixmap(QPixmap(QString::fromUtf8(":/Images/XRestrict.png")), QIcon::Normal, QIcon::Off);
    actionHRestrict->setIcon(icon4);
    actionVRestrict = new QAction(TemplandWindow);
    actionVRestrict->setObjectName(QString::fromUtf8("actionVRestrict"));
    actionVRestrict->setCheckable(true);
    QIcon icon5;
    icon5.addPixmap(QPixmap(QString::fromUtf8(":/Images/YRestrict.png")), QIcon::Normal, QIcon::Off);
    actionVRestrict->setIcon(icon5);
    actionTwist = new QAction(TemplandWindow);
    actionTwist->setObjectName(QString::fromUtf8("actionTwist"));
    actionTwist->setCheckable(true);
    QIcon icon6;
    icon6.addPixmap(QPixmap(QString::fromUtf8(":/Images/ZRestrict.png")), QIcon::Normal, QIcon::Off);
    actionTwist->setIcon(icon6);
    actionTem_plate_View = new QAction(TemplandWindow);
    actionTem_plate_View->setObjectName(QString::fromUtf8("actionTem_plate_View"));
    actionTemplate_Sli_ce = new QAction(TemplandWindow);
    actionTemplate_Sli_ce->setObjectName(QString::fromUtf8("actionTemplate_Sli_ce"));
    action_Target_View = new QAction(TemplandWindow);
    action_Target_View->setObjectName(QString::fromUtf8("action_Target_View"));
    actionTarget_Slice = new QAction(TemplandWindow);
    actionTarget_Slice->setObjectName(QString::fromUtf8("actionTarget_Slice"));
    actionBack_ground = new QAction(TemplandWindow);
    actionBack_ground->setObjectName(QString::fromUtf8("actionBack_ground"));
    actionDelete_Tableau = new QAction(TemplandWindow);
    actionDelete_Tableau->setObjectName(QString::fromUtf8("actionDelete_Tableau"));
    QIcon icon7;
    icon7.addPixmap(QPixmap(QString::fromUtf8(":/Images/delete.png")), QIcon::Normal, QIcon::Off);
    actionDelete_Tableau->setIcon(icon7);
    actionAdd_Tableau = new QAction(TemplandWindow);
    actionAdd_Tableau->setObjectName(QString::fromUtf8("actionAdd_Tableau"));
    actionAdd_Tableau->setIcon(icon2);
    action_Replace_Tableau = new QAction(TemplandWindow);
    action_Replace_Tableau->setObjectName(QString::fromUtf8("action_Replace_Tableau"));
    action_Replace_Tableau->setIcon(icon2);
    actionMove_Tableau_Up = new QAction(TemplandWindow);
    actionMove_Tableau_Up->setObjectName(QString::fromUtf8("actionMove_Tableau_Up"));
    actionMove_Tableau_Down = new QAction(TemplandWindow);
    actionMove_Tableau_Down->setObjectName(QString::fromUtf8("actionMove_Tableau_Down"));
    action_New = new QAction(TemplandWindow);
    action_New->setObjectName(QString::fromUtf8("action_New"));
    QIcon icon8;
    icon8.addPixmap(QPixmap(QString::fromUtf8(":/Images/I_New.png")), QIcon::Normal, QIcon::Off);
    action_New->setIcon(icon8);
    actionClose = new QAction(TemplandWindow);
    actionClose->setObjectName(QString::fromUtf8("actionClose"));
    actionClose_All = new QAction(TemplandWindow);
    actionClose_All->setObjectName(QString::fromUtf8("actionClose_All"));
    actionTile = new QAction(TemplandWindow);
    actionTile->setObjectName(QString::fromUtf8("actionTile"));
    actionCascade = new QAction(TemplandWindow);
    actionCascade->setObjectName(QString::fromUtf8("actionCascade"));
    actionNext = new QAction(TemplandWindow);
    actionNext->setObjectName(QString::fromUtf8("actionNext"));
    actionPrevious = new QAction(TemplandWindow);
    actionPrevious->setObjectName(QString::fromUtf8("actionPrevious"));
    actionOpen_Filmstrip_2 = new QAction(TemplandWindow);
    actionOpen_Filmstrip_2->setObjectName(QString::fromUtf8("actionOpen_Filmstrip_2"));
    actionCompute_Spline = new QAction(TemplandWindow);
    actionCompute_Spline->setObjectName(QString::fromUtf8("actionCompute_Spline"));
    actionMap_all_landmarks = new QAction(TemplandWindow);
    actionMap_all_landmarks->setObjectName(QString::fromUtf8("actionMap_all_landmarks"));
    actionMap_single_Landmark = new QAction(TemplandWindow);
    actionMap_single_Landmark->setObjectName(QString::fromUtf8("actionMap_single_Landmark"));
    //actionLandmark = new QAction(TemplandWindow);
    //actionLandmark->setObjectName(QString::fromUtf8("actionLandmark"));
    //actionLandmark->setCheckable(true);
    //actionLandmark->setChecked(true);
    //actionNon_Landmark = new QAction(TemplandWindow);
    //actionNon_Landmark->setObjectName(QString::fromUtf8("actionNon_Landmark"));
    //actionNon_Landmark->setCheckable(true);
    //actionNon_Landmark->setVisible(false);
    //actionSemi_Landmark_2 = new QAction(TemplandWindow);
    //actionSemi_Landmark_2->setObjectName(QString::fromUtf8("actionSemi_Landmark_2"));
    //actionSemi_Landmark_2->setCheckable(true);
    //actionSelect_points = new QAction(TemplandWindow);
    //actionSelect_points->setObjectName(QString::fromUtf8("actionSelect_points"));
    //actionSelect_points->setCheckable(true);
    //actionSelect_points->setChecked(true);

	QAction *actionRefresh = new QAction(TemplandWindow);
    actionRefresh->setObjectName(QString::fromUtf8("actionRefresh"));
    actionRefresh->setText(QApplication::translate("TemplandWindow", "Refresh Output Ports", 0, QApplication::UnicodeUTF8));

	centralwidget = new QWidget(TemplandWindow);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    verticalLayout_3 = new QVBoxLayout(centralwidget);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    mdiArea = new QMdiArea(centralwidget);
    mdiArea->setObjectName(QString::fromUtf8("mdiArea"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(mdiArea->sizePolicy().hasHeightForWidth());
    mdiArea->setSizePolicy(sizePolicy);
    mdiArea->setViewMode(QMdiArea::TabbedView);
    mdiArea->setTabShape(QTabWidget::Triangular);

    verticalLayout_3->addWidget(mdiArea);

    TemplandWindow->setCentralWidget(centralwidget);
    menubar = new QMenuBar(TemplandWindow);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 778, 19));
    menuF_ile = new QMenu(menubar);
    menuF_ile->setObjectName(QString::fromUtf8("menuF_ile"));
    menu_View = new QMenu(menubar);
    menu_View->setObjectName(QString::fromUtf8("menu_View"));
    menuWindow = new QMenu(menubar);
    menuWindow->setObjectName(QString::fromUtf8("menuWindow"));
    //menu_Landmark = new QMenu(menubar);
    //menu_Landmark->setObjectName(QString::fromUtf8("menu_Landmark"));
    //menuPoint_type = new QMenu(menu_Landmark);
    //menuPoint_type->setObjectName(QString::fromUtf8("menuPoint_type"));
    TemplandWindow->setMenuBar(menubar);
    statusbar = new QStatusBar(TemplandWindow);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    TemplandWindow->setStatusBar(statusbar);
    toolBar = new QToolBar(TemplandWindow);
    toolBar->setObjectName(QString::fromUtf8("toolBar"));
    TemplandWindow->addToolBar(Qt::TopToolBarArea, toolBar);

    menubar->addAction(menuF_ile->menuAction());
    menubar->addAction(menu_View->menuAction());
    //menubar->addAction(menu_Landmark->menuAction());
    menubar->addAction(menuWindow->menuAction());
    menuF_ile->addAction(action_New);
    menuF_ile->addAction(action_Open_File);
    menuF_ile->addAction(action_Save);
    menuF_ile->addAction(actionSave_As);
    menuF_ile->addSeparator();
    menuF_ile->addAction(actionRefresh);
    menuF_ile->addSeparator();
    menuF_ile->addAction(actionE_xit);
    menu_View->addAction(actionShow_template);
    menu_View->addAction(actionBack_ground);
    menuWindow->addAction(actionClose);
    menuWindow->addAction(actionClose_All);
    menuWindow->addSeparator();
    menuWindow->addAction(actionTile);
    menuWindow->addAction(actionCascade);
    menuWindow->addSeparator();
    menuWindow->addAction(actionNext);
    menuWindow->addAction(actionPrevious);
    //menu_Landmark->addAction(menuPoint_type->menuAction());
    //menu_Landmark->addAction(actionSelect_points);
    //menuPoint_type->addAction(actionLandmark);
    //menuPoint_type->addAction(actionSemi_Landmark_2);
    //menuPoint_type->addAction(actionNon_Landmark);
    toolBar->addAction(actionFree_Movement);
    toolBar->addAction(actionHRestrict);
    toolBar->addAction(actionVRestrict);
    toolBar->addAction(actionTwist);



    QDesktopWidget* qdw = QApplication::desktop();
    QRect rect = qdw->screenGeometry( qdw->primaryScreen() );
    int screenWidth = rect.width();
    screenWidth -= 191;//m_targetItemTree->width(); //  tree view
    screenWidth -= 30; //  borders
    TemplandWindow->resize(rect.width()-10/*778*/, (int)(screenWidth * 0.5) + 140/*710*/);
    TemplandWindow->move( 0, 0 );

    retranslateUi(TemplandWindow);
    QObject::connect(actionE_xit, SIGNAL(triggered()), TemplandWindow, SLOT(close()));
    QObject::connect(actionClose, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));
    QObject::connect(actionClose_All, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));
    QObject::connect(actionTile, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));
    QObject::connect(actionCascade, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));
    QObject::connect(actionNext, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));
    QObject::connect(actionPrevious, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

//    QObject::connect(actionLandmark, SIGNAL(triggered()), TemplandWindow, SLOT(typeLandmark()));
//    QObject::connect(actionSemi_Landmark_2, SIGNAL(triggered()), TemplandWindow, SLOT(typeSemiLandmark()));
//    QObject::connect(actionNon_Landmark, SIGNAL(triggered()), TemplandWindow, SLOT(typeNonLandmark()));
//    QObject::connect(actionSelect_points, SIGNAL(toggled(bool)), TemplandWindow, SLOT(selectPoints(bool)));

    QObject::connect(actionBack_ground, SIGNAL(triggered()), TemplandWindow, SLOT(changeBckgndColor()));
    QObject::connect(action_New, SIGNAL(triggered()), TemplandWindow, SLOT(newTableau()));
    QObject::connect(action_Open_File, SIGNAL(triggered()), TemplandWindow, SLOT(openTableau()));
    QObject::connect(actionSave_As, SIGNAL(triggered()), TemplandWindow, SLOT(saveAsTableau()));
    QObject::connect(action_Save, SIGNAL(triggered()), TemplandWindow, SLOT(saveTableau()));
    QObject::connect(actionE_xit, SIGNAL(triggered()), TemplandWindow, SLOT(close()));
    QObject::connect(actionShow_template, SIGNAL(triggered()), TemplandWindow, SLOT(templateHide()));

    QObject::connect(actionRefresh, SIGNAL(triggered()), TemplandWindow, SLOT(refreshOutputs()));

    QMetaObject::connectSlotsByName(TemplandWindow);
    } // setupUi

    void retranslateUi(QMainWindow *TemplandWindow)
    {
    TemplandWindow->setWindowTitle(QApplication::translate("TemplandWindow", "Templand - Untitled.ltb", 0, QApplication::UnicodeUTF8));
    actionShow_template->setText(QApplication::translate("TemplandWindow", "Template", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    actionShow_template->setToolTip(QApplication::translate("TemplandWindow", "Show Template Windows", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    action_Open_File->setText(QApplication::translate("TemplandWindow", "&Open File", 0, QApplication::UnicodeUTF8));
    action_Open_File->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
    actionOpen_Filmstrip->setText(QApplication::translate("TemplandWindow", "Open &Filmstrip", 0, QApplication::UnicodeUTF8));
    actionOpen_Filmstrip->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+F", 0, QApplication::UnicodeUTF8));
    action_Save->setText(QApplication::translate("TemplandWindow", "&Save", 0, QApplication::UnicodeUTF8));
    action_Save->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
    actionSave_As->setText(QApplication::translate("TemplandWindow", "Save &As...", 0, QApplication::UnicodeUTF8));
    actionE_xit->setText(QApplication::translate("TemplandWindow", "E&xit", 0, QApplication::UnicodeUTF8));
    actionE_xit->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+X", 0, QApplication::UnicodeUTF8));
    actionFree_Movement->setText(QApplication::translate("TemplandWindow", "Free Movement", 0, QApplication::UnicodeUTF8));
    actionHRestrict->setText(QApplication::translate("TemplandWindow", "HRestrict", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    actionHRestrict->setToolTip(QApplication::translate("TemplandWindow", "Restrict Movement along the Horizontal Axis", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    actionVRestrict->setText(QApplication::translate("TemplandWindow", "VRestrict", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    actionVRestrict->setToolTip(QApplication::translate("TemplandWindow", "Restrict Movement along the Vertical Axis", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_STATUSTIP
    actionVRestrict->setStatusTip(QString());
#endif // QT_NO_STATUSTIP

    actionTwist->setText(QApplication::translate("TemplandWindow", "Twist around Z-axis", 0, QApplication::UnicodeUTF8));
    actionTem_plate_View->setText(QApplication::translate("TemplandWindow", "Tem&plate View", 0, QApplication::UnicodeUTF8));
    actionTemplate_Sli_ce->setText(QApplication::translate("TemplandWindow", "Template Sli&ce", 0, QApplication::UnicodeUTF8));
    action_Target_View->setText(QApplication::translate("TemplandWindow", "&Target View", 0, QApplication::UnicodeUTF8));
    actionTarget_Slice->setText(QApplication::translate("TemplandWindow", "Target Slice", 0, QApplication::UnicodeUTF8));
    actionBack_ground->setText(QApplication::translate("TemplandWindow", "Back&ground", 0, QApplication::UnicodeUTF8));
    actionDelete_Tableau->setText(QApplication::translate("TemplandWindow", "&Delete Current Tableau", 0, QApplication::UnicodeUTF8));
    actionDelete_Tableau->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+D", 0, QApplication::UnicodeUTF8));
    actionAdd_Tableau->setText(QApplication::translate("TemplandWindow", "&Add Tableau", 0, QApplication::UnicodeUTF8));
    actionAdd_Tableau->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+T", 0, QApplication::UnicodeUTF8));
    action_Replace_Tableau->setText(QApplication::translate("TemplandWindow", "&Replace Current Tableau", 0, QApplication::UnicodeUTF8));
    action_Replace_Tableau->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+Shift+T", 0, QApplication::UnicodeUTF8));
    actionMove_Tableau_Up->setText(QApplication::translate("TemplandWindow", "Move Tableau &Up", 0, QApplication::UnicodeUTF8));
    actionMove_Tableau_Up->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+U", 0, QApplication::UnicodeUTF8));
    actionMove_Tableau_Down->setText(QApplication::translate("TemplandWindow", "Move Tableau &Down", 0, QApplication::UnicodeUTF8));
    actionMove_Tableau_Down->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+Shift+U", 0, QApplication::UnicodeUTF8));
    action_New->setText(QApplication::translate("TemplandWindow", "&New", 0, QApplication::UnicodeUTF8));
    action_New->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+N", 0, QApplication::UnicodeUTF8));
    actionClose->setText(QApplication::translate("TemplandWindow", "Cl&ose", 0, QApplication::UnicodeUTF8));
    actionClose_All->setText(QApplication::translate("TemplandWindow", "Close &All", 0, QApplication::UnicodeUTF8));
    actionTile->setText(QApplication::translate("TemplandWindow", "&Tile", 0, QApplication::UnicodeUTF8));
    actionCascade->setText(QApplication::translate("TemplandWindow", "&Cascade", 0, QApplication::UnicodeUTF8));
    actionNext->setText(QApplication::translate("TemplandWindow", "&Next", 0, QApplication::UnicodeUTF8));
    actionPrevious->setText(QApplication::translate("TemplandWindow", "&Previous", 0, QApplication::UnicodeUTF8));
    actionOpen_Filmstrip_2->setText(QApplication::translate("TemplandWindow", "Open &Filmstrip", 0, QApplication::UnicodeUTF8));
    actionOpen_Filmstrip_2->setShortcut(QApplication::translate("TemplandWindow", "Ctrl+F", 0, QApplication::UnicodeUTF8));
    actionCompute_Spline->setText(QApplication::translate("TemplandWindow", "Compute Spline", 0, QApplication::UnicodeUTF8));
    actionMap_all_landmarks->setText(QApplication::translate("TemplandWindow", "Map all Landmarks", 0, QApplication::UnicodeUTF8));
    actionMap_single_Landmark->setText(QApplication::translate("TemplandWindow", "Map single Landmark", 0, QApplication::UnicodeUTF8));
    //actionLandmark->setText(QApplication::translate("TemplandWindow", "Landmark", 0, QApplication::UnicodeUTF8));
    //actionSemi_Landmark->setText(QApplication::translate("TemplandWindow", "Semi-Landmark", 0, QApplication::UnicodeUTF8));
    //actionNon_Landmark->setText(QApplication::translate("TemplandWindow", "Non-Landmark", 0, QApplication::UnicodeUTF8));
    //actionSemi_Landmark_2->setText(QApplication::translate("TemplandWindow", "Semi-Landmark", 0, QApplication::UnicodeUTF8));
    //actionSelect_points->setText(QApplication::translate("TemplandWindow", "Select points in template and specimen", 0, QApplication::UnicodeUTF8));
    menuF_ile->setTitle(QApplication::translate("TemplandWindow", "&File", 0, QApplication::UnicodeUTF8));
    menu_View->setTitle(QApplication::translate("TemplandWindow", "&View", 0, QApplication::UnicodeUTF8));
    menuWindow->setTitle(QApplication::translate("TemplandWindow", "&Window", 0, QApplication::UnicodeUTF8));
    //menu_Landmark->setTitle(QApplication::translate("TemplandWindow", "&Landmark", 0, QApplication::UnicodeUTF8));
    //menuPoint_type->setTitle(QApplication::translate("TemplandWindow", "Point type", 0, QApplication::UnicodeUTF8));
    toolBar->setWindowTitle(QApplication::translate("TemplandWindow", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TemplandWindow: public Ui_TemplandWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEMPLANDWINDOW_H
