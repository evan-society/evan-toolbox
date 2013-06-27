#ifndef VARIABLES_H
#define VARIABLES_H

#include "MatrixD.h"
#include "SpecimenGroup.h"
#include <QStringList>


class Variables : public MatrixD
{
private:
    QStringList m_colLabels;
    QStringList m_rowLabels;

    QStringList m_specimenGroupNames;
    QVector<DataTypePtr> m_specimenGroups;
public:
    Variables (size_t row = 0, size_t col = 0) : MatrixD(row,col)
    {
        addType(VARIABLES_T);
        addType(MATRIXD_T);
        for (unsigned int i=0;i<col;i++)
        {
        	QString lbl = QString("Col%i").arg(i+1);
            setColumnLabel(i,lbl);
        }
        for (unsigned int i=0;i<row;i++)
        {
            QString lbl = QString("%i").arg(i+1);
            setRowLabel(i,lbl);
        }
    }

    virtual ~Variables()
    {}
    QString getColumnLabel(int i)
    {
        QString retString = "";
        if (i<m_colLabels.size())
        {
            retString = m_colLabels.at(i);
        }
        return retString;
    }

    QString getRowLabel(int i)
    {
        QString retString = "";
        if (i<m_rowLabels.size())
        {
            retString = m_rowLabels.at(i);
        }
        return retString;
    }

    void setColumnLabel(int i, const QString& text)
    {
        if (i >= m_colLabels.size())
        {
            m_colLabels << text;
        }
        else
        {
            m_colLabels.replace(i,text);
        }
    }

    void setRowLabel(int i, const QString& text)
    {
        if (i >= m_rowLabels.size())
        {
            m_rowLabels << text;
        }
        else
        {
            m_rowLabels.replace(i,text);
        }
    }
    void addSpecimenGroup(const QString& groupName, SpecimenGroup* sg)
    {
        m_specimenGroups.push_back(DataTypePtr(sg));
        m_specimenGroupNames.push_back(groupName);
    }
    SpecimenGroup* getSpecimenGroup(int i)      const {return (SpecimenGroup*)m_specimenGroups[i].getPtr();}
    const QString& getSpecimenGroupName(int i)  const {return m_specimenGroupNames[i];}
    unsigned int getSpecimenGroupNumber()       const {return m_specimenGroups.size();}
    void clearSpecimenGroups()
    {
        for(int i=0; i<m_specimenGroups.size(); ++i)
            m_specimenGroups[i].release();
        m_specimenGroups.clear();
        m_specimenGroupNames.clear();
    }


    bool setFromContainer(DataTypePtr& rList, unsigned int partIndex)
    {
        return MatrixD::setFromContainer(rList,partIndex);
    }

};

#endif
