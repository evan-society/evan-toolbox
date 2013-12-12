/********************************************************************************
** Form generated from reading ui file 'SelectDialog.ui'
**
** Created: Wed 22. Jul 11:14:25 2009
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SELECTDIALOG_H
#define UI_SELECTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include "../SpecimensModel.h"
#include "../LandmarkSetModel.h"

QT_BEGIN_NAMESPACE

class Ui_SelectDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    SpecimensView *m_specimensView;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    LandmarkSetView *m_landmarksView;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *m_exprLineEdit;
    QPushButton *m_exprButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SelectDialog)
    {
    if (SelectDialog->objectName().isEmpty())
        SelectDialog->setObjectName(QString::fromUtf8("SelectDialog"));
    SelectDialog->resize(710, 718);
    SelectDialog->setModal(true);
    verticalLayout_2 = new QVBoxLayout(SelectDialog);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    groupBox = new QGroupBox(SelectDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    horizontalLayout = new QHBoxLayout(groupBox);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    m_specimensView = new SpecimensView(groupBox);
    m_specimensView->setObjectName(QString::fromUtf8("m_specimensView"));

    horizontalLayout->addWidget(m_specimensView);


    horizontalLayout_2->addWidget(groupBox);

    groupBox_2 = new QGroupBox(SelectDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    verticalLayout = new QVBoxLayout(groupBox_2);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    m_landmarksView = new LandmarkSetView(groupBox_2);
    m_landmarksView->setObjectName(QString::fromUtf8("m_landmarksView"));

    verticalLayout->addWidget(m_landmarksView);


    horizontalLayout_2->addWidget(groupBox_2);


    verticalLayout_2->addLayout(horizontalLayout_2);

    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    m_exprLineEdit = new QLineEdit(SelectDialog);
    m_exprLineEdit->setObjectName(QString::fromUtf8("m_exprLineEdit"));

    horizontalLayout_3->addWidget(m_exprLineEdit);

    m_exprButton = new QPushButton(SelectDialog);
    m_exprButton->setObjectName(QString::fromUtf8("m_exprButton"));

    horizontalLayout_3->addWidget(m_exprButton);


    verticalLayout_2->addLayout(horizontalLayout_3);

    buttonBox = new QDialogButtonBox(SelectDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout_2->addWidget(buttonBox);


    retranslateUi(SelectDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), SelectDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), SelectDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(SelectDialog);
    } // setupUi

    void retranslateUi(QDialog *SelectDialog)
    {
    SelectDialog->setWindowTitle(QApplication::translate("SelectDialog", "Select Specimen(s)", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("SelectDialog", "Specimens", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("SelectDialog", "Landmarks", 0, QApplication::UnicodeUTF8));
    m_exprButton->setText(QApplication::translate("SelectDialog", "Add to Selection", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(SelectDialog);
    } // retranslateUi

};

namespace Ui {
    class SelectDialog: public Ui_SelectDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTDIALOG_H
