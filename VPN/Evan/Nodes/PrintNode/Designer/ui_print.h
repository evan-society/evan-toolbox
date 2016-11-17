/********************************************************************************
** Form generated from reading ui file 'print.ui'
**
** Created: Tue 27. Sep 20:30:53 2011
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PRINT_H
#define UI_PRINT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_PrintDialog
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit_2;

    void setupUi(QDialog *PrintDialog)
    {
    if (PrintDialog->objectName().isEmpty())
        PrintDialog->setObjectName(QString::fromUtf8("PrintDialog"));
    PrintDialog->resize(573, 415);
    gridLayout = new QGridLayout(PrintDialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    buttonBox = new QDialogButtonBox(PrintDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 2, 0, 1, 1);

    textEdit_2 = new QTextEdit(PrintDialog);
    textEdit_2->setObjectName(QString::fromUtf8("textEdit_2"));

    gridLayout->addWidget(textEdit_2, 1, 0, 1, 1);


    retranslateUi(PrintDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), PrintDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), PrintDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(PrintDialog);
    } // setupUi

    void retranslateUi(QDialog *PrintDialog)
    {
    PrintDialog->setWindowTitle(QApplication::translate("PrintDialog", "Print Output", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(PrintDialog);
    } // retranslateUi

};

namespace Ui {
    class PrintDialog: public Ui_PrintDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRINT_H
