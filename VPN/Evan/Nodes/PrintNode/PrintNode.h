#ifndef PRINTNODE_H
#define PRINTNODE_H

#include "../INode.h"
#include <QDialog>
#include "../../DataTypes/Specimens.h"
#include "../../DataTypes/TransformMatrix.h"
#include "../../DataTypes/CentroidSizes.h"
#include "../../DataTypes/Loadings.h"
#include "../../DataTypes/Surface.h"
#include "Designer/ui_print.h"

#include "../../DataTypes/Printables.h"



class PrintNode : public QDialog , public INode, public Ui::PrintDialog
{
    Q_OBJECT

private:
    QString m_lastText;

protected:
    Printables* getInputPortData0()  const { return dynamic_cast<Printables *>( getInputData( 0 ) ); }

public:
    PrintNode(QWidget* parent=0);

    void showGUI()
    {
        QWidget::activateWindow();
        QWidget::show();
    }
    const QObject* getDialog() const
    {
        return this;
    }
    void process();

	bool isUpdated() const {return false;}


public slots:
    void accept();
    void reject();
    void saveToFile();

};

#endif
