#ifndef EXPORTNODE_H
#define EXPORTNODE_H

#include "../INode.h"
#include <QDialog>
#include "../../DataTypes/MatrixD.h"
#include "../../DataTypes/Surface.h"
#include "../../DataTypes/Specimens.h"

#include "Designer/ui_export.h"

class ExportNode : public QDialog , public INode, public Ui::ExportDialog
{
Q_OBJECT
private:
	MatrixD* m_matrix;
	Surface* m_surface;
	Specimens* m_specimens;
//	bool m_updated;

protected:
	Specimens* getInputPortData0()const { return dynamic_cast<Specimens *>( getInputData( 0 ) ); }
	Surface* getInputPortData1()const { return dynamic_cast<Surface *>( getInputData( 1 ) ); }
	MatrixD* getInputPortData2()const { return dynamic_cast<MatrixD *>( getInputData( 2 ) ); }

public:
	ExportNode(QWidget* parent=0);
	~ExportNode();
	void showGUI()
	{
        QDialog::activateWindow();
        QDialog::show();
    }
	const QObject* getDialog() const {return this;}
	void process();
	bool isUpdated() const;
	QString toString() const;
	void fromString(const QString&);

signals:
	void dataReady();
public slots:
    void exportSpecimens();
    void exportSurface();
    void exportMatrix();
    void controlsChanged()
    {
        m_updated=true;
    };
    void enableLabelsCheckBox()
    {
        checkBox->setDisabled(false);
    };
    void disableLabelsCheckBox()
    {
        checkBox->setDisabled(true);
    };
};
#endif
