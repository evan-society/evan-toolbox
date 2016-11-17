#ifndef NODEBUTTON_H
#define NODEBUTTON_H

#include <QPushButton>

class NodeButton : public QPushButton
{
	Q_OBJECT

public:
	NodeButton(QWidget* parent=0) : QPushButton(parent)
	{QObject::connect(this, SIGNAL(toggled(bool)), this, SLOT(toggleNode(bool)));}
signals:
	void nodeButtonClicked(QAbstractButton* me);
public slots:
	void toggleNode(bool checked = false)
	{
		if(checked)
			emit nodeButtonClicked(this);
		QAbstractButton::clicked(checked);
	}
};

#endif
