#include <mmr.h>
#include <svd.h>

//#include "ibetaf/ibetaf.h" // incomplete beta function from ALGLIB library
#include "eig.h"

//#include <gsl/gsl_sf.h>     // incomplete beta function from GNU Scientific Library


#include <sstream>


MMR::MMR(double ** independent, double ** dependent, int numIndependent, int numDependent, int numRows)
{
    m_calculated = false;
    if (dependent != 0 && independent != 0)
    {
        LoadData(independent, dependent, numIndependent, numDependent, numRows);
    }
}

MMR::~MMR()
{
}

void MMR::LoadData(Matrix<double> *independent, Matrix<double> *dependent)
{
    if (dependent->GetRows() != independent->GetRows())
    {
        cout<<"Error: Number of observations must be the same"<<endl;
        return;
    }

    m_dependent = *dependent;
    m_independent = *independent;
    m_numDependent = dependent->GetCols();
    m_numIndependent = independent->GetCols();
    m_rows = dependent->GetRows();
    m_calculated = false;
}

void MMR::LoadData(double ** independent, double ** dependent, int numIndependent, int numDependent, int numRows)
{
    m_dependent.ReSize(numRows,numDependent);
    m_independent.ReSize(numRows,numIndependent);
    for (int i=0;i<numRows;i++)
    {
        for (int j=0;j<numDependent;j++)
        {
            m_dependent[i][j] = dependent[i][j];
        }
        for (int j=0;j<numIndependent;j++)
        {
            m_independent[i][j] = independent[i][j];
        }
    }
    m_numDependent = numDependent;
    m_numIndependent = numIndependent;
    m_rows = numRows;
    m_calculated = false;
}

void MMR::GetBeta(Matrix<double> *beta)
{
    *beta = m_beta;
}

void MMR::GetResiduals(Matrix<double> *residuals)
{
    if (m_calculated)
    {
        *residuals = m_y - m_x * m_beta;
    }
}

void MMR::GetPredictions(Matrix<double> *predictions)
{
    if (m_calculated)
    {
        *predictions = m_x * m_beta;
    }
}


void MMR::GetRegressionStatistics(string *statistics)
{
    *statistics = m_statistics;
}

