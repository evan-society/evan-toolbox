#ifndef SELECTNODE_H
#define SELECTNODE_H

#include "../INode.h"
#include "Designer/ui_SelectDialog.h"
#include "../../DataTypes/Specimens.h"

class SpecimensModel;
class LandmarkSetModel;
class QModelIndex;

class SelectNode : public QDialog , public INode, public Ui::SelectDialog
{
Q_OBJECT
private:

	Specimens* m_specimenInput;
	Specimens* m_specimenOutput;

	SpecimensModel* m_specimensModel;
	LandmarkSetModel* m_lmkSetModel;

	QSet<unsigned int> m_loadedSelection;
	QSet<unsigned int> m_loadedLmksSelection;

//    bool m_updated;

    void doSelection();

protected:
	Specimens* getInputPortData0()const { return dynamic_cast< Specimens* >( getInputData(0) ); }
	void setOutputPort0(Specimens* p){setOutputData(0,p);}

public:
	SelectNode(QWidget* parent=0);
	~SelectNode();
	void showGUI()
    {
        QWidget::activateWindow();
        QWidget::show();
    }
	const QObject* getDialog() const {return this;}
	void process();
	bool isUpdated() const {return false;}

    void fromString(const QString& params)
    {
        QStringList selection = params.split("::");
        if(selection.count() == 2)
        {
            QStringList indeces = selection[0].split(",");
            m_loadedSelection.clear();
            foreach(QString index, indeces)
            {
                if(index == "" || index.isEmpty())
                    continue;
                bool ok = false;
                unsigned int i = index.toInt(&ok);
                if(ok)
                    m_loadedSelection.insert(i);
            }

            QStringList lmks = selection[1].split(",");
            m_loadedLmksSelection.clear();
            foreach(QString index, lmks)
            {
                if(index == "" || index.isEmpty())
                    continue;
                bool ok = false;
                unsigned int i = index.toInt(&ok);
                if(ok)
                    m_loadedLmksSelection.insert(i);
            }
        }
        else
        {
            QStringList indeces = params.split(",");
            m_loadedSelection.clear();
            foreach(QString index, indeces)
            {
                if(index == "" || index.isEmpty())
                    continue;
                bool ok = false;
                unsigned int i = index.toInt(&ok);
                if(ok)
                    m_loadedSelection.insert(i);
            }
        }
    }
	QString toString() const
	{
	    QString result = "";
	    foreach(unsigned int i, m_specimensModel->getSelectedSpecimens())
            result += QString().setNum(i) + ",";

        result += "::";

	    foreach(unsigned int i, m_specimensModel->getUnselectedLMKs())
            result += QString().setNum(i) + ",";

	    return result;
	}

public slots:
    void accept();
    void evaluateExpression();
    void controlsChanged() {m_updated = true;}
    void specimenChanged(const QModelIndex&);
    void parseIndices();

signals:
	void dataReady();
};
#endif
