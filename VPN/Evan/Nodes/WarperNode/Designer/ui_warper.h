/********************************************************************************
** Form generated from reading ui file 'Warper.ui'
**
** Created: Wed 9. Dec 14:53:24 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WARPER_H
#define UI_WARPER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "../GridTreeWidget.h"

QT_BEGIN_NAMESPACE

class Ui_WarperDialog
{
public:
    QWidget *tab;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *rangeLineEdit;
    QPushButton *rangePushButton;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *m_useShadersCheck;
    QCheckBox *checkBox;
    QCheckBox *checkbox4;
    QPushButton *pushButton;
    QGridLayout *laySliders;
    QWidget *tab2;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    GridTreeWidget *treeWidget;
    QLabel *label_4;
    QWidget *tab_2;
    QGridLayout *gridLayout;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QComboBox *comboBox;
    QComboBox *comboBox_2;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QTabWidget *WarperDialog)
    {
    if (WarperDialog->objectName().isEmpty())
        WarperDialog->setObjectName(QString::fromUtf8("WarperDialog"));
    WarperDialog->resize(400, 389);
    WarperDialog->setMinimumSize(QSize(400, 0));
    WarperDialog->setFocusPolicy(Qt::ClickFocus);
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    verticalLayout = new QVBoxLayout(tab);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    label = new QLabel(tab);
    label->setObjectName(QString::fromUtf8("label"));
    label->setMaximumSize(QSize(16777215, 155));

    horizontalLayout->addWidget(label);

    rangeLineEdit = new QLineEdit(tab);
    rangeLineEdit->setObjectName(QString::fromUtf8("rangeLineEdit"));

    horizontalLayout->addWidget(rangeLineEdit);

    rangePushButton = new QPushButton(tab);
    rangePushButton->setObjectName(QString::fromUtf8("rangePushButton"));

    horizontalLayout->addWidget(rangePushButton);


    verticalLayout->addLayout(horizontalLayout);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    horizontalLayout_2->setSizeConstraint(QLayout::SetFixedSize);
    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    horizontalLayout_3->setSizeConstraint(QLayout::SetFixedSize);
    m_useShadersCheck = new QCheckBox(tab);
    m_useShadersCheck->setObjectName(QString::fromUtf8("m_useShadersCheck"));

    horizontalLayout_3->addWidget(m_useShadersCheck);

    checkBox = new QCheckBox(tab);
    checkBox->setObjectName(QString::fromUtf8("checkBox"));

    horizontalLayout_3->addWidget(checkBox);

    checkbox4 = new QCheckBox(tab);
    checkbox4->setObjectName(QString::fromUtf8("checkbox4"));

    horizontalLayout_3->addWidget(checkbox4);

    pushButton = new QPushButton(tab);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    horizontalLayout_3->addWidget(pushButton);


    horizontalLayout_2->addLayout(horizontalLayout_3);


    verticalLayout->addLayout(horizontalLayout_2);

    laySliders = new QGridLayout();
    laySliders->setObjectName(QString::fromUtf8("laySliders"));

    verticalLayout->addLayout(laySliders);

    WarperDialog->addTab(tab, QString());
    tab2 = new QWidget();
    tab2->setObjectName(QString::fromUtf8("tab2"));
    verticalLayout_2 = new QVBoxLayout(tab2);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    groupBox = new QGroupBox(tab2);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setMinimumSize(QSize(0, 14));
    verticalLayout_3 = new QVBoxLayout(groupBox);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    treeWidget = new GridTreeWidget(groupBox);
    treeWidget->setObjectName(QString::fromUtf8("treeWidget"));

    verticalLayout_3->addWidget(treeWidget);

    label_4 = new QLabel(groupBox);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    verticalLayout_3->addWidget(label_4);


    verticalLayout_2->addWidget(groupBox);

    WarperDialog->addTab(tab2, QString());
    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    gridLayout = new QGridLayout(tab_2);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    pushButton_2 = new QPushButton(tab_2);
    pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

    gridLayout->addWidget(pushButton_2, 1, 1, 1, 1);

    pushButton_3 = new QPushButton(tab_2);
    pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

    gridLayout->addWidget(pushButton_3, 1, 2, 1, 1);

    lineEdit = new QLineEdit(tab_2);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
    lineEdit->setMinimumSize(QSize(20, 20));

    gridLayout->addWidget(lineEdit, 2, 2, 1, 1);

    label_2 = new QLabel(tab_2);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout->addWidget(label_2, 2, 1, 1, 1);

    comboBox = new QComboBox(tab_2);
    comboBox->setObjectName(QString::fromUtf8("comboBox"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());
    comboBox->setSizePolicy(sizePolicy);
    comboBox->setMinimumSize(QSize(80, 0));
    comboBox->setBaseSize(QSize(80, 0));

    gridLayout->addWidget(comboBox, 3, 1, 1, 1);

    comboBox_2 = new QComboBox(tab_2);
    comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

    gridLayout->addWidget(comboBox_2, 3, 2, 1, 1);

    pushButton_4 = new QPushButton(tab_2);
    pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
    pushButton_4->setCheckable(true);

    gridLayout->addWidget(pushButton_4, 4, 1, 1, 1);

    pushButton_5 = new QPushButton(tab_2);
    pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
    pushButton_5->setCheckable(true);

    gridLayout->addWidget(pushButton_5, 4, 2, 1, 1);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 0, 2, 1, 1);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer, 1, 0, 1, 1);

    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_2, 1, 3, 1, 1);

    verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer_2, 5, 1, 1, 1);

    //WarperDialog->addTab(tab_2, QString()); //YN 2/2/2019 Removing Grid Animation Issue #35

    retranslateUi(WarperDialog);

    WarperDialog->setCurrentIndex(2);


    QMetaObject::connectSlotsByName(WarperDialog);
    } // setupUi

    void retranslateUi(QTabWidget *WarperDialog)
    {
    WarperDialog->setWindowTitle(QApplication::translate("WarperDialog", "TabWidget", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WarperDialog", "Change slider(s) range:", 0, QApplication::UnicodeUTF8));
    rangePushButton->setText(QApplication::translate("WarperDialog", "Set Range", 0, QApplication::UnicodeUTF8));
    m_useShadersCheck->setText(QApplication::translate("WarperDialog", "Hardware Acceleration", 0, QApplication::UnicodeUTF8));
    checkBox->setText(QApplication::translate("WarperDialog", "Sliders independent", 0, QApplication::UnicodeUTF8));
    checkbox4->setText(QApplication::translate("WarperDialog", "Reset Sliders", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("WarperDialog", "Play All", 0, QApplication::UnicodeUTF8));
    WarperDialog->setTabText(WarperDialog->indexOf(tab), QApplication::translate("WarperDialog", "Warping", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("WarperDialog", "Addition of grid", 0, QApplication::UnicodeUTF8));
    label_4->setText(QString());
    WarperDialog->setTabText(WarperDialog->indexOf(tab2), QApplication::translate("WarperDialog", "Grid", 0, QApplication::UnicodeUTF8));
    pushButton_2->setText(QApplication::translate("WarperDialog", "Start Position", 0, QApplication::UnicodeUTF8));
    pushButton_3->setText(QApplication::translate("WarperDialog", "Stop Position", 0, QApplication::UnicodeUTF8));
    lineEdit->setText(QApplication::translate("WarperDialog", "10.0", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("WarperDialog", "Lenght (sec)", 0, QApplication::UnicodeUTF8));
    comboBox->clear();
    comboBox->insertItems(0, QStringList()
     << QApplication::translate("WarperDialog", "Min to Max", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("WarperDialog", "Max to Min", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("WarperDialog", "Min to Max then Max to Min", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("WarperDialog", "Max to Min then Min to Max", 0, QApplication::UnicodeUTF8)
    );
    comboBox_2->clear();
    comboBox_2->insertItems(0, QStringList()
     << QApplication::translate("WarperDialog", "One cycle", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("WarperDialog", "Always", 0, QApplication::UnicodeUTF8)
    );
    pushButton_4->setText(QApplication::translate("WarperDialog", "Play", 0, QApplication::UnicodeUTF8));
    pushButton_5->setText(QApplication::translate("WarperDialog", "Pause", 0, QApplication::UnicodeUTF8));
    WarperDialog->setTabText(WarperDialog->indexOf(tab_2), QApplication::translate("WarperDialog", "Grid Animation", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WarperDialog);
    } // retranslateUi

};

namespace Ui {
    class WarperDialog: public Ui_WarperDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WARPER_H
