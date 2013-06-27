#ifndef PCO_H_
#define PCO_H_

#include <Matrix.h>
#include <vector>

using namespace std;

//! Principal Coordinate Analysis
class PCO
{

public:

    //! Constructor
    //! \param distances a symmetric matrix of distances
    //! \param size size of distance matrix
    PCO(double ** distances=0, int size=0);

    //! Destructor
    virtual ~PCO();

    //! LoadData
    //! \param distances a symmetric matrix of distances
    //! \param size size of distance matrix
    void LoadData(double ** distances, int size);

    //! Perform Principal Coordinates Analysis
    void PerformPCO();

    //! Get principal coordinates
    //! \return the principal coordinates
    void GetPrincipalCoordinates(Matrix<double> &pcoords);

    //! Get the vector with eigenvalues
    //! \return the eigenvalues
    void GetEigenvalues(std::vector<double> &eigenvalues);

    //! Get the vector of eigenvectors
    //! \return the eigenvectors
    void GetEigenvectors(std::vector<std::vector<double> > &eigenvectors);

    //! Get distance matrix
    //! \return the distance matrix
    void GetDistances(Matrix<double> &dist);

    //! Get size
    //! \return the size of distance matrix
    int GetSize();

    //! Has PCO been performed
    //! \return true if PCO analysis has been performed on our data
    bool IsCalculated();

private:

    bool m_calculated;      //!< Has Principal Coordinates Analysis been performed
    int m_size;                             //!< Size of distance matrix
    Matrix<double> m_distances;             //!< Matrix of distances
    Matrix<double> m_principalCoordinates;  //!< Principal Coordinates
    std::vector<double> m_eigenvalues;           //!< The eigenvalues (sorted descending)
    std::vector<std::vector<double> > m_eigenvectors; //!< The eigenvectors
};


#endif
