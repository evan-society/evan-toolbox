#include <tps.h>

#include <ew/Tps2.h>
#include <ew/Tps3.h>


// Constructor
TPS::TPS( double **reference, double **target, int landmarks, int dimensions)
{
    m_calculated = false;
    m_landmarks = landmarks;
    m_dimensions = dimensions;
    m_bendingEnergy = 0.0;

    if (reference != 0 && target != 0)
    {
        LoadData( reference, target, landmarks, dimensions);
    }
}

// Destructor
TPS::~TPS()
{
}

// Load landmark data to our TPS module
void
TPS::LoadData( double **reference, double **target, int landmarks, int dimensions)
{
    m_calculated = false;
    m_landmarks = landmarks;
    m_dimensions = dimensions;
    m_bendingEnergy = 0.0;
    m_reference.resize(landmarks*dimensions);
    m_target.resize(landmarks*dimensions);
    m_spline.resize((landmarks+dimensions+1) * dimensions);

    int k=0;
    for (int i=0;i<landmarks;i++)
    {
        for (int j=0;j<dimensions;j++)
        {
            m_target[k] = target[i][j];
            m_reference[k] = reference[i][j];
            k++;
        }
    }
}

void
TPS::LoadData(Matrix<double> *reference, Matrix<double> *target)
{
    m_calculated = false;
    m_landmarks = reference->GetRows();
    m_dimensions = reference->GetCols();
    m_bendingEnergy = 0.0;
    m_reference.resize(m_landmarks*m_dimensions);
    m_target.resize(m_landmarks*m_dimensions);
    m_spline.resize((m_landmarks+m_dimensions+1) * m_dimensions);

    memcpy(&m_target[0],target->GetSinglePtr(),(m_landmarks*m_dimensions)*sizeof(double));
    memcpy(&m_reference[0],reference->GetSinglePtr(),(m_landmarks*m_dimensions)*sizeof(double));
}

// Perform TPS on our data
int
TPS::PerformTPS()
{
    if ( m_landmarks < 3 )
    {
        // too few landmarks
        return 1;
    }

    int l_size = (m_landmarks+m_dimensions+1)*(m_landmarks+m_dimensions+1);
    std::vector<double> L(l_size);

    int t_size;
    bool is_mixed;
    bool is_reduced;
    double is_nonsingular;

    if (m_dimensions == 2)
    {
        ew::Tps2::interaction(&L[0], &m_reference[0], m_landmarks);
        ew::Tps2::algebraic_size(&t_size, &is_mixed, &is_reduced, 0, m_landmarks);

        // Check t_size satisfies basic constraints.
        if ( t_size < (is_mixed ? ((m_dimensions) * (m_dimensions+1)) : (m_dimensions+1))
            || t_size > (m_landmarks + m_dimensions + 1) * m_dimensions )
        {
            // bad sizes
            return 2;
        }

        std::vector<double> T(t_size * t_size);
        std::vector<int> pivots(t_size);

        ew::Tps2::factorize(&T[0], &pivots[0], &is_nonsingular, &L[0], 0, 0,
                         t_size, is_mixed, is_reduced, m_landmarks);

        if (is_nonsingular < MIN_TOLERANCE)
        {
            // data is singular
            return 3;
        }

        ew::Tps2::solve(&m_spline[0], &m_bendingEnergy, 0, &T[0], &pivots[0], &m_target[0],
                     0,0, &L[0], m_landmarks, t_size, is_mixed, is_reduced);
    }
    else
    {
        ew::Tps3::interaction(&L[0], &m_reference[0], m_landmarks);
        ew::Tps3::algebraic_size(&t_size, &is_mixed, &is_reduced, 0, m_landmarks);

        // Check t_size satisfies basic constraints.
        if ( t_size < (is_mixed ? ((m_dimensions) * (m_dimensions+1)) : (m_dimensions+1))
         || t_size > (m_landmarks + m_dimensions + 1) * m_dimensions )
        {
            // bad sizes
            return 2;
        }

        std::vector<double> T(t_size * t_size);
        std::vector<int> pivots(t_size);

        ew::Tps3::factorize(&T[0], &pivots[0], &is_nonsingular, &L[0], 0, 0,
                         t_size, is_mixed, is_reduced, m_landmarks);

        if (is_nonsingular < MIN_TOLERANCE)
        {
            // data is singular
            return 3;
        }

        ew::Tps3::solve(&m_spline[0], &m_bendingEnergy, 0, &T[0], &pivots[0], &m_target[0],
                     0,0, &L[0], m_landmarks, t_size, is_mixed, is_reduced);
    }
    m_calculated = true;
    return 0;
}

