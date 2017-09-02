/********************************************************************************
** Form generated from reading UI file 'SlideDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLIDEDIALOG_H
#define UI_SLIDEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SlideDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *m_slideCB;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QRadioButton *m_slideOnce_RB;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *m_slideNX_RB;
    QSpacerItem *horizontalSpacer_4;
    QSpinBox *m_slideNum_SB;
    QHBoxLayout *horizontalLayout_5;
    QRadioButton *m_slideConverge_RB;
    QSpacerItem *horizontalSpacer_2;
    QDoubleSpinBox *m_slideError_SB;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SlideDialog)
    {
        if (SlideDialog->objectName().isEmpty())
            SlideDialog->setObjectName(QString::fromUtf8("SlideDialog"));
        SlideDialog->setWindowModality(Qt::ApplicationModal);
        SlideDialog->resize(327, 323);
        verticalLayout_2 = new QVBoxLayout(SlideDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        m_slideCB = new QCheckBox(SlideDialog);
        m_slideCB->setObjectName(QString::fromUtf8("m_slideCB"));

        horizontalLayout_2->addWidget(m_slideCB);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout_2->addLayout(horizontalLayout_2);

        groupBox = new QGroupBox(SlideDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setEnabled(false);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        m_slideOnce_RB = new QRadioButton(groupBox);
        m_slideOnce_RB->setObjectName(QString::fromUtf8("m_slideOnce_RB"));
        m_slideOnce_RB->setChecked(true);

        verticalLayout->addWidget(m_slideOnce_RB);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        m_slideNX_RB = new QRadioButton(groupBox);
        m_slideNX_RB->setObjectName(QString::fromUtf8("m_slideNX_RB"));

        horizontalLayout_4->addWidget(m_slideNX_RB, 0, Qt::AlignLeft);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        m_slideNum_SB = new QSpinBox(groupBox);
        m_slideNum_SB->setObjectName(QString::fromUtf8("m_slideNum_SB"));
        m_slideNum_SB->setEnabled(false);
        m_slideNum_SB->setMinimum(1);

        horizontalLayout_4->addWidget(m_slideNum_SB);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        m_slideConverge_RB = new QRadioButton(groupBox);
        m_slideConverge_RB->setObjectName(QString::fromUtf8("m_slideConverge_RB"));

        horizontalLayout_5->addWidget(m_slideConverge_RB, 0, Qt::AlignLeft);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);

        m_slideError_SB = new QDoubleSpinBox(groupBox);
        m_slideError_SB->setObjectName(QString::fromUtf8("m_slideError_SB"));
        m_slideError_SB->setEnabled(false);
        m_slideError_SB->setDecimals(3);
        m_slideError_SB->setMinimum(0.001);
        m_slideError_SB->setMaximum(1);
        m_slideError_SB->setSingleStep(0.001);
        m_slideError_SB->setValue(0.01);

        horizontalLayout_5->addWidget(m_slideError_SB);


        verticalLayout->addLayout(horizontalLayout_5);


        verticalLayout_2->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(SlideDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(SlideDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SlideDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SlideDialog, SLOT(reject()));
        QObject::connect(m_slideNX_RB, SIGNAL(toggled(bool)), m_slideNum_SB, SLOT(setEnabled(bool)));
        QObject::connect(m_slideConverge_RB, SIGNAL(toggled(bool)), m_slideError_SB, SLOT(setEnabled(bool)));
        QObject::connect(m_slideCB, SIGNAL(toggled(bool)), groupBox, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(SlideDialog);
    } // setupUi

    void retranslateUi(QDialog *SlideDialog)
    {
        SlideDialog->setWindowTitle(QApplication::translate("SlideDialog", "Warp-Project-Slide", 0, QApplication::UnicodeUTF8));
        m_slideCB->setText(QApplication::translate("SlideDialog", "Slide Semi-Landmarks", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("SlideDialog", "Options", 0, QApplication::UnicodeUTF8));
        m_slideOnce_RB->setText(QApplication::translate("SlideDialog", "Slide-Project Once", 0, QApplication::UnicodeUTF8));
        m_slideNX_RB->setText(QApplication::translate("SlideDialog", "Slide-Project # iterations", 0, QApplication::UnicodeUTF8));
        m_slideConverge_RB->setText(QApplication::translate("SlideDialog", "Slide-Project until error", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SlideDialog: public Ui_SlideDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLIDEDIALOG_H
