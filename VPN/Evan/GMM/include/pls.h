#ifndef PLS_H_
#define PLS_H_

#include <Matrix.h>
#include <vector>

using namespace std;

//! Two Block Partial Least Squares
class PLS
{

public:

    static const double EPSILON;// = 1e-12;      //!< A Small floating point number

    //! Constructor
    PLS();

    //! Destructor
    virtual ~PLS();

    //! Load shape with selected landmarks for block 1 and block 2
    //! The data matrix contains shape variables for each specimen as row vectors
    //! (1st specimen): x1 y1 z1 x2 y2 z2 ... xn yn zn
    //! (2nd specimen): x1 y1 z1 x2 y2 z2 ... xn yn zn
    //! etc.
    //! \param data the data matrix
    //! \param landmarks number of landmarks
    //! \param dimensions number of dimensions
    //! \param individuals number of individuals
    //! \param leftBlock the landmarks in the left block
    //! \param rightBlock the landmarks in the right block
    void LoadData(double ** data, int landmarks, int dimensions, int individuals, std::vector<int> leftBlock, std::vector<int> rightBlock);

    //! Load left and right data blocks
    //! The left and right blocks contain shape variables for each specimen as row vectors
    //! (1st specimen): x1 y1 z1 x2 y2 z2 ... xn yn zn
    //! (2nd specimen): x1 y1 z1 x2 y2 z2 ... xn yn zn
    //! etc.
    //! \param left the left block
    //! \param right the right block
    //! \param lcols number of columns in left block
    //! \param rcols number of columns in right block
    //! \param rows number of rows
    void LoadData(double ** left, double ** right, int lcols, int rcols, int rows);

    //! Load shape and variables
    //! The data matrix contains shape variables for each specimen as row vectors
    //! (1st specimen): x1 y1 z1 x2 y2 z2 ... xn yn zn
    //! (2nd specimen): x1 y1 z1 x2 y2 z2 ... xn yn zn
    //! etc.
    //! Variable matrix contains variables as columns
    //! \param data the data matrix
    //! \param landmarks number of landmarks
    //! \param dimensions number of dimensions
    //! \param individuals number of individuals
    //! \param leftBlock the landmarks in the left block
    //! \param rightBlock the landmarks in the right block
    void LoadData(double ** data, double ** variables, int numvar, int landmarks, int dimensions, int individuals);

    //! Perform Partial Least Squares
    void PerformPLS();

    //! Print results to stream
    void PrintResults();

    //! Has PLS been performed
    //! \return true if PLS calculations have been performed on our data
    bool IsCalculated();


private:

    int m_landmarks;                    //!< Number of landmarks
    int m_dimensions;                   //!< Number of dimensions
    int m_individuals;                  //!< Number of specimens

    bool m_calculated;      //!< Has Partial Least Squares Analysis been performed
    int m_rows;                             //!< Number of rows
    Matrix<double> m_data;                  //!< Data matrix
    Matrix<double> m_leftBlock;             //!< Left Block
    Matrix<double> m_rightBlock;            //!< Right Block
    Matrix<double> m_covMatrix;             //!< Covariance Matrix

    Matrix<double> m_singularValues;
    Matrix<double> m_leftSingularWarps;
    Matrix<double> m_rightSingularWarps;
    Matrix<double> m_leftSingularWarpScores;
    Matrix<double> m_rightSingularWarpScores;

};


#endif
