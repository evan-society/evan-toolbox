#ifndef TPS_H_
#define TPS_H_

#include <vector>
#include <Matrix.h>

using namespace std;

//! Minimum tolerance for singularity
const double MIN_TOLERANCE = 1e-8;

//! Thin Plate Spline
class TPS
{

public:

    //! Constructor
    //! \param reference the landmark coordinates for the reference
    //! \param target the landmark coordinates for the target
    //! \param landmarks number of landmarks
    //! \param dimensions number of dimensions
    TPS( double **reference = 0,
         double **target = 0,
         int landmarks = 0,
         int dimensions = 0 );

    //! Destructor
    virtual ~TPS();

    //! Load data
    //! \param reference the landmark coordinates for the reference
    //! \param target the landmark coordinates for the target
    //! \param landmarks number of landmarks
    //! \param dimensions number of dimensions
    void LoadData( double **reference,
                   double **target,
                   int landmarks = 0,
                   int dimensions = 0 );
    void LoadData( Matrix<double> *reference,
                   Matrix<double> *target);

    //! Perform the actual Thin Plate Spline
    int PerformTPS();

    //! Get reference
    //! \return the coordinates of the reference form/shape.
    void GetReference(Matrix<double> *ref);

    //! Get target
    //! \return the coordinates of the target form/shape.
    void GetTarget(Matrix<double> *target);

    //! Set reference
    //! \param ref the coordinates of the reference form/shape.
    void SetReference(double **ref);
    void SetReference(Matrix<double> *ref);

    //! Set target
    //! \param target the coordinates of the target form/shape.
    void SetTarget(double **target);
    void SetTarget(Matrix<double> *target);

    //! Warps one point using the previously calculated spline
    //! \param point the point to be warped
    //! \param wpoint the warped point
    void WarpPoint(double *point, std::vector<double> *wpoint);

    //! Warps a list of points using the previously calculated spline
    //! \param points the points to be warped
    //! \param wpoints the warped points
    //! \param n number of points
    void WarpPoints(double **points, Matrix<double> *wpoints, int n);
    void WarpPoints(Matrix<double> *points, Matrix<double> *wpoints);
    void WarpPoints2d(Matrix<double> *points, Matrix<double> *wpoints);
    void WarpPoints3d(Matrix<double> *points, Matrix<double> *wpoints);

    //! Warps a list of points using the previously calculated spline
    //! \param points the points to be warped
    //! \param wpoints the warped points
    //! \param n number of points
    void WarpPoints(double *points, Matrix<double> *wpoints, int n);

    //! Get number of landmarks
    //! \return the number of landmarks
    int GetNumberOfLandmarks();

    //! Get number of dimensions
    //! \return the number of dimensions
    int GetNumberOfDimensions();

    //! Get bending energy
    //! \return the bending energy
    double GetBendingEnergy();

    //! Get bending energy matrix
    //! \return the bending energy matrix
    void GetBendingEnergyMatrix(Matrix<double> *BE);

    //! Has TPS been performed on the loaded object
    //! \return true if a TPS calculation has been performed
    bool IsCalculated();


private:

    bool m_calculated;                  //!< Has TPS been performed?
    int m_landmarks;                    //!< Number of landmarks
    int m_dimensions;                   //!< Number of dimensions

    std::vector<double> m_reference;                //!< Coordinates of reference
    std::vector<double> m_target;                   //!< Coordinates of target
    std::vector<double> m_spline;                   //!< The calculated spline

    double m_bendingEnergy;             //!< The bending energy
};

#endif // TPS_H_

