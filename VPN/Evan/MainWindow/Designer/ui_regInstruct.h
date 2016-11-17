/********************************************************************************
** Form generated from reading ui file 'registerInstructions.ui'
**
** Created: Thu 1. Apr 14:05:31 2010
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_REGISTERINSTRUCTIONS_H
#define UI_REGISTERINSTRUCTIONS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_3;
    QPushButton *pushButton;

    void setupUi(QDialog *Dialog)
    {
    if (Dialog->objectName().isEmpty())
        Dialog->setObjectName(QString::fromUtf8("Dialog"));
    Dialog->resize(443, 162);
    gridLayout = new QGridLayout(Dialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label = new QLabel(Dialog);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 2);

    label_3 = new QLabel(Dialog);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout->addWidget(label_3, 1, 0, 1, 2);

    pushButton = new QPushButton(Dialog);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    gridLayout->addWidget(pushButton, 2, 1, 1, 1);


    retranslateUi(Dialog);

    QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Evan Registration Instructions", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("Dialog", "To register this copy of the Evan Toolbox\n"
"Select Help->Register from the file menu\n"
"", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("Dialog", "There you will get a request code for a license.\n"
"To obtain a license you need to log onto the members area of www.evan-society.org", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("Dialog", "Close", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Dialog);
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERINSTRUCTIONS_H
