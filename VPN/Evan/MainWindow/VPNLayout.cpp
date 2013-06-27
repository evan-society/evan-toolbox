#include "VPNLayout.h"
#include "../Utilities/XMLParser.h"
#include "../Nodes/INode.h"
#include "../Nodes/NodeLink.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <assert.h> // needed for MacOS

#include "MainWindow.h"

extern QString workingDirectory;

/*
class MyFileDialog : public QFileDialog
{
public:

    MyFileDialog(QWidget *parent);

    void setVPNLayout(VPNLayout * vpn){ m_vpn = vpn;}

private:
    QCheckBox *m_cb;
    VPNLayout *m_vpn;

public slots:
    void accept()
    {
        QStringList fileNames = selectedFiles();
        if(fileNames.size()>0)
        {
            if(m_vpn)
            {
                m_vpn->setFilePath(fileNames[0]);
                m_vpn->setSaveStates(m_cb->isChecked());
            }
        }
        QFileDialog::accept();
    }
};

MyFileDialog::MyFileDialog( QWidget *parent) : QFileDialog( parent ), m_cb(0)
{
    QGridLayout* mainLayout = dynamic_cast<QGridLayout*>(layout());

    if ( ! mainLayout )
    {
        assert(0); // in case of future changes
    }
    else
    {
        QApplication::restoreOverrideCursor();
        QHBoxLayout *hbl = new QHBoxLayout();//"My Layout",0);

        // add some widgets
        m_cb = new QCheckBox("Save states of nodes");
        m_cb->setChecked(true);
        hbl->addWidget(m_cb);
        int numRows = mainLayout->rowCount();

        // add the new layout to the bottom of mainLayout
        // and span all columns
        mainLayout->addLayout( hbl, numRows,0,1,-1);
    }
}
*/

VPNLayout::VPNLayout(QWidget* parent) : QMdiSubWindow(parent), m_bSaved(false), m_filePath("")
{
    m_projectArea = new DropArea(this);
    m_projectArea->setObjectName(QString::fromUtf8("projectArea"));
    m_projectView = new QGraphicsView(m_projectArea);
    m_projectView->setDragMode(QGraphicsView::RubberBandDrag);
    m_projectView->raise();
    m_projectView->setAcceptDrops(true);

    setWidget(m_projectView);
    setAttribute(Qt::WA_DeleteOnClose);

    QObject::connect(m_projectArea, SIGNAL(changed(const QList<QRectF>&)), this, SLOT(updated()));
    m_saveStates = true;
}

void VPNLayout::closeEvent(QCloseEvent *event)
{
    if (!m_bSaved)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Save?"),
                                   tr("The network <b>'%1'</b> has been modified.\nDo you want to save your changes?")
                                   .arg(windowTitle()),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            save() ? QMdiSubWindow::closeEvent(event) : event->ignore();
        else if (ret == QMessageBox::Cancel)
            event->ignore();
        else QMdiSubWindow::closeEvent(event);
    }
    else QMdiSubWindow::closeEvent(event);

}

void VPNLayout::updated(bool changed)
{
    m_bSaved = !changed;
    if (m_filePath != "" && !m_filePath.isEmpty())
        setWindowTitle(m_filePath+ (changed ? "*" : "") );
}

void VPNLayout::saveAs()
{
    m_saveStates = true;
    m_filePath = "";
    save();
}

void VPNLayout::saveWithoutStates()
{
    m_saveStates = false;
    m_filePath = "";
    save();
    m_saveStates = true;
}

bool VPNLayout::save()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    if(m_filePath == "" || m_filePath.isEmpty())
    {
/*
        MyFileDialog *fd = new MyFileDialog(this);
        fd->setWindowTitle("Save Network");
        fd->setNameFilter("VPN Network (*.vpn)");
        fd->setAcceptMode(QFileDialog::AcceptSave);
        fd->selectFile("Untitled.vpn");
        fd->setVPNLayout(this);
        fd->setOption(MyFileDialog::DontUseNativeDialog,true);
        fd->exec();
*/
        m_filePath = QFileDialog::getSaveFileName(this, "Save Network",fsLastDir + "\\Untitled","VPN Network (*.vpn)");

        if(m_filePath == "" || m_filePath.isEmpty())
            return false;

        if(m_filePath.contains("/"))
        {
            QString dirName = m_filePath.left(m_filePath.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(m_filePath.contains("\\"))
        {
            QString dirName = m_filePath.left(m_filePath.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }

        QByteArray tags = "<VPN>\n</VPN>";
        QFile networkFile( m_filePath );
        if (!networkFile.open(QFile::WriteOnly | QFile::Text))
        {
            QMessageBox::critical(0,tr("Failed to create new file"),
                                    tr("Cannot Save Network File %1:\n%2")
                                    .arg(networkFile.fileName())
                                    .arg(networkFile.errorString()));
            return false;
        }
        networkFile.write(tags);
    }

    XMLParser parser(m_filePath);
    parser.clearContents();

    const QList<INode*>* nodes = m_projectArea->nodes();
    for(int i=0; i<nodes->size(); ++i)
    {
        INode* node = nodes->at(i);
        QPointF nodePos = m_projectArea->getNodePos(i);
        QString annotation = m_projectArea->getNodeAnnotations(i);
        parser.addElement("Node");
        parser.addAttribute("type", node->getType().c_str());
        parser.addAttribute("x", QString("%1").arg(nodePos.x()));
        parser.addAttribute("y", QString("%1").arg(nodePos.y()));
        if(m_saveStates)
        {
            parser.addAttribute("params", node->toString());
        }
        else
        {
            parser.addAttribute("params", "");
        }
        parser.addAttribute("annotation", annotation);
    }

    foreach(NodeLink* link, m_projectArea->links())
    {
        parser.addElement("Link");
        parser.addAttribute("sNode", QString("%1").arg(nodes->indexOf(link->startNode)));
        parser.addAttribute("eNode", QString("%1").arg(nodes->indexOf(link->endNode)));
        QString portData = "";
        foreach(PortLink* pLink, link->portLinks)
            portData += pLink->toString() + ",";
        parser.addAttribute("ports", portData);
    }

    parser.addElement("Settings");
    parser.addAttribute("zoom", QString("%1").arg(m_projectArea->getZoom()));

    parser.commit();

    setWindowTitle(m_filePath);
    m_bSaved = true;
    workingDirectory = "";
    return true;
}