void
TPS::WarpPoint(double *point, vector <double> *wpoint)
{
    if (!m_calculated)
    {
        PerformTPS();
    }

    wpoint->resize(m_dimensions);

    double *wpointSinglePtr = new double[m_dimensions];

    if (m_dimensions == 2)
    {
        ew::Tps2::map(wpointSinglePtr, point, 1, &m_spline[0], &m_reference[0], m_landmarks);
    }
    else
    {
        ew::Tps3::map(wpointSinglePtr, point, 1, &m_spline[0], &m_reference[0], m_landmarks);
    }

    for (int j=0;j<m_dimensions;j++)
    {
        (*wpoint)[j] = wpointSinglePtr[j];
    }

    delete[] wpointSinglePtr;
}

void
TPS::WarpPoints(double *points, Matrix<double> *wpoints, int n)
{
    if (!m_calculated)
    {
        PerformTPS();
    }

    if (m_dimensions == 2)
    {
        ew::Tps2::map(wpoints->GetSinglePtr(), points, n, &m_spline[0], &m_reference[0], m_landmarks);
    }
    else
    {
        ew::Tps3::map(wpoints->GetSinglePtr(), points, n, &m_spline[0], &m_reference[0], m_landmarks);
    }
}

void
TPS::WarpPoints(double **points, Matrix<double> *wpoints, int n)
{
    if (!m_calculated)
    {
        PerformTPS();
    }

    double *pointSinglePtr = points[0];

    if (m_dimensions == 2)
    {
        ew::Tps2::map(wpoints->GetSinglePtr(), pointSinglePtr, n, &m_spline[0], &m_reference[0], m_landmarks);
    }
    else
    {
        ew::Tps3::map(wpoints->GetSinglePtr(), pointSinglePtr, n, &m_spline[0], &m_reference[0], m_landmarks);
    }
}

void
TPS::WarpPoints(Matrix<double> *points, Matrix<double> *wpoints)
{
    if (m_dimensions == 2)
    {
        ew::Tps2::map(wpoints->GetSinglePtr(), points->GetSinglePtr(), points->GetRows(), &m_spline[0], &m_reference[0], m_landmarks);
    }
    else
    {
        ew::Tps3::map(wpoints->GetSinglePtr(), points->GetSinglePtr(), points->GetRows(), &m_spline[0], &m_reference[0], m_landmarks);
    }
}

void
TPS::WarpPoints2d(Matrix<double> *points, Matrix<double> *wpoints)
{
    ew::Tps2::map(wpoints->GetSinglePtr(), points->GetSinglePtr(), points->GetRows(), &m_spline[0], &m_reference[0], m_landmarks);
}

void
TPS::WarpPoints3d(Matrix<double> *points, Matrix<double> *wpoints)
{
    ew::Tps3::map(wpoints->GetSinglePtr(), points->GetSinglePtr(), points->GetRows(), &m_spline[0], &m_reference[0], m_landmarks);
}

void
TPS::SetReference(double **ref)
{
    int k=0;
    for (int i=0;i<m_landmarks;i++)
    {
        for (int j=0;j<m_dimensions;j++)
        {
            m_reference[k++] = ref[i][j];
        }
    }
    m_calculated = false;
}

void
TPS::SetReference(Matrix<double> *reference)
{
    memcpy(&m_reference[0],reference->GetSinglePtr(),(m_landmarks*m_dimensions)*sizeof(double));
    m_calculated = false;
}

void
TPS::SetTarget(double **target)
{
    int k=0;
    for (int i=0;i<m_landmarks;i++)
    {
        for (int j=0;j<m_dimensions;j++)
        {
            m_target[k++] = target[i][j];
        }
    }
    m_calculated = false;
}

void
TPS::SetTarget(Matrix<double> *target)
{
    memcpy(&m_target[0],target->GetSinglePtr(),(m_landmarks*m_dimensions)*sizeof(double));
    m_calculated = false;
}

int
TPS::GetNumberOfLandmarks()
{
    return m_landmarks;
}

int
TPS::GetNumberOfDimensions()
{
    return m_dimensions;
}

double
TPS::GetBendingEnergy()
{
    return m_bendingEnergy;
}

bool
TPS::IsCalculated()
{
    return m_calculated;
}


