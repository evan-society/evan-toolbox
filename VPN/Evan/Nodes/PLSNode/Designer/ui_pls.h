/********************************************************************************
** Form generated from reading ui file 'pls.ui'
**
** Created: Sat 10. Oct 09:58:00 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PLS_H
#define UI_PLS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
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

class Ui_PLSDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QListWidget *listWidget;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_3;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_5;
    QRadioButton *radioButton_6;
    QListWidget *listWidget_2;
    QCheckBox *checkBox_4;
    QCheckBox *checkBox_5;
    QDialogButtonBox *buttonBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *PLSDialog)
    {
    if (PLSDialog->objectName().isEmpty())
        PLSDialog->setObjectName(QString::fromUtf8("PLSDialog"));
    PLSDialog->resize(489, 367);
    gridLayout = new QGridLayout(PLSDialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label = new QLabel(PLSDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(label, 0, 0, 1, 2);

    groupBox = new QGroupBox(PLSDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout_2 = new QGridLayout(groupBox);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    radioButton = new QRadioButton(groupBox);
    radioButton->setObjectName(QString::fromUtf8("radioButton"));
    radioButton->setChecked(true);

    gridLayout_2->addWidget(radioButton, 0, 0, 1, 1);

    radioButton_2 = new QRadioButton(groupBox);
    radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

    gridLayout_2->addWidget(radioButton_2, 1, 0, 1, 1);

    radioButton_3 = new QRadioButton(groupBox);
    radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));

    gridLayout_2->addWidget(radioButton_3, 2, 0, 1, 1);

    listWidget = new QListWidget(groupBox);
    listWidget->setObjectName(QString::fromUtf8("listWidget"));

    gridLayout_2->addWidget(listWidget, 3, 0, 1, 1);

    checkBox_2 = new QCheckBox(groupBox);
    checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

    gridLayout_2->addWidget(checkBox_2, 4, 0, 1, 1);

    checkBox_3 = new QCheckBox(groupBox);
    checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));

    gridLayout_2->addWidget(checkBox_3, 5, 0, 1, 1);


    gridLayout->addWidget(groupBox, 1, 0, 1, 1);

    groupBox_2 = new QGroupBox(PLSDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    gridLayout_3 = new QGridLayout(groupBox_2);
    gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
    radioButton_4 = new QRadioButton(groupBox_2);
    radioButton_4->setObjectName(QString::fromUtf8("radioButton_4"));
    radioButton_4->setChecked(true);

    gridLayout_3->addWidget(radioButton_4, 0, 0, 1, 1);

    radioButton_5 = new QRadioButton(groupBox_2);
    radioButton_5->setObjectName(QString::fromUtf8("radioButton_5"));

    gridLayout_3->addWidget(radioButton_5, 1, 0, 1, 1);

    radioButton_6 = new QRadioButton(groupBox_2);
    radioButton_6->setObjectName(QString::fromUtf8("radioButton_6"));

    gridLayout_3->addWidget(radioButton_6, 2, 0, 1, 1);

    listWidget_2 = new QListWidget(groupBox_2);
    listWidget_2->setObjectName(QString::fromUtf8("listWidget_2"));

    gridLayout_3->addWidget(listWidget_2, 3, 0, 1, 1);

    checkBox_4 = new QCheckBox(groupBox_2);
    checkBox_4->setObjectName(QString::fromUtf8("checkBox_4"));

    gridLayout_3->addWidget(checkBox_4, 4, 0, 1, 1);

    checkBox_5 = new QCheckBox(groupBox_2);
    checkBox_5->setObjectName(QString::fromUtf8("checkBox_5"));

    gridLayout_3->addWidget(checkBox_5, 5, 0, 1, 1);


    gridLayout->addWidget(groupBox_2, 1, 1, 1, 1);

    buttonBox = new QDialogButtonBox(PLSDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 6, 0, 1, 2);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    label_2 = new QLabel(PLSDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    horizontalLayout->addWidget(label_2);

    lineEdit = new QLineEdit(PLSDialog);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

    horizontalLayout->addWidget(lineEdit);

    pushButton = new QPushButton(PLSDialog);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    horizontalLayout->addWidget(pushButton);


    gridLayout->addLayout(horizontalLayout, 4, 0, 1, 2);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 5, 0, 1, 1);


    retranslateUi(PLSDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), PLSDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), PLSDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(PLSDialog);
    } // setupUi

    void retranslateUi(QDialog *PLSDialog)
    {
    PLSDialog->setWindowTitle(QApplication::translate("PLSDialog", "Partial Least Squares (2 Blocks)", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("PLSDialog", "Select data for Block 1 and Block 2", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("PLSDialog", "Block 1", 0, QApplication::UnicodeUTF8));
    radioButton->setText(QApplication::translate("PLSDialog", "Shape variables", 0, QApplication::UnicodeUTF8));
    radioButton_2->setText(QApplication::translate("PLSDialog", "Form variables", 0, QApplication::UnicodeUTF8));
    radioButton_3->setText(QApplication::translate("PLSDialog", "Other variables", 0, QApplication::UnicodeUTF8));
    checkBox_2->setText(QApplication::translate("PLSDialog", "Centre columns", 0, QApplication::UnicodeUTF8));
    checkBox_3->setText(QApplication::translate("PLSDialog", "Scale columns to unit variances", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("PLSDialog", "Block 2", 0, QApplication::UnicodeUTF8));
    radioButton_4->setText(QApplication::translate("PLSDialog", "Shape variables", 0, QApplication::UnicodeUTF8));
    radioButton_5->setText(QApplication::translate("PLSDialog", "Form variables", 0, QApplication::UnicodeUTF8));
    radioButton_6->setText(QApplication::translate("PLSDialog", "Other variables", 0, QApplication::UnicodeUTF8));
    checkBox_4->setText(QApplication::translate("PLSDialog", "Centre columns", 0, QApplication::UnicodeUTF8));
    checkBox_5->setText(QApplication::translate("PLSDialog", "Scale columns to unit variances", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("PLSDialog", "Output file", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("PLSDialog", "Browse", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(PLSDialog);
    } // retranslateUi

};

namespace Ui {
    class PLSDialog: public Ui_PLSDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLS_H
