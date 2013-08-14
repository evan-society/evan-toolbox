#ifndef TRANSFORMNODE_H
#define TRANSFORMNODE_H

#include "../INode.h"
#include <QtGui>
#include "../../DataTypes/Surface.h"
#include "../../DataTypes/LandmarkSet.h"
#include "../../DataTypes/TransformMatrix.h"

class TransformNode : public QDialog , public INode
{
Q_OBJECT

private:
	LandmarkSet* m_landmarkInput;
	Surface* m_surfaceInput;
	TransformMatrix* m_transformInput;
	LandmarkSet* m_landmarkOutput;
	Surface* m_surfaceOutput;

	QLabel* m_indexLabel;
	QSpinBox* m_selectedIndex;
    QVBoxLayout* m_mainLayout;
    QDialogButtonBox* m_buttonBox;
    bool m_indexChanged;

protected:
	LandmarkSet* getInputPortData0()const { return dynamic_cast< LandmarkSet* >( getInputData(0) ); }
	Surface* getInputPortData1()const { return dynamic_cast< Surface* >( getInputData(1) ); }
	TransformMatrix* getInputPortData2()const { return dynamic_cast< TransformMatrix* >( getInputData(2) ); }
	void setOutputPort0(LandmarkSet* p){setOutputData(0,p);}
	void setOutputPort1(Surface* p){setOutputData(1,p);}

public:
	TransformNode();
	~TransformNode();
	void showGUI()
    {
        QWidget::activateWindow();
        QWidget::show();
    }
	const QObject* getDialog() const {return this;}
	void process();
	bool isUpdated() const;

public slots:
    void accept();

signals:
	void dataReady();
};
#endif
