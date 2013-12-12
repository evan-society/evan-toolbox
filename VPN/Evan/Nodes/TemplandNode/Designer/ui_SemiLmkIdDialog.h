/********************************************************************************
** Form generated from reading ui file 'SemiLmkIdDialog.ui'
**
** Created: Sat 16. Oct 11:41:59 2010
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SEMILMKIDDIALOG_H
#define UI_SEMILMKIDDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SemiLmkIdDialog
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
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *comboBox;
    QSpacerItem *verticalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SemiLmkIdDialog)
    {
    if (SemiLmkIdDialog->objectName().isEmpty())
        SemiLmkIdDialog->setObjectName(QString::fromUtf8("SemiLmkIdDialog"));
    SemiLmkIdDialog->setWindowModality(Qt::ApplicationModal);
    SemiLmkIdDialog->resize(376, 157);
    SemiLmkIdDialog->setSizeGripEnabled(false);
    SemiLmkIdDialog->setModal(true);
    verticalLayout = new QVBoxLayout(SemiLmkIdDialog);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    label = new QLabel(SemiLmkIdDialog);
    label->setObjectName(QString::fromUtf8("label"));

    horizontalLayout->addWidget(label);

    m_idLineEdit = new QLineEdit(SemiLmkIdDialog);
    m_idLineEdit->setObjectName(QString::fromUtf8("m_idLineEdit"));

    horizontalLayout->addWidget(m_idLineEdit);


    verticalLayout->addLayout(horizontalLayout);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    label_2 = new QLabel(SemiLmkIdDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    horizontalLayout_2->addWidget(label_2);

    m_labelLineEdit = new QLineEdit(SemiLmkIdDialog);
    m_labelLineEdit->setObjectName(QString::fromUtf8("m_labelLineEdit"));

    horizontalLayout_2->addWidget(m_labelLineEdit);


    verticalLayout->addLayout(horizontalLayout_2);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer);

    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    label_3 = new QLabel(SemiLmkIdDialog);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    horizontalLayout_3->addWidget(label_3);

    comboBox = new QComboBox(SemiLmkIdDialog);
    comboBox->setObjectName(QString::fromUtf8("comboBox"));

    horizontalLayout_3->addWidget(comboBox);


    verticalLayout->addLayout(horizontalLayout_3);

    verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer_2);

    buttonBox = new QDialogButtonBox(SemiLmkIdDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout->addWidget(buttonBox);


    retranslateUi(SemiLmkIdDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), SemiLmkIdDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), SemiLmkIdDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(SemiLmkIdDialog);
    } // setupUi

    void retranslateUi(QDialog *SemiLmkIdDialog)
    {
    SemiLmkIdDialog->setWindowTitle(QApplication::translate("SemiLmkIdDialog", "Edit Details", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("SemiLmkIdDialog", "Item ID     : ", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("SemiLmkIdDialog", "Item Label :", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("SemiLmkIdDialog", "Embed with item:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(SemiLmkIdDialog);
    } // retranslateUi

};

namespace Ui {
    class SemiLmkIdDialog: public Ui_SemiLmkIdDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEMILMKIDDIALOG_H