void MMR::PerformRegression(bool constantIsZero)
{
    Matrix<double> y = m_dependent;
    Matrix<double> x;

    m_constantIsZero = constantIsZero;

    // Degrees of Freedom
    int df = m_rows - m_numIndependent - 1;

    if (constantIsZero)
    {
        x = m_independent;
        df++;
    }
    else
    {
        x.ReSize(m_rows,m_numIndependent+1);
        for (int i=0;i<m_rows;i++)
        {
            x[i][0] = 1;
            for (int j=0;j<m_numIndependent;j++)
            {
                x[i][j+1] = m_independent[i][j];
            }
        }
    }

    m_x = x;
    m_y = y;

    Matrix<double> xtx = Transpose(x) * x;
    Matrix<double> xty = Transpose(x) * y;
    Matrix<double> yty = Transpose(y) * y;

//    Matrix<double> invxtx = Inverse(xtx);
//    Matrix<double> beta = invxtx * xty;

    // solve XtX * beta = XtY using SVD
    Matrix<double> U,S,V;
    svd(xtx,&U,&S,&V);

    Matrix<double> invS(S.GetRows(),S.GetCols());
    for(unsigned int i=0;i<S.GetCols();i++)
    {
        if (S[i][i] > EPSILON)
        {
            invS[i][i]=1.0/S[i][i];
        }
        else
        {
            invS[i][i] = 0.0;
        }
    }

    Matrix<double> beta = V * invS * Transpose(U) * xty;

    Matrix<double> btxty = Transpose(beta) * Transpose(x) * y;

    Matrix<double> rss = yty-btxty;
    Matrix<double> rms = rss/(double)(m_rows-m_numIndependent);

    Matrix<double> FStat(m_numDependent,m_numDependent);
    Matrix<double> Rsqr(m_numDependent,m_numDependent);
    Matrix<double> AdjRsqr(m_numDependent,m_numDependent);
    Matrix<double> tss(m_numDependent,m_numDependent);
    Matrix<double> ess(m_numDependent,m_numDependent);

    Row<double> meany = Mean(y);

    if (constantIsZero)
    {
        for ( int i = 0; i < m_numDependent; i++)
        {
            //diagonals store relevant values
            for (int j = 0; j < m_numDependent; j++)
            {
                tss[i][j] = yty[i][j];
                ess[i][j] = btxty[i][j]; // explained variance (regression sum of squares)
                if (fabs(tss[i][j]) > EPSILON)
                {
                    Rsqr[i][j] = ess[i][j] / tss[i][j];
                }
                else
                {
                    Rsqr[i][j] = 0.0;
                }
                AdjRsqr[i][j] = 1.0 - (1.0 - Rsqr[i][j]) * ((m_rows) / (double)(m_rows - m_numIndependent));
                FStat[i][j] = (Rsqr[i][j] / (double)m_numIndependent) / ((1.0 - Rsqr[i][j]) / (double)(m_rows - m_numIndependent));
            }
        }
    }
    else
    {
        for ( int i = 0; i < m_numDependent; i++)
        {
            //diagonals store relevant values
            for (int j = 0; j < m_numDependent; j++)
            {
                tss[i][j] = yty[i][j] - (m_rows * (meany[j] * meany[j]));
                ess[i][j] = btxty[i][j] - (m_rows* (meany[j] * meany[j])); // explained variance (regression sum of squares)
                if (fabs(tss[i][j]) > EPSILON)
                {
                    Rsqr[i][j] = ess[i][j] / tss[i][j];
                }
                else
                {
                    Rsqr[i][j] = 0.0;
                }
                AdjRsqr[i][j] = 1.0 - (1.0 - Rsqr[i][j]) * ((m_rows - 1.0) / (m_rows- m_numIndependent - 1.0));
                FStat[i][j] = (Rsqr[i][j] / m_numIndependent) / ((1.0 - Rsqr[i][j]) / (m_rows - m_numIndependent - 1.0));
            }
        }
    }
/*
    // Compute Standard Error for the betas - values held in diagonal
    int nr = invS.GetRows();//invxtx.GetRows();
    int nc = rms.GetCols();
    Matrix<double> seB(nr,nc);
    Matrix<double> tStatB(nr,nc);

    for ( int i = 0; i < nr; i++)
    {
        for ( int j = 0; j < nc; j++)
        {
//            if (fabs(rms[j][j]*invxtx[i][i]) > EPSILON)
            if (fabs(rms[j][j]*invS[i][i]) > EPSILON)
            {
                if (constantIsZero)
                {
//                    seB[i][j] = sqrt(rms[j][j]*invxtx[i][i]);
                    seB[i][j] = sqrt(rms[j][j]*invS[i][i]);
                }
                else
                {
//                    seB[i][j] = sqrt(rms[j][j]*invxtx[i][i]*(m_rows-m_numIndependent)/(m_rows-m_numIndependent-1));
                    seB[i][j] = sqrt(rms[j][j]*invS[i][i]*(m_rows-m_numIndependent)/(m_rows-m_numIndependent-1));
                }
                tStatB[i][j] = beta[i][j] / seB[i][j]; //element-wise division
            }
            else
            {
                seB[i][j] = 0.0;
                tStatB[i][j] = 0.0;
            }
        }
    }

    // Compute Standard Error for the y estimate

    Row<double> seY(m_numDependent);

    Matrix<double> xb = x*beta;
    Row<double> meanxb = Mean(xb);

    if (constantIsZero)
    {
        for (int j = 0; j < m_numDependent; j++)
        {
            double sumxx = 0.0;
            double sumyy = 0.0;
            double sumxy = 0.0;
            for (int i = 0; i < m_rows; i++)
            {
                sumxx += (xb[i][j]-meanxb[j]) * (xb[i][j]-meanxb[j]);
                sumyy += y[i][j] * y[i][j];
                sumxy += y[i][j] * (xb[i][j]-meanxb[j]);
            }
            seY[j] = sqrt( ( sumyy - (sumxy*sumxy/sumxx) ) /(m_rows - m_numIndependent));
        }
    }
    else
    {
        for (int j = 0; j < m_numDependent; j++)
        {
            double sumxx = 0.0;
            double sumyy = 0.0;
            double sumxy = 0.0;
            for (int i = 0; i < m_rows; i++)
            {
                sumxx += (xb[i][j]-meanxb[j]) * (xb[i][j]-meanxb[j]);
                sumyy += (y[i][j]-meany[j]) * (y[i][j]-meany[j]);
                sumxy += (y[i][j]-meany[j]) * (xb[i][j]-meanxb[j]);
            }
            seY[j] = sqrt( ( sumyy - (sumxy*sumxy/sumxx) ) /(m_rows - m_numIndependent - 1));
        }
    }

    Matrix<double> tProbB(nr,nc);

    for (int i = 0; i < nr; i++)
    {
        for (int j = 0; j < nc; j++)
        {
            double xval = df / (df + tStatB[i][j] * tStatB[i][j]);
            double a = df / 2.0;
            double b = 0.5;
            // incomplete beta: calc integrals in gaussian curves
            // check out: http://mathworld.wolfram.com/BetaFunction.html
//            tProbB[i][j] =  incompletebeta(a, b, xval);
tProbB[i][j] = gsl_sf_beta_inc (a,b,xval);

        }
    }

    // F-test probs
    Matrix<double> FProb(m_numDependent,m_numDependent);

    for (int i = 0; i < m_numDependent; i++)
    {
        double a = df / 2.0;
        double b = m_numIndependent / 2.0;
        double xval = df / (df + m_numIndependent * FStat[i][i]);

//        FProb[i][i] = incompletebeta(a, b, xval);
FProb[i][i] = gsl_sf_beta_inc (a,b,xval);

    }

    Matrix<double> WilksLambda = ess*Inverse(rss);

    Row<double> D(m_numDependent);

    Matrix<double> Hi(m_numDependent+1,m_numDependent+1);
    for (int i = 0; i < m_numDependent; i++)
    {
        for (int j = 0; j < m_numDependent; j++)
        {
            Hi[i+1][j+1] = WilksLambda[i][j];
        }
    }

    double Er[m_numDependent+1];
    double Ei[m_numDependent+1];

    // balance matrix, convert to Hessenburg form then use QR decomposition

    balanc(Hi.GetMatrix(), m_numDependent);
    elmhes(Hi.GetMatrix(), m_numDependent);
    hqr(Hi.GetMatrix(), m_numDependent, Er, Ei);

    for (int i = 0; i < m_numDependent; i++)
    {
        D[i] = Er[i+1];
    }

    double test1 = 0.0;
    double test2 = 0.0;
    double test3 = 0.0;
    double test4 = 0.0;
    double tmp1 = 0.0;
    double tmp2 = 0.0;

    //calculate wilks lambda and pillai trace etc
    for (unsigned int i = 0; i < D.GetCols(); i++)
    {
        tmp1 = 1.0 / (1.0 + D[i]);
        tmp2 = D[i] / (1.0 + D[i]);
        if (fabs(test1) < EPSILON)
        {
            test1 = tmp1;
        }
        else
        {
            test1 *= tmp1;
        }

        test2 += tmp2;      //Pillai's trace
        test3 += D[i];      //Hotelling-Lawley trace
        if (D[i] > test4)
        {
            test4 = D[i];   //Roy's largest root
        }
    }

    // Now for the F-stat of Wilks Lambda
    tmp1 = 0.0;
    tmp2 = 0.0;
    double FTestWilks = 0.0;
    double s = 0.0;

*/
    /*
    Rao's F-test for Wilk's

    Ra = [ (1 -  L^1/s) /  L^1/s ] x [ (1 + t s - p q / 2) / p q ]

    where	t=tmp1 = (n-1) - (p + q + 1) / 2,
            s = 1 if p^2 + q^2 <= 5; otherwise s = [(p^2 q^2 -4)/(p^2 + q^2 - 5)]^0.5 ,
            n = the number of observations,
            p = the number of X variables (i.e., the rank of X), and
            q = the number of Y variables (i.e., the rank of Y).

        Ra has an approximate F-distribution with p q degrees of freedom in the numerator and 1 + t s - p q / 2 degrees of freedom in the denominator.  When p = 1 or 2 or when q = 1 or 2, then Ra has an exact F-distribution.
    */
/*
    if ((m_numIndependent * m_numIndependent) + (m_numDependent * m_numDependent) > 5 )
    {
        s = sqrt(((m_numIndependent * m_numIndependent)*(m_numDependent * m_numDependent) - 4)/((m_numIndependent * m_numIndependent) + (m_numDependent * m_numDependent) - 5));
    }
    else
    {
        s = 1.0;
    }

    tmp2 = ((m_rows - 2) -(m_numDependent - m_numIndependent + 2) / 2)*s - (m_numIndependent*m_numDependent - 2) / 2;
    tmp1 = m_numDependent * m_numIndependent;
    FTestWilks = ((1.0 - pow(test1, 1.0/s)) / pow(test1, 1.0/s))*(tmp2/tmp1);

//    double df1 = 1.0;
//    double df2 = m_rows - 2.0;
    double df1 = m_numIndependent;
    double df2 = m_rows - m_numIndependent - 1.0;

    double a = df1 / 2.0;
    double b = df2 / 2.0;
    double xval = df2 / (df2 + df1 * FTestWilks);
//    double FProbWilks = incompletebeta(a, b, xval);
double FProbWilks = gsl_sf_beta_inc (a,b,xval);
*/
    // Save statistics
    m_beta = beta;
    m_rsqr = Rsqr;
/*
    m_degreesOfFreedom = df;
    m_rss = rss;
    m_tss = tss;
    m_ess = ess;
    m_rms = rms;
    m_rsqr = Rsqr;
    m_adjRsqr = AdjRsqr;
    m_standardErrorB = seB;
    m_standardErrorY = seY;
    m_FStat = FStat;
    m_FProb = FProb;
    m_tStatB = tStatB;
    m_tProbB = tProbB;
    m_WilksLambdaMatrix = WilksLambda;
    m_FProbWilks = FProbWilks;
    m_FTestWilks = FTestWilks;

    m_WilksLambda = test1;
    m_PillaisTrace = test2;
    m_HotellingLawleyTrace = test3;
    m_RoysLargestRoot = test4;
*/
    // print report
//    PrintReport(constantIsZero);

    m_calculated = true;
}

