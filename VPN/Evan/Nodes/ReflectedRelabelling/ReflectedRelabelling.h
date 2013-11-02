#ifndef REFLECTEDRELABELLING_H
#define REFLECTEDRELABELLING_H

#include "../INode.h"
#include <QDialog>
#include "../../DataTypes/Specimens.h"

class ReflectedRelabelling : public QDialog , public INode
{
Q_OBJECT
private:
	Specimens* m_specimensInput;
	Specimens* m_specimensOutput;
//	bool m_updated;

protected:
	Specimens* getInputPortData0()const { return dynamic_cast<Specimens *>( getInputData( 0 ) ); }
	void setOutputPort0(Specimens* p){setOutputData(0,p);}

public:
	ReflectedRelabelling(QWidget* parent=0);
	~ReflectedRelabelling();
	const QObject* getDialog() const {return this;}
	void process();
	bool isUpdated() const;
signals:
	void dataReady();
};
#endif
