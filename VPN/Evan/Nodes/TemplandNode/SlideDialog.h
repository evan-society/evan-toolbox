/*
 * SlideDialog.h
 *
 *  Created on: Jan 8, 2016
 *      Author: ynashed
 */

#ifndef SLIDEDIALOG_H_
#define SLIDEDIALOG_H_

#include "Designer/ui_SlideDialog.h"

class SlideDialog : public QDialog, public Ui::SlideDialog
{
public:
	SlideDialog(QWidget* parent, bool showSlideCB = true) : QDialog(parent)
	{
	    setupUi(this);
    	m_slideCB->setVisible(showSlideCB);
    	groupBox->setEnabled(!showSlideCB);
	}
	~SlideDialog(){}

	bool performSliding() const {return m_slideCB->isChecked();}
	int getIterations() const
	{
		if (m_slideOnce_RB->isChecked())
			return 1;
		else if (m_slideNX_RB->isChecked())
			return m_slideNum_SB->value();
		return -1;
	}
	double getEpsilon() const
	{return (m_slideConverge_RB->isChecked())?m_slideError_SB->value():m_slideError_SB->minimum();}
};

#endif /* SLIDEDIALOG_H_ */