void MMR::PrintReport(bool constantIsZero)
{
    stringstream out;

    int numIndependent = m_numIndependent;

    if (!m_constantIsZero)
    {
        numIndependent++;
    }

    for (int i=0;i<m_numDependent;i++)
    {

        out<<"Dependent Variable "<< i+1 <<endl;

        out<<"Coefficients"<<endl;

        for (int j =0; j<numIndependent;j++)
        {
            out<<"\tBeta["<<j<<"]:\t"<< m_beta[j][i]<<endl;
            out<<"\t\tStandard error:\t"<< m_standardErrorB[j][i] <<endl;
            out<<"\t\tt-stat:\t"<< m_tStatB[j][i] <<endl;
            out<<"\t\tp-value:\t"<< m_tProbB[j][i] <<endl;
        }

        out<<"R-Square:\t"<<m_rsqr[i][i] <<endl;
        out<<"Adjusted R-Square:\t"<<m_adjRsqr[i][i] <<endl;
        out<<"F-test:\t"<<m_FStat[i][i] <<endl;
        out<<"p-value for F-test:\t"<<m_FProb[i][i] <<endl;
        out<<"Residual Sum of Squares:\t"<<m_rss[i][i] <<endl;
        out<<"Error Sum of Squares:\t"<<m_ess[i][i] <<endl;
        out<<"Residual Mean Squares:\t"<<m_rms[i][i] <<endl;
        out<<endl;
    }

    for (int j =0; j<m_numIndependent;j++)
    {
//        out<<"Standard error for Y["<<j<<"]:\t"<< m_standardErrorY[j][j]<<endl;
    }
    out<<endl;

    out<<"Multivariate Statistics:"<<endl;

    out<< "Wilk's Lambda:\t" << m_WilksLambda << "\t" <<endl;
    out<< "\tF-test\t" << m_FTestWilks <<endl;
    out<< "\tProbability\t" << m_FProbWilks <<endl;
/*
    double s = 0.0;
    if ((m_numIndependent * m_numIndependent) + (m_numDependent * m_numDependent) > 5 )
    {
        s = sqrt(((m_numIndependent * m_numIndependent)*(m_numDependent * m_numDependent) - 4)/((m_numIndependent * m_numIndependent) + (m_numDependent * m_numDependent) - 5));
    }
    else
    {
        s = 1.0;
    }
*/
//    int df1 = m_numDependent * m_numIndependent;
//    int df2 = (int)(((m_rows - 2) -(m_numDependent - m_numIndependent + 2) / 2)*s - (m_numIndependent*m_numDependent - 2) / 2);

    double df1 = 1.0;
    double df2 = m_rows - 2.0;


    out<< "\tDF1:\t" << df1 <<endl;
    out<< "\tDF2:\t" << df2 <<endl;

    out<< "Pillai's Trace:\t" << m_PillaisTrace <<endl;
    out<< "Hotelling Lawley's Trace:\t" << m_HotellingLawleyTrace <<endl;
    out<< "Roy's Largest Root:\t" << m_RoysLargestRoot <<endl;

//    out<< "Total sample variance explained by this regression:\t" << blabla <<endl;
//    out<< "Variance accounted for by the selected PCs is explained by this regression:\t" << blabla <<endl;

    m_statistics = out.str();
}

void MMR::GetRSqr(std::vector<double> *rsqr)
{
    rsqr->clear();
    for (int i=0;i<m_numDependent;i++)
    {
        rsqr->push_back(m_rsqr[i][i]);
    }
}

bool MMR::IsCalculated()
{
    return m_calculated;
}
