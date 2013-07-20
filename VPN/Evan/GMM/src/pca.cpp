#include <pca.h>
#include <svd.h>

#include <cmath> // for sqrt

PCA::PCA(double ** data, int rows, int cols)
{
    m_calculated = false;
    if (data != 0)
    {
        LoadData(data,rows,cols);
    }
}

PCA::PCA(Matrix<double> * data)
{
    m_calculated = false;
    if (!data->IsEmpty())
    {
        LoadData(data);
    }
}

PCA::~PCA()
{
}

void PCA::LoadData(double ** data, int rows, int cols)
{
    m_data.ReSize(rows,cols);

    for (int i=0;i<rows;i++)
    {
        for (int j=0;j<cols;j++)
        {
            m_data[i][j] = data[i][j];
        }
    }
    m_rows = rows;
    m_cols = cols;

    m_calculated = false;
}

void PCA::LoadData(Matrix<double> *data)
{
    m_data = *data;
    m_rows = data->GetRows();
    m_cols = data->GetCols();

    m_calculated = false;
}

void PCA::GetPCScores(Matrix<double> *scores)
{
    *scores = m_PCScores;
}

void PCA::GetPCLoadings(Matrix<double> *loadings, bool scale)
{
    *loadings = m_PCLoadings;
    if(scale)
    {
        if ( m_eigenvalues.size() == m_PCLoadings.GetCols() )
        {
            for (unsigned int i=0;i<m_PCLoadings.GetCols();i++)
            {
                for (unsigned int j=0;j<m_PCLoadings.GetRows();j++)
                {
                    (*loadings)[j][i] *= sqrt(m_eigenvalues[i]);
                }
            }
        }
    }
}

void PCA::GetEigenvalues(std::vector<double> *eigenvalues)
{
    *eigenvalues = m_eigenvalues;
}

void PCA::GetMean(std::vector<double> *mean)
{
    *mean = m_mean;
}

void PCA::PerformPCA(bool center, bool scale)
{
    Matrix<double> U,S,V;
    Matrix<double> tempdata = m_data;
    Row<double> mean = Mean(m_data);
    int pcs;   // number of principal components

    m_mean.resize(m_cols);
    if (center)
    {
        for (int i=0;i<m_cols;i++)
        {
            for (int j=0;j<m_rows;j++)
            {
                tempdata[j][i] -= mean[i];
            }
            m_mean[i] = mean[i];
        }
    }
    else
    {
        for (int i=0;i<m_cols;i++)
        {
            m_mean[i] = 0.0;
        }
    }

    if (scale)
    {
        for (int i=0;i<m_cols;i++)
        {
            double sumsquared = 0.0;
            for (int j=0;j<m_rows;j++)
            {
                sumsquared += tempdata[j][i] * tempdata[j][i];
            }
            double stdev = 0.0;
            if (center)
            {
                stdev = sqrt(sumsquared / (m_rows-1));
            }
            else
            {
                stdev = sqrt(sumsquared);
            }
            for (int j=0;j<m_rows;j++)
            {
                tempdata[j][i] /= stdev;
            }
        }
    }

    if (tempdata.GetRows() >= tempdata.GetCols())
    {
        svd(tempdata,&U,&S,&V);
    }
    else
    {
        Matrix<double> tempdataT = Transpose(tempdata);
        svd(tempdataT,&V,&S,&U);
    }

    pcs = S.GetCols();

    for (unsigned int i=0;i<S.GetCols();i++)
    {
        S[i][i] *= S[i][i] / (m_rows-1);
    }

    m_eigenvalues.clear();
    for (int i=0;i<pcs;i++)
    {
        m_eigenvalues.push_back(S[i][i]);
    }

    m_PCScores.ReSize(m_rows,pcs);
    m_PCLoadings.ReSize(V.GetRows(),pcs);
    Matrix<double> scores = tempdata * V;

    for (int i=0;i<m_rows;i++)
    {
        for (int j=0;j<pcs;j++)
        {
            m_PCScores[i][j] = scores[i][j];
        }
    }
    for (unsigned int i=0;i<V.GetRows();i++)
    {
        for (int j=0;j<pcs;j++)
        {
            m_PCLoadings[i][j] = V[i][j];
        }
    }

    m_calculated = true;
}

bool PCA::IsCalculated()
{
    return m_calculated;
}

