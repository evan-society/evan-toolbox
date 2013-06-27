#ifndef PLSNODE_H
#define PLSNODE_H

#include <QDialog>

#include "../INode.h"
#include "../../DataTypes/Variables.h"
#include "../../DataTypes/Specimens.h"
#include "../../DataTypes/Loadings.h"
#include "../../DataTypes/PCScore.h"
#include "../../DataTypes/Float.h"

#include "Designer/ui_pls.h"
#include <pls.h>

class PLSNode : public QDialog , public INode, public Ui::PLSDialog
{

Q_OBJECT
private:
	Specimens* m_leftSpecimens;
	Specimens* m_rightSpecimens;

	PCScore* m_rightScores;
	PCScore* m_leftScores;
	Loadings* m_rightLoadings;
	Loadings* m_leftLoadings;
	Variables* m_singularValues;
	Variables* m_correlation;
//	Float* m_RVCoefficient;
    double m_rv;

    Variables m_leftBlock;
    Variables m_rightBlock;
//    bool m_updated;

    void LoadData();
    void CalculatePLS();
    void FillLeftList();
    void FillRightList();
    void PrintOutput();

protected:
	Specimens* getInputPortData0()const {return (Specimens*)getInputData(0);}
	Specimens* getInputPortData1()const {return (Specimens*)getInputData(1);}

	void setOutputPort0(PCScore* p){setOutputData(0,p);}
	void setOutputPort1(PCScore* p){setOutputData(1,p);}
	void setOutputPort2(Loadings* p){setOutputData(2,p);}
	void setOutputPort3(Loadings* p){setOutputData(3,p);}
	void setOutputPort4(Variables* p){setOutputData(4,p);}
	void setOutputPort5(Variables* p){setOutputData(5,p);}
//	void setOutputPort6(Float* p){setOutputData(6,p);}

public:
	PLSNode(QWidget* parent=0);
	~PLSNode();
	void showGUI()
    {
        QWidget::activateWindow();
        QWidget::show();
    }
    const QObject* getDialog() const {return this;}
	void process();
	bool isUpdated() const;
    void fromString(const QString& params);
    QString toString() const;
    bool inputChanged(int);

public slots:
    void accept();
    void controlsChanged()
    {
        m_updated = true;
    }
    void leftRadioChanged()
    {
        if(radioButton->isChecked() || radioButton_2->isChecked())
        {
            listWidget->setDisabled(true);
            checkBox_2->setChecked(true);
            checkBox_3->setChecked(false);
        }
        else if(radioButton_3->isChecked())
        {
            listWidget->setDisabled(false);
            checkBox_2->setChecked(true);
            checkBox_3->setChecked(true);
        }
        m_updated = true;
    }
    void rightRadioChanged()
    {
        if(radioButton_4->isChecked() || radioButton_5->isChecked())
        {
            listWidget_2->setDisabled(true);
            checkBox_4->setChecked(true);
            checkBox_5->setChecked(false);
        }
        else if(radioButton_6->isChecked())
        {
            listWidget_2->setDisabled(false);
            checkBox_4->setChecked(true);
            checkBox_5->setChecked(true);
        }
        m_updated = true;
    }
    void browseOutputFile();

signals:
	void dataReady();
};
#endif
