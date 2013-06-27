/********************************************************************************
** Form generated from reading ui file 'gpa.ui'
**
** Created: Thu 22. Oct 19:49:05 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_GPA_H
#define UI_GPA_H

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
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GPADialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_6;
    QLineEdit *lineEdit_3;
    QPushButton *pushButton;
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_6;
    QCheckBox *checkBox_8;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_3;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_3;
    QCheckBox *checkBox;
    QCheckBox *checkBox_4;
    QSpacerItem *verticalSpacer;
    QWidget *tab_2;
    QGridLayout *gridLayout_5;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_7;
    QSpacerItem *horizontalSpacer;
    QLabel *label_3;
    QCheckBox *checkBox_6;
    QCheckBox *checkBox_7;
    QLabel *label_2;
    QLabel *label;
    QGroupBox *groupBox_6;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_7;
    QRadioButton *radioButton_6;
    QGroupBox *groupBox_5;
    QWidget *layoutWidget_2;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton_5;
    QRadioButton *radioButton_8;
    QCheckBox *checkBox_5;
    QWidget *tab_3;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLabel *label_4;
    QLabel *label_5;

    void setupUi(QDialog *GPADialog)
    {
    if (GPADialog->objectName().isEmpty())
        GPADialog->setObjectName(QString::fromUtf8("GPADialog"));
    GPADialog->resize(528, 481);
    gridLayout = new QGridLayout(GPADialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    label_6 = new QLabel(GPADialog);
    label_6->setObjectName(QString::fromUtf8("label_6"));

    horizontalLayout_2->addWidget(label_6);

    lineEdit_3 = new QLineEdit(GPADialog);
    lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));

    horizontalLayout_2->addWidget(lineEdit_3);

    pushButton = new QPushButton(GPADialog);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    horizontalLayout_2->addWidget(pushButton);


    gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

    buttonBox = new QDialogButtonBox(GPADialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 2, 0, 1, 1);

    tabWidget = new QTabWidget(GPADialog);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    gridLayout_2 = new QGridLayout(tab);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    groupBox_2 = new QGroupBox(tab);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    gridLayout_4 = new QGridLayout(groupBox_2);
    gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
    radioButton = new QRadioButton(groupBox_2);
    radioButton->setObjectName(QString::fromUtf8("radioButton"));
    radioButton->setChecked(true);

    gridLayout_4->addWidget(radioButton, 0, 0, 1, 1);

    radioButton_2 = new QRadioButton(groupBox_2);
    radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

    gridLayout_4->addWidget(radioButton_2, 1, 0, 1, 1);

    radioButton_3 = new QRadioButton(groupBox_2);
    radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));

    gridLayout_4->addWidget(radioButton_3, 2, 0, 1, 1);


    gridLayout_2->addWidget(groupBox_2, 0, 1, 1, 1);

    groupBox_4 = new QGroupBox(tab);
    groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
    gridLayout_6 = new QGridLayout(groupBox_4);
    gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
    checkBox_8 = new QCheckBox(groupBox_4);
    checkBox_8->setObjectName(QString::fromUtf8("checkBox_8"));
    checkBox_8->setChecked(true);

    gridLayout_6->addWidget(checkBox_8, 0, 0, 1, 1);


    gridLayout_2->addWidget(groupBox_4, 1, 0, 1, 2);

    groupBox_3 = new QGroupBox(tab);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    gridLayout_3 = new QGridLayout(groupBox_3);
    gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
    checkBox_2 = new QCheckBox(groupBox_3);
    checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
    checkBox_2->setChecked(true);

    gridLayout_3->addWidget(checkBox_2, 0, 0, 1, 1);

    checkBox_3 = new QCheckBox(groupBox_3);
    checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));
    checkBox_3->setChecked(true);

    gridLayout_3->addWidget(checkBox_3, 1, 0, 1, 1);

    checkBox = new QCheckBox(groupBox_3);
    checkBox->setObjectName(QString::fromUtf8("checkBox"));
    checkBox->setChecked(true);

    gridLayout_3->addWidget(checkBox, 2, 0, 1, 1);

    checkBox_4 = new QCheckBox(groupBox_3);
    checkBox_4->setObjectName(QString::fromUtf8("checkBox_4"));
    checkBox_4->setChecked(false);

    gridLayout_3->addWidget(checkBox_4, 3, 0, 1, 1);


    gridLayout_2->addWidget(groupBox_3, 0, 0, 1, 1);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_2->addItem(verticalSpacer, 2, 0, 1, 1);

    tabWidget->addTab(tab, QString());
    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    gridLayout_5 = new QGridLayout(tab_2);
    gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
    groupBox = new QGroupBox(tab_2);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout_7 = new QGridLayout(groupBox);
    gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_7->addItem(horizontalSpacer, 1, 0, 1, 2);

    label_3 = new QLabel(groupBox);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout_7->addWidget(label_3, 17, 0, 1, 2);

    checkBox_6 = new QCheckBox(groupBox);
    checkBox_6->setObjectName(QString::fromUtf8("checkBox_6"));
    checkBox_6->setChecked(true);

    gridLayout_7->addWidget(checkBox_6, 3, 0, 1, 1);

    checkBox_7 = new QCheckBox(groupBox);
    checkBox_7->setObjectName(QString::fromUtf8("checkBox_7"));
    checkBox_7->setChecked(false);

    gridLayout_7->addWidget(checkBox_7, 3, 1, 1, 1);

    label_2 = new QLabel(groupBox);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout_7->addWidget(label_2, 4, 0, 1, 1);

    label = new QLabel(groupBox);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout_7->addWidget(label, 4, 1, 1, 1);

    groupBox_6 = new QGroupBox(groupBox);
    groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
    layoutWidget = new QWidget(groupBox_6);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 30, 182, 71));
    verticalLayout_2 = new QVBoxLayout(layoutWidget);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    verticalLayout_2->setContentsMargins(0, 0, 0, 0);
    radioButton_4 = new QRadioButton(layoutWidget);
    radioButton_4->setObjectName(QString::fromUtf8("radioButton_4"));
    radioButton_4->setEnabled(false);
    radioButton_4->setChecked(true);

    verticalLayout_2->addWidget(radioButton_4);

    radioButton_7 = new QRadioButton(layoutWidget);
    radioButton_7->setObjectName(QString::fromUtf8("radioButton_7"));
    radioButton_7->setEnabled(false);

    verticalLayout_2->addWidget(radioButton_7);

    radioButton_6 = new QRadioButton(layoutWidget);
    radioButton_6->setObjectName(QString::fromUtf8("radioButton_6"));
    radioButton_6->setEnabled(false);

    verticalLayout_2->addWidget(radioButton_6);


    gridLayout_7->addWidget(groupBox_6, 14, 1, 3, 1);

    groupBox_5 = new QGroupBox(groupBox);
    groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
    layoutWidget_2 = new QWidget(groupBox_5);
    layoutWidget_2->setObjectName(QString::fromUtf8("layoutWidget_2"));
    layoutWidget_2->setGeometry(QRect(10, 30, 171, 71));
    verticalLayout = new QVBoxLayout(layoutWidget_2);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    radioButton_5 = new QRadioButton(layoutWidget_2);
    radioButton_5->setObjectName(QString::fromUtf8("radioButton_5"));
    radioButton_5->setChecked(true);

    verticalLayout->addWidget(radioButton_5);

    radioButton_8 = new QRadioButton(layoutWidget_2);
    radioButton_8->setObjectName(QString::fromUtf8("radioButton_8"));

    verticalLayout->addWidget(radioButton_8);


    gridLayout_7->addWidget(groupBox_5, 14, 0, 3, 1);

    checkBox_5 = new QCheckBox(groupBox);
    checkBox_5->setObjectName(QString::fromUtf8("checkBox_5"));

    gridLayout_7->addWidget(checkBox_5, 0, 0, 1, 1);


    gridLayout_5->addWidget(groupBox, 0, 0, 1, 1);

    tabWidget->addTab(tab_2, QString());
    tab_3 = new QWidget();
    tab_3->setObjectName(QString::fromUtf8("tab_3"));
    lineEdit = new QLineEdit(tab_3);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
    lineEdit->setGeometry(QRect(150, 30, 113, 20));
    lineEdit_2 = new QLineEdit(tab_3);
    lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
    lineEdit_2->setGeometry(QRect(150, 70, 113, 20));
    label_4 = new QLabel(tab_3);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(30, 30, 81, 16));
    label_5 = new QLabel(tab_3);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setGeometry(QRect(30, 70, 91, 16));
    tabWidget->addTab(tab_3, QString());

    gridLayout->addWidget(tabWidget, 0, 0, 1, 1);


    retranslateUi(GPADialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), GPADialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), GPADialog, SLOT(reject()));

    tabWidget->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(GPADialog);
    } // setupUi

    void retranslateUi(QDialog *GPADialog)
    {
    GPADialog->setWindowTitle(QApplication::translate("GPADialog", "Generalised Procrustes Analysis", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("GPADialog", "Output file", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("GPADialog", "Browse", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("GPADialog", "Alignment", 0, QApplication::UnicodeUTF8));
    radioButton->setText(QApplication::translate("GPADialog", "Align with principal axes", 0, QApplication::UnicodeUTF8));
    radioButton_2->setText(QApplication::translate("GPADialog", "Align with first specimen", 0, QApplication::UnicodeUTF8));
    radioButton_3->setText(QApplication::translate("GPADialog", "Align with plane of symmetry", 0, QApplication::UnicodeUTF8));
    groupBox_4->setTitle(QApplication::translate("GPADialog", "Tangent Projection", 0, QApplication::UnicodeUTF8));
    checkBox_8->setText(QApplication::translate("GPADialog", "Tangent project coordinates", 0, QApplication::UnicodeUTF8));
    groupBox_3->setTitle(QApplication::translate("GPADialog", "Procrustes Fit", 0, QApplication::UnicodeUTF8));
    checkBox_2->setText(QApplication::translate("GPADialog", "Rotate", 0, QApplication::UnicodeUTF8));
    checkBox_3->setText(QApplication::translate("GPADialog", "Translate", 0, QApplication::UnicodeUTF8));
    checkBox->setText(QApplication::translate("GPADialog", "Scale", 0, QApplication::UnicodeUTF8));
    checkBox_4->setText(QApplication::translate("GPADialog", "Enable Reflections", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("GPADialog", "Fitting options", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("GPADialog", "Distances between specimens", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("GPADialog", "Note: If you dont know what distances to calculate you should probably\n"
"          choose Euclidean distances between all specimens", 0, QApplication::UnicodeUTF8));
    checkBox_6->setText(QApplication::translate("GPADialog", "All", 0, QApplication::UnicodeUTF8));
    checkBox_7->setText(QApplication::translate("GPADialog", "Pairwise", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("GPADialog", "Compute distances between\n"
"the fitted specimens", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("GPADialog", "Compare two specimens at a time", 0, QApplication::UnicodeUTF8));
    groupBox_6->setTitle(QApplication::translate("GPADialog", "Pairwise", 0, QApplication::UnicodeUTF8));
    radioButton_4->setText(QApplication::translate("GPADialog", "Procrustes Distances (rho)", 0, QApplication::UnicodeUTF8));
    radioButton_7->setText(QApplication::translate("GPADialog", "Partial Procrustes Distances (Dp)", 0, QApplication::UnicodeUTF8));
    radioButton_6->setText(QApplication::translate("GPADialog", "Full Procrustes Distances (Df)", 0, QApplication::UnicodeUTF8));
    groupBox_5->setTitle(QApplication::translate("GPADialog", "All", 0, QApplication::UnicodeUTF8));
    radioButton_5->setText(QApplication::translate("GPADialog", "Euclidean Distances", 0, QApplication::UnicodeUTF8));
    radioButton_8->setText(QApplication::translate("GPADialog", "Tangent Distances", 0, QApplication::UnicodeUTF8));
    checkBox_5->setText(QApplication::translate("GPADialog", "Form Space Distances (include natural log of centroid size)", 0, QApplication::UnicodeUTF8));    
    tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("GPADialog", "Distances", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("GPADialog", "Max iterations", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("GPADialog", "Min tolerance", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("GPADialog", "Algorithm settings", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(GPADialog);
    } // retranslateUi

};

namespace Ui {
    class GPADialog: public Ui_GPADialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GPA_H
