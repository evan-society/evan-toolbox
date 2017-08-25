/********************************************************************************
** Form generated from reading ui file 'Tab.ui'
**
** Created: Wed 23. Dec 11:40:02 2009
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TAB_H
#define UI_TAB_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "../MySplitter.h"
//#include <QtGui/QSplitter>
#include "../ViewTree/ViewTree.h"
#include "../Viewers/View3Qt.h"


QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QGroupBox *m_srcGrpBox;
    QHBoxLayout *horizontalLayout;
    View3Qt *m_templateView;
    View3Qt *m_templateSliceView;
    QGroupBox *m_trgtGrpBox;
    QHBoxLayout *horizontalLayout_2;
    View3Qt *m_targetView;
    View3Qt *m_targetSliceView;
    QVBoxLayout *verticalLayout_2;
    ViewTree *m_templateItemTree;
    ViewTree *m_targetItemTree;
    QHBoxLayout *horizontalLayout_4;
    QSpinBox *m_frameSBox;
    QPushButton *m_tblStart;
    QPushButton *m_tblLeftE;
    QPushButton *m_tblLeft;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *m_tblRight;
    QPushButton *m_tblListRightE;
    QPushButton *m_tblEnd;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_consensusPB;
    QToolButton *m_filmShow;
    QGroupBox *m_filmGrpBox;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_7;
    QSlider *m_frameSlider;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_6;
    QLabel *m_frameLBL;
    QLabel *m_frameCountLBL;
    QLabel *m_lblStepsPerFrame;
    QLineEdit *m_stepsFrameTXT;
    QLabel *m_lblSecPerFrame;
    QLineEdit *m_secFrameTXT;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *m_filmFirstFrame;
    QPushButton *m_filmStepPrev;
    QPushButton *m_filmStepNext;
    QPushButton *m_filmLastFrame;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *m_filmPlayBackward;
    QPushButton *m_filmPlay;
    QPushButton *m_filmPause;
    QPushButton *m_filmStop;
    MySplitter* m_templateSplitter;
    MySplitter* m_targetSplitter;
    MySplitter* m_viewsSplitter;

    void setupUi(QWidget *Form, ew::DataflowNetwork *nw, QGLWidget* share2 = 0, ew::Dig3* dig3 = 0)
    {
    if (Form->objectName().isEmpty())
        Form->setObjectName(QString::fromUtf8("Form"));
    Form->resize(746, 591);
    gridLayout = new QGridLayout(Form);

    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setHorizontalSpacing( 0 );
    gridLayout->setVerticalSpacing( 0 );
    gridLayout->setSpacing( 0 );


    m_templateSplitter = new MySplitter( Qt::Horizontal );
    m_targetSplitter = new MySplitter( Qt::Horizontal );

    QObject::connect(m_templateSplitter, SIGNAL(splitterMoved(int, int)), m_targetSplitter, SLOT(moveSplitterPosition(int, int)));
    QObject::connect(m_targetSplitter, SIGNAL(splitterMoved(int, int)), m_templateSplitter, SLOT(moveSplitterPosition(int, int)));


    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setSpacing( 0 );
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    QWidget* verticalLayoutWidget = new QWidget( Form );
    verticalLayout = new QVBoxLayout(verticalLayoutWidget);
    verticalLayout->setSpacing( 0 );
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    m_srcGrpBox = new QGroupBox(verticalLayoutWidget);
    m_srcGrpBox->setObjectName(QString::fromUtf8("m_srcGrpBox"));
    m_srcGrpBox->setEnabled(true);
    m_srcGrpBox->setFlat(false);
    m_srcGrpBox->setCheckable(false);
    m_srcGrpBox->setChecked(false);
    horizontalLayout = new QHBoxLayout(m_srcGrpBox);
    horizontalLayout->setSpacing( 3 );
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

    // initialize the target view before the other 3
    m_trgtGrpBox = new QGroupBox(verticalLayoutWidget);//Form);
    m_targetView = new View3Qt(nw, m_trgtGrpBox, 0, share2, dig3,1,0);

    m_templateView = new View3Qt(nw, m_srcGrpBox, m_targetView, share2, dig3,0,0);
    m_templateView->setObjectName(QString::fromUtf8("m_templateView"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_templateView->sizePolicy().hasHeightForWidth());
    m_templateView->setSizePolicy(sizePolicy);
    m_templateView->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));

    //horizontalLayout->addWidget(m_templateView);

    m_templateSliceView = new View3Qt(nw, m_srcGrpBox, m_targetView, share2, dig3,0,1);
    m_templateSliceView->setObjectName(QString::fromUtf8("m_templateSliceView"));
    sizePolicy.setHeightForWidth(m_templateSliceView->sizePolicy().hasHeightForWidth());
    m_templateSliceView->setSizePolicy(sizePolicy);
    m_templateSliceView->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));

    m_templateSplitter->addWidget( m_templateView );
    m_templateSplitter->addWidget( m_templateSliceView );
    m_templateSplitter->setStretchFactor(0,5);
    m_templateSplitter->setStretchFactor(1,5);
    horizontalLayout->addWidget(m_templateSplitter);

    //horizontalLayout->addWidget(m_templateSliceView);

//    m_targetView = new View3Qt(nw, m_trgtGrpBox, 0, share2, dig3,1,0);

    //verticalLayout->addWidget(m_srcGrpBox);

    m_trgtGrpBox->setObjectName(QString::fromUtf8("m_trgtGrpBox"));
    horizontalLayout_2 = new QHBoxLayout(m_trgtGrpBox);
    horizontalLayout_2->setSpacing( 3 );
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));

    m_targetView->setObjectName(QString::fromUtf8("m_targetView"));
    sizePolicy.setHeightForWidth(m_targetView->sizePolicy().hasHeightForWidth());
    m_targetView->setSizePolicy(sizePolicy);
    m_targetView->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));

    //horizontalLayout_2->addWidget(m_targetView);

    m_targetSliceView = new View3Qt(nw, m_trgtGrpBox, m_targetView, share2, dig3,1,1);
    m_targetSliceView->setObjectName(QString::fromUtf8("m_targetSliceView"));
    sizePolicy.setHeightForWidth(m_targetSliceView->sizePolicy().hasHeightForWidth());
    m_targetSliceView->setSizePolicy(sizePolicy);
    m_targetSliceView->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));


    m_targetSplitter->addWidget( m_targetView );
    m_targetSplitter->addWidget( m_targetSliceView );
    m_targetSplitter->setStretchFactor(0,5);
    m_targetSplitter->setStretchFactor(1,5);
    horizontalLayout_2->addWidget(m_targetSplitter);

    //horizontalLayout_2->addWidget(m_targetSliceView);

    m_viewsSplitter = new MySplitter( Qt::Vertical );
    m_viewsSplitter->addWidget( m_srcGrpBox );
    m_viewsSplitter->addWidget( m_trgtGrpBox );
    m_viewsSplitter->setStretchFactor(0,5);
    m_viewsSplitter->setStretchFactor(1,5);
    verticalLayout->addWidget(m_viewsSplitter);


    //verticalLayout->addWidget(m_trgtGrpBox);


    //horizontalLayout_3->addLayout(verticalLayout);

    QWidget* verticalLayoutWidget_2 = new QWidget(Form);
    verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
    verticalLayout_2->setSpacing( 0 );
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    m_templateItemTree = new ViewTree(verticalLayoutWidget_2);
    m_templateItemTree->setObjectName(QString::fromUtf8("m_templateItemTree"));
    m_templateItemTree->setMinimumSize(QSize(191, 0));
    //m_templateItemTree->setMaximumSize(QSize(191, 16777215));

    verticalLayout_2->addWidget(m_templateItemTree);

    m_targetItemTree = new ViewTree(verticalLayoutWidget_2);
    m_targetItemTree->setObjectName(QString::fromUtf8("m_targetItemTree"));
    m_targetItemTree->setMinimumSize(QSize(191, 0));
    //m_targetItemTree->setMaximumSize(QSize(191, 16777215));

    verticalLayout_2->addWidget(m_targetItemTree);


    QSplitter* page = new QSplitter( Qt::Horizontal );
    page->addWidget( verticalLayoutWidget );
    page->addWidget( verticalLayoutWidget_2 );
    page->setStretchFactor(0,10);
    page->setStretchFactor(1,1);
    QList<int> sizes;
    sizes.append(800);
    sizes.append(100);
    page->setSizes(sizes);
    horizontalLayout_3->addWidget(page);
    //horizontalLayout_3->addLayout(verticalLayout_2);


    gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);

    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
    m_frameSBox = new QSpinBox(Form);
    m_frameSBox->setObjectName(QString::fromUtf8("m_frameSBox"));
    m_frameSBox->setMinimum(0);
    m_frameSBox->setMaximum(0);

    horizontalLayout_4->addWidget(m_frameSBox);

    m_tblStart = new QPushButton(Form);
    m_tblStart->setObjectName(QString::fromUtf8("m_tblStart"));
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/Images/startframe.png")), QIcon::Normal, QIcon::Off);
    m_tblStart->setIcon(icon);

    horizontalLayout_4->addWidget(m_tblStart);

    m_tblLeftE = new QPushButton(Form);
    m_tblLeftE->setObjectName(QString::fromUtf8("m_tblLeftE"));
    QIcon icon1;
    icon1.addPixmap(QPixmap(QString::fromUtf8(":/Images/firstframe.png")), QIcon::Normal, QIcon::Off);
    m_tblLeftE->setIcon(icon1);

    horizontalLayout_4->addWidget(m_tblLeftE);

    m_tblLeft = new QPushButton(Form);
    m_tblLeft->setObjectName(QString::fromUtf8("m_tblLeft"));
    QIcon icon2;
    icon2.addPixmap(QPixmap(QString::fromUtf8(":/Images/stepprev.png")), QIcon::Normal, QIcon::Off);
    m_tblLeft->setIcon(icon2);

    horizontalLayout_4->addWidget(m_tblLeft);

    horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

    horizontalLayout_4->addItem(horizontalSpacer_2);

    m_tblRight = new QPushButton(Form);
    m_tblRight->setObjectName(QString::fromUtf8("m_tblRight"));
    QIcon icon3;
    icon3.addPixmap(QPixmap(QString::fromUtf8(":/Images/stepnext.png")), QIcon::Normal, QIcon::Off);
    m_tblRight->setIcon(icon3);

    horizontalLayout_4->addWidget(m_tblRight);

    m_tblListRightE = new QPushButton(Form);
    m_tblListRightE->setObjectName(QString::fromUtf8("m_tblListRightE"));
    QIcon icon4;
    icon4.addPixmap(QPixmap(QString::fromUtf8(":/Images/lastframe.png")), QIcon::Normal, QIcon::Off);
    m_tblListRightE->setIcon(icon4);

    horizontalLayout_4->addWidget(m_tblListRightE);

    m_tblEnd = new QPushButton(Form);
    m_tblEnd->setObjectName(QString::fromUtf8("m_tblEnd"));
    QIcon icon5;
    icon5.addPixmap(QPixmap(QString::fromUtf8(":/Images/endframe.png")), QIcon::Normal, QIcon::Off);
    m_tblEnd->setIcon(icon5);

    horizontalLayout_4->addWidget(m_tblEnd);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_4->addItem(horizontalSpacer);

    m_consensusPB = new QPushButton(Form);
    m_consensusPB->setObjectName(QString::fromUtf8("m_consensusPB"));
    horizontalLayout_4->addWidget(m_consensusPB);

    m_filmShow = new QToolButton(Form);
    m_filmShow->setObjectName(QString::fromUtf8("m_filmShow"));
    m_filmShow->setCheckable(true);

    horizontalLayout_4->addWidget(m_filmShow);


    gridLayout->addLayout(horizontalLayout_4, 1, 0, 1, 1);

    m_filmGrpBox = new QGroupBox(Form);
    m_filmGrpBox->setObjectName(QString::fromUtf8("m_filmGrpBox"));
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_filmGrpBox->sizePolicy().hasHeightForWidth());
    m_filmGrpBox->setSizePolicy(sizePolicy1);
    m_filmGrpBox->setFlat(false);
    m_filmGrpBox->setCheckable(false);
    verticalLayout_3 = new QVBoxLayout(m_filmGrpBox);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    horizontalLayout_7 = new QHBoxLayout();
    horizontalLayout_7->setSpacing(6);
    horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
    m_frameSlider = new QSlider(m_filmGrpBox);
    m_frameSlider->setObjectName(QString::fromUtf8("m_frameSlider"));
    m_frameSlider->setMinimum(0);
    m_frameSlider->setMaximum(0);
    m_frameSlider->setPageStep(1);
    m_frameSlider->setOrientation(Qt::Horizontal);
    m_frameSlider->setTickPosition(QSlider::TicksBelow);

    horizontalLayout_7->addWidget(m_frameSlider);

    verticalLayout_4 = new QVBoxLayout();
    verticalLayout_4->setSpacing(2);
    verticalLayout_4->setMargin(0);
    verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
    horizontalLayout_6 = new QHBoxLayout();
#ifndef Q_OS_MAC
    horizontalLayout_6->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    horizontalLayout_6->setMargin(0);
#endif
    horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
    m_frameLBL = new QLabel(m_filmGrpBox);
    m_frameLBL->setObjectName(QString::fromUtf8("m_frameLBL"));
    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(m_frameLBL->sizePolicy().hasHeightForWidth());
    m_frameLBL->setSizePolicy(sizePolicy2);

    horizontalLayout_6->addWidget(m_frameLBL);

    m_frameCountLBL = new QLabel(m_filmGrpBox);
    m_frameCountLBL->setObjectName(QString::fromUtf8("m_frameCountLBL"));
    m_frameCountLBL->setMinimumSize(QSize(40, 0));
    m_frameCountLBL->setMaximumSize(QSize(40, 16777215));
    m_frameCountLBL->setBaseSize(QSize(0, 0));
    m_frameCountLBL->setLineWidth(1);
    m_frameCountLBL->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    m_frameCountLBL->setWordWrap(false);

    horizontalLayout_6->addWidget(m_frameCountLBL);

    m_lblStepsPerFrame = new QLabel(m_filmGrpBox);
    m_lblStepsPerFrame->setObjectName(QString::fromUtf8("m_lblStepsPerFrame"));
    QSizePolicy sizePolicyShow(QSizePolicy::Maximum, QSizePolicy::Preferred);
    sizePolicyShow.setHorizontalStretch(0);
    sizePolicyShow.setVerticalStretch(0);
    sizePolicyShow.setHeightForWidth(m_lblStepsPerFrame->sizePolicy().hasHeightForWidth());
    m_lblStepsPerFrame->setSizePolicy(sizePolicyShow);

    horizontalLayout_6->addWidget(m_lblStepsPerFrame);

    m_stepsFrameTXT = new QLineEdit(m_filmGrpBox);
    m_stepsFrameTXT->setObjectName(QString::fromUtf8("m_stepsFrameTXT"));
    QSizePolicy sizePolicy3(QSizePolicy::Maximum, QSizePolicy::Fixed);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(m_stepsFrameTXT->sizePolicy().hasHeightForWidth());
    m_stepsFrameTXT->setSizePolicy(sizePolicy3);
    m_stepsFrameTXT->setMaximumSize(QSize(40, 16777215));
    m_stepsFrameTXT->setMaxLength(3);

    horizontalLayout_6->addWidget(m_stepsFrameTXT);

    m_lblSecPerFrame = new QLabel(m_filmGrpBox);
    m_lblSecPerFrame->setObjectName(QString::fromUtf8("m_lblSecPerFrame"));

    horizontalLayout_6->addWidget(m_lblSecPerFrame);

    m_secFrameTXT = new QLineEdit(m_filmGrpBox);
    m_secFrameTXT->setObjectName(QString::fromUtf8("m_secFrameTXT"));
    QSizePolicy sizePolicy4(QSizePolicy::Maximum, QSizePolicy::Fixed);
    sizePolicy4.setHorizontalStretch(1);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(m_secFrameTXT->sizePolicy().hasHeightForWidth());
    m_secFrameTXT->setSizePolicy(sizePolicy4);
    m_secFrameTXT->setMaximumSize(QSize(40, 16777215));
    m_secFrameTXT->setMaxLength(10);

    horizontalLayout_6->addWidget(m_secFrameTXT);


    verticalLayout_4->addLayout(horizontalLayout_6);

    horizontalLayout_5 = new QHBoxLayout();
    horizontalLayout_5->setSpacing(3);
    horizontalLayout_5->setMargin(0);
    horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
    m_filmFirstFrame = new QPushButton(m_filmGrpBox);
    m_filmFirstFrame->setObjectName(QString::fromUtf8("m_filmFirstFrame"));
    m_filmFirstFrame->setIcon(icon1);

    horizontalLayout_5->addWidget(m_filmFirstFrame);

    m_filmStepPrev = new QPushButton(m_filmGrpBox);
    m_filmStepPrev->setObjectName(QString::fromUtf8("m_filmStepPrev"));
    m_filmStepPrev->setIcon(icon2);

    horizontalLayout_5->addWidget(m_filmStepPrev);

    m_filmStepNext = new QPushButton(m_filmGrpBox);
    m_filmStepNext->setObjectName(QString::fromUtf8("m_filmStepNext"));
    m_filmStepNext->setIcon(icon3);

    horizontalLayout_5->addWidget(m_filmStepNext);

    m_filmLastFrame = new QPushButton(m_filmGrpBox);
    m_filmLastFrame->setObjectName(QString::fromUtf8("m_filmLastFrame"));
    m_filmLastFrame->setIcon(icon4);

    horizontalLayout_5->addWidget(m_filmLastFrame);

    horizontalSpacer_3 = new QSpacerItem(10, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

    horizontalLayout_5->addItem(horizontalSpacer_3);

    m_filmPlayBackward = new QPushButton(m_filmGrpBox);
    m_filmPlayBackward->setObjectName(QString::fromUtf8("m_filmPlayBackward"));
    QIcon icon6;
    icon6.addPixmap(QPixmap(QString::fromUtf8(":/Images/playb.png")), QIcon::Normal, QIcon::Off);
    m_filmPlayBackward->setIcon(icon6);

    horizontalLayout_5->addWidget(m_filmPlayBackward);

    m_filmPlay = new QPushButton(m_filmGrpBox);
    m_filmPlay->setObjectName(QString::fromUtf8("m_filmPlay"));
    QIcon icon7;
    icon7.addPixmap(QPixmap(QString::fromUtf8(":/Images/play.png")), QIcon::Normal, QIcon::Off);
    m_filmPlay->setIcon(icon7);

    horizontalLayout_5->addWidget(m_filmPlay);

    m_filmPause = new QPushButton(m_filmGrpBox);
    m_filmPause->setObjectName(QString::fromUtf8("m_filmPause"));
    QIcon icon8;
    icon8.addPixmap(QPixmap(QString::fromUtf8(":/Images/pause.png")), QIcon::Normal, QIcon::Off);
    m_filmPause->setIcon(icon8);

    horizontalLayout_5->addWidget(m_filmPause);

    m_filmStop = new QPushButton(m_filmGrpBox);
    m_filmStop->setObjectName(QString::fromUtf8("m_filmStop"));
    QIcon icon9;
    icon9.addPixmap(QPixmap(QString::fromUtf8(":/Images/stop.png")), QIcon::Normal, QIcon::Off);
    m_filmStop->setIcon(icon9);

    horizontalLayout_5->addWidget(m_filmStop);


    verticalLayout_4->addLayout(horizontalLayout_5);


    horizontalLayout_7->addLayout(verticalLayout_4);


    verticalLayout_3->addLayout(horizontalLayout_7);


    gridLayout->addWidget(m_filmGrpBox, 2, 0, 1, 1);


    retranslateUi(Form);

    QObject::connect(m_filmShow, SIGNAL(clicked(bool)), m_filmGrpBox, SLOT(setVisible(bool)));
    QObject::connect(m_filmShow, SIGNAL(clicked(bool)), Form, SLOT(showFilmstrip(bool)));
    QObject::connect(m_frameSlider, SIGNAL(sliderMoved(int)), m_frameSBox, SLOT(setValue(int)));
    QObject::connect(m_filmFirstFrame, SIGNAL(clicked()), Form, SLOT(firstFrameFilmstrip()));
    QObject::connect(m_filmPlayBackward, SIGNAL(clicked()), Form, SLOT(backwardFilmstip()));
    QObject::connect(m_filmPlay, SIGNAL(clicked()), Form, SLOT(forwardFilmstip()));
    QObject::connect(m_filmPause, SIGNAL(clicked()), Form, SLOT(pauseFilmstip()));
    QObject::connect(m_filmStop, SIGNAL(clicked()), Form, SLOT(stopFilmstip()));
    QObject::connect(m_filmLastFrame, SIGNAL(clicked()), Form, SLOT(lastFrameFilmstrip()));

    QObject::connect(m_secFrameTXT, SIGNAL(textEdited(const QString&)), Form, SLOT(secPerFrame(const QString&)));
    QObject::connect(m_stepsFrameTXT, SIGNAL(textEdited(const QString&)), Form, SLOT(stepPerFrame(const QString&)));
    QObject::connect(m_filmStepPrev, SIGNAL(clicked()), Form, SLOT(stepPrevFrameFilmstrip()));
    QObject::connect(m_filmStepNext, SIGNAL(clicked()), Form, SLOT(stepNextFrameFilmstrip()));

    QObject::connect(m_tblStart, SIGNAL(clicked()), Form, SLOT(tblMoveToStart()));
    QObject::connect(m_tblLeftE, SIGNAL(clicked()), Form, SLOT(tblMoveLeftE()));
    QObject::connect(m_tblLeft, SIGNAL(clicked()), Form, SLOT(tblMoveLeft()));
    QObject::connect(m_tblRight, SIGNAL(clicked()), Form, SLOT(tblMoveRight()));
    QObject::connect(m_tblListRightE, SIGNAL(clicked()), Form, SLOT(tblMoveRightE()));
    QObject::connect(m_tblEnd, SIGNAL(clicked()), Form, SLOT(tblMoveToEnd()));

    QObject::connect(m_consensusPB, SIGNAL(clicked()), Form, SLOT(slideOnConsensus()));

    QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
    Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
    m_srcGrpBox->setTitle(QApplication::translate("Form", "Template", 0, QApplication::UnicodeUTF8));
    m_trgtGrpBox->setTitle(QApplication::translate("Form", "Specimen", 0, QApplication::UnicodeUTF8));
    m_templateItemTree->headerItem()->setText(0, QApplication::translate("Form", "Items", 0, QApplication::UnicodeUTF8));
    m_templateItemTree->headerItem()->setText(1, QApplication::translate("Form", "View", 0, QApplication::UnicodeUTF8));
    m_templateItemTree->headerItem()->setText(2, QApplication::translate("Form", "Slice", 0, QApplication::UnicodeUTF8));
    m_targetItemTree->headerItem()->setText(0, QApplication::translate("Form", "Items", 0, QApplication::UnicodeUTF8));
    m_targetItemTree->headerItem()->setText(1, QApplication::translate("Form", "View", 0, QApplication::UnicodeUTF8));
    m_targetItemTree->headerItem()->setText(2, QApplication::translate("Form", "Slice", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    m_tblStart->setToolTip(QApplication::translate("Form", "Display first tableau", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_tblStart->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_tblLeftE->setToolTip(QApplication::translate("Form", "Display previous tableau x10", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_tblLeftE->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_tblLeft->setToolTip(QApplication::translate("Form", "Display previous tableau", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_tblLeft->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_tblRight->setToolTip(QApplication::translate("Form", "Display next tableau", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_tblRight->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_tblListRightE->setToolTip(QApplication::translate("Form", "Display next tableau x10", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_tblListRightE->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_tblEnd->setToolTip(QApplication::translate("Form", "Display last tableau", 0, QApplication::UnicodeUTF8));
    m_consensusPB->setToolTip(QApplication::translate("Form", "Slide tableau on the consensus", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_tblEnd->setText(QString());
    m_consensusPB->setText(QApplication::translate("Form", "Consensus", 0, QApplication::UnicodeUTF8));
    m_filmShow->setText(QApplication::translate("Form", "+", 0, QApplication::UnicodeUTF8));
    m_filmGrpBox->setTitle(QApplication::translate("Form", "Filmstrip", 0, QApplication::UnicodeUTF8));
    m_frameLBL->setText(QApplication::translate("Form", "Frame:", 0, QApplication::UnicodeUTF8));
    m_frameCountLBL->setText(QApplication::translate("Form", "0", 0, QApplication::UnicodeUTF8));
    m_lblStepsPerFrame->setText(QApplication::translate("Form", "Steps/Frame:", 0, QApplication::UnicodeUTF8));
    m_stepsFrameTXT->setText(QApplication::translate("Form", "10", 0, QApplication::UnicodeUTF8));
    m_lblSecPerFrame->setText(QApplication::translate("Form", "Secs/Frame:", 0, QApplication::UnicodeUTF8));
    m_secFrameTXT->setInputMask(QString());
    m_secFrameTXT->setText(QApplication::translate("Form", "1.0", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    m_filmFirstFrame->setToolTip(QApplication::translate("Form", "Moves to previous frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_filmFirstFrame->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_filmStepPrev->setToolTip(QApplication::translate("Form", "Moves to previous frame step", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_filmStepPrev->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_filmStepNext->setToolTip(QApplication::translate("Form", "Moves to next frame step", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_filmStepNext->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_filmLastFrame->setToolTip(QApplication::translate("Form", "Moves to next frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_filmLastFrame->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_filmPlayBackward->setToolTip(QApplication::translate("Form", "Play backwards", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_filmPlayBackward->setText(QString());

#ifndef QT_NO_TOOLTIP
    m_filmPlay->setToolTip(QApplication::translate("Form", "Play forwards", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    m_filmPlay->setText(QString());
    m_filmPause->setText(QString());
    m_filmStop->setText(QString());
    Q_UNUSED(Form);
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TAB_H
