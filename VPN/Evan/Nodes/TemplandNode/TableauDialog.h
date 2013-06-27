#ifndef TABLEAUDIALOG_H
#define TABLEAUDIALOG_H

#include "Designer/ui_TableauDialog.h"

class TableauDialog : public QDialog, public Ui::TableauDialog
{
public:
	TableauDialog(QWidget* parent, const QString& file, unsigned int rCount)
	{
	    setupUi(this);
	    m_pathLabel->setText(file);
	    m_recordsLabel->setText(QString("%1 Record(s)").arg(rCount));
	}
	~TableauDialog(){}

    bool loadTemplate() const {return m_templateCheck->isChecked();}
    bool loadTarget()   const {return m_targetCheck->isChecked();}
    bool loadSettings() const {return m_settingsCheck->isChecked();}
    bool loadView()     const {return m_viewCheck->isChecked();}
};
#endif

