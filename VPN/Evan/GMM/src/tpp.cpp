#include "tpp.h"
#include <vector>


void
helmertize(Matrix<double> &h, int k)
{
    //  fill array k x k-1 with
    //  1/sqrt(j(j+1) in all elements left of and including the diagonal
    //  and j/sqrt(j(j+1) in the elements immediatly to the right of the diagonal
    //  diagonal goes from top left to bottom right, all other elements zero

    int i,j;

    // first set all cells to zero
    for (j=0; j<k;j++)
    {
        for (i=0; i<k-1;i++)
        {
            h[i][j] = 0.0;
        }
    }

    //  now fill values
    for (j=1; j<=k-1;j++)
    {
        for (i=1; i<=j;i++)
        {
            h[j-1][i-1] = -1.0 / sqrtf(j * (j + 1.0));
        }
        h[j-1][j] = j / sqrtf(j * (j + 1.0));
    }
}


void
overallmean(double *mean, double **data, int n, int k, int m)
{
    // calculate the mean of data matrix
    int i,j;

    // first set all values to zero
    for (j=0;j<k*m;j++)
    {
        mean[j] = 0.0;
    }

    // sum up values
    for (i=0;i<n;i++)
    {
        for (j=0; j<k*m;j++)
        {
            mean[j] += data[i][j];
        }
    }

    // find mean
    for (j=0;j<k*m;j++)
    {
        mean[j] /= n;
    }
}

void
unitsize(double *unit, int k, int m)
{
    // scale vector to unit size
    int i,j;
    std::vector<double> centroid(m);

    for (i=0;i<m;i++)
    {
        centroid[i] = 0.0;
        for (j=0;j<k*m;j+=m)
        {
            centroid[i] += unit[j];
        }
        centroid[i] /= k;
    }

    double tmpsize =0.0;

    for (i=0;i<k*m;i+=m)
    {
        for (j=0;j<m;j++)
        {
            tmpsize += (unit[i+j]-centroid[j])*(unit[i+j]-centroid[j]);
        }
    }
    double size = sqrtf(tmpsize);

    for (i=0;i<k*m;i++)
    {
        unit[i] /= size;
    }
}

void
loader(Matrix<double> &newdat, double *unit, int k, int m)
{
    // load data from 1 dimensional vector unit
    // into 2 dimensional matrix newdat
    int count = 0;
    for (int i=0;i<=k;i++)
    {
        for (int j=0;j<m;j++)
        {
            newdat[i][j] = unit[count];
            count++;
        }
    }
}


void
rearrange(double *unit, int k, int m)
{
    // this subroutine rearranges data such that all x's are placed in the first
    // k rows, all y's in the next k and all z's in the next k
    int i,j;
    int count = 0;

    std::vector<double> tmp(k*m);

    for (i=0; i<m;i++)
    {
        for (j=1; j<=k*m;j+=m)
        {
            tmp[count] = unit[j+i-1];
            count++;
        }
    }

    // now write tmp into unit
    for (i=0; i<k*m;i++)
    {
        unit[i] = tmp[i];
    }
}

void
calcvecgamma(double *unit, double *vecgamma, int landmarks, int dimensions)
{
    // calculate vectorized, unit sized, helmertised vector, vecgamma

    Matrix<double> mnewdat(landmarks+1,dimensions);
    Matrix<double> mhelmert(landmarks+1,landmarks+1);
    Matrix<double> mproductc(landmarks-1,dimensions);

    int i,j;

    // first create helmert matrix
    helmertize(mhelmert,landmarks+1);

    // now prepare data such that the helmert matrix
    // can multiply the data matrix for this otu
    unitsize(unit, landmarks, dimensions);

    // load newdat
    // subroutine loader shifts data from 1-d vector to 2-d array
    loader(mnewdat, unit, landmarks, dimensions);

    // multiply helmert matrix and newdat
    mproductc = mhelmert * mnewdat;

    // just empty array unit
    for (i=0; i<landmarks*dimensions;i++)
    {
        unit[i] = 0.0;
    }

    int count = 0;

    // load contents of productc into unit , unit (x,y,z,x,y,z...)
    for (j=0; j<landmarks-1;j++)
    {
        for (i=0; i<dimensions;i++)
        {
            unit[count] = mproductc[j][i];
            count++;
        }
    }

    // now rearrange the data such that all x's first, all y's next all z's next
    // ie unit(xxxx,yyyyy,zzzz) this is the vectorized, unit sized, helmertized
    // matrix vecgamma
    rearrange(unit, landmarks - 1, dimensions);

    // this is the vectorised, unit sized, helmertized matrix
    for (i=0; i<landmarks*dimensions;i++)
    {
        vecgamma[i] = unit[i];
    }
}

TPP::TPP(double ** data, int individuals, int landmarks, int dimensions)
{
    m_calculated = false;

    if (data != 0)
    {
        LoadData(data, individuals, landmarks, dimensions);
    }
}

TPP::~TPP()
{
}

