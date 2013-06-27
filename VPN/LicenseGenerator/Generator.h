#ifndef GENERATOR_H_INCLUDED
#define GENERATOR_H_INCLUDED

#include <QObject>
#include "ui_generator.h"

#include <iostream>
#include <fstream>

using namespace std;


class Generator : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT

public:
QString m_macAddress;
QDate m_date;

Generator()
{
    setupUi(this);
    connect(textEdit, SIGNAL(textChanged ()), this, SLOT(enterMacAddress()));
    connect(pushButton, SIGNAL(clicked()), this, SLOT(generate()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(callexit()));
    connect(dateEdit, SIGNAL(dateChanged ( const QDate & )), this, SLOT(setExpireDate(const QDate &)));

	dateEdit->setCalendarPopup(true);
	dateEdit->setDate(m_date.currentDate().addDays(14));
	textEdit_2->setReadOnly(true);
}
virtual ~Generator(){}

public slots:
void enterMacAddress();
void generate();
void setExpireDate(const QDate & date){m_date = date;};
void callexit(){exit(0);};

};

#endif // GENERATOR_H_INCLUDED
