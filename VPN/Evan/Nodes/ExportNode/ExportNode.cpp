#include "ExportNode.h"
#include <QFileDialog>
#include <unistd.h>
#include "../../Utilities/Logger.h"
#include "../../Utilities/mexcel.h"
#include "../../DataTypes/Variables.h"
#include <iomanip>
#include <sstream>
#include "../../Labels/StringLabelValue.h"
#include "../../Labels/ScalarLabelValue.h"
#include <QSettings>

#include <cstdio>

using namespace miniexcel;


ExportNode::ExportNode(QWidget* parent) : QDialog(parent),  INode()
{
    m_matrix = NULL;
    m_surface = NULL;
    m_specimens = NULL;
    setupUi(this);

    QObject::connect(pushButton, SIGNAL(clicked()), this, SLOT(exportSpecimens()));
    QObject::connect(pushButton_2, SIGNAL(clicked()), this, SLOT(exportSurface()));
    QObject::connect(pushButton_3, SIGNAL(clicked()), this, SLOT(exportMatrix()));

    QObject::connect(radioButton, SIGNAL(clicked()), this, SLOT(enableLabelsCheckBox()));
    QObject::connect(radioButton_2, SIGNAL(clicked()), this, SLOT(enableLabelsCheckBox()));
    QObject::connect(radioButton_3, SIGNAL(clicked()), this, SLOT(enableLabelsCheckBox()));
    QObject::connect(radioButton_4, SIGNAL(clicked()), this, SLOT(disableLabelsCheckBox()));
    QObject::connect(radioButton_5, SIGNAL(clicked()), this, SLOT(disableLabelsCheckBox()));

    m_updated = true;

    // export of ply and sur surfaces not implemented
    radioButton_9->setDisabled(true);
    radioButton_14->setDisabled(true);

}

ExportNode::~ExportNode()
{}

void ExportNode::process()
{}

bool ExportNode::isUpdated() const
{
    return m_updated;
}

