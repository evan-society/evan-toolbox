#ifndef PCONODE_H
#define PCONODE_H

#include "../INode.h"
#include <QDialog>
#include "../../DataTypes/Variables.h"
#include <pco.h>


class PCONode : public QDialog , public INode
{
Q_OBJECT
private:
	Variables* m_distances;
    Variables* m_eigenvectors;    // principal coordinates
    Variables* m_eigenvalues;

    PCO m_pco;
//    bool m_updated;

protected:
	Variables* getInputPortData0()const { return dynamic_cast<Variables *>( getInputData( 0 ) ); }
	void setOutputPort0(Variables* p){setOutputData(0,p);}
	void setOutputPort1(Variables* p){setOutputData(1,p);}

public:
	PCONode(QWidget* parent=0);
	~PCONode();

	const QObject* getDialog() const {return this;}
	void process();
	bool isUpdated() const;
signals:
	void dataReady();
};
#endif
