/********************************************************************************
** Form generated from reading ui file 'CreateGrid.ui'
**
** Created: Fri 6. Nov 11:54:50 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CREATEGRID_H
#define UI_CREATEGRID_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_CreateGridDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label_15;
    QLineEdit *lineEdit;
    QLabel *label_13;
    QLineEdit *lineEdit_2;
    QLabel *label_2;
    QLineEdit *line1;
    QLabel *label_3;
    QLineEdit *line2;
    QRadioButton *radioButton;
    QRadioButton *radioButton_4;
    QLabel *label_11;
    QRadioButton *radioButton_2;
    QSpinBox *spinBox_2;
    QDialogButtonBox *buttonBox;
    QSpinBox *spinBox;
    QRadioButton *radioButton_5;
    QRadioButton *radioButton_9;
    QLabel *label_5;
    QLabel *label_7;
    QLabel *label_8;
    QRadioButton *radioButton_3;
    QSpinBox *spinBox_3;
    QRadioButton *radioButton_6;

    void setupUi(QDialog *CreateGridDialog)
    {
    if (CreateGridDialog->objectName().isEmpty())
        CreateGridDialog->setObjectName(QString::fromUtf8("CreateGridDialog"));
    CreateGridDialog->resize(545, 197);
    gridLayout = new QGridLayout(CreateGridDialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label_15 = new QLabel(CreateGridDialog);
    label_15->setObjectName(QString::fromUtf8("label_15"));

    gridLayout->addWidget(label_15, 0, 0, 1, 1);

    lineEdit = new QLineEdit(CreateGridDialog);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

    gridLayout->addWidget(lineEdit, 0, 1, 1, 4);

    label_13 = new QLabel(CreateGridDialog);
    label_13->setObjectName(QString::fromUtf8("label_13"));

    gridLayout->addWidget(label_13, 0, 5, 1, 4);

    lineEdit_2 = new QLineEdit(CreateGridDialog);
    lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

    gridLayout->addWidget(lineEdit_2, 0, 9, 1, 4);

    label_2 = new QLabel(CreateGridDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout->addWidget(label_2, 1, 0, 1, 1);

    line1 = new QLineEdit(CreateGridDialog);
    line1->setObjectName(QString::fromUtf8("line1"));

    gridLayout->addWidget(line1, 1, 1, 1, 4);

    label_3 = new QLabel(CreateGridDialog);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout->addWidget(label_3, 1, 5, 1, 4);

    line2 = new QLineEdit(CreateGridDialog);
    line2->setObjectName(QString::fromUtf8("line2"));

    gridLayout->addWidget(line2, 1, 9, 1, 4);

    radioButton = new QRadioButton(CreateGridDialog);
    radioButton->setObjectName(QString::fromUtf8("radioButton"));

    gridLayout->addWidget(radioButton, 4, 0, 1, 2);

    radioButton_4 = new QRadioButton(CreateGridDialog);
    radioButton_4->setObjectName(QString::fromUtf8("radioButton_4"));
    radioButton_4->setChecked(true);

    gridLayout->addWidget(radioButton_4, 4, 10, 1, 2);

    label_11 = new QLabel(CreateGridDialog);
    label_11->setObjectName(QString::fromUtf8("label_11"));

    gridLayout->addWidget(label_11, 4, 12, 1, 1);

    radioButton_2 = new QRadioButton(CreateGridDialog);
    radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

    gridLayout->addWidget(radioButton_2, 5, 0, 1, 2);

    spinBox_2 = new QSpinBox(CreateGridDialog);
    spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));

    gridLayout->addWidget(spinBox_2, 5, 7, 1, 1);

    buttonBox = new QDialogButtonBox(CreateGridDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 8, 0, 1, 13);

    spinBox = new QSpinBox(CreateGridDialog);
    spinBox->setObjectName(QString::fromUtf8("spinBox"));

    gridLayout->addWidget(spinBox, 4, 7, 1, 1);

    radioButton_5 = new QRadioButton(CreateGridDialog);
    radioButton_5->setObjectName(QString::fromUtf8("radioButton_5"));

    gridLayout->addWidget(radioButton_5, 5, 10, 1, 1);

    radioButton_9 = new QRadioButton(CreateGridDialog);
    radioButton_9->setObjectName(QString::fromUtf8("radioButton_9"));
    radioButton_9->setChecked(false);

    gridLayout->addWidget(radioButton_9, 3, 0, 1, 1);

    label_5 = new QLabel(CreateGridDialog);
    label_5->setObjectName(QString::fromUtf8("label_5"));

    gridLayout->addWidget(label_5, 4, 2, 1, 1);

    label_7 = new QLabel(CreateGridDialog);
    label_7->setObjectName(QString::fromUtf8("label_7"));

    gridLayout->addWidget(label_7, 5, 2, 1, 1);

    label_8 = new QLabel(CreateGridDialog);
    label_8->setObjectName(QString::fromUtf8("label_8"));

    gridLayout->addWidget(label_8, 6, 2, 1, 1);

    radioButton_3 = new QRadioButton(CreateGridDialog);
    radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));

    gridLayout->addWidget(radioButton_3, 6, 0, 1, 1);

    spinBox_3 = new QSpinBox(CreateGridDialog);
    spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));

    gridLayout->addWidget(spinBox_3, 6, 7, 1, 1);

    radioButton_6 = new QRadioButton(CreateGridDialog);
    radioButton_6->setObjectName(QString::fromUtf8("radioButton_6"));

    gridLayout->addWidget(radioButton_6, 6, 10, 1, 1);


    retranslateUi(CreateGridDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), CreateGridDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), CreateGridDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(CreateGridDialog);
    } // setupUi

    void retranslateUi(QDialog *CreateGridDialog)
    {
    CreateGridDialog->setWindowTitle(QApplication::translate("CreateGridDialog", "Dialog", 0, QApplication::UnicodeUTF8));
    label_15->setText(QApplication::translate("CreateGridDialog", "Name:", 0, QApplication::UnicodeUTF8));
    lineEdit->setText(QApplication::translate("CreateGridDialog", "Grid", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("CreateGridDialog", "Size in GPA unit:", 0, QApplication::UnicodeUTF8));
    lineEdit_2->setText(QApplication::translate("CreateGridDialog", "1.0", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("CreateGridDialog", "Number of rows:", 0, QApplication::UnicodeUTF8));
    line1->setText(QApplication::translate("CreateGridDialog", "15", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("CreateGridDialog", "Number of columns:", 0, QApplication::UnicodeUTF8));
    line2->setText(QApplication::translate("CreateGridDialog", "15", 0, QApplication::UnicodeUTF8));
    radioButton->setText(QApplication::translate("CreateGridDialog", "One Landmark", 0, QApplication::UnicodeUTF8));
    radioButton_4->setText(QApplication::translate("CreateGridDialog", "X-Y plane", 0, QApplication::UnicodeUTF8));
    label_11->setText(QString());
    radioButton_2->setText(QApplication::translate("CreateGridDialog", "Two landmarks", 0, QApplication::UnicodeUTF8));
    radioButton_5->setText(QApplication::translate("CreateGridDialog", "X-Z plane", 0, QApplication::UnicodeUTF8));
    radioButton_9->setText(QApplication::translate("CreateGridDialog", "Initialization at the centre", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("CreateGridDialog", "First landmark index:", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("CreateGridDialog", "Second landmark index", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("CreateGridDialog", "Third landmark index", 0, QApplication::UnicodeUTF8));
    radioButton_3->setText(QApplication::translate("CreateGridDialog", "Three landmarks", 0, QApplication::UnicodeUTF8));
    radioButton_6->setText(QApplication::translate("CreateGridDialog", "Y-Z plane", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(CreateGridDialog);
    } // retranslateUi

};

namespace Ui {
    class CreateGridDialog: public Ui_CreateGridDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEGRID_H