QString ExportNode::toString() const
{
    QString nodeData = "";

    if (radioButton->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_2->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_3->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_4->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_5->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_6->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_7->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_8->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_9->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_10->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_11->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_12->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_13->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (radioButton_14->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (checkBox->isChecked())
        nodeData += "1$$";
    else
        nodeData += "0$$";

    if (checkBox_3->isChecked())
        nodeData += "1";
    else
        nodeData += "0";

    return  nodeData;
}

void ExportNode::fromString(const QString& params)
{
    QStringList nodeData = params.split("$$");

    if (nodeData.count() == 16)
    {
        if (nodeData[0] == "1")
        {
            radioButton->setChecked(true);
        }
        if (nodeData[1] == "1")
        {
            radioButton_2->setChecked(true);
        }
        if (nodeData[2] == "1")
        {
            radioButton_3->setChecked(true);
        }
        if (nodeData[3] == "1")
        {
            radioButton_4->setChecked(true);
        }
        if (nodeData[4] == "1")
        {
            radioButton_5->setChecked(true);
        }
        if (nodeData[5] == "1")
        {
            radioButton_6->setChecked(true);
        }
        if (nodeData[6] == "1")
        {
            radioButton_7->setChecked(true);
        }
        if (nodeData[7] == "1")
        {
            radioButton_8->setChecked(true);
        }
        if (nodeData[8] == "1")
        {
            radioButton_9->setChecked(true);
        }
        if (nodeData[9] == "1")
        {
            radioButton_10->setChecked(true);
        }
        if (nodeData[10] == "1")
        {
            radioButton_11->setChecked(true);
        }
        if (nodeData[11] == "1")
        {
            radioButton_12->setChecked(true);
        }
        if (nodeData[12] == "1")
        {
            radioButton_13->setChecked(true);
        }
        if (nodeData[13] == "1")
        {
            radioButton_14->setChecked(true);
        }

        checkBox->setChecked(nodeData[14] == "1");
        checkBox_3->setChecked(nodeData[15] == "1");

        m_updated = true;
    }
}

void ExportNode::exportSpecimens()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    m_specimens = getInputPortData0();
    if (m_specimens != NULL && m_specimens->isValid())
    {
        QString ext;
        if (radioButton->isChecked()) // txt
        {
            ext = "Text file (*.txt)";
        }
        else if (radioButton_2->isChecked()) // csv
        {
            ext = "Comma seperated variables (*.csv)";
        }
        else if (radioButton_3->isChecked()) // xls
        {
            ext = "Excel spreadsheet (*.xls)";
        }
        else if (radioButton_4->isChecked()) // xls
        {
            ext = "NTSYSpc file (*.nts)";
        }
        else if (radioButton_5->isChecked()) // morphologika
        {
            ext = "Morphologika file (*.txt)";
        }

        QString fileName = QFileDialog::getSaveFileName(this, tr("Export specimens"),
                           fsLastDir, ext);
        if (fileName != "")
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

            if (radioButton->isChecked()) // txt
            {
                ofstream out(fileName.toStdString().c_str());
                out << setprecision(12);

                if (checkBox->isChecked()) // include labels
                {
                    out<<"ID";
                    if (m_specimens->getLandmarkDimensions() == 3)
                    {
                        for (unsigned int i=0;i<m_specimens->getLandmarkCount();i++)
                        {
                            out<<"\t"<<"x"<<i+1;
                            out<<"\t"<<"y"<<i+1;
                            out<<"\t"<<"z"<<i+1;
                        }
                    }
                    else
                    {
                        for (unsigned int i=0;i<m_specimens->getLandmarkCount();i++)
                        {
                            out<<"\t"<<"x"<<i+1;
                            out<<"\t"<<"y"<<i+1;
                        }
                    }
                    out<<endl;
                    for (unsigned int i=0;i<m_specimens->getSize();i++)
                    {
                        LandmarkSet *landmarkSet = m_specimens->getLandmarkSet(i);
                        out<<m_specimens->getSpecimenLabelValue(i,"ID").toStdString();
                        for (unsigned int k=0;k<m_specimens->getLandmarkDimensions();k++)
                        {
                            out<<"\t"<<(*landmarkSet)[0][k];
                        }
                        for (unsigned int j=1;j<m_specimens->getLandmarkCount();j++)
                        {
                            for (unsigned int k=0;k<m_specimens->getLandmarkDimensions();k++)
                            {
                                out<<"\t"<<(*landmarkSet)[j][k];
                            }
                        }
                        out<<endl;
                    }
                }
                else
                {
                    for (unsigned int i=0;i<m_specimens->getSize();i++)
                    {
                        LandmarkSet *landmarkSet = m_specimens->getLandmarkSet(i);
                        out<<(*landmarkSet)[0][0];
                        for (unsigned int k=1;k<m_specimens->getLandmarkDimensions();k++)
                        {
                            out<<"\t"<<(*landmarkSet)[0][k];
                        }
                        for (unsigned int j=1;j<m_specimens->getLandmarkCount();j++)
                        {
                            for (unsigned int k=0;k<m_specimens->getLandmarkDimensions();k++)
                            {
                                out<<"\t"<<(*landmarkSet)[j][k];
                            }
                        }
                        out<<endl;
                    }
                }
                out.close();
            }
            else if (radioButton_2->isChecked()) // csv
            {
                ofstream out(fileName.toStdString().c_str());
                out << setprecision(12);

                if (checkBox->isChecked()) // include labels
                {
                    out<<"ID";
                    if (m_specimens->getLandmarkDimensions() == 3)
                    {
                        for (unsigned int i=0;i<m_specimens->getLandmarkCount();i++)
                        {
                            out<<","<<"x"<<i+1;
                            out<<","<<"y"<<i+1;
                            out<<","<<"z"<<i+1;
                        }
                    }
                    else
                    {
                        for (unsigned int i=0;i<m_specimens->getLandmarkCount();i++)
                        {
                            out<<","<<"x"<<i+1;
                            out<<","<<"y"<<i+1;
                        }
                    }
                    out<<endl;
                    for (unsigned int i=0;i<m_specimens->getSize();i++)
                    {
                        LandmarkSet *landmarkSet = m_specimens->getLandmarkSet(i);
                        out<<m_specimens->getSpecimenLabelValue(i,"ID").toStdString();
                        for (unsigned int k=0;k<m_specimens->getLandmarkDimensions();k++)
                        {
                            out<<","<<(*landmarkSet)[0][k];
                        }
                        for (unsigned int j=1;j<m_specimens->getLandmarkCount();j++)
                        {
                            for (unsigned int k=0;k<m_specimens->getLandmarkDimensions();k++)
                            {
                                out<<","<<(*landmarkSet)[j][k];
                            }
                        }
                        out<<endl;
                    }
                }
                else
                {
                    for (unsigned int i=0;i<m_specimens->getSize();i++)
                    {
                        LandmarkSet *landmarkSet = m_specimens->getLandmarkSet(i);
                        out<<(*landmarkSet)[0][0];
                        for (unsigned int k=1;k<m_specimens->getLandmarkDimensions();k++)
                        {
                            out<<","<<(*landmarkSet)[0][k];
                        }
                        for (unsigned int j=1;j<m_specimens->getLandmarkCount();j++)
                        {
                            for (unsigned int k=0;k<m_specimens->getLandmarkDimensions();k++)
                            {
                                out<<","<<(*landmarkSet)[j][k];
                            }
                        }
                        out<<endl;
                    }
                }
                out.close();
            }
            else if (radioButton_3->isChecked()) // xls
            {
                FILE *f;
                if (!(f = fopen (fileName.toStdString().c_str(), "wb")))
                {
                    Logger::getInstance()->log("\tFailed to open file for exporting: " + fileName, Logger::WARNING);
                    return;
                }

                CMiniExcel miniexcel;
                if (checkBox->isChecked()) // include labels
                {
                    miniexcel(0,0)="ID";
                    int k=1;
                    if (m_specimens->getLandmarkDimensions() == 3)
                    {
                        for (unsigned int i=0;i<m_specimens->getLandmarkCount();i++)
                        {
                            stringstream ssx;
                            stringstream ssy;
                            stringstream ssz;
                            ssx<<"x"<<i+1;
                            ssy<<"y"<<i+1;
                            ssz<<"z"<<i+1;
                            miniexcel(0,k)=ssx.str().c_str();
                            k++;
                            miniexcel(0,k)=ssy.str().c_str();
                            k++;
                            miniexcel(0,k)=ssz.str().c_str();
                            k++;
                        }
                    }
                    else
                    {
                        for (unsigned int i=0;i<m_specimens->getLandmarkCount();i++)
                        {
                            stringstream ssx;
                            stringstream ssy;
                            ssx<<"x"<<i+1;
                            ssy<<"y"<<i+1;
                            miniexcel(0,k)=ssx.str().c_str();
                            k++;
                            miniexcel(0,k)=ssy.str().c_str();
                            k++;
                        }
                    }

                    for (unsigned int i=0;i<m_specimens->getSize();i++)
                    {
                        miniexcel(i+1,0) = m_specimens->getSpecimenLabelValue(i,"ID").toStdString().c_str();

                        LandmarkSet *landmarkSet = m_specimens->getLandmarkSet(i);
                        for (unsigned int j=0;j<m_specimens->getLandmarkCount();j++)
                        {
                            for (unsigned int k=0;k<m_specimens->getLandmarkDimensions();k++)
                            {
                                miniexcel(i+1,j*m_specimens->getLandmarkDimensions()+k+1) = (*landmarkSet)[j][k];
                            }
                        }
                    }
                }
                else
                {
                    for (unsigned int i=0;i<m_specimens->getSize();i++)
                    {
                        LandmarkSet *landmarkSet = m_specimens->getLandmarkSet(i);
                        QString number;
                        number.setNum(i+1);
                        QString name = "' #" + number;

                        for (unsigned int j=0;j<m_specimens->getLandmarkCount();j++)
                        {
                            for (unsigned int k=0;k<m_specimens->getLandmarkDimensions();k++)
                            {
                                miniexcel(i,j*m_specimens->getLandmarkDimensions()+k) = (*landmarkSet)[j][k];
                            }
                        }
                    }
                }
                miniexcel.Write(f);
            }
            else if (radioButton_4->isChecked()) // nts
            {
                ofstream out(fileName.toStdString().c_str());
                out << setprecision(12);

                out<<"1 ";
                out<<m_specimens->getSize();
                out<<"L ";
                out<<m_specimens->getLandmarkCount()*m_specimens->getLandmarkDimensions();
                out<<" 0";
                if (m_specimens->getLandmarkDimensions()==3)
                {
                    out<<" DIM=3";
                }
                out<<endl;

                for (unsigned int i=0;i<m_specimens->getSize();i++)
                {
                    QString qstr = m_specimens->getSpecimenLabelValue(i,"ID");
                    out<<qstr.toStdString()<<" ";
                }
                out<<endl;

                for (unsigned int i=0;i<m_specimens->getSize();i++)
                {
                    LandmarkSet *landmarkSet = m_specimens->getLandmarkSet(i);
                    out<<(*landmarkSet)[0][0];
                    for (unsigned int k=1;k<m_specimens->getLandmarkDimensions();k++)
                    {
                        out<<" "<<(*landmarkSet)[0][k];
                    }
                    for (unsigned int j=1;j<m_specimens->getLandmarkCount();j++)
                    {
                        for (unsigned int k=0;k<m_specimens->getLandmarkDimensions();k++)
                        {
                            out<<" "<<(*landmarkSet)[j][k];
                        }
                    }
                    out<<endl;
                }
                out.close();
            }
            else if (radioButton_5->isChecked()) // morphologika
            {
                ofstream out(fileName.toStdString().c_str());
                out << setprecision(12);

                out<<"[individuals]"<<endl;
                out<<m_specimens->getSize()<<endl;
                out<<"[landmarks]"<<endl;
                out<<m_specimens->getLandmarkCount()<<endl;
                out<<"[dimensions]"<<endl;
                out<<m_specimens->getLandmarkDimensions()<<endl;

                if (!m_specimens->getSpecimenLabels(0)->empty())
                {
                    QStringList groups;
                    QString group = m_specimens->getSpecimenLabelValue(0,"Group");
                    if (group.length()>0)
                    {
                        out<<"[groups]"<<endl;
                        for (unsigned int i=0;i<m_specimens->getSize();i++)
                        {
                            QString qstr = m_specimens->getSpecimenLabelValue(i,"Group");
                            if (!groups.contains(qstr))
                            {
                                groups.push_back(qstr);
                                int count = 0;
                                for (unsigned int j=0;j<m_specimens->getSize();j++)
                                {
                                    if (qstr == m_specimens->getSpecimenLabelValue(j,"Group"))
                                    {
                                        count++;
                                    }
                                }
                                out<<qstr.toStdString()<<" "<<count<<" ";
                            }
                        }
                        out<<endl;
                    }
                    QString id = m_specimens->getSpecimenLabelValue(0,"ID");
                    if (id.length()>0)
                    {
                        out<<"[names]"<<endl;
                        for (unsigned int i=0;i<m_specimens->getSize();i++)
                        {
                            QString qstr = m_specimens->getSpecimenLabelValue(i,"ID");
                            out<<qstr.toStdString()<<endl;
                        }
                    }
                    Labels *lbl = m_specimens->getSpecimenLabels(0);
                    QStringList labelNames = lbl->keys();

                    if (labelNames.size()>2 || (labelNames.size()==2 && (!labelNames.contains("Group") || !labelNames.contains("ID"))) )
                    {
                        out<<"[labels]"<<endl;
                        for (int i=0;i<labelNames.size();i++)
                        {
                            if (labelNames[i] != "ID" && labelNames[i] != "Group")
                                out<<labelNames[i].toStdString()<<" ";
                        }
                        out<<endl;
                        out<<"[labelvalues]"<<endl;
                        for (unsigned int i=0;i<m_specimens->getSize();i++)
                        {
                            Labels *lbl = m_specimens->getSpecimenLabels(i);
                            QList<ILabelValue*> labelValues = lbl->values();

                            for (int j=0;j<labelValues.size();j++)
                            {
                                if (labelNames[j] != "ID" && labelNames[j] != "Group")
                                {
                                    if (labelValues[j]->getType()==ILabelValue::STRING_VALUE)
                                    {
                                        StringLabelValue * val = (StringLabelValue*)(labelValues[j]);
                                        out<<val->getValue().toStdString()<<" ";
                                    }
                                    else if (labelValues[j]->getType()==ILabelValue::SCALAR_VALUE)
                                    {
                                        ScalarLabelValue * val = (ScalarLabelValue*)(labelValues[j]);
                                        out<<val->getValue()<<" ";
                                    }
                                }
                            }
                            out<<endl;
                        }
                    }
                }
                out<<"[rawpoints]"<<endl;
                for (unsigned int i=0;i<m_specimens->getSize();i++)
                {
                    LandmarkSet *landmarkSet = m_specimens->getLandmarkSet(i);
                    QString qstr = m_specimens->getSpecimenLabelValue(i,"ID");
                    if (qstr!="")
                    {
                        out<<"' "<<qstr.toStdString()<<endl;
                    }
                    else
                    {
                        out<<"' #"<<i+1<<endl;
                    }
                    for (unsigned int j=0;j<m_specimens->getLandmarkCount();j++)
                    {
                        out<<(*landmarkSet)[j][0];
                        for (unsigned int k=1;k<m_specimens->getLandmarkDimensions();k++)
                        {
                            out<<" "<<(*landmarkSet)[j][k];
                        }
                        out<<endl;
                    }
                }
            }

            Logger::getInstance()->log("\tSpecimens exported to file: " + fileName);
        }
    }
    else
    {
        Logger::getInstance()->log("No specimens found",Logger::WARNING);
    }
}

