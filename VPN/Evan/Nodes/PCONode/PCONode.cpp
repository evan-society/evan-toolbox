#include "PCONode.h"
#include "../../Utilities/Logger.h"

#include <sstream>

PCONode::PCONode(QWidget* parent) : QDialog(parent),  INode()
{
    m_distances = NULL;
    m_eigenvectors = new Variables();
    m_eigenvalues = new Variables();

    m_updated = true;
}

PCONode::~PCONode()
{}

void PCONode::process()
{
    m_distances = getInputPortData0();
    if (m_distances == NULL || !m_distances->isValid())
        return;

    if (!m_distances->IsSymmetric())
    {
        Logger::getInstance()->log("[PCO Node] Distance matrix must be symmetric.", Logger::RUN_ERROR);
        return;
    }
    Logger::getInstance()->log("[PCO Node] Processing started");
    m_pco.LoadData(m_distances->GetMatrix(),m_distances->GetRows());

    m_pco.PerformPCO();

    vector<double> eigval;
    vector<vector<double> > eigvec;

    m_pco.GetEigenvalues(eigval);
    m_pco.GetEigenvectors(eigvec);

    m_eigenvalues->ReSize(eigval.size(),1);
    m_eigenvalues->setColumnLabel(0,"Eigenvalues");
    for (unsigned int i=0;i<eigval.size();i++)
    {
         stringstream sstr;
         sstr<<i+1;
         m_eigenvalues->setRowLabel(i,sstr.str().c_str());
        (*m_eigenvalues)[i][0] = eigval[i];
    }

    if (eigvec.size()>0)
    {
        m_eigenvectors->ReSize(eigvec[0].size(),eigvec.size());
    }

    for (unsigned int i=0;i<eigvec.size();i++)
    {
        vector<double> tmpvec = eigvec[i];
        stringstream sstr;
        sstr<<"PCoord"<<i+1;
        m_eigenvectors->setColumnLabel(i,sstr.str().c_str());

        for (unsigned int j=0;j<tmpvec.size();j++)
        {
            (*m_eigenvectors)[j][i] = tmpvec[j];
        }

        for (unsigned int i=0;i<m_distances->GetRows();i++)
        {
            QString qstr = m_distances->getRowLabel(i);
            if(qstr=="")
            {
                qstr.setNum(i+1);
            }
            m_eigenvectors->setRowLabel(i,qstr);
        }
    }

    m_eigenvectors->clearSpecimenGroups();
    for (unsigned int i=0;i<m_distances->getSpecimenGroupNumber();i++)
    {
        SpecimenGroup* group = m_distances->getSpecimenGroup(i);
        if(group!=0)
        {
            m_eigenvectors->addSpecimenGroup(m_distances->getSpecimenGroupName(i), m_distances->getSpecimenGroup(i));
        }
    }

    setOutputPort0(m_eigenvectors);
    setOutputPort1(m_eigenvalues);

    Logger::getInstance()->log("[PCO Node] Processing finished");
}

bool PCONode::isUpdated() const
{
    return m_updated;
}

