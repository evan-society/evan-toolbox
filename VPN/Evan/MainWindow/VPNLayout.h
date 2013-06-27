#ifndef VPNLAYOUT_H
#define VPNLAYOUT_H

#include <QMdiSubWindow>
#include <QtGui/QGraphicsView>
#include "DropArea.h"
#include "../Utilities/Logger.h"


class VPNLayout : public QMdiSubWindow
{
Q_OBJECT

private:
    QGraphicsView* m_projectView;
    DropArea* m_projectArea;

    bool m_bSaved;
    QString m_filePath;
    bool m_saveStates;

protected:
    void closeEvent(QCloseEvent *event);

public:
    VPNLayout(QWidget *parent = 0);

    QGraphicsView* getProjectView() const {return m_projectView;}
    DropArea* getProjectArea()  const {return m_projectArea;}
    bool isSaved()              const {return m_bSaved;}
    const QString& getFilePath()const {return m_filePath;}

    void setFilePath(const QString& f){m_filePath = f;}
    void setSaveStates(bool saveStates){m_saveStates = saveStates;}

    bool save();
    void saveAs();
    void saveWithoutStates();

public slots:
    void updated(bool changed=true);
};



#endif