void ExportNode::exportSurface()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    m_surface = getInputPortData1();
    if (m_surface != NULL && m_surface->isValid())
    {
        QString ext;
        if (radioButton_6->isChecked()) // stl
        {
            ext = "Stereolithography (*.stl)";
        }
        else if (radioButton_7->isChecked()) // obj
        {
            ext = "Wavefront object (*.obj)";
        }
        else if (radioButton_8->isChecked()) // osg
        {
            ext = "OpenSceneGraph (*.osg)";
        }
        else if (radioButton_9->isChecked()) // ply
        {
            ext = "Polygons (*.ply)";
        }
        else if (radioButton_14->isChecked()) // sur
        {
            ext = "Edgewarp (*.sur)";
        }

        QString fileName = QFileDialog::getSaveFileName(this, tr("Export surface"),
                                                    fsLastDir,ext);

        if (fileName != "")
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

            bool success = m_surface->saveToFile(fileName);
            if(success)
            {
                Logger::getInstance()->log("Surface exported to: " + fileName);
            }
            else
            {
                Logger::getInstance()->log("Failed to export surface",Logger::RUN_ERROR);
            }
        }
    }
    else
    {
        Logger::getInstance()->log("No surface found",Logger::WARNING);
    }
}

void ExportNode::exportMatrix()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    m_matrix = getInputPortData2();

    if (m_matrix != NULL && m_matrix->isValid())
    {
        QString ext;
        if (radioButton_10->isChecked()) // txt
        {
            ext = "Text file (*.txt)";
        }
        else if (radioButton_11->isChecked()) // csv
        {
            ext = "Comma seperated variables (*.csv)";
        }
        else if (radioButton_12->isChecked()) // xls
        {
            ext = "Excel spreadsheet (*.xls)";
        }
        else if (radioButton_13->isChecked()) // xls
        {
            ext = "NTSYSpc file (*.nts)";
        }

        QString fileName = QFileDialog::getSaveFileName(this, tr("Export matrix"),
                           fsLastDir,ext);
        if(fileName!="")
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

            ofstream out(fileName.toStdString().c_str());
            out << setprecision(12);

            Variables * var;
            if (m_matrix->isType(IDataType::VARIABLES_T))
            {
                var = (Variables*) m_matrix;
            }
            else
            {
                var = new Variables(m_matrix->GetRows(),m_matrix->GetCols());
            }
            if (radioButton_10->isChecked()) // txt
            {
                if (checkBox_3->isChecked()) //include labels
                {
                    if (m_matrix->GetCols()>0)
                    {
                        out<<"ID";
                        for (unsigned int i=0;i<m_matrix->GetCols();i++)
                        {
                            out<<"\t"<<var->getColumnLabel(i).toStdString();
                        }
                        out<<endl;
                    }
                    for (unsigned int i=0;i<m_matrix->GetRows();i++)
                    {
                        out<<var->getRowLabel(i).toStdString();

                        for (unsigned int j=0;j<m_matrix->GetCols();j++)
                        {
                            out<<"\t"<<(*m_matrix)[i][j];
                        }
                        out<<endl;
                    }
                }
                else
                {
                    out<<(*m_matrix)<<endl;
                }
            }
            else if (radioButton_11->isChecked()) // cvs
            {
                if (checkBox_3->isChecked()) //include labels
                {
                    if (m_matrix->GetCols()>0)
                    {
                        out<<"ID";
                        for (unsigned int i=0;i<m_matrix->GetCols();i++)
                        {
                            out<<","<<var->getColumnLabel(i).toStdString();
                        }
                        out<<endl;
                    }
                    for (unsigned int i=0;i<m_matrix->GetRows();i++)
                    {
                        out<<var->getRowLabel(i).toStdString();

                        for (unsigned int j=0;j<m_matrix->GetCols();j++)
                        {
                            out<<","<<(*m_matrix)[i][j];
                        }
                        out<<endl;
                    }
                }
                else
                {
                    for (unsigned int i=0;i<m_matrix->GetRows();i++)
                    {
                        out<<(*m_matrix)[i][0];
                        for (unsigned int j=1;j<m_matrix->GetCols();j++)
                        {
                            out<<","<<(*m_matrix)[i][j];
                        }
                        out<<endl;
                    }
                }
            }
            else if (radioButton_12->isChecked()) // xls
            {
                FILE *f;
                if (!(f = fopen (fileName.toStdString().c_str(), "wb")))
                {
                    Logger::getInstance()->log("\tFailed to open file for exporting: " + fileName, Logger::WARNING);
                    return;
                }

                CMiniExcel miniexcel;

                if (checkBox_3->isChecked()) //include labels
                {
                    miniexcel(0,0) = "ID";
                    for (unsigned int i=0;i<m_matrix->GetCols();i++)
                    {
                        miniexcel(0,i+1) = var->getColumnLabel(i).toStdString().c_str();
                    }
                    for (unsigned int i=0;i<m_matrix->GetRows();i++)
                    {
                        miniexcel(i+1,0) = var->getRowLabel(i).toStdString().c_str();
                        for (unsigned int j=0;j<m_matrix->GetCols();j++)
                        {
                            miniexcel(i+1,j+1) = (*m_matrix)[i][j];
                        }
                    }
                }
                else
                {
                    for (unsigned int i=0;i<m_matrix->GetRows();i++)
                    {
                        for (unsigned int j=0;j<m_matrix->GetCols();j++)
                        {
                            miniexcel(i,j) = (*m_matrix)[i][j];
                        }
                    }
                }
                miniexcel.Write(f);
            }
            else if (radioButton_13->isChecked()) // nts
            {
                if (m_matrix->IsSymmetric()) //is symmetric
                {
                    out<<"2 ";
                    out<<m_matrix->GetRows();
                    out<<" ";
                    out<<m_matrix->GetCols();
                    out<<" 0"<<endl;
                    for (unsigned int i=0;i<m_matrix->GetRows();i++)
                    {
                        out<<(*m_matrix)[i][0];
                        for (unsigned int j=1;j<=i;j++)
                        {
                            out<<" "<<(*m_matrix)[i][j];
                        }
                        out<<endl;
                    }
                }
                else
                {
                    out<<"1 ";
                    out<<m_matrix->GetRows();
                    out<<" ";
                    out<<m_matrix->GetCols();
                    out<<" 0"<<endl;

                    for (unsigned int i=0;i<m_matrix->GetRows();i++)
                    {
                        for (unsigned int j=0;j<m_matrix->GetCols();j++)
                        {
                            out<<(*m_matrix)[i][j]<<" ";
                        }
                        out<<endl;
                    }
                }
            }
            if (!m_matrix->isType(IDataType::VARIABLES_T))
            {
                delete var;
            }
            out.close();

            Logger::getInstance()->log("\tMatrix exported to file: " + fileName);
        }
        else
        {
            Logger::getInstance()->log("No data matrix found",Logger::WARNING);
        }
    }
}
