#ifndef TPP_H_
#define TPP_H_

#include <iostream>
#include <fstream>
#include <cmath>

#include <Matrix.h>


//! Tangent Plane Projection
class TPP
{

public:

    //! Constructor
    //! \param data the landmark coordinates
    //! \param individuals number of specimens
    //! \param landmarks number of landmarks
    //! \param dimensions number of dimensions (2 or 3)
    TPP(double ** data=0, int individuals=0, int landmarks=0, int dimensions=0);

    //! Destructor
    virtual ~TPP();

    //! Load data
    //! \param data the landmark coordinates
    //! \param individuals number of specimens
    //! \param landmarks number of landmarks
    //! \param dimensions number of dimensions (2 or 3)
    void LoadData(double ** data, int individuals, int landmarks, int dimensions);

    //! Get number of specimens
    //! \return the number of specimens
    int GetNumberOfSpecimens()
    {
        return m_individuals;
    }

    //! Get number of landmarks
    //! \return the number of landmarks
    int GetNumberOfLandmarks()
    {
        return m_landmarks;
    }

    //! Get number of dimensions
    //! \return the number of dimensions
    int GetNumberOfDimensions()
    {
        return m_dimensions;
    }

    //! Get Projected Data
    //! \return Tangent Plane Projected data
    void GetProjectedData(double ** projData);

    //! Perform the actual Tangent Plane Projection
    void PerformTPP();

    //! Do the inverse Tangent Plane Projection
    //! \param tanEshape tangent coordinates for shape
    //! \return procrustes coordinates for shape
    void InverseTPP(double *tanEshape, double *result[]);

    //! Has TPP been performed on the loaded data
    //! \return true if TPP has been performed on our data
    bool IsCalculated()
    {
        return m_calculated;
    }

private:

    Matrix<double> m_mean;            //!< mean value of data
    Matrix<double> m_data;            //!< loaded data
    Matrix<double> m_projectedData;   //!< projected data
    int m_individuals;          //!< number of individuals
    int m_landmarks;            //!< number of landmarks
    int m_dimensions;           //!< number of dimensions
    bool m_calculated;          //!< has TPP been performed?
};

#endif // TPP_H_
