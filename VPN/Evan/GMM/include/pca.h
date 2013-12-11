#ifndef PCA_H_
#define PCA_H_

#include <Matrix.h>
#include <vector>

using namespace std;

//! Principal Component Analysis
class PCA
{

public:

    //! Constructor
    //! \param data the data coordinates
    //! \param rows number of rows
    //! \param cols number of columns
    //! \param datasets number of datasets
    PCA(double ** data=0, int rows=0, int cols=0);

    //! Constructor
    //! \param data the data coordinates
    PCA(Matrix<double> * data);

    //! Destructor
    virtual ~PCA();

    //! LoadData
    //! \param data the data coordinates
    //! \param rows number of rows
    //! \param cols number of columns
    void LoadData(double ** data, int rows, int cols);

    //! LoadData
    //! \param data the data coordinates
    void LoadData(Matrix<double> * data);

    //! Perform Principal Component Analysis on our data
    //! \param center center data, subtract the mean of each column
    //! \param scale scale data, divide with root mean square of each column
    void PerformPCA(bool center = true, bool scale = false);

    //! Get PC scores
    //! \return the PC scores
    void GetPCScores(Matrix<double> *scores);

    //! Get PC loadings
    //! \param scale if true, loadings are scaled so variance equals eigenvalues
    //! \return the PC loadings
    void GetPCLoadings(Matrix<double> *loadings, bool scale = false);

    //! Get the vector with eigenvalues
    //! \return the eigenvalues
    void GetEigenvalues(std::vector<double> *eigenvalues);

    //! Get the vector with mean values
    //! \return the vector of means
    void GetMean(std::vector<double> *mean);

    //! Has PCA been performed
    //! \return true if PCA analysis has been performed on our data
    bool IsCalculated();


private:

    bool m_calculated;      //!< Has Principal Component Analysis been performed
    int m_rows;                             //!< Number of rows
    int m_cols;                             //!< Number of columns
    Matrix<double> m_data;                  //!< Our data matrix
    Matrix<double> m_PCScores;              //!< PC scores
    Matrix<double> m_PCLoadings;            //!< PC Loadings
    std::vector<double> m_eigenvalues;           //!< The eigenvalues (sorted descending)
    std::vector<double> m_mean;                  //!< The mean values
};


#endif
