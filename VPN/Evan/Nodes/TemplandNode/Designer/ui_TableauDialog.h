/********************************************************************************
** Form generated from reading ui file 'TableauDialog.ui'
**
** Created: Thu 27. Aug 15:04:23 2009
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TABLEAUDIALOG_H
#define UI_TABLEAUDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TableauDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QLabel *m_pathLabel;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QLabel *m_recordsLabel;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *m_templateCheck;
    QCheckBox *m_targetCheck;
    QCheckBox *m_settingsCheck;
    QCheckBox *m_viewCheck;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TableauDialog)
    {
    if (TableauDialog->objectName().isEmpty())
        TableauDialog->setObjectName(QString::fromUtf8("TableauDialog"));
    TableauDialog->setWindowModality(Qt::ApplicationModal);
    TableauDialog->resize(327, 323);
    verticalLayout_2 = new QVBoxLayout(TableauDialog);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_2->addItem(horizontalSpacer);

    m_pathLabel = new QLabel(TableauDialog);
    m_pathLabel->setObjectName(QString::fromUtf8("m_pathLabel"));

    horizontalLayout_2->addWidget(m_pathLabel);

    horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_2->addItem(horizontalSpacer_3);


    verticalLayout_2->addLayout(horizontalLayout_2);

    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_3->addItem(horizontalSpacer_2);

    m_recordsLabel = new QLabel(TableauDialog);
    m_recordsLabel->setObjectName(QString::fromUtf8("m_recordsLabel"));

    horizontalLayout_3->addWidget(m_recordsLabel);

    horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_3->addItem(horizontalSpacer_4);


    verticalLayout_2->addLayout(horizontalLayout_3);

    groupBox = new QGroupBox(TableauDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    verticalLayout = new QVBoxLayout(groupBox);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    m_templateCheck = new QCheckBox(groupBox);
    m_templateCheck->setObjectName(QString::fromUtf8("m_templateCheck"));
    m_templateCheck->setChecked(true);

    verticalLayout->addWidget(m_templateCheck);

    m_targetCheck = new QCheckBox(groupBox);
    m_targetCheck->setObjectName(QString::fromUtf8("m_targetCheck"));
    m_targetCheck->setChecked(true);

    verticalLayout->addWidget(m_targetCheck);

    m_settingsCheck = new QCheckBox(groupBox);
    m_settingsCheck->setObjectName(QString::fromUtf8("m_settingsCheck"));
    m_settingsCheck->setChecked(true);

    verticalLayout->addWidget(m_settingsCheck);

    m_viewCheck = new QCheckBox(groupBox);
    m_viewCheck->setObjectName(QString::fromUtf8("m_viewCheck"));
    m_viewCheck->setChecked(true);

    verticalLayout->addWidget(m_viewCheck);


    verticalLayout_2->addWidget(groupBox);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout_2->addItem(verticalSpacer);

    buttonBox = new QDialogButtonBox(TableauDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout_2->addWidget(buttonBox);


    retranslateUi(TableauDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), TableauDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), TableauDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(TableauDialog);
    } // setupUi

    void retranslateUi(QDialog *TableauDialog)
    {
    TableauDialog->setWindowTitle(QApplication::translate("TableauDialog", "Load Tableau", 0, QApplication::UnicodeUTF8));
    m_pathLabel->setText(QApplication::translate("TableauDialog", "Tableau filename", 0, QApplication::UnicodeUTF8));
    m_recordsLabel->setText(QApplication::translate("TableauDialog", "# Record(s)", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("TableauDialog", "Loaded Fields", 0, QApplication::UnicodeUTF8));
    m_templateCheck->setText(QApplication::translate("TableauDialog", "Template", 0, QApplication::UnicodeUTF8));
    m_targetCheck->setText(QApplication::translate("TableauDialog", "Target", 0, QApplication::UnicodeUTF8));
    m_settingsCheck->setText(QApplication::translate("TableauDialog", "Settings", 0, QApplication::UnicodeUTF8));
    m_viewCheck->setText(QApplication::translate("TableauDialog", "View Mapping", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(TableauDialog);
    } // retranslateUi

};

namespace Ui {
    class TableauDialog: public Ui_TableauDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TABLEAUDIALOG_H
