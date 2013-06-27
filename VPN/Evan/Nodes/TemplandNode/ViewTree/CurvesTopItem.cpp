#include "CurvesTopItem.h"
#include "CurveItem.h"
#include "../IdDialog.h"
#include <QFileDialog>
#include <QTextStream>
#include <QSettings>

#include "../Utilities/Logger.h"

void CurvesTopItem::addCurve(const QString& id, const QString& file, bool loaded)
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = loaded ? file : QFileDialog::getOpenFileName(0, tr("Locate a curve file"),
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

        QString curveID = loaded ? id : fileName.left(fileName.lastIndexOf(".obj"));
        if((curveID.contains('\\') || curveID.contains('/')) && !loaded)
            curveID = curveID.right(curveID.length() - (curveID.lastIndexOf(QRegExp("[\\/]"))+1));

        IdDialog* detailsDialog = new IdDialog(0, curveID, curveID);
        detailsDialog->hideLabel();
        if(loaded ? true : detailsDialog->exec())
        {
            curveID = loaded? curveID : detailsDialog->getID();
            CurveItem* curveChild = new CurveItem();
            curveChild->setText(0, curveID);
            curveChild->setCheckState(1, Qt::Checked);
            curveChild->setCheckState(2, Qt::Checked);
            curveChild->setCurveFile(fileName);
            curveChild->setCurveID(curveID);

            if( addSpecimentOptions )
                curveChild->addSpecimentOptions();

            //For sorting
            int i = 0;
            for( ; i<childCount(); ++i)
            {
                if(curveID == child(i)->text(0))
                {
                    delete curveChild;
                    return;
                }
                else if(curveID < child(i)->text(0))
                    break;
            }
            insertChild(i, curveChild);

            //Signal the MainWindow to add the curve in EW lib
            if(!loaded)
                if( !emit curveItemAdded(curveChild, m_parentForm) )
                {
                    removeChild(curveChild);
                    delete curveChild;
                    return;
                }

            setCheckState(1, Qt::Checked);
            curveChild->setCurveIndex(i);

            //Update the curve index the old curves
            for(int j=i+1 ; j<childCount(); ++j)
            {
                CurveItem* oldCurve = (CurveItem*)child(j);
                oldCurve->setCurveIndex(j);
            }

            setExpanded(true);
            connect(curveChild, SIGNAL(curveFocus(CurveItem*)), this, SLOT(signalCurveFocus(CurveItem*)));
            connect(curveChild, SIGNAL(curveColorAction(CurveItem*)), this, SLOT(signalCurveColor(CurveItem*)));
            connect(curveChild, SIGNAL(curveDeleteAction(CurveItem*)), this, SLOT(signalDelete(CurveItem*)));
            delete detailsDialog;

            emit signalCurveFocus(curveChild);
        }
    }
}

void CurvesTopItem::signalDelete(CurveItem* curItem)
{
    emit curveDeleted(m_parentForm, curItem->getCurveIndex());
    QTreeWidgetItem::removeChild(curItem);
    delete curItem;
    for(int i=0; i<childCount(); ++i)
    {
        CurveItem* curveItem = (CurveItem*)child(i);
        curveItem->setCurveIndex(i);
    }
}

void CurvesTopItem::createCurve(const QString& id, const QString& file, bool loaded)
{
/*
	QString fileName = QFileDialog::getSaveFileName(0, tr("Create a curve file"),
                                                QString(),"*.obj");
	if(fileName != "")
	{
        QString curveID = loaded ? id : fileName.left(fileName.lastIndexOf(".obj"));
        if((curveID.contains('\\') || curveID.contains('/')) && !loaded)
            curveID = curveID.right(curveID.length() - (curveID.lastIndexOf(QRegExp("[\\/]"))+1));

		IdDialog* detailsDialog = new IdDialog(0, curveID, curveID);
        detailsDialog->hideLabel();
        if(loaded ? true : detailsDialog->exec())
        {
            curveID = detailsDialog->getID();



        QFile file(fileName);
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream out(&file);

//        if( m_form != 0 )
		{
        	int num = 0;
			//for(unsigned int j=0; j<m_form->pointsets.size(); ++j)
			m_creating = true;
			while(m_creating)
			{
				if( m_form->pointsets[j].type == ew::Form3::TYPE_LANDMARK )
				{
          //      	out<<"v\t"
					<<m_form->pointsets[j].locations[0]<<"\t"
	//)				<<m_form->pointsets[j].locations[1]<<"\t"
		//			<<m_form->pointsets[j].locations[2]<<"\n";
		//			num++;
				}
			}
            for( int i = 1; i < num; ++i )
            {
            	out<<"e\t"<<i<<"\t"<<i+1<<"\n";
            }
		}
		file.close();




            CurveItem* curveChild = new CurveItem();
            curveChild->setText(0, curveID);
            curveChild->setCheckState(1, Qt::Checked);
            curveChild->setCheckState(2, Qt::Checked);
            curveChild->setCurveFile(fileName);
            curveChild->setCurveID(curveID);

            if( addSpecimentOptions )
                curveChild->addSpecimentOptions();

            //For sorting
            int i = 0;
            for( ; i<childCount(); ++i)
            {
                if(curveID == child(i)->text(0))
                {
                    delete curveChild;
                    return;
                }
                else if(curveID < child(i)->text(0))
                    break;
            }
            insertChild(i, curveChild);

            //Signal the MainWindow to add the curve in EW lib
            if(!loaded)
                if( !emit curveItemAdded(curveChild, m_parentForm) )
                {
                    removeChild(curveChild);
                    delete curveChild;
                    return;
                }

            setCheckState(1, Qt::Checked);
            curveChild->setCurveIndex(i);

            //Update the curve index the old curves
            for(int j=i+1 ; j<childCount(); ++j)
            {
                CurveItem* oldCurve = (CurveItem*)child(j);
                oldCurve->setCurveIndex(j);
            }

            setExpanded(true);
            connect(curveChild, SIGNAL(curveFocus(CurveItem*)), this, SLOT(signalCurveFocus(CurveItem*)));
            connect(curveChild, SIGNAL(curveColorAction(CurveItem*)), this, SLOT(signalCurveColor(CurveItem*)));
            connect(curveChild, SIGNAL(curveDeleteAction(CurveItem*)), this, SLOT(signalDelete(CurveItem*)));
            delete detailsDialog;

            emit signalCurveFocus(curveChild);
        }
	}
*/
}
