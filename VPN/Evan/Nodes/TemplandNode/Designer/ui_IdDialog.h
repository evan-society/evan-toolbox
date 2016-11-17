/********************************************************************************
** Form generated from reading ui file 'IdDialog.ui'
**
** Created: Tue 8. Sep 16:24:44 2009
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_IDDIALOG_H
#define UI_IDDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_IdDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *m_idLineEdit;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *m_labelLineEdit;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *IdDialog)
    {
    if (IdDialog->objectName().isEmpty())
        IdDialog->setObjectName(QString::fromUtf8("IdDialog"));
    IdDialog->setWindowModality(Qt::ApplicationModal);
    IdDialog->resize(376, 113);
    IdDialog->setMaximumSize(QSize(376, 113));
    IdDialog->setSizeGripEnabled(false);
    IdDialog->setModal(true);
    verticalLayout = new QVBoxLayout(IdDialog);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    label = new QLabel(IdDialog);
    label->setObjectName(QString::fromUtf8("label"));

    horizontalLayout->addWidget(label);

    m_idLineEdit = new QLineEdit(IdDialog);
    m_idLineEdit->setObjectName(QString::fromUtf8("m_idLineEdit"));

    horizontalLayout->addWidget(m_idLineEdit);


    verticalLayout->addLayout(horizontalLayout);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    label_2 = new QLabel(IdDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    horizontalLayout_2->addWidget(label_2);

    m_labelLineEdit = new QLineEdit(IdDialog);
    m_labelLineEdit->setObjectName(QString::fromUtf8("m_labelLineEdit"));

    horizontalLayout_2->addWidget(m_labelLineEdit);


    verticalLayout->addLayout(horizontalLayout_2);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer);

    buttonBox = new QDialogButtonBox(IdDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout->addWidget(buttonBox);


    retranslateUi(IdDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), IdDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), IdDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(IdDialog);
    } // setupUi

    void retranslateUi(QDialog *IdDialog)
    {
    IdDialog->setWindowTitle(QApplication::translate("IdDialog", "Edit Details", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("IdDialog", "Item ID     : ", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("IdDialog", "Item Label :", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(IdDialog);
    } // retranslateUi

};

namespace Ui {
    class IdDialog: public Ui_IdDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IDDIALOG_H
