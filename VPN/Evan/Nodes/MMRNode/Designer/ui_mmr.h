/********************************************************************************
** Form generated from reading ui file 'mmr.ui'
**
** Created: Wed 13. Jan 14:15:36 2010
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MMR_H
#define UI_MMR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_RegressionDialog
{
public:
    QGridLayout *gridLayout_4;
    QLabel *label;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QListWidget *listWidget;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_5;
    QRadioButton *radioButton_6;
    QListWidget *listWidget_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_3;
    QComboBox *comboBox;
    QLineEdit *lineEdit_2;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *RegressionDialog)
    {
    if (RegressionDialog->objectName().isEmpty())
        RegressionDialog->setObjectName(QString::fromUtf8("RegressionDialog"));
    RegressionDialog->resize(492, 497);
    gridLayout_4 = new QGridLayout(RegressionDialog);
    gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
    label = new QLabel(RegressionDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setAlignment(Qt::AlignCenter);

    gridLayout_4->addWidget(label, 0, 0, 1, 2);

    groupBox = new QGroupBox(RegressionDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout = new QGridLayout(groupBox);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    radioButton = new QRadioButton(groupBox);
    radioButton->setObjectName(QString::fromUtf8("radioButton"));
    radioButton->setChecked(true);

    gridLayout->addWidget(radioButton, 0, 0, 1, 1);

    radioButton_2 = new QRadioButton(groupBox);
    radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

    gridLayout->addWidget(radioButton_2, 1, 0, 1, 1);

    radioButton_3 = new QRadioButton(groupBox);
    radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));

    gridLayout->addWidget(radioButton_3, 2, 0, 1, 1);

    listWidget = new QListWidget(groupBox);
    listWidget->setObjectName(QString::fromUtf8("listWidget"));

    gridLayout->addWidget(listWidget, 3, 0, 1, 1);


    gridLayout_4->addWidget(groupBox, 1, 0, 1, 1);

    groupBox_2 = new QGroupBox(RegressionDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    gridLayout_2 = new QGridLayout(groupBox_2);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    radioButton_4 = new QRadioButton(groupBox_2);
    radioButton_4->setObjectName(QString::fromUtf8("radioButton_4"));
    radioButton_4->setChecked(true);

    gridLayout_2->addWidget(radioButton_4, 0, 0, 1, 1);

    radioButton_5 = new QRadioButton(groupBox_2);
    radioButton_5->setObjectName(QString::fromUtf8("radioButton_5"));

    gridLayout_2->addWidget(radioButton_5, 1, 0, 1, 1);

    radioButton_6 = new QRadioButton(groupBox_2);
    radioButton_6->setObjectName(QString::fromUtf8("radioButton_6"));

    gridLayout_2->addWidget(radioButton_6, 2, 0, 1, 1);

    listWidget_2 = new QListWidget(groupBox_2);
    listWidget_2->setObjectName(QString::fromUtf8("listWidget_2"));

    gridLayout_2->addWidget(listWidget_2, 3, 0, 1, 1);


    gridLayout_4->addWidget(groupBox_2, 1, 1, 1, 1);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    label_2 = new QLabel(RegressionDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    horizontalLayout->addWidget(label_2);

    lineEdit = new QLineEdit(RegressionDialog);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

    horizontalLayout->addWidget(lineEdit);

    pushButton = new QPushButton(RegressionDialog);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    horizontalLayout->addWidget(pushButton);


    gridLayout_4->addLayout(horizontalLayout, 2, 0, 1, 2);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_4->addItem(verticalSpacer, 3, 0, 1, 1);

    groupBox_3 = new QGroupBox(RegressionDialog);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    gridLayout_3 = new QGridLayout(groupBox_3);
    gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
    comboBox = new QComboBox(groupBox_3);
    comboBox->setObjectName(QString::fromUtf8("comboBox"));

    gridLayout_3->addWidget(comboBox, 0, 0, 1, 1);

    lineEdit_2 = new QLineEdit(groupBox_3);
    lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

    gridLayout_3->addWidget(lineEdit_2, 0, 1, 1, 1);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_3->addItem(horizontalSpacer, 0, 3, 1, 1);

    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_3->addItem(horizontalSpacer_2, 0, 2, 1, 1);


    gridLayout_4->addWidget(groupBox_3, 4, 0, 1, 2);

    buttonBox = new QDialogButtonBox(RegressionDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout_4->addWidget(buttonBox, 5, 0, 1, 2);


    retranslateUi(RegressionDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), RegressionDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), RegressionDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(RegressionDialog);
    } // setupUi

    void retranslateUi(QDialog *RegressionDialog)
    {
    RegressionDialog->setWindowTitle(QApplication::translate("RegressionDialog", "Multiple Multivariate Regression", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("RegressionDialog", "Select data for dependent and independent variables", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("RegressionDialog", "Dependent variables (Y)", 0, QApplication::UnicodeUTF8));
    radioButton->setText(QApplication::translate("RegressionDialog", "Shape variables", 0, QApplication::UnicodeUTF8));
    radioButton_2->setText(QApplication::translate("RegressionDialog", "Form variables", 0, QApplication::UnicodeUTF8));
    radioButton_3->setText(QApplication::translate("RegressionDialog", "Other variables", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("RegressionDialog", "Independent variables (X)", 0, QApplication::UnicodeUTF8));
    radioButton_4->setText(QApplication::translate("RegressionDialog", "Shape variables", 0, QApplication::UnicodeUTF8));
    radioButton_5->setText(QApplication::translate("RegressionDialog", "Form variables", 0, QApplication::UnicodeUTF8));
    radioButton_6->setText(QApplication::translate("RegressionDialog", "Other variables", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("RegressionDialog", "Output file", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("RegressionDialog", "Browse", 0, QApplication::UnicodeUTF8));
    groupBox_3->setTitle(QApplication::translate("RegressionDialog", "Standardise Specimens", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(RegressionDialog);
    } // retranslateUi

};

namespace Ui {
    class RegressionDialog: public Ui_RegressionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MMR_H
