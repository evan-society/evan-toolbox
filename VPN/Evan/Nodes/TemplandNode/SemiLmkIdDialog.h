#ifndef SEMILMKIDDIALOG_H
#define SEMILMKIDDIALOG_H

#include "Designer/ui_SemiLmkIdDialog.h"
#include <QRegExpValidator>

class SemiLmkIdDialog : public QDialog, public Ui::SemiLmkIdDialog
{
public:
	SemiLmkIdDialog(QWidget* parent, const QString& id, const QString& label)
	{
	    setupUi(this);
	    m_idLineEdit->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z_][a-zA-Z0-9_]*$"),0));
	    m_idLineEdit->setText(id);
	    m_labelLineEdit->setText(label);
	}
	~SemiLmkIdDialog(){}

	void hideLabel() { label_2->setVisible( false ); m_labelLineEdit->setVisible( false ); }

    void fillComboBox(QStringList& list)
    {
        comboBox->insertItems ( 0,  list );
    }
    int getIndex()
    {
        return comboBox->currentIndex();
    }
    QString getEmbeddedObjectName()
    {
        return comboBox->currentText();
    }

    QString getID() const {return m_idLineEdit->text();}
    QString getLabel()   const {return m_labelLineEdit->text();}
};
#endif


