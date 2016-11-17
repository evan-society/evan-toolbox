#ifndef GPA_H_
#define GPA_H_

#include <vector>
#include <complex>
#include <Matrix.h>


using namespace std;


//! Generalised Procrustes Analysis
class GPA
{

public:

    static const double EPSILON;// = 1e-12;            //!< A small number
    static const double SQRT2;// = 1.414213562373095;  //!< The square root of two

    //! Constructor
    //! \param data the landmark coordinates
    //! \param individuals number of specimens
    //! \param landmarks number of landmarks
    //! \param dimensions number of dimensions (2 or 3)
    GPA(Matrix<double> *data=0, int individuals=0, int landmarks=0, int dimensions=0);

    //! Destructor
    virtual ~GPA();

    //! LoadData
    //! \param data the landmark coordinates
    //! \param individuals number of specimens
    //! \param landmarks number of landmarks
    //! \param dimensions number of dimensions (2 or 3)
    void LoadData(Matrix<double> *data, int individuals, int landmarks, int dimensions);

    //! Perform Generalised Procrustes Analysis
    //! \param alignment (0: use default, 1: align with principal axes, 2: align  with first specimen)
    //! \param scale false if we want to keep scaling
    //! \param rotate false if we want to keep rotation
    //! \param translate false of we want to keep location
    //! \param reflection enable reflections
    //! \param midline landmarks on the midline for alignment with plane of symmetry
    void PerformGPA(int alignment = 0,
                    bool scale = true,
                    bool rotate = true,
                    bool translate = true,
                    bool reflection = true,
                    std::vector<int> midline = std::vector<int>());

    //! Set method for calculating Procrustes distances
    //! \param method method used for calculation (see Dryden and Mardia 1998)
    //!        if method is 1 then full Procrustes distance is used (eq. 4.15)
    //!        if method is 2 then partial Procrustes distance is used (eq. 4.21)
    //!        if method is 3 then Procrustes distance is used (eq. 4.22)
    //!        otherwise euclidean distance is used (sum up the squares of the residuals and take the square root)
    void SetProcrustesDistanceMethod(int method);

    //! Set Alignment of procrustes coordinates
    //! \param alignment
    //!        if alignment is 0 use default (principal axes)
    //!        if alignment is 1 then align with principal axes
    //!        if alignment is 2 then align with first specimen
    void SetAlignment(int alignment);

    //! Get number of specimens
    //! \return the number of specimens
    int GetNumberOfSpecimens();

    //! Get number of landmarks
    //! \return the number of landmarks
    int GetNumberOfLandmarks();

    //! Get number of dimensions
    //! \return the number of dimensions
    int GetNumberOfDimensions();

    //! Get Procrustes Mean
    //! \return mean shape of data
    void GetProcrustesMean(std::vector<std::vector<double> > *m);

    //! Get Procrustes Mean
    //! \return mean shape of data
    void GetProcrustesMean(Matrix<double> *m);

    //! Get Mean in the form of complex numbers
    //! \return mean shape in the form of complex numbers
    void GetMeanAsComplex(std::vector<complex<double> > *m);

    //! Get Procrustes Residuals
    //! \param i specimen
    //! \return the procrustes residuals for specimen i
    void GetProcrustesResiduals(int i, std::vector<std::vector<double> > *r);
    void GetProcrustesResiduals(int i, Matrix<double> *r);

    //! Get procrustes coordinates
    //! \param i specimen
    //! \param scale scale coordinates with cos(rho) to give Kendall´s shape space
    //! \return procrustes coordinates for specimen i
    void GetProcrustesCoordinates(int i, Matrix<double> *proc, bool scale = false);

    //! Get procrustes coordinates for all specimens
    //! \param scale scale coordinates with cos(rho) to give Kendall´s shape space
    //! \return procrustes coordinates for all specimen
    void GetProcrustesCoordinates(Matrix<double> *P, bool scale = false);

    //! Get centroid sizes for all specimens
    //! \return vector of centroid sizes
    void GetCentroidSizes(std::vector<double> *c);

