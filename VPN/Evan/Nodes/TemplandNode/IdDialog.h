#ifndef IDDIALOG_H
#define IDDIALOG_H

#include "Designer/ui_IdDialog.h"
#include <QRegExpValidator>

class IdDialog : public QDialog, public Ui::IdDialog
{
public:
	IdDialog(QWidget* parent, const QString& id, const QString& label)
	{
	    setupUi(this);
	    m_idLineEdit->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z_][a-zA-Z0-9_]*$"),0));
	    m_idLineEdit->setText(id);
	    m_labelLineEdit->setText(label);
	}
	~IdDialog(){}

	void hideLabel() { label_2->setVisible( false ); m_labelLineEdit->setVisible( false ); }

    QString getID() const {return m_idLineEdit->text();}
    QString getLabel()   const {return m_labelLineEdit->text();}
};
#endif