// Load landmark data to our TPP module
void
TPP::LoadData(double ** data, int individuals, int landmarks, int dimensions)
{
    m_individuals = individuals;
    m_landmarks = landmarks;
    m_dimensions = dimensions;

    m_mean.ReSize(landmarks,dimensions);
    m_projectedData.ReSize(individuals,landmarks*dimensions);
    m_data.ReSize(individuals,landmarks*dimensions);

    for(int i=0;i<individuals;i++)
    {
        for(int j=0;j<landmarks*dimensions;j++)
        {
            m_data[i][j] = data[i][j];
        }
    }

    m_calculated = false;
}

void
TPP::GetProjectedData(double ** projData)
{
    for (int i=0;i<m_individuals;i++)
    {
        for (int j=0;j<m_landmarks*m_dimensions;j++)
        {
            projData[i][j] = m_projectedData[i][j];
        }
    }
}

void
TPP::PerformTPP()
{
    // do the actual tangent projection

    // temporary variables
    std::vector<double> mean(m_landmarks*m_dimensions);
    std::vector<double> vecgamma(m_landmarks*m_dimensions);
    std::vector<double> unit(m_landmarks*m_dimensions);


    Matrix<double> ident(m_landmarks*m_dimensions,m_landmarks*m_dimensions);
    Matrix<double> meanvm(m_landmarks*m_dimensions,m_landmarks*m_dimensions);

    int i,j;

    // create the identity matrix
    ident.Identity();

    // now calculate overall mean
    overallmean(&mean[0], m_data.GetMatrix(), m_individuals, m_landmarks, m_dimensions);

    // save mean to m_mean
    for (i=0;i<m_landmarks;i++)
    {
        for (j=0; j<m_dimensions;j++)
        {
            m_mean[i][j] = mean[i*m_dimensions+j];
        }
    }

    // now scale the mean to unit size
    // calculate mean vecgamma
    calcvecgamma(&mean[0], &vecgamma[0], m_landmarks, m_dimensions);

    // now calculate product of vecgamma and vecgamma transposed
    for (j=0; j<(m_landmarks-1)*m_dimensions;j++)
    {
        for (i=0; i<(m_landmarks-1)*m_dimensions;i++)
        {
            meanvm[i][j] = vecgamma[i]*vecgamma[j];
        }
    }

    // for each individual
    // do tangent projection
    for (int q=0; q<m_individuals;q++)
    {
        for (j=0; j<m_landmarks*m_dimensions;j++)
        {
            unit[j]=m_data[q][j];
        }

        // get vecgamma for this individual
        calcvecgamma(&unit[0], &vecgamma[0], m_landmarks, m_dimensions);

        // calculate (I-vecgamma*vecgamma')*vecgamma
        // eq. 4.35 Statistical shape analysis (Dryden and Mardia)
        for (i=0; i<(m_landmarks-1)*m_dimensions;i++)
        {
            double sum = 0.0;
            for (j=0; j<(m_landmarks-1)*m_dimensions;j++)
            {
                sum += (ident[i][j] - meanvm[i][j])*vecgamma[j];
            }
            m_projectedData[q][i] = sum;
        }
    }

    m_calculated = true;
}


void
TPP::InverseTPP(double *tanEshape, double *result[])
{
    // eq. 4.36 Statistical shape analysis (Dryden and Mardia)

    double r,s;
    int i,j;

    int dimensions = m_dimensions;
    int landmarks = m_landmarks - 1;

    Matrix<double> mhelmert(landmarks+1,landmarks+1);
    Matrix<double> mnewdat(landmarks+1,dimensions);

    // temporary variables
    std::vector<double> mean(landmarks*dimensions);
    std::vector<double> vecgamma(landmarks*dimensions);
    std::vector<double> unit(landmarks*dimensions);
    std::vector<double> tmp(landmarks*dimensions);


    // create helmert matrix
    helmertize(mhelmert,landmarks+1);

    s = 0.0;
    for (i=0; i<landmarks*dimensions;i++)
    {
        s += tanEshape[i]*tanEshape[i];
    }

    if (s>1.0)
    {
        s = 0.9;
    }
    r = sqrt(1.0-s);

    overallmean(&mean[0], m_data.GetMatrix(), m_individuals, m_landmarks, m_dimensions);

    // now scale the mean to unit size and helmertize
    calcvecgamma(&mean[0], &vecgamma[0], landmarks+1, dimensions);

    // calculate  r*vecgamma ie multiply each element of the mean by r
    // then add each element of the otu tanEshape to vecgamma
    // that is the inverse projection store in unit
    for (i=0; i<landmarks*dimensions;i++)
    {
        unit[i] = tanEshape[i] + vecgamma[i] * r;
    }

    // rearrange values in unit
    int count = 0;
    for (i=0; i<landmarks;i++)
    {
        for (j=0; j<landmarks*dimensions;j+=landmarks)
        {
            tmp[count] = unit[i+j];
            count++;
        }
    }

    for (i=0; i<landmarks*dimensions;i++)
    {
        unit[i] = tmp[i];
    }

    // load newdat
    // subroutine loader shifts data from 1-d vector to 2-d array
    loader(mnewdat, &unit[0], landmarks, dimensions);

    // calculate the inverse tangent projected data
    Matrix<double> mresult = Transpose(mhelmert) * mnewdat;

    for (i=0; i<landmarks+1;i++)
    {
        for (j=0; j<dimensions;j++)
        {
            result[i][j] = mresult[i][j];
        }
    }
}



