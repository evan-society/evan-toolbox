#ifndef SUBTRACTNODE_H
#define SUBTRACTNODE_H

#include "../INode.h"
#include "../../DataTypes/Specimens.h"
#include "../../DataTypes/Loadings.h"
#include "Designer/ui_subtract.h"

class SubtractNode : public QDialog, public INode, public Ui::SubtractDialog
{
Q_OBJECT
private:
	LandmarkSet* m_baseInput;
	Specimens* m_targetInput;
	Loadings* m_diffVecOutput;
	Specimens* m_specimensOutput;
	double m_parameterA;
	double m_parameterB;
	QString m_textA;
    QString m_textB;

protected:
	LandmarkSet* getInputPortData0()const { return dynamic_cast<LandmarkSet *>( getInputData( 0 ) ); }
	Specimens* getInputPortData1()const { return dynamic_cast<Specimens *>( getInputData( 1 ) ); }
	void setOutputPort0(Loadings* p){setOutputData(0,p);}
	void setOutputPort1(Specimens* p){setOutputData(1,p);}

public:
	SubtractNode();
	~SubtractNode();
	void process();
    void showGUI()
    {
        QWidget::activateWindow();
        QWidget::show();
    }
	const QObject* getDialog() const {return this;}
	bool isUpdated() const {return false;}
	QString toString() const;
	void fromString(const QString&);

public slots:

    void accept();
    void reject();

signals:
    void dataReady();
};
#endif
