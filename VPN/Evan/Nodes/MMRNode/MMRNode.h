#ifndef MMRNODE_H
#define MMRNODE_H

#include "../INode.h"
#include <QDialog>
#include "../../DataTypes/Loadings.h"
#include "../../DataTypes/Variables.h"
#include "../../DataTypes/MatrixD.h"
#include "../../DataTypes/Specimens.h"

#include "Designer/ui_mmr.h"
#include <mmr.h>


class MMRNode : public QDialog , public INode, public Ui::RegressionDialog
{
Q_OBJECT
private:
    // inputs
    Specimens* m_specimens;
    Variables* m_additionalVariables;

    // work variables
    Variables m_X;
    Variables m_Y;

    // outputs
    Loadings* m_regressionCoefficients;
    Specimens* m_predictedValues;
    Specimens* m_residuals;
    Specimens* m_standardisedSpecimens;
    Variables* m_RSqr;

    MMR m_mmr;
    bool m_multiple;
//    bool m_updated;
    bool m_standardised;

    void LoadData();
    void Regress();
    void FillLeftList();
    void FillRightList();
    void PrintOutput();
    void FillComboBox();

protected:
	Specimens* getInputPortData0()const {return (Specimens*)getInputData(0);}
    Variables* getInputPortData1()const {return (Variables*)getInputData(1);}

	void setOutputPort0(Loadings* p){setOutputData(0,p);}
	void setOutputPort1(Specimens* p){setOutputData(1,p);}
	void setOutputPort2(Specimens* p){setOutputData(2,p);}
	void setOutputPort3(Specimens* p){setOutputData(3,p);}
	void setOutputPort4(Variables* p){setOutputData(4,p);}

	void standardise();

public:
	MMRNode(QWidget* parent=0);
	~MMRNode();
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

signals:
	void dataReady();

public slots:
    void accept();
    void controlsChanged() {m_updated = true;}
    void standardValueChanged() {m_standardised = false;}

    void leftRadioChanged()
    {
        if(radioButton->isChecked() || radioButton_2->isChecked())
        {
            listWidget->setDisabled(true);
        }
        else if(radioButton_3->isChecked())
        {
            listWidget->setDisabled(false);
        }
        m_updated = true;
    }
    void rightRadioChanged()
    {
        if(radioButton_4->isChecked() || radioButton_5->isChecked())
        {
            listWidget_2->setDisabled(true);
        }
        else if(radioButton_6->isChecked())
        {
            listWidget_2->setDisabled(false);
        }
        m_updated = true;
    }
    void comboChanged();
    void browseOutputFile();

};

#endif

