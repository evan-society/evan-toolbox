#ifndef RNODE_H
#define RNODE_H

#ifdef USE_R_

#include "../INode.h"
#include <QDialog>
#include "../../DataTypes/MatrixD.h"
#include "Designer/ui_RDialog.h"

class RInside;
class RList;

class RNode : public QDialog , public INode, public Ui::RDialog
{
Q_OBJECT
private:
    RInside* m_REnv;

	RList* m_rInputs;
	RList* m_rOutputs;

//	bool m_updated;
    bool m_foundR;

	void renameInputs();
	void clearInputsTable();

protected:
	RList* getInputPortData0()const { return dynamic_cast<RList *>( getInputData( 0 ) ); }
	void setOutputPort0(RList* p){ setOutputData(0, dynamic_cast<IDataType *>( p ) ); }

public:
	RNode(QWidget* parent=0);
	~RNode();

	void showGUI()
    {
        QWidget::activateWindow();
        QWidget::show();
    }
    const QObject* getDialog() const {return this;}
	void process();
	bool isUpdated() const;

    QString toString() const;
    void fromString(const QString& params);

public slots:
    void accept();
    void controlsChanged() {m_updated = true;}

signals:
	void dataReady();
};

#endif
#endif
