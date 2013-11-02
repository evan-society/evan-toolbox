#ifndef MMR_H_
#define MMR_H_

#include <Matrix.h>
#include <vector>
#include <cmath>

using namespace std;

//! Multiple Multivariate Regression
class MMR
{

public:

    //! A small number
    static const double EPSILON;// = 1e-12;

    //! Constructor
    //! \param independent the independent variables
    //! \param dependent the dependent variables
    //! \param numIndependent number of independent variables
    //! \param numDependent number of dependent variables
    //! \param numRows number of rows
    MMR(double ** independent=0, double ** dependent=0, int numIndependent=0, int numDependent=0, int numRows=0);

    //! Destructor
    virtual ~MMR();

    //! LoadData
    //! \param independent the independent variables
    //! \param dependent the dependent variables
    void LoadData(Matrix<double> *independent, Matrix<double> *dependent);

    //! LoadData
    //! \param independent the independent variables
    //! \param dependent the dependent variables
    //! \param numIndependent number of independent variables
    //! \param numDependent number of dependent variables
    //! \param numRows number of rows
    void LoadData(double ** independent, double ** dependent, int numIndependent, int numDependent, int numRows);

    //! Perform Multiple Multivariate Regression on our data
    //! \param constantIsZero if true then force regression line to intercept at zero (b0 = 0)
    void PerformRegression(bool constantIsZero = false);

    //! Get Beta
    //! \param beta the Beta Matrix
    void GetBeta(Matrix<double> *beta);

    //! Get residuals
    //! \param residuals expexted values minus observed values
    void GetResiduals(Matrix<double> *residuals);

    //! Get predictions
    //! \param predictions expected values
    void GetPredictions(Matrix<double> *predictions);

    //! Get R-squared
    //! \param rsqr R-squared
    void GetRSqr(std::vector<double> *rsqr);

    //! Get regression statistics
    //! \param statistics the regression statistics
    void GetRegressionStatistics(string *statistics);

    //! Has MMR been performed
    //! \return true if Multiple Multivariate Regression has been performed on our data
    bool IsCalculated();

    //! Print report to stream
    //! \param constantIsZero slightly differernt report if no constant term
    void PrintReport(bool constantIsZero);


private:

    bool m_calculated;      //!< Has Multiple Multivariate Regression been performed
    int m_numIndependent;                   //!< Number of independent variables
    int m_numDependent;                     //!< Number of dependent variables
    int m_rows;                             //!< Number of rows
    bool m_constantIsZero;                  //!< True if constant is zero
    double m_degreesOfFreedom;              //!< Degrees of Freedom
    Matrix<double> m_independent;           //!< Indndependent variables
    Matrix<double> m_dependent;             //!< Dependent variables
    Matrix<double> m_beta;                  //!< Gradients and intercepts
    Matrix<double> m_rss;                   //!< Residual Sum of Squares
    Matrix<double> m_tss;                   //!< Y Sum of Squares
    Matrix<double> m_ess;                   //!< Error Sum of Squares
    Matrix<double> m_rms;                   //!< Residual Mean Square
    Matrix<double> m_rsqr;                  //!< Pearson큦 coefficient of regression
    Matrix<double> m_adjRsqr;               //!< Adjusted Pearson큦 coefficient
    Matrix<double> m_standardErrorB;        //!< Standard Error for Beta
    Matrix<double> m_standardErrorY;        //!< Standard Error for the y estimate
    Matrix<double> m_tStatB;                //!< t test
    Matrix<double> m_tProbB;                //!< P values for t test
    Matrix<double> m_FStat;                 //!< F test
    Matrix<double> m_FProb;                 //!< P values for F test
    Matrix<double> m_WilksLambdaMatrix;     //!< Wilks Lambda Matrix
    double m_FTestWilks;                    //!< F-test for Wilk's Lambda
    double m_FProbWilks;                    //!< Probability for Wilk's Lambda
    double m_WilksLambda;                   //!< Wilk's Lambda
    double m_PillaisTrace;                  //!< Pillai큦 trace
    double m_HotellingLawleyTrace;          //!< Hotelling Lawley trace
    double m_RoysLargestRoot;               //!< Roy큦 Largest Root
    Matrix<double> m_y;                     //!< y matrix
    Matrix<double> m_x;                     //!< x matrix
    string m_statistics;
};


#endif
