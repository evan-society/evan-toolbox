#include "PrintNode.h"

#include <QFileDialog>
#include <QDateTime>
#include <QLocale>
#include <QTextStream>
#include <QMenuBar>
#include <QSettings>

#include "Labels/StringLabelValue.h"
#include "Labels/ScalarLabelValue.h"
#include "../../Utilities/Logger.h"


PrintNode::PrintNode(QWidget* parent) : QDialog(parent),  INode()
{
    setupUi(this);

    QMenuBar *menubar = new QMenuBar(this);
    QMenu *filemenu = new QMenu("File",menubar);
    QAction *saveAction = new QAction("Save",filemenu);
    filemenu->addAction(saveAction);
    menubar->addMenu(filemenu);
    gridLayout->setMenuBar(menubar);

    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveToFile()));

    textEdit_2->setWordWrapMode ( QTextOption::NoWrap );
    m_lastText = "";
}

void PrintNode::process()
{
	Printables* p = getInputPortData0();

	if (p == NULL || !p->isValid())
        return;

    QString text;
    QTextStream str(&text);

	for(unsigned int i=0; i<p->getSize(); ++i)
	{
   		if(p->getPrintable(i)->isType(IDataType::SPECIMENS_T))
        {
            Specimens * specimens = (Specimens*)p->getPrintable(i);
                str<<"[individuals]"<<endl;
                str<<specimens->getSize()<<endl;
                str<<"[landmarks]"<<endl;
                str<<specimens->getLandmarkCount()<<endl;
                str<<"[dimensions]"<<endl;
                str<<specimens->getLandmarkDimensions()<<endl;

                if (!specimens->getSpecimenLabels(0)->empty())
                {
                    QStringList groups;
                    QString group = specimens->getSpecimenLabelValue(0,"Group");
                    if (group.length()>0)
                    {
                        str<<"[groups]"<<endl;
                        for (unsigned int i=0;i<specimens->getSize();i++)
                        {
                            QString qstr = specimens->getSpecimenLabelValue(i,"Group");
                            if (!groups.contains(qstr))
                            {
                                groups.push_back(qstr);
                                int count = 0;
                                for (unsigned int j=0;j<specimens->getSize();j++)
                                {
                                    if (qstr == specimens->getSpecimenLabelValue(j,"Group"))
                                    {
                                        count++;
                                    }
                                }
                                str<<qstr<<" "<<count<<" ";
                            }
                        }
                        str<<endl;
                    }
                    QString id = specimens->getSpecimenLabelValue(0,"ID");
                    if (id.length()>0)
                    {
                        str<<"[names]"<<endl;
                        for (unsigned int i=0;i<specimens->getSize();i++)
                        {
                            QString qstr = specimens->getSpecimenLabelValue(i,"ID");
                            str<<qstr<<endl;
                        }
                    }
                    Labels *lbl = specimens->getSpecimenLabels(0);
                    QStringList labelNames = lbl->keys();

                    if (labelNames.size()>2 || (labelNames.size()==2 && (!labelNames.contains("Group") || !labelNames.contains("ID"))) )
                    {
                        str<<"[labels]"<<endl;
                        for (int i=0;i<labelNames.size();i++)
                        {
                            if (labelNames[i] != "ID" && labelNames[i] != "Group")
                                str<<labelNames[i]<<" ";
                        }
                        str<<endl;
                        str<<"[labelvalues]"<<endl;
                        for (unsigned int i=0;i<specimens->getSize();i++)
                        {
                            Labels *lbl = specimens->getSpecimenLabels(i);
                            QList<ILabelValue*> labelValues = lbl->values();

                            for (int j=0;j<labelValues.size();j++)
                            {
                                if (labelNames[j] != "ID" && labelNames[j] != "Group")
                                {
                                    if (labelValues[j]->getType()==ILabelValue::STRING_VALUE)
                                    {
                                        StringLabelValue * val = (StringLabelValue*)(labelValues[j]);
                                        str<<val->getValue()<<" ";
                                    }
                                    else if (labelValues[j]->getType()==ILabelValue::SCALAR_VALUE)
                                    {
                                        ScalarLabelValue * val = (ScalarLabelValue*)(labelValues[j]);
                                        str<<val->getValue()<<" ";
                                    }
                                }
                            }
                            str<<endl;
                        }
                    }
                }
                str<<"[rawpoints]"<<endl;
                for (unsigned int i=0;i<specimens->getSize();i++)
                {
                    LandmarkSet *landmarkSet = specimens->getLandmarkSet(i);
                    QString qstr = specimens->getSpecimenLabelValue(i,"ID");
                    if (qstr!="")
                    {
                        str<<"' "<<qstr<<endl;
                    }
                    else
                    {
                        str<<"' #"<<i+1<<endl;
                    }
                    for (unsigned int j=0;j<specimens->getLandmarkCount();j++)
                    {
                        str<<(*landmarkSet)[j][0];
                        for (unsigned int k=1;k<specimens->getLandmarkDimensions();k++)
                        {
                            str<<" "<<(*landmarkSet)[j][k];
                        }
                        str<<endl;
                    }
                }
        }
        else if(p->getPrintable(i)->isType(IDataType::CENTROIDSIZES_T))
		{
			Variables *v = (Variables*)(p->getPrintable(i));
			str <<	"\t" << "Centroid Size" << "\t" << "Ln of Centroid Size" << endl;
			for (unsigned int j=0;j<v->GetRows();j++)
			{
				QLocale loc;
				QString cs = loc.toString((*v)[j][0],'f',8);
				QString logcs = loc.toString(log((*v)[j][0]),'f',8);

				str << v->getRowLabel(j);
				str << "\t" << cs;
				str << "\t" << logcs <<endl;
			}
			str<< endl;
		}
		else if (p->getPrintable(i)->isType(IDataType::VARIABLES_T))
		{
			Variables *v = (Variables*)p->getPrintable(i);

			for (unsigned int j=0;j<v->GetCols();j++)
			{
				str << "\t" << v->getColumnLabel(j);
			}
			str << endl;

			for (unsigned int j=0;j<v->GetRows();j++)
			{
				str << v->getRowLabel(j);
				for (unsigned int i=0;i<v->GetCols();i++)
				{
					QLocale loc;
					QString d = loc.toString((*v)[j][i],'f',8);

					str << "\t" << d;
				}
				str<< endl;
			}
			str << endl;
		}
		else if(p->getPrintable(i)->isType(IDataType::TRANSFORMMATRIX_T))
		{
			TransformMatrix *tms = (TransformMatrix*)p->getPrintable(i);

			str << "Transform matrices" << endl;

			for (int j=0;j< tms->getSize();j++)
			{
				Matrix<double> *tm = tms->getTransformMatrix(j);
				for (int k=0;k< tm->GetRows();k++)
				{
					for (int l=0;l< tm->GetCols();l++)
					{
						QLocale loc;
						QString d = loc.toString((*tm)[k][l],'f',8);

						str << d << "\t";
					}
					str << endl;
				}
				str << endl;
			}
			str << endl;
		}
		else
		{
	//    	str << *v <<endl;
		}
	}

	if(m_lastText != text)
	{
		textEdit_2->append(QDateTime::currentDateTime().toString() + "\n\n" + text);
    	m_lastText = text;
        Logger::getInstance()->log("[Print Node] New output printed");
	}

//    Logger::getInstance()->log("[Print Node] Processing finished");
}

void PrintNode::accept()
{
    QDialog::accept();
}

void PrintNode::reject()
{
    QDialog::reject();
}

void PrintNode::saveToFile()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

	QString text = textEdit_2->toPlainText();
    if(text != "")
    {
		QString fileName = QFileDialog::getSaveFileName(0, tr("Save to file"),
														QString(),"*.txt\n*.*");

		if(fileName != "")
		{
			QFile file(fileName);
			file.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream out(&file);

			out << text;

			file.close();

			Logger::getInstance()->log("[Print Node] Output saved to file: " + fileName);

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
		}
    }
}

