/********************************************************************************
** Form generated from reading ui file 'GroupDialog.ui'
**
** Created: Mon 12. Oct 10:35:20 2009
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_GROUPDIALOG_H
#define UI_GROUPDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include "../../SelectNode/SpecimensModel.h"
#include "../GroupTreeWidget.h"

QT_BEGIN_NAMESPACE

class Ui_GroupDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QCheckBox *m_autoMeanCheck;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    SpecimensView *m_specimensView;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    GroupTreeWidget *m_groupsView;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *m_newGrpBtn;
    QPushButton *m_delGroupBtn;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *m_exprLineEdit;
    QPushButton *m_exprButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *GroupDialog)
    {
    if (GroupDialog->objectName().isEmpty())
        GroupDialog->setObjectName(QString::fromUtf8("GroupDialog"));
    GroupDialog->resize(711, 737);
    GroupDialog->setModal(true);
    verticalLayout_3 = new QVBoxLayout(GroupDialog);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    m_autoMeanCheck = new QCheckBox(GroupDialog);
    m_autoMeanCheck->setObjectName(QString::fromUtf8("m_autoMeanCheck"));
    m_autoMeanCheck->setChecked(true);

    verticalLayout_3->addWidget(m_autoMeanCheck);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    groupBox = new QGroupBox(GroupDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    horizontalLayout = new QHBoxLayout(groupBox);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    m_specimensView = new SpecimensView(groupBox);
    m_specimensView->setObjectName(QString::fromUtf8("m_specimensView"));

    horizontalLayout->addWidget(m_specimensView);


    horizontalLayout_2->addWidget(groupBox);

    verticalLayout_2 = new QVBoxLayout();
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout_2->addItem(verticalSpacer);

    m_addButton = new QPushButton(GroupDialog);
    m_addButton->setObjectName(QString::fromUtf8("m_addButton"));
    m_addButton->setEnabled(false);

    verticalLayout_2->addWidget(m_addButton);

    m_removeButton = new QPushButton(GroupDialog);
    m_removeButton->setObjectName(QString::fromUtf8("m_removeButton"));
    m_removeButton->setEnabled(false);

    verticalLayout_2->addWidget(m_removeButton);

    verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout_2->addItem(verticalSpacer_2);


    horizontalLayout_2->addLayout(verticalLayout_2);

    groupBox_2 = new QGroupBox(GroupDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    verticalLayout = new QVBoxLayout(groupBox_2);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    m_groupsView = new GroupTreeWidget(groupBox_2);
    m_groupsView->setObjectName(QString::fromUtf8("m_groupsView"));

    verticalLayout->addWidget(m_groupsView);

    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
    m_newGrpBtn = new QPushButton(groupBox_2);
    m_newGrpBtn->setObjectName(QString::fromUtf8("m_newGrpBtn"));

    horizontalLayout_4->addWidget(m_newGrpBtn);

    m_delGroupBtn = new QPushButton(groupBox_2);
    m_delGroupBtn->setObjectName(QString::fromUtf8("m_delGroupBtn"));
    m_delGroupBtn->setEnabled(false);

    horizontalLayout_4->addWidget(m_delGroupBtn);


    verticalLayout->addLayout(horizontalLayout_4);


    horizontalLayout_2->addWidget(groupBox_2);


    verticalLayout_3->addLayout(horizontalLayout_2);

    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    m_exprLineEdit = new QLineEdit(GroupDialog);
    m_exprLineEdit->setObjectName(QString::fromUtf8("m_exprLineEdit"));

    horizontalLayout_3->addWidget(m_exprLineEdit);

    m_exprButton = new QPushButton(GroupDialog);
    m_exprButton->setObjectName(QString::fromUtf8("m_exprButton"));

    horizontalLayout_3->addWidget(m_exprButton);


    verticalLayout_3->addLayout(horizontalLayout_3);

    buttonBox = new QDialogButtonBox(GroupDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout_3->addWidget(buttonBox);


    retranslateUi(GroupDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), GroupDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), GroupDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(GroupDialog);
    } // setupUi

    void retranslateUi(QDialog *GroupDialog)
    {
    GroupDialog->setWindowTitle(QApplication::translate("GroupDialog", "Group Specimens", 0, QApplication::UnicodeUTF8));
    m_autoMeanCheck->setText(QApplication::translate("GroupDialog", "Automatically Append A Group Mean", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("GroupDialog", "Specimens", 0, QApplication::UnicodeUTF8));
    m_addButton->setText(QApplication::translate("GroupDialog", ">>", 0, QApplication::UnicodeUTF8));
    m_removeButton->setText(QApplication::translate("GroupDialog", "<<", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("GroupDialog", "Groups", 0, QApplication::UnicodeUTF8));
    m_groupsView->headerItem()->setText(0, QApplication::translate("GroupDialog", "Name", 0, QApplication::UnicodeUTF8));
    m_groupsView->headerItem()->setText(1, QApplication::translate("GroupDialog", "Size", 0, QApplication::UnicodeUTF8));
    m_groupsView->headerItem()->setText(2, QApplication::translate("GroupDialog", "Plot Symbol", 0, QApplication::UnicodeUTF8));
    m_newGrpBtn->setText(QApplication::translate("GroupDialog", "New Group", 0, QApplication::UnicodeUTF8));
    m_delGroupBtn->setText(QApplication::translate("GroupDialog", "Delete Group", 0, QApplication::UnicodeUTF8));
    m_exprButton->setText(QApplication::translate("GroupDialog", "Add to Selection", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(GroupDialog);
    } // retranslateUi

};

namespace Ui {
    class GroupDialog: public Ui_GroupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GROUPDIALOG_H
