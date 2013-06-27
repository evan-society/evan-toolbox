#ifndef GROUPNODE_H
#define GROUPNODE_H

#include "../INode.h"
#include "Designer/ui_GroupDialog.h"
#include "../../DataTypes/Specimens.h"

class GroupNode : public QDialog , public INode, public Ui::GroupDialog
{
Q_OBJECT
private:
	Specimens* m_specimensInput;
	Specimens* m_specimensOutput;
	Specimens* m_groupMeans;

	SpecimensModel* m_specimensModel;

//	bool m_updated;

    QList<GroupTreeItem*> m_loadedGroupItems;

    void clearLoadedGroups();
    void fillLoadedGroups();
	void doGrouping();
	void removeFromUngrouped(unsigned int index);
	void addToUngrouped(unsigned int index);

protected:
	Specimens* getInputPortData0()const {return (Specimens*)getInputData(0);}
	void setOutputPort0(Specimens* p){setOutputData(0,p);}
	void setOutputPort1(Specimens* p){setOutputData(1,p);}

public:
	GroupNode(QWidget* parent=0);
	~GroupNode();
	void showGUI()
    {
        QWidget::activateWindow();
        QWidget::show();
    }
    const QObject* getDialog() const {return this;}
	bool isUpdated() const {return false;}
	void process();

	void fromString(const QString& params);
    QString toString() const;

public slots:
    void accept();
    void evaluateExpression();
    void checkSelection();
    void addToGroup();
    void deleteGroup();
    void removeFromGroup();
    void controlsChanged() {m_updated = true;}

signals:
	void dataReady();
};
#endif
