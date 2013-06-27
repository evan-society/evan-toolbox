#ifndef MULTIPLY_H
#define MULTIPLY_H

#include "../INode.h"
#include <QDialog>
#include "../../DataTypes/Loadings.h"
#include "Designer/ui_multiply.h"

class MultiplyNode : public QDialog , public INode, public Ui::multiplyDialog
{
Q_OBJECT
private:
	Loadings* m_input;
	Loadings* m_output;
//	bool m_updated;
	double m_mult;

protected:
	Loadings* getInputPortData0()const {return (Loadings*)getInputData(0);}
	void setOutputPort0(Loadings* p){setOutputData(0,p);}

public:
	MultiplyNode(QWidget* parent=0);
	~MultiplyNode();
	void process();
	void showGUI() {QDialog::show();}
	const QObject* getDialog() const {return this;}
	bool isUpdated() const;

	QString toString() const;
	void fromString(const QString&);

signals:
	void dataReady();

public slots:
    void controlsChanged()
    {
        m_updated = true;
    }
    void accept();
    void reject();

};
#endif
