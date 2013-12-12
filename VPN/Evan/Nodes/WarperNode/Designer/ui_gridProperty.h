/********************************************************************************
** Form generated from reading ui file 'GridProperty.ui'
**
** Created: Sat 7. Nov 12:02:46 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_GRIDPROPERTY_H
#define UI_GRIDPROPERTY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_GridProperty
{
public:
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLineEdit *line1;
    QLineEdit *lineEdit;
    QLabel *label_15;
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLineEdit *mat00;
    QLineEdit *mat01;
    QLineEdit *mat02;
    QLineEdit *mat03;
    QLineEdit *mat33;
    QLineEdit *mat32;
    QLineEdit *mat31;
    QLineEdit *mat30;
    QLineEdit *mat10;
    QLineEdit *mat11;
    QLineEdit *mat12;
    QLineEdit *mat13;
    QLineEdit *mat20;
    QLineEdit *mat21;
    QLineEdit *mat22;
    QLineEdit *mat23;
    QLabel *label_13;
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QLineEdit *line2;

    void setupUi(QDialog *GridProperty)
    {
    if (GridProperty->objectName().isEmpty())
        GridProperty->setObjectName(QString::fromUtf8("GridProperty"));
    GridProperty->resize(588, 233);
    gridLayout = new QGridLayout(GridProperty);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label_2 = new QLabel(GridProperty);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout->addWidget(label_2, 3, 0, 1, 1);

    line1 = new QLineEdit(GridProperty);
    line1->setObjectName(QString::fromUtf8("line1"));

    gridLayout->addWidget(line1, 3, 1, 1, 1);

    lineEdit = new QLineEdit(GridProperty);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

    gridLayout->addWidget(lineEdit, 1, 1, 1, 1);

    label_15 = new QLabel(GridProperty);
    label_15->setObjectName(QString::fromUtf8("label_15"));

    gridLayout->addWidget(label_15, 1, 0, 1, 1);

    buttonBox = new QDialogButtonBox(GridProperty);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 9, 3, 1, 1);

    groupBox = new QGroupBox(GridProperty);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout_2 = new QGridLayout(groupBox);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    mat00 = new QLineEdit(groupBox);
    mat00->setObjectName(QString::fromUtf8("mat00"));

    gridLayout_2->addWidget(mat00, 0, 0, 1, 1);

    mat01 = new QLineEdit(groupBox);
    mat01->setObjectName(QString::fromUtf8("mat01"));

    gridLayout_2->addWidget(mat01, 0, 1, 1, 1);

    mat02 = new QLineEdit(groupBox);
    mat02->setObjectName(QString::fromUtf8("mat02"));

    gridLayout_2->addWidget(mat02, 0, 2, 1, 1);

    mat03 = new QLineEdit(groupBox);
    mat03->setObjectName(QString::fromUtf8("mat03"));

    gridLayout_2->addWidget(mat03, 0, 3, 1, 1);

    mat33 = new QLineEdit(groupBox);
    mat33->setObjectName(QString::fromUtf8("mat33"));

    gridLayout_2->addWidget(mat33, 3, 3, 1, 1);

    mat32 = new QLineEdit(groupBox);
    mat32->setObjectName(QString::fromUtf8("mat32"));

    gridLayout_2->addWidget(mat32, 3, 2, 1, 1);

    mat31 = new QLineEdit(groupBox);
    mat31->setObjectName(QString::fromUtf8("mat31"));

    gridLayout_2->addWidget(mat31, 3, 1, 1, 1);

    mat30 = new QLineEdit(groupBox);
    mat30->setObjectName(QString::fromUtf8("mat30"));

    gridLayout_2->addWidget(mat30, 3, 0, 1, 1);

    mat10 = new QLineEdit(groupBox);
    mat10->setObjectName(QString::fromUtf8("mat10"));

    gridLayout_2->addWidget(mat10, 1, 0, 1, 1);

    mat11 = new QLineEdit(groupBox);
    mat11->setObjectName(QString::fromUtf8("mat11"));

    gridLayout_2->addWidget(mat11, 1, 1, 1, 1);

    mat12 = new QLineEdit(groupBox);
    mat12->setObjectName(QString::fromUtf8("mat12"));

    gridLayout_2->addWidget(mat12, 1, 2, 1, 1);

    mat13 = new QLineEdit(groupBox);
    mat13->setObjectName(QString::fromUtf8("mat13"));

    gridLayout_2->addWidget(mat13, 1, 3, 1, 1);

    mat20 = new QLineEdit(groupBox);
    mat20->setObjectName(QString::fromUtf8("mat20"));

    gridLayout_2->addWidget(mat20, 2, 0, 1, 1);

    mat21 = new QLineEdit(groupBox);
    mat21->setObjectName(QString::fromUtf8("mat21"));

    gridLayout_2->addWidget(mat21, 2, 1, 1, 1);

    mat22 = new QLineEdit(groupBox);
    mat22->setObjectName(QString::fromUtf8("mat22"));

    gridLayout_2->addWidget(mat22, 2, 2, 1, 1);

    mat23 = new QLineEdit(groupBox);
    mat23->setObjectName(QString::fromUtf8("mat23"));

    gridLayout_2->addWidget(mat23, 2, 3, 1, 1);


    gridLayout->addWidget(groupBox, 4, 0, 1, 4);

    label_13 = new QLabel(GridProperty);
    label_13->setObjectName(QString::fromUtf8("label_13"));

    gridLayout->addWidget(label_13, 1, 2, 1, 1);

    lineEdit_2 = new QLineEdit(GridProperty);
    lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

    gridLayout->addWidget(lineEdit_2, 1, 3, 1, 1);

    label_3 = new QLabel(GridProperty);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout->addWidget(label_3, 3, 2, 1, 1);

    line2 = new QLineEdit(GridProperty);
    line2->setObjectName(QString::fromUtf8("line2"));

    gridLayout->addWidget(line2, 3, 3, 1, 1);


    retranslateUi(GridProperty);
    QObject::connect(buttonBox, SIGNAL(accepted()), GridProperty, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), GridProperty, SLOT(reject()));

    QMetaObject::connectSlotsByName(GridProperty);
    } // setupUi

    void retranslateUi(QDialog *GridProperty)
    {
    GridProperty->setWindowTitle(QApplication::translate("GridProperty", "Dialog", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("GridProperty", "Number of rows:", 0, QApplication::UnicodeUTF8));
    line1->setText(QApplication::translate("GridProperty", "15", 0, QApplication::UnicodeUTF8));
    lineEdit->setText(QApplication::translate("GridProperty", "Grid", 0, QApplication::UnicodeUTF8));
    label_15->setText(QApplication::translate("GridProperty", "Name:", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("GridProperty", "Transformation Matrix", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("GridProperty", "Size in GPA unit:", 0, QApplication::UnicodeUTF8));
    lineEdit_2->setText(QApplication::translate("GridProperty", "1.0", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("GridProperty", "Number of columns:", 0, QApplication::UnicodeUTF8));
    line2->setText(QApplication::translate("GridProperty", "15", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(GridProperty);
    } // retranslateUi

};

namespace Ui {
    class GridProperty: public Ui_GridProperty {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRIDPROPERTY_H
