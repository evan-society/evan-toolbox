#include <pls.h>
#include <svd.h>
#include <cerrno>


void GetTime(string &str)
{
//    time_t curtime = time(0);
//    tm now = *localtime(&curtime);

    char dest[255];

    //Format string determines the conversion specification's behaviour
    const char *format = "Date: %B %d %Y\nTime: %X";
    //"%A day of week (Wednesday)
    // %B month (November)
    // %d day (12)
    // %Y year (2008)
    // %X time (10:01:00)

    //strftime - converts date and time to a string
//    if (strftime(dest, sizeof(dest)-1, format, &now)>0)
//    {
//        str = dest;
//    }
//    else
//    {
//        cout<<"Failed to get time and date:\t"<<errno<<endl;
//    }
}

PLS::PLS()
{
}

PLS::~PLS()
{
}

void PLS::LoadData(double ** data, int landmarks, int dimensions, int individuals, std::vector<int> leftBlock, std::vector<int> rightBlock)
{
    m_data.ReSize(individuals,dimensions*landmarks);

    for(int i=0;i<individuals;i++)
	{
	    for(int j=0;j<dimensions*landmarks;j++)
		{
		    m_data[i][j] = data[i][j];
		}
	}

    m_rightBlock.ReSize(individuals,dimensions*rightBlock.size());
    m_leftBlock.ReSize(individuals,dimensions*leftBlock.size());

    for(int i=0;i<individuals;i++)
	{
		for(unsigned int j=0;j<leftBlock.size();j++)
		{
    		for(int k=0;k<dimensions;k++)
    		{
                m_leftBlock[i][j*dimensions+k] = data[i][leftBlock[j]*dimensions+k];
    		}
		}
		for(unsigned int j=0;j<rightBlock.size();j++)
		{
    		for(int k=0;k<dimensions;k++)
    		{
                m_rightBlock[i][j*dimensions+k] = data[i][rightBlock[j]*dimensions+k];
    		}
		}
	}

    m_rows = individuals;
    m_landmarks = landmarks;
    m_dimensions =  dimensions;
    m_individuals = individuals;

    m_calculated = false;
}

void PLS::LoadData(double ** data, double ** variables, int numvar, int landmarks, int dimensions, int individuals)
{
    m_data.ReSize(individuals,dimensions*landmarks);

    for(int i=0;i<individuals;i++)
	{
	    for(int j=0;j<dimensions*landmarks;j++)
		{
		    m_data[i][j] = data[i][j];
		}
	}
    m_leftBlock = m_data;

    m_rightBlock.ReSize(individuals,numvar);
    for(int i=0;i<individuals;i++)
	{
	    for(int j=0;j<numvar;j++)
		{
		    m_rightBlock[i][j] = variables[i][j];
		}
	}

    m_rows = individuals;
    m_landmarks = landmarks;
    m_dimensions =  dimensions;
    m_individuals = individuals;

    m_calculated = false;
}

void PLS::LoadData(double ** left, double ** right, int lcols, int rcols, int rows)
{
    m_rightBlock.ReSize(rows,rcols);
    m_leftBlock.ReSize(rows,lcols);
    for(int i=0;i<rows;i++)
	{
		for(int j=0;j<rcols;j++)
		{
		    m_rightBlock[i][j] = right[i][j];
		}
		for(int j=0;j<lcols;j++)
		{
		    m_leftBlock[i][j] = left[i][j];
		}
	}
    m_rows = rows;

    m_calculated = false;
}

void CalculateCovarianceMatrix(Matrix<double> left, Matrix<double> right, Matrix<double>& cov)
{
    int lcols = left.GetCols();
    int rcols = right.GetCols();
    int rows = left.GetRows();

    Row<double> meanl = Mean(left);
    Row<double> meanr = Mean(right);

    cov.ReSize(lcols,rcols);

	for (int i=0;i<lcols;i++)
	{
		for (int j=0;j<rcols;j++)
		{
            double sum=0.0;
            for (int k=0;k<rows;k++)
            {
                sum+=(left[k][i]-meanl[i])*(right[k][j]-meanr[j]);
            }
            cov[i][j] = sum / (rows-1);
	    }
    }
}

void CalculateCorrelationMatrix(Matrix<double> left, Matrix<double> right, Matrix<double>& corr)
{
    Matrix<double> cov;
    CalculateCovarianceMatrix(left, right, cov);
    corr.ReSize(cov.GetRows(),cov.GetCols());

    for (unsigned int i=0;i<cov.GetRows();i++)
    {
        for (unsigned int j=0;j<cov.GetCols();j++)
        {
            double tmp = sqrt(cov[i][i]) * sqrt(cov[j][j]);
            if (tmp > PLS::EPSILON)
            {
                corr[i][j] = cov[i][j] / tmp;
            }
            else
            {
                corr[i][j] = 0.0;
            }
        }
    }
}

void PLS::PrintResults()
{
    // print results to file
    // this may be changed to console later
    ofstream out("results.txt");

    out<<"EVAN Toolbox"<<endl;
    out<<"Partial Least Squares (PLS)"<<endl;

    string timestr;
    GetTime(timestr);
    out<<timestr<<endl<<endl;

    out<<"Left block:"<<endl;
    out<<m_leftBlock<<endl;

    out<<"Right block:"<<endl;
    out<<m_rightBlock<<endl;

    out<<"Covariance matrix:"<<endl;
    out<<m_covMatrix<<endl;

Matrix<double> corr;
    CalculateCorrelationMatrix(m_leftBlock, m_rightBlock, corr);
    out<<"Corre matrix:"<<endl;
    out<<corr<<endl;

    out<<"Left Singular Values:"<<endl;
    out<<m_singularValues<<endl;

    out<<"Left Singular Warp Scores:"<<endl;
    out<<m_leftSingularWarpScores<<endl;

    out<<"Right Singular Warp Scores:"<<endl;
    out<<m_rightSingularWarpScores<<endl;

    out.close();
}

void PLS::PerformPLS()
{

    Matrix<double> U;
    Matrix<double> S;
    Matrix<double> V;

    CalculateCovarianceMatrix(m_leftBlock,m_rightBlock,m_covMatrix);

    svd(m_covMatrix,&U,&S,&V);

    m_singularValues = S;
    m_leftSingularWarps = U;
    m_rightSingularWarps = V;

    m_leftSingularWarpScores = m_leftBlock * U;
    m_rightSingularWarpScores = m_rightBlock * V;

    m_calculated = true;

    PrintResults();
}

bool PLS::IsCalculated()
{
    return m_calculated;
}

