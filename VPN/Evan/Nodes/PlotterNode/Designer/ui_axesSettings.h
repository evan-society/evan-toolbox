/********************************************************************************
** Form generated from reading ui file 'axes.ui'
**
** Created: Thu 3. Dec 21:22:18 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_AXES_H
#define UI_AXES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_AxisDialog
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *AxisDialog)
    {
    if (AxisDialog->objectName().isEmpty())
        AxisDialog->setObjectName(QString::fromUtf8("AxisDialog"));
    AxisDialog->resize(283, 169);
    gridLayout_2 = new QGridLayout(AxisDialog);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label_2 = new QLabel(AxisDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout->addWidget(label_2, 0, 0, 1, 1);

    label = new QLabel(AxisDialog);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 1, 0, 1, 1);

    lineEdit = new QLineEdit(AxisDialog);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

    gridLayout->addWidget(lineEdit, 0, 1, 1, 1);

    lineEdit_2 = new QLineEdit(AxisDialog);
    lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

    gridLayout->addWidget(lineEdit_2, 1, 1, 1, 1);

    label_3 = new QLabel(AxisDialog);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout->addWidget(label_3, 2, 0, 1, 1);

    label_4 = new QLabel(AxisDialog);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    gridLayout->addWidget(label_4, 3, 0, 1, 1);

    lineEdit_3 = new QLineEdit(AxisDialog);
    lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));

    gridLayout->addWidget(lineEdit_3, 2, 1, 1, 1);

    lineEdit_4 = new QLineEdit(AxisDialog);
    lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));

    gridLayout->addWidget(lineEdit_4, 3, 1, 1, 1);


    gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

    buttonBox = new QDialogButtonBox(AxisDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout_2->addWidget(buttonBox, 1, 0, 1, 1);


    retranslateUi(AxisDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), AxisDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), AxisDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(AxisDialog);
    } // setupUi

    void retranslateUi(QDialog *AxisDialog)
    {
    AxisDialog->setWindowTitle(QApplication::translate("AxisDialog", "Set Axes Limits", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("AxisDialog", "X-Axis minimum", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("AxisDialog", "X-Axis maximum", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("AxisDialog", "Y-Axis minimum", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("AxisDialog", "Y-Axis maximum", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(AxisDialog);
    } // retranslateUi

};

namespace Ui {
    class AxisDialog: public Ui_AxisDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AXES_H
