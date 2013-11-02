#include "SurfaceTopItem.h"
#include "SurfaceItem.h"
#include "../IdDialog.h"
#include <QFileDialog>
#include <QSettings>

void SurfaceTopItem::addSurface(const QString& id, const QString& file, bool loaded)
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = loaded ? file : QFileDialog::getOpenFileName(0, tr("Locate a surface file"),
                                                                        fsLastDir,
                                                                        "Wavefront OBJ (*.obj)");
    if(fileName != "" && !fileName.isEmpty())
    {
        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }

        QString surfaceID = loaded ? id : fileName.left(fileName.lastIndexOf(".obj"));
        if((surfaceID.contains('\\') || surfaceID.contains('/')) && !loaded)
            surfaceID = surfaceID.right(surfaceID.length() - (surfaceID.lastIndexOf(QRegExp("[\\/]"))+1));

        IdDialog* detailsDialog = new IdDialog(0, surfaceID, surfaceID);
        detailsDialog->hideLabel();
        if(loaded ? true : detailsDialog->exec())
        {
            surfaceID = loaded? surfaceID : detailsDialog->getID();
            SurfaceItem* surfaceChild = new SurfaceItem();
            surfaceChild->setText(0, surfaceID);
            surfaceChild->setCheckState(1, Qt::Checked);
            surfaceChild->setCheckState(2, Qt::Checked);
            surfaceChild->setSurfaceFile(fileName);
            surfaceChild->setSurfaceID(surfaceID);
            //surfaceChild->setSurfaceLabel(loaded? surfaceID : detailsDialog->getLabel());
            if( addSpecimentOptions )
                surfaceChild->addSpecimentOptions();

            //For sorting
            int i = 0;
            for( ; i<childCount(); ++i)
            {
                if(surfaceID == child(i)->text(0))
                {
                    delete surfaceChild;
                    return;
                }
                else if(surfaceID < child(i)->text(0))
                    break;
            }
            insertChild(i, surfaceChild);


            //Signal the MainWindow to add the surfce in EW lib
            if(!loaded)
                if( !emit surfaceItemAdded(surfaceChild, m_parentForm) )
                {
                    removeChild(surfaceChild);
                    delete surfaceChild;
                    return;
                }

            setCheckState(1, Qt::Checked);
            surfaceChild->setSurfaceIndex(i);

            //Update the surface index the old surfaces
            for(int j=i+1 ; j<childCount(); ++j)
            {
                SurfaceItem* oldSurface = dynamic_cast< SurfaceItem* >( child(j) );
                oldSurface->setSurfaceIndex(j);
            }

            setExpanded(true);
            connect(surfaceChild, SIGNAL(surfaceFocus(SurfaceItem*)), this, SLOT(signalSurfaceFocus(SurfaceItem*)));
            connect(surfaceChild, SIGNAL(surfaceColorAction(SurfaceItem*, bool)), this, SLOT(signalSurfaceColor(SurfaceItem*, bool)));
            connect(surfaceChild, SIGNAL(surfaceDeleteAction(SurfaceItem*)), this, SLOT(signalDelete(SurfaceItem*)));
            delete detailsDialog;
        }
    }
}

void SurfaceTopItem::signalDelete(SurfaceItem* surItem)
{
    emit surfaceDeleted(m_parentForm, surItem->getSurfaceIndex());
    QTreeWidgetItem::removeChild(surItem);
    delete surItem;
    for(int i=0; i<childCount(); ++i)
    {
        SurfaceItem* surfaceItem = dynamic_cast< SurfaceItem* >( child(i) );
        surfaceItem->setSurfaceIndex(i);
    }
}