    //! Get similarity matrix for specimen i
    //! \param i specimen
    //! \return similarity matrix for specimen i
    void GetSimilarityMatrix(int i, Matrix<double> *S);

    //! Get similarity matrices for all specimens
    //! \return similarity matrices for all specimens
    void GetSimilarityMatrices(std::vector<Matrix<double> > *S);

    //! Get transformation matrix for specimen i
    //! \param i specimen
    //! \return transformation matrix for specimen i
    void GetTransformationMatrix(int i, Matrix<double> *T);

    //! Get transformation matrices for all specimens
    //! \return transformation matrices for all specimens
    void GetTransformationMatrices(std::vector<Matrix<double> > *T);

    //! Get Procrustes distances between two specimens
    //! \param i first specimen
    //! \param j second specimen
    //! \param method method used for calculation (see Dryden and Mardia 1998)
    //!        if method is 1 then full Procrustes distance is used (eq. 4.15)
    //!        if method is 2 then partial Procrustes distance is used (eq. 4.21)
    //!        if method is 3 then Procrustes distance is used (eq. 4.22)
    //!        otherwise euclidean distance is used (sum up the squares of the residuals and take the square root)
    //! \param formSpace if true then we return formspace distance
    double GetProcrustesDistance(int i, int j, int method = 0, bool formSpace = false);

    //! Get Procrustes distance matrix between all specimens
    //! \param method method used for calculation (see Dryden and Mardia 1998)
    //!        if method is 1 then full Procrustes distance is used (eq. 4.15)
    //!        if method is 2 then partial Procrustes distance is used (eq. 4.21)
    //!        if method is 3 then Procrustes distance is used (eq. 4.22)
    //!        otherwise euclidean distance is used (sum up the squares of the residuals and take the square root)
    //! \param formSpace if true then we return formspace distance
    void GetProcrustesDistances(Matrix<double> *dist, int method = 0, bool formSpace = false);

    //! Get Form Space Coordinates
    //! \return A matrix with the procrustes coordinates and a column with ln of centroid size
    void GetFormSpaceCoordinates(Matrix<double> *F);

    //! Get Uniform Basis
    //! \return The uniform basis of shape variation
    //!        if data is 2D these are 2 vectors of length 2 * number of landmarks
    //!        if data is 3D these are 5 vectors of length 3 * number of landmarks
    void GetUniformBasis(Matrix<double> *uniform);

    //! Has GPA been performed
    //! \return true if GPA analysis has been performed on our data
    bool IsCalculated();

    void SetMaxIterations(int maxIterations)
    {
        m_maxIterations = maxIterations;
    }

    void SetMinTolerance(double minTolerance)
    {
        m_minTolerance = minTolerance;
    }

private:

    bool m_calculated;      //!< Has GPA been performed?
    int m_method;           //!< Method for calculating Procrustes distances
    int m_alignment;        //!< Alignment of coordinates
    int m_landmarks;        //!< Number of landmarks
    int m_dimensions;       //!< Number of dimensions
    int m_individuals;      //!< Number of specimens
    int m_currentSpecimen;  //!< Current specimen
    int m_scale;            //!< Scale coordinates with cos(rho) (for Kendall´s shape space)

    int m_maxIterations;        //!< Maximum number of iterations for the GPA algorithm
    double m_minTolerance;      //!< Minimum tolerance for the GPA algorithm

    Matrix<double> m_procrustesMean;                    //!< Mean shape of data
    std::vector<Matrix<double> > m_originalCoordinates;      //!< Coordinates before GPA
    std::vector<Matrix<double> > m_procrustesCoordinates;    //!< Coordinates after GPA
    std::vector<Matrix<double> > m_procrustesResiduals;      //!< Procrustes residuals
    std::vector<double> m_centroidSizes;                     //!< Centroid sizes
    std::vector<std::vector<double> > m_translations;             //!< Translation vectors
};

#endif // GPA_H_

