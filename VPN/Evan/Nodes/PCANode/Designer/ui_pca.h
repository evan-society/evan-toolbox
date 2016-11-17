/********************************************************************************
** Form generated from reading ui file 'pca.ui'
**
** Created: Sat 10. Oct 09:47:27 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PCA_H
#define UI_PCA_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PCADialog
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout;
    QCheckBox *m_centerCheckBox;
    QCheckBox *m_scaleCheckBox;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QCheckBox *checkBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QDialogButtonBox *buttonBox;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *PCADialog)
    {
    if (PCADialog->objectName().isEmpty())
        PCADialog->setObjectName(QString::fromUtf8("PCADialog"));
    PCADialog->resize(356, 353);
    gridLayout = new QGridLayout(PCADialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    groupBox_3 = new QGroupBox(PCADialog);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    verticalLayout = new QVBoxLayout(groupBox_3);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    m_centerCheckBox = new QCheckBox(groupBox_3);
    m_centerCheckBox->setObjectName(QString::fromUtf8("m_centerCheckBox"));
    m_centerCheckBox->setChecked(true);

    verticalLayout->addWidget(m_centerCheckBox);

    m_scaleCheckBox = new QCheckBox(groupBox_3);
    m_scaleCheckBox->setObjectName(QString::fromUtf8("m_scaleCheckBox"));

    verticalLayout->addWidget(m_scaleCheckBox);


    gridLayout->addWidget(groupBox_3, 0, 0, 1, 1);

    groupBox = new QGroupBox(PCADialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    verticalLayout_3 = new QVBoxLayout(groupBox);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    radioButton = new QRadioButton(groupBox);
    radioButton->setObjectName(QString::fromUtf8("radioButton"));
    radioButton->setChecked(true);

    verticalLayout_3->addWidget(radioButton);

    radioButton_2 = new QRadioButton(groupBox);
    radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

    verticalLayout_3->addWidget(radioButton_2);


    gridLayout->addWidget(groupBox, 1, 0, 1, 1);

    groupBox_2 = new QGroupBox(PCADialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    gridLayout_2 = new QGridLayout(groupBox_2);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    checkBox = new QCheckBox(groupBox_2);
    checkBox->setObjectName(QString::fromUtf8("checkBox"));

    gridLayout_2->addWidget(checkBox, 0, 0, 1, 1);


    gridLayout->addWidget(groupBox_2, 2, 0, 1, 1);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    label = new QLabel(PCADialog);
    label->setObjectName(QString::fromUtf8("label"));

    horizontalLayout->addWidget(label);

    lineEdit = new QLineEdit(PCADialog);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

    horizontalLayout->addWidget(lineEdit);

    pushButton = new QPushButton(PCADialog);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    horizontalLayout->addWidget(pushButton);


    gridLayout->addLayout(horizontalLayout, 3, 0, 1, 1);

    buttonBox = new QDialogButtonBox(PCADialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 5, 0, 1, 1);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 4, 0, 1, 1);


    retranslateUi(PCADialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), PCADialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), PCADialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(PCADialog);
    } // setupUi

    void retranslateUi(QDialog *PCADialog)
    {
    PCADialog->setWindowTitle(QApplication::translate("PCADialog", "Principal Component Analysis", 0, QApplication::UnicodeUTF8));
    groupBox_3->setTitle(QApplication::translate("PCADialog", "Pre-processing of data", 0, QApplication::UnicodeUTF8));
    m_centerCheckBox->setText(QApplication::translate("PCADialog", "Centre columns", 0, QApplication::UnicodeUTF8));
    m_scaleCheckBox->setText(QApplication::translate("PCADialog", "Scale columns to unit variances", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("PCADialog", "Scaling of eigenvectors", 0, QApplication::UnicodeUTF8));
    radioButton->setText(QApplication::translate("PCADialog", "Scale to unit variances", 0, QApplication::UnicodeUTF8));
    radioButton_2->setText(QApplication::translate("PCADialog", "Scale to eigenvalues", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("PCADialog", "Form Space", 0, QApplication::UnicodeUTF8));
    checkBox->setText(QApplication::translate("PCADialog", "Include natural log of Centroid Sizes", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("PCADialog", "Output file", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("PCADialog", "Browse", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(PCADialog);
    } // retranslateUi

};

namespace Ui {
    class PCADialog: public Ui_PCADialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCA_H
