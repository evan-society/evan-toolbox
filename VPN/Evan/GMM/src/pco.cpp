#include <pco.h>
#include <svd.h>

#include <cmath> // for sqrt

PCO::PCO(double ** distances, int size)
{
    m_calculated = false;
    if (distances != 0)
    {
        LoadData(distances,size);
    }
}

PCO::~PCO()
{
}

void PCO::LoadData(double ** distances, int size)
{
    m_distances.ReSize(size,size);
    for (int i=0;i<size;i++)
    {
        for (int j=0;j<size;j++)
        {
            m_distances[i][j] = distances[i][j];
        }
    }
    m_size = size;

    m_calculated = false;
}

void PCO::GetPrincipalCoordinates(Matrix<double> &pcoords)
{
    pcoords = m_principalCoordinates;
}

void PCO::GetEigenvalues(std::vector<double> &eigenvalues)
{
    eigenvalues = m_eigenvalues;
}

void PCO::GetEigenvectors(std::vector<std::vector<double> > &eigenvectors)
{
    eigenvectors = m_eigenvectors;
}

void PCO::GetDistances(Matrix<double> &dist)
{
    dist = m_distances;
}

void PCO::PerformPCO()
{
    int n = m_size;
    double A = 0.0;
    for (int i=0;i<n;i++)
    {
        for (int j=0;j<n;j++)
        {
            A -= 0.5 * (m_distances[i][j] * m_distances[i][j]);
        }
    }
    A = A / (double)(n*n);

//    double Arow[n];
    std::vector<double> Arow(n);

    for (int i=0;i<n;i++)
    {
        Arow[i] = 0.0;
    }
    for (int i=0;i<n;i++)
    {
        for (int j=0;j<n;j++)
        {
            Arow[i] -= 0.5 * (m_distances[i][j] * m_distances[i][j]);
        }
        Arow[i] /= (double)(n);
    }

    Matrix<double> Bm(n,n);

    for (int i=0;i<n;i++)
    {
        for (int j=0;j<n;j++)
        {
            Bm[i][j] = -0.5 * (m_distances[i][j] * m_distances[i][j]) - Arow[i] - Arow[j] + A;
        }
    }

    Matrix<double> U(n,n);
    Matrix<double> S(n,n);
    Matrix<double> V(n,n);

    svd(Bm,&U,&S,&V);
    Matrix<double> D(n,3);

    double sumEV = 0.0;
    for (int i=0;i<n;i++)
    {
        sumEV += S[i][i];
    }

    D[0][0] = S[0][0];
    D[0][1] = D[0][0] / sumEV;
    D[0][2] = D[0][1];

    for (int i=1;i<n;i++)
    {
        D[i][0] = S[i][i];
        D[i][1] = D[i][0] / sumEV;
        D[i][2] = D[i][1]+D[i-1][2];
    }

    m_eigenvalues.clear();
    int dim = n;
    for (int i=0;i<n;i++)
    {
        cout<<D[i][2]<<endl;

        m_eigenvalues.push_back(S[i][i]);
        if ( 1.0 - D[i][2] < 1.0e-8)
        {
            dim = i+1;
            break;
        }
    }

    for (int i=0;i<dim;i++)
    {
        double temp = sqrt(D[i][0]);
        for (int j=0;j<n;j++)
        {
            V[j][i] *= temp;
        }
    }

    // store results
    m_principalCoordinates.ReSize(n,dim);

    for (int i=0;i<n;i++)
    {
        for (int j=0;j<dim;j++)
        {
            m_principalCoordinates[i][j] = V[i][j];
        }
    }

    m_eigenvectors.clear();
    for (int j=0;j<dim;j++)
    {
        std::vector<double> eig;
        for (int i=0;i<n;i++)
        {
            eig.push_back(V[i][j]);
        }
        m_eigenvectors.push_back(eig);
    }
    m_calculated = true;
}

int PCO::GetSize()
{
    return m_size;
}

bool PCO::IsCalculated()
{
    return m_calculated;
}

