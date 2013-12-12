/********************************************************************************
** Form generated from reading ui file 'register.ui'
**
** Created: Tue 30. Mar 20:15:24 2010
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_REGISTER_H
#define UI_REGISTER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_RegisterDialog
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QLabel *label_2;
    QLabel *label;
    QLabel *label_4;
    QTextEdit *textEdit;
    QTextEdit *textEdit_2;

    void setupUi(QDialog *RegisterDialog)
    {
    if (RegisterDialog->objectName().isEmpty())
        RegisterDialog->setObjectName(QString::fromUtf8("RegisterDialog"));
    RegisterDialog->resize(388, 349);
    gridLayout = new QGridLayout(RegisterDialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    buttonBox = new QDialogButtonBox(RegisterDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 6, 0, 1, 1);

    label_2 = new QLabel(RegisterDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout->addWidget(label_2, 2, 0, 1, 1);

    label = new QLabel(RegisterDialog);
    label->setObjectName(QString::fromUtf8("label"));
    QPalette palette;
    QBrush brush(QColor(255, 0, 0, 255));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    QBrush brush1(QColor(0, 0, 0, 255));
    brush1.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Text, brush1);
    palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Text, brush1);
    QBrush brush2(QColor(118, 116, 108, 255));
    brush2.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
    palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
    label->setPalette(palette);
    QFont font;
    font.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font.setPointSize(8);
    label->setFont(font);

    gridLayout->addWidget(label, 0, 0, 1, 1);

    label_4 = new QLabel(RegisterDialog);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    gridLayout->addWidget(label_4, 4, 0, 1, 1);

    textEdit = new QTextEdit(RegisterDialog);
    textEdit->setObjectName(QString::fromUtf8("textEdit"));
    textEdit->setReadOnly(true);

    gridLayout->addWidget(textEdit, 3, 0, 1, 1);

    textEdit_2 = new QTextEdit(RegisterDialog);
    textEdit_2->setObjectName(QString::fromUtf8("textEdit_2"));

    gridLayout->addWidget(textEdit_2, 5, 0, 1, 1);


    retranslateUi(RegisterDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), RegisterDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), RegisterDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(RegisterDialog);
    } // setupUi

    void retranslateUi(QDialog *RegisterDialog)
    {
    RegisterDialog->setWindowTitle(QApplication::translate("RegisterDialog", "Register", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("RegisterDialog", "To register please send the following code", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("RegisterDialog", "Unregistered Copy", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("RegisterDialog", "We will send you a key which you must enter below", 0, QApplication::UnicodeUTF8));
    textEdit->setHtml(QApplication::translate("RegisterDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:12pt; font-weight:600;\">00:00:00:00:00:00</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(RegisterDialog);
    } // retranslateUi

};

namespace Ui {
    class RegisterDialog: public Ui_RegisterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTER_H
