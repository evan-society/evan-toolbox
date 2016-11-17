/********************************************************************************
** Form generated from reading ui file 'RDialog.ui'
**
** Created: Mon 20. Jul 14:34:05 2009
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_RDIALOG_H
#define UI_RDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    QTableWidget *m_inputsTable;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QPlainTextEdit *m_rCommandArea;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *RDialog)
    {
    if (RDialog->objectName().isEmpty())
        RDialog->setObjectName(QString::fromUtf8("RDialog"));
    RDialog->resize(741, 611);
    verticalLayout_2 = new QVBoxLayout(RDialog);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    groupBox_2 = new QGroupBox(RDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setMaximumSize(QSize(231, 16777215));
    verticalLayout = new QVBoxLayout(groupBox_2);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    m_inputsTable = new QTableWidget(groupBox_2);
    if (m_inputsTable->columnCount() < 2)
        m_inputsTable->setColumnCount(2);
    QTableWidgetItem *__colItem = new QTableWidgetItem();
    m_inputsTable->setHorizontalHeaderItem(0, __colItem);
    QTableWidgetItem *__colItem1 = new QTableWidgetItem();
    m_inputsTable->setHorizontalHeaderItem(1, __colItem1);
    m_inputsTable->setObjectName(QString::fromUtf8("m_inputsTable"));
    m_inputsTable->setAlternatingRowColors(true);
    m_inputsTable->setSelectionMode(QAbstractItemView::SingleSelection);

    verticalLayout->addWidget(m_inputsTable);


    horizontalLayout_2->addWidget(groupBox_2);

    groupBox = new QGroupBox(RDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    horizontalLayout = new QHBoxLayout(groupBox);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    m_rCommandArea = new QPlainTextEdit(groupBox);
    m_rCommandArea->setObjectName(QString::fromUtf8("m_rCommandArea"));

    horizontalLayout->addWidget(m_rCommandArea);


    horizontalLayout_2->addWidget(groupBox);


    verticalLayout_2->addLayout(horizontalLayout_2);

    buttonBox = new QDialogButtonBox(RDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout_2->addWidget(buttonBox);


    retranslateUi(RDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), RDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), RDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(RDialog);
    } // setupUi

    void retranslateUi(QDialog *RDialog)
    {
    RDialog->setWindowTitle(QApplication::translate("RDialog", "R Node", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("RDialog", "Inputs", 0, QApplication::UnicodeUTF8));
    m_inputsTable->horizontalHeaderItem(0)->setText(QApplication::translate("RDialog", "Variable Name", 0, QApplication::UnicodeUTF8));
    m_inputsTable->horizontalHeaderItem(1)->setText(QApplication::translate("RDialog", "Description", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("RDialog", "R Code", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(RDialog);
    } // retranslateUi

};

namespace Ui {
    class RDialog: public Ui_RDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RDIALOG_H
