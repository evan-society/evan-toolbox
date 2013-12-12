/********************************************************************************
** Form generated from reading ui file 'SubtractDialog.ui'
**
** Created: Wed 4. Nov 20:14:08 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SUBTRACTDIALOG_H
#define UI_SUBTRACTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_SubtractDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_3;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBox;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *SubtractDialog)
    {
    if (SubtractDialog->objectName().isEmpty())
        SubtractDialog->setObjectName(QString::fromUtf8("SubtractDialog"));
    SubtractDialog->resize(400, 203);
    gridLayout = new QGridLayout(SubtractDialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label_4 = new QLabel(SubtractDialog);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    gridLayout->addWidget(label_4, 5, 0, 1, 1);

    label_5 = new QLabel(SubtractDialog);
    label_5->setObjectName(QString::fromUtf8("label_5"));

    gridLayout->addWidget(label_5, 6, 0, 1, 1);

    label_3 = new QLabel(SubtractDialog);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout->addWidget(label_3, 7, 0, 1, 1);

    gridLayout_2 = new QGridLayout();
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    label = new QLabel(SubtractDialog);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout_2->addWidget(label, 0, 0, 1, 1);

    lineEdit = new QLineEdit(SubtractDialog);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

    gridLayout_2->addWidget(lineEdit, 0, 2, 1, 1);

    label_2 = new QLabel(SubtractDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

    lineEdit_2 = new QLineEdit(SubtractDialog);
    lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

    gridLayout_2->addWidget(lineEdit_2, 1, 2, 1, 1);


    gridLayout->addLayout(gridLayout_2, 1, 0, 1, 1);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer, 1, 1, 1, 1);

    buttonBox = new QDialogButtonBox(SubtractDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 10, 1, 1, 1);

    checkBox = new QCheckBox(SubtractDialog);
    checkBox->setObjectName(QString::fromUtf8("checkBox"));

    gridLayout->addWidget(checkBox, 9, 0, 1, 2);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 8, 0, 1, 1);


    retranslateUi(SubtractDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), SubtractDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), SubtractDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(SubtractDialog);
    } // setupUi

    void retranslateUi(QDialog *SubtractDialog)
    {
    SubtractDialog->setWindowTitle(QApplication::translate("SubtractDialog", "Subtract", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("SubtractDialog", "Output is A * X -  B * Y", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("SubtractDialog", "X is the Target specimen", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("SubtractDialog", "Y is the Source specimen", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("SubtractDialog", "A", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("SubtractDialog", "B", 0, QApplication::UnicodeUTF8));
    checkBox->setText(QApplication::translate("SubtractDialog", "Form space (add natural log of centroid size to warp vector)", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(SubtractDialog);
    } // retranslateUi

};

namespace Ui {
    class SubtractDialog: public Ui_SubtractDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUBTRACTDIALOG_H
