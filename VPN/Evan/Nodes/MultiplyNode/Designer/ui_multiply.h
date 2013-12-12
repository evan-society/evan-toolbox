/********************************************************************************
** Form generated from reading ui file 'multiply.ui'
**
** Created: Thu Jul 29 21:25:20 2010
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MULTIPLY_H
#define UI_MULTIPLY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_multiplyDialog
{
public:
    QDialogButtonBox *buttonBox;
    QDoubleSpinBox *doubleSpinBox;
    QLabel *label;

    void setupUi(QDialog *multiplyDialog)
    {
    if (multiplyDialog->objectName().isEmpty())
        multiplyDialog->setObjectName(QString::fromUtf8("multiplyDialog"));
    multiplyDialog->resize(228, 151);
    buttonBox = new QDialogButtonBox(multiplyDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(20, 100, 181, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    doubleSpinBox = new QDoubleSpinBox(multiplyDialog);
    doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
    doubleSpinBox->setGeometry(QRect(80, 40, 81, 22));
    doubleSpinBox->setMaximum(1e+09);
    doubleSpinBox->setValue(1);
    label = new QLabel(multiplyDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(30, 20, 131, 16));

    retranslateUi(multiplyDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), multiplyDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), multiplyDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(multiplyDialog);
    } // setupUi

    void retranslateUi(QDialog *multiplyDialog)
    {
    multiplyDialog->setWindowTitle(QApplication::translate("multiplyDialog", "Multiply", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("multiplyDialog", "Multiply value:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(multiplyDialog);
    } // retranslateUi

};

namespace Ui {
    class multiplyDialog: public Ui_multiplyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MULTIPLY_H
