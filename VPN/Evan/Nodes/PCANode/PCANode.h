#ifndef PCANODE_H
#define PCANODE_H

#include "../INode.h"
#include "../../DataTypes/Loadings.h"
#include "../../DataTypes/PCScore.h"
#include "../../DataTypes/Specimens.h"
#include "../../DataTypes/Variables.h"
#include "Designer/ui_pca.h"
#include <pca.h>


class PCANode : public QDialog, public INode, public Ui::PCADialog
{
Q_OBJECT

private:
	Specimens* m_specimensInput;

    Loadings* m_loadingsOutput;
	PCScore* m_pcscoreOutput;
    Variables* m_eigenvalues;

    PCA m_pca;
//    bool m_updated;
    QString m_settings;

    void PrintOutput();

protected:
	Specimens* getInputPortData0()const { return dynamic_cast< Specimens* >( getInputData(0) ); }

    void setOutputPort0(PCScore* p){setOutputData(0,p);}
	void setOutputPort1(Variables* p){setOutputData(1,p);}
	void setOutputPort2(Loadings* p){setOutputData(2,p);}

public:
	PCANode();
	~PCANode();

	void process();
	void showGUI()
    {
        m_settings = toString();
        QWidget::activateWindow();
        QWidget::show();
    }
    const QObject* getDialog() const {return this;}
	bool isUpdated() const;
	void CalculatePCA();

	QString toString() const;
	void fromString(const QString&);

public slots:
    void accept();
    void reject();
    void controlsChanged() {m_updated = true;}
    void browseOutputFile();

signals:
	void dataReady();
};

#endif
