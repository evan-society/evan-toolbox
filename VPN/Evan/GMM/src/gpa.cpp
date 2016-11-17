#include <gpa.h>
#include <svd.h>

#include <tps.h>
#include <ew/Tps2.h>
#include <ew/Tps3.h>

static double valueEPSILON() { return 1e-12; }
static double valueSQRT2() { return 1.414213562373095; }

const double GPA::EPSILON = valueEPSILON();// = 1e-12;            //!< A small number
const double GPA::SQRT2 = valueSQRT2();// = 1.414213562373095;  //!< The square root of two

void
GPA::GetProcrustesDistances(Matrix<double> *dist, int method, bool formSpace)
{
    dist->ReSize(m_individuals,m_individuals);
    for (int i=0;i<m_individuals;i++)
    {
        for (int j=0;j<m_individuals;j++)
        {
            (*dist)[i][j] = GetProcrustesDistance(i,j,method,formSpace);
        }
    }
}

double
GPA::GetProcrustesDistance(int i, int j, int method, bool formSpace)
{
    Matrix<double> X1TX2;
    Row<double> lambda;
    double diff = 0.0;
    double sum = 0.0;
    double dist = 0.0;

    if (i == j)
    {
        return 0.0;
    }

    if (method == 0)
    {
        // use default method if not specified
        method = m_method;
    }

    // use different methods to calculate procrustes distances
    switch (method)
    {
    case 3:
        // Procrustes distance (rho)
        // eq. 4.22 Dryden and Mardia 1998
        X1TX2 = Transpose(m_procrustesCoordinates[i]) * (m_procrustesCoordinates[j]);

        // Do Singular Value Decomposition on the X1TX2 matrix
        lambda = svd(X1TX2);

        for (int k=0;k<m_dimensions;k++)
        {
            sum += lambda[k];
        }
        dist = acos(sum);
        break;
    case 2:
        // Partial Procrustes distance (Dp)
        // eq. 4.21 Dryden and Mardia 1998
        X1TX2 = Transpose(m_procrustesCoordinates[i]) * (m_procrustesCoordinates[j]);

        // Do Singular Value Decomposition on the X1TX2 matrix
        lambda = svd(X1TX2);

        for (int k=0;k<m_dimensions;k++)
        {
            sum += lambda[k];
        }
        dist = SQRT2 * sqrt(1.0 - sum);
        break;
    case 1:
        // Full Procrustes distance (Df)
        // eq. 4.15 Dryden and Mardia 1998
        X1TX2 = Transpose(m_procrustesCoordinates[i]) * (m_procrustesCoordinates[j]);

        // Do Singular Value Decomposition on the X1TX2 matrix
        lambda = svd(X1TX2);

        for (int k=0;k<m_dimensions;k++)
        {
            sum += lambda[k];
        }
        dist = sqrt(1.0 - sum * sum);
        break;
    default:
        // use euclidean distance
        // sum up the squares of the differences of coordinates
        // and take the square root
        for (int k=0;k<m_landmarks;k++)
        {
            for (int m=0;m<m_dimensions;m++)
            {
                diff = m_procrustesCoordinates[i][k][m] - m_procrustesCoordinates[j][k][m];
                sum += diff * diff;
            }
        }
        dist = sqrt(sum);
        break;
    }

    if (formSpace)
    {
        // include log of centroid sizes
        double logcs = log(m_centroidSizes[i]/m_centroidSizes[j]);
        dist = sqrt(logcs * logcs + dist * dist);
    }

    return dist;
}

// move the data to its centroid
void
centre(Matrix<double> *data, int n, int m, std::vector<double> &translation)
{
    int i, j;
    std::vector<double> alpha(m);

    double ** mat = data->GetMatrix();

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            alpha[j] += mat[i][j] / n;
        }
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            mat[i][j] -= alpha[j];
        }
    }

    translation = alpha;
}

// calculate centroid size
void
calcsize(Matrix<double> *data, int k, int m, double *rsize)
{
    int i,j;
    Row<double> centr(m);
    double ** mat = data->GetMatrix();

    for (j = 0; j < m; j++)
    {
        for (i = 0; i < k; i++)
        {
            centr[j] += mat[i][j] / k;
        }
        if (centr[j] < 1e-17)
        {
            centr[j] = 0.0;
        }
    }

    double temp = 0.0;
    for (i = 0; i < k; i++)
    {
        for (j = 0; j < m; j++)
        {
            temp += (mat[i][j] - centr[j]) * (mat[i][j] - centr[j]);
        }
    }
    *rsize = sqrt(temp);
}

// scale data to unit size
void
unitsize(Matrix<double> *data, int k, int m, double *rsize)
{
    int i,j;
    double ** mat = data->GetMatrix();

    // calculate centroid size
    calcsize(data, k,  m, rsize);

    // scale to unit sixe
    for (i = 0; i < k; i++)
    {
        for (j = 0; j < m; j++)
        {
            mat[i][j] /= *rsize;
        }
    }
}


// perform procrustes fit, store output in result
void
procrustes(Matrix<double>* x2, Matrix<double>& x1, int k, int m, bool reflection)
{
    Matrix<double> X1TX2(m, m);

    Matrix<double> U(m, m);
    Matrix<double> S(m, m);
    Matrix<double> V(m, m);

    X1TX2 = Transpose(x1) * (*x2);

    // Do Singular Value Decomposition on the X1TX2 matrix

    svd(X1TX2, &U, &S, &V);

    // now fix reflections
    if (!reflection)
    {
        //! \note determinant is (-1) times the determinant from morphologika
        //!       because of different SVD function

        // determine which column to flip
        double small = S[0][0];
        int col = 0;
        for (int i = 1;i<m;i++)
        {
            if (S[i][i] < small)
            {
                col = i;
            }
        }
        double detU = Det(U);
        double detV = Det(V);

        if (detU > 0)
        {
            for (int i=0;i<m;i++)
            {
                U[i][col] = -U[i][col];
            }
        }
        if (detV > 0)
        {
            for (int i=0;i<m;i++)
            {
                V[i][col] = -V[i][col];
            }
        }
    }

    //  now calculate X2 * V * UT to get new coords, output in result
    *x2 = *x2 * V * Transpose(U);
}

void evalf(Matrix<double> x, double theta, double *f, double *df)
{
    Matrix<double> rot(2,2);
    Matrix<double> drot(2,2);
    rot[0][0] = cos(theta);
    rot[0][1] = sin(theta);
    rot[1][0] = -sin(theta);
    rot[1][1] = cos(theta);
    drot[0][0] = -sin(theta);
    drot[0][1] = cos(theta);
    drot[1][0] = -cos(theta);
    drot[1][1] = -sin(theta);

    Matrix<double> y = x * rot;
    Matrix<double> dy = x * drot;

    *f = 0.0;
    *df = 0.0;

    for (unsigned int i=0;i<y.GetRows();i++)
    {
        *f += y[i][0]*y[i][1];
        *df += dy[i][0]*y[i][1] + y[i][0]*dy[i][1];
    }
}

void evalf3(Matrix<double> x, std::vector<double> theta, std::vector<double>& f, Matrix<double>& J)
{
    Matrix<double> rotx(3,3);
    Matrix<double> roty(3,3);
    Matrix<double> rotz(3,3);
    Matrix<double> drotx(3,3);
    Matrix<double> droty(3,3);
    Matrix<double> drotz(3,3);

    rotx[0][0] = 1.0;
    rotx[0][1] = 0.0;
    rotx[0][2] = 0.0;
    rotx[1][0] = 0.0;
    rotx[1][1] = cos(theta[0]);
    rotx[1][2] = -sin(theta[0]);
    rotx[2][0] = 0.0;
    rotx[2][1] = sin(theta[0]);
    rotx[2][2] = cos(theta[0]);

    roty[0][0] = cos(theta[1]);
    roty[0][1] = 0.0;
    roty[0][2] = sin(theta[1]);
    roty[1][0] = 0.0;
    roty[1][1] = 1.0;
    roty[1][2] = 0.0;
    roty[2][0] = -sin(theta[1]);
    roty[2][1] = 0.0;
    roty[2][2] = cos(theta[1]);

    rotz[0][0] = cos(theta[2]);
    rotz[0][1] = -sin(theta[2]);
    rotz[0][2] = 0.0;
    rotz[1][0] = sin(theta[2]);
    rotz[1][1] = cos(theta[2]);
    rotz[1][2] = 0.0;
    rotz[2][0] = 0.0;
    rotz[2][1] = 0.0;
    rotz[2][2] = 1.0;

    drotx[0][0] = 0.0;
    drotx[0][1] = 0.0;
    drotx[0][2] = 0.0;
    drotx[1][0] = 0.0;
    drotx[1][1] = -sin(theta[0]);
    drotx[1][2] = -cos(theta[0]);
    drotx[2][0] = 0.0;
    drotx[2][1] = cos(theta[0]);
    drotx[2][2] = -sin(theta[0]);

    droty[0][0] = -sin(theta[1]);
    droty[0][1] = 0.0;
    droty[0][2] = cos(theta[1]);
    droty[1][0] = 0.0;
    droty[1][1] = 0.0;
    droty[1][2] = 0.0;
    droty[2][0] = -cos(theta[1]);
    droty[2][1] = 0.0;
    droty[2][2] = -sin(theta[1]);

    drotz[0][0] = -sin(theta[2]);
    drotz[0][1] = -cos(theta[2]);
    drotz[0][2] = 0.0;
    drotz[1][0] = cos(theta[2]);
    drotz[1][1] = -sin(theta[2]);
    drotz[1][2] = 0.0;
    drotz[2][0] = 0.0;
    drotz[2][1] = 0.0;
    drotz[2][2] = 0.0;

    Matrix<double> rot = rotx*roty*rotz;
    Matrix<double> y = x*rot;
    Matrix<double> dfdx = x*drotx*roty*rotz;
    Matrix<double> dfdy = x*rotx*droty*rotz;
    Matrix<double> dfdz = x*rotx*roty*drotz;

    J.ReSize(3,3);
    J.Zeros();
    f.resize(3);
    f[0] = 0.0;
    f[1] = 0.0;
    f[2] = 0.0;
    for (unsigned int i=0;i<y.GetRows();i++)
    {
        f[0] += y[i][0]*y[i][1];
        f[1] += y[i][1]*y[i][2];
        f[2] += y[i][0]*y[i][2];

        J[0][0] += y[i][0]*dfdx[i][1]+dfdx[i][0]*y[i][1];
        J[1][0] += y[i][1]*dfdx[i][2]+dfdx[i][1]*y[i][2];
        J[2][0] += y[i][0]*dfdx[i][2]+dfdx[i][0]*y[i][2];

        J[0][1] += y[i][0]*dfdy[i][1]+dfdy[i][0]*y[i][1];
        J[1][1] += y[i][1]*dfdy[i][2]+dfdy[i][1]*y[i][2];
        J[2][1] += y[i][0]*dfdy[i][2]+dfdy[i][0]*y[i][2];

        J[0][2] += y[i][0]*dfdz[i][1]+dfdz[i][0]*y[i][1];
        J[1][2] += y[i][1]*dfdz[i][2]+dfdz[i][1]*y[i][2];
        J[2][2] += y[i][0]*dfdz[i][2]+dfdz[i][0]*y[i][2];
    }
}

// align coordinates with principal axes using Newton´s method
void
pcalign(Matrix<double>& x, Matrix<double>& rot)
{
    int m = x.GetCols();

    if (m==2)
    {
        double th0 = 0.0;
        double th1 = 0.0;
        double f = 0.0;
        double df = 0.0;

        for (int k=0;k<100;k++)
        {
			double err = 0.0;

            evalf(x,th0,&f,&df);
            if (df>1.0e-12)
            {
                th1=th0-f/df;
            }
            else
            {
                break;
            }
            err=abs(th1-th0);
            th0=th1;
            if (err<1.0e-12)
            {
                break;
            }
        }

        rot.ReSize(2,2);
        rot[0][0] = cos(th0);
        rot[0][1] = sin(th0);
        rot[1][0] = -sin(th0);
        rot[1][1] = cos(th0);

        x = x * rot;
    }

    if (m==3)
    {
        std::vector<double> th0(3);
        std::vector<double> th1(3);

        std::vector<double> f(3);
        Matrix<double> J(3,3);

        for (int k=0;k<100;k++)
        {
			double err = 0.0;

            // find function values and jacobian matrix
            evalf3(x,th0,f,J);

            Matrix<double> invJ = Inverse(J);
            for (int i=0;i<3;i++)
            {
                th1[i]=th0[i]-(invJ[i][0]*f[0]+invJ[i][1]*f[1]+invJ[i][2]*f[2]);
            }

            // estimate error
            err = 0.0;
            for (int i=0;i<3;i++)
            {
                err += (th1[i]-th0[i])*(th1[i]-th0[i]);
            }
            err = sqrt(err);

            th0 = th1;
            if (err<1.0e-12)
            {
                break;
            }
        }

        Matrix<double> rotx(3,3);
        Matrix<double> roty(3,3);
        Matrix<double> rotz(3,3);

        rotx[0][0] = 1.0;
        rotx[0][1] = 0.0;
        rotx[0][2] = 0.0;
        rotx[1][0] = 0.0;
        rotx[1][1] = cos(th0[0]);
        rotx[1][2] = -sin(th0[0]);
        rotx[2][0] = 0.0;
        rotx[2][1] = sin(th0[0]);
        rotx[2][2] = cos(th0[0]);

        roty[0][0] = cos(th0[1]);
        roty[0][1] = 0.0;
        roty[0][2] = sin(th0[1]);
        roty[1][0] = 0.0;
        roty[1][1] = 1.0;
        roty[1][2] = 0.0;
        roty[2][0] = -sin(th0[1]);
        roty[2][1] = 0.0;
        roty[2][2] = cos(th0[1]);

        rotz[0][0] = cos(th0[2]);
        rotz[0][1] = -sin(th0[2]);
        rotz[0][2] = 0.0;
        rotz[1][0] = sin(th0[2]);
        rotz[1][1] = cos(th0[2]);
        rotz[1][2] = 0.0;
        rotz[2][0] = 0.0;
        rotz[2][1] = 0.0;
        rotz[2][2] = 1.0;

        rot.ReSize(3,3);
        rot = rotx*roty*rotz;

        x = x * rot;
    }
}

// align coordinates with principal axes
void
pcalign(Matrix<double>& x)
{
    // n is number of landmarks
    // m is number of dimensions

    int n = (int)x.GetRows();
    int m = (int)x.GetCols();

    Matrix<double> distances(n,n);

    // get the euclidan distances between landmarks
    double diff = 0.0;
    double sum = 0.0;
    for (int i=0;i<n;i++)
    {
        for (int j=0;j<i;j++)
        {
            sum = 0.0;
            for (int k=0;k<m;k++)
            {
                diff = x[i][k] - x[j][k];
                sum += diff * diff;
            }
            distances[i][j] = sqrt(sum);
            distances[j][i] = distances[i][j];
        }
    }

    double A = 0.0;
    for (int i=0;i<n;i++)
    {
        for (int j=0;j<n;j++)
        {
            A -= 0.5 * (distances[i][j] * distances[i][j]);
        }
    }
    A = A / (double)(n*n);

    double Arow[n];
    for (int i=0;i<n;i++)
    {
        Arow[i] = 0.0;
    }
    for (int i=0;i<n;i++)
    {
        for (int j=0;j<n;j++)
        {
            Arow[i] -= 0.5 * (distances[i][j] * distances[i][j]);
        }
        Arow[i] /= (double)(n);
    }

    Matrix<double> Bm(n,n);

    for (int i=0;i<n;i++)
    {
        for (int j=0;j<n;j++)
        {
            Bm[i][j] = -0.5 * (distances[i][j] * distances[i][j]) - Arow[i] - Arow[j] + A;
        }
    }

    Matrix<double> U(n,n);
    Matrix<double> S(n,n);
    Matrix<double> V(n,n);

    svd(Bm,&U,&S,&V);
    Matrix<double> D(n,3);

    double sumEV = 0.0;
    for (int i=0;i<n;i++)
    {
        sumEV += S[i][i];
    }

    D[0][0] = S[0][0];
    D[0][1] = D[0][0] / sumEV;
    D[0][2] = D[0][1];

    for (int i=1;i<n;i++)
    {
        D[i][0] = S[i][i];
        D[i][1] = D[i][0] / sumEV;
        D[i][2] = D[i][1]+D[i-1][2];
    }

    int dim = n;
    for (int i=0;i<n;i++)
    {
        if ( 1.0 - D[i][2] < 1.0e-6)
        {
            dim = i+1;
            break;
        }
    }

    if (dim != m)
    {
        cout << "Warning: Dimensions of aligned coordinates don't match." << endl;
    }

    for (int i=0;i<m;i++)
    {
        double temp = sqrt(D[i][0]);
        for (int j=0;j<n;j++)
        {
            V[j][i] *= temp;
        }
    }

    // store results
    for (int i=0;i<n;i++)
    {
        for (int j=0;j<m;j++)
        {
            x[i][j] = V[i][j];
        }
    }
}

// Constructor
GPA::GPA(Matrix<double> *data, int individuals, int landmarks, int dimensions)
{
    m_calculated = false;
    m_method = 0;
    m_alignment = 1;
    m_maxIterations = 100;
    m_minTolerance = 1.0e-12;
    if (data != 0)
    {
        LoadData(data, individuals, landmarks, dimensions);
    }
}

// Destructor
GPA::~GPA()
{}

// Load landmark data to our GPA module
void
GPA::LoadData(Matrix<double> *data, int individuals, int landmarks, int dimensions)
{
    m_individuals = individuals;
    m_landmarks = landmarks;
    m_dimensions = dimensions;

    m_originalCoordinates.resize(individuals);

    for (int i=0;i<individuals;i++)     // RP 1/1/09 Save original coordinates
    {
        Matrix<double> lm(landmarks,dimensions);
        for (int j=0;j<landmarks;j++)
        {
            for (int k=0;k<dimensions;k++)
            {
                lm[j][k] = (*data)[i*landmarks+j][k];
            }
        }
        m_originalCoordinates[i] = lm;
    }

    m_calculated = false;
}

// Get similarity matrix
void GPA::GetSimilarityMatrix(int i, Matrix<double> *T)
{
    if (i < 0 || i >= m_individuals)
    {
        cout << "GetSimilarityMatrix: index " << i <<" out if range." << endl;
        return;
    }

    Matrix<double> X1TX2(m_dimensions, m_dimensions);

    Matrix<double> U(m_dimensions, m_dimensions);
    Matrix<double> S(m_dimensions, m_dimensions);
    Matrix<double> V(m_dimensions, m_dimensions);

    X1TX2 = Transpose(m_procrustesCoordinates[i]) * (m_originalCoordinates[i]);

    // Do Singular Value Decomposition on the X1TX2 matrix
    svd(X1TX2, &U, &S, &V);

    Matrix<double> sim(m_dimensions, m_dimensions+1);
    Matrix<double> rot = V * Transpose(U);

    for (int j=0;j<m_dimensions;j++)
    {
        for (int k=0;k<m_dimensions;k++)
        {
            sim[j][k] = rot[j][k];
        }
    }

//rp 31/12/08 code below is probably wrong - no scale of translation or rotation
    for (int j=0;j<m_dimensions;j++)
    {
        sim[j][m_dimensions] = m_translations[i][j]; //rp 31/12/08 This may need negating
    }

    (*T) = sim;
}

void
GPA::GetSimilarityMatrices(std::vector<Matrix<double> > *S)
{
    std::vector<Matrix<double> > sim(m_individuals);
    Matrix<double> simi;
    for (int i=0;i<m_individuals;i++)
    {
        GetSimilarityMatrix(i,&simi);
        sim[i] = simi;
    }
    (*S) = sim;
}

// Get transformation matrix
void
GPA::GetTransformationMatrix(int i, Matrix<double> *T)
{
    if (i < 0 || i >= m_individuals)
    {
        cout << "GetTransformationMatrix: index " << i <<" out if range." << endl;
        return;
    }

    Matrix<double> X1TX2(m_dimensions, m_dimensions);

    Matrix<double> U(m_dimensions, m_dimensions);
    Matrix<double> S(m_dimensions, m_dimensions);
    Matrix<double> V(m_dimensions, m_dimensions);

    X1TX2 = Transpose(m_procrustesCoordinates[i]) * (m_originalCoordinates[i]); //RP 1/1/09 need rot matrix between original coord and GPA coords

    // Do Singular Value Decomposition on the X1TX2 matrix
    svd(X1TX2, &U, &S, &V);

    Matrix<double> trans(m_dimensions+1, m_dimensions+1);
    Matrix<double> rot = Transpose (V * Transpose(U)); // RP 1/1/09 added Transpose as Transf matrix used with coords in column format

    for (int j=0;j<m_dimensions;j++)
    {
        for (int k=0;k<m_dimensions;k++)
        {
            trans[j][k] = rot[j][k] / m_centroidSizes[i]; //RP 28/12/08 scale rotation
        }
    }

    for (int j=0;j<m_dimensions;j++) //RP 28/12/08 Rotate & scale translation
    {
        double tr = 0.0;
        for (int k=0;k<m_dimensions;k++)
        {
            tr += rot[j][k] * (- m_translations[i][k]); // translation need negating
        }
        trans[j][m_dimensions] = tr / m_centroidSizes[i];
        trans[m_dimensions][j] = 0.0;
    } //RP end

    trans[m_dimensions][m_dimensions] = 1.0;

    (*T) = trans;
}

void
GPA::GetTransformationMatrices(std::vector<Matrix<double> > *T)
{
    std::vector<Matrix<double> > tran(m_individuals);
    Matrix<double> trani;
    for (int i=0;i<m_individuals;i++)
    {
        GetTransformationMatrix(i,&trani);
        tran[i] = trani;
    }
    (*T) = tran;
}

// Perform GPA on our data
void
GPA::PerformGPA(int alignment, bool scale, bool rotate, bool translate, bool reflection, std::vector<int> midline)
{
    if (m_dimensions > m_landmarks)
    {
        cout << "Error: to few landmarks." << endl;
        return;
    }

    if (alignment == 0)
    {
        alignment = m_alignment;
    }

    Matrix<double> concens(m_landmarks, m_dimensions);
    double rsize;

    std::vector<Matrix<double> > tempCoordinates = m_originalCoordinates;

    m_procrustesCoordinates.clear();
    m_procrustesResiduals.clear();
    m_centroidSizes.clear();
    m_translations.clear();

    // remove location and scale
    for (int i=0;i<m_individuals;i++)
    {
        std::vector<double> translation(m_dimensions);

        if (translate)
        {
            // centre sets the zero to the centroid
            centre(&tempCoordinates[i],m_landmarks, m_dimensions, translation);
        }
        if (scale)
        {
            // unitzise calculates centroid size and scales to unit size
            unitsize(&tempCoordinates[i], m_landmarks, m_dimensions, &rsize);
        }
        else
        {
            // calczise just calculates centroid size
            calcsize(&tempCoordinates[i], m_landmarks, m_dimensions, &rsize);
        }
        // save centroid size
        m_centroidSizes.push_back(rsize);
        // save translation
        m_translations.push_back(translation);
    }

    if (rotate)
    {
/*
        if (m_landmarks == 3 && m_dimensions == 3)
        {
            // put a flat triangle into concens
            concens[1][0] = 1.0;
            concens[2][0] = 1.0;
            concens[2][1] = 1.0;

            //now rotate
            procrustes(&tempCoordinates[0], concens, m_landmarks, m_dimensions, reflection);
        }
*/
        // now we start the rotation fitting; each shape is fitted to the first one
        concens = tempCoordinates[0];

        for (int i=1;i<m_individuals;i++)
        {
            procrustes(&tempCoordinates[i], concens, m_landmarks, m_dimensions, reflection);
        }
    }

    Matrix<double> lastconcens(m_landmarks, m_dimensions);

    for (int iterations=0;iterations<m_maxIterations;iterations++)
    {
        lastconcens = concens;
        concens.Zeros();

        for (int i=0;i<m_individuals;i++)
        {
            concens += tempCoordinates[i];
        }
        concens /= (double)m_individuals;

        if (scale)
        {
            unitsize(&concens, m_landmarks, m_dimensions, &rsize);
        }

        if (rotate)
        {
            for (int i=0;i<m_individuals;i++)
            {
                procrustes(&tempCoordinates[i], concens, m_landmarks, m_dimensions, reflection);
            }
        }

        double error = 0.0;
        for (int i=0;i<m_landmarks;i++)
        {
            for (int j=0;j<m_dimensions;j++)
            {
                error += (concens[i][j] - lastconcens[i][j]) * (concens[i][j] - lastconcens[i][j]);
            }
        }

        if (error < m_minTolerance)
        {
            break;
        }
    }

    if (alignment == 2)
    {
        // align with first specimen
        // do nothing
    }
    else if (alignment == 3)
    {
        // align with plane of symmetry
        if (!midline.empty())
        {
            Matrix<double> rot;
            Matrix<double> temp(midline.size(),m_dimensions);
            for (unsigned int i=0;i<midline.size();i++)
            {
                for (int j=0;j<m_dimensions;j++)
                {
                    temp[i][j] = concens[midline[i]-1][j];
                }
            }
            pcalign(temp,rot);
            for (int i=0;i<m_individuals;i++)
            {
                tempCoordinates[i] = tempCoordinates[i]*rot;
            }
            concens = concens * rot;
        }
    }
    else
    {
        // align coordiates with principal axes
        Matrix<double> rot;
        pcalign(concens,rot);
        for (int i=0;i<m_individuals;i++)
        {
            tempCoordinates[i] = tempCoordinates[i]*rot;
        }
    }

    // store results
    m_procrustesMean = concens;

    for (int i=0;i<m_individuals;i++)
    {
        m_procrustesCoordinates.push_back(tempCoordinates[i]);
        m_procrustesResiduals.push_back(tempCoordinates[i] - m_procrustesMean);
    }

    m_calculated = true;
}

void
GPA::GetProcrustesMean(std::vector<std::vector<double> > *m)
{
    std::vector<std::vector<double> > rows;
    std::vector<double> cols;

    for (int j=0;j<m_landmarks;j++)
    {
        cols.clear();
        for (int k=0;k<m_dimensions;k++)
        {
            cols.push_back(m_procrustesMean[j][k]);
        }
        rows.push_back(cols);
    }
    (*m) = rows;
}

void
GPA::GetProcrustesMean(Matrix<double> *m)
{
    (*m) = m_procrustesMean;
}

void
GPA::GetMeanAsComplex(std::vector<complex<double> > *m)
{
    std::vector<complex<double> > r;

    if (m_dimensions != 2)
    {
        cout << "GetMeanAsComplex: this option is only available for 2 dimensions." << endl;
        return;
    }

    for (int i=0;i<m_landmarks;i++)
    {
        complex<double> c(m_procrustesMean[i][0], m_procrustesMean[i][1]);
        r.push_back(c);
    }
    (*m) = r;
}

void
GPA::GetFormSpaceCoordinates(Matrix<double> *F)
{
    Matrix<double> fsc(m_individuals, m_landmarks*m_dimensions+1);

    for (int i=0;i<m_individuals;i++)
    {
        for (int k=0;k<m_landmarks;k++)
        {
            for (int j=0;j<m_dimensions;j++)
            {
                fsc[i][k*m_dimensions+j] = m_procrustesCoordinates[i][k][j];
            }
            fsc[i][m_landmarks*m_dimensions] = log(m_centroidSizes[i]);
        }
    }

    (*F) = fsc;
}

void
GPA::GetProcrustesCoordinates(int i, Matrix<double> *proc, bool scale)
{
    if (i < 0 || i >= (int)m_procrustesCoordinates.size())
    {
        cout << "GetProcrustesCoordinates: index " << i <<" out if range." << endl;
        return;
    }

    (*proc) = m_procrustesCoordinates[i];
    if (scale)
    {
        // scale coordintes with cos(rho)
        Matrix<double> X1TX2;
        Row<double> lambda;
        double sum = 0.0;
        double cosrho = 0.0;

        X1TX2 = Transpose(m_procrustesCoordinates[i]) * (m_procrustesMean);

        // Do Singular Value Decomposition on the X1TX2 matrix
        lambda = svd(X1TX2);
        for (int k=0;k<m_dimensions;k++)
        {
            sum += lambda[k];
        }
        cosrho = sum;
        (*proc) *= cosrho;
    }
}

void
GPA::GetProcrustesCoordinates(Matrix<double> *P, bool scale)
{
    Matrix<double> temp(m_individuals*m_landmarks, m_dimensions);
    for (int i=0;i<m_individuals;i++)
    {
        Matrix<double> temp2(m_landmarks, m_dimensions);
        GetProcrustesCoordinates(i, &temp2, scale);
        for (int j=0;j<m_landmarks;j++)
        {
            for (int k=0;k<m_dimensions;k++)
            {
                temp[i*m_landmarks+j][k] = temp2[j][k];
            }
        }
    }
    (*P) = temp;
}

void
GPA::GetProcrustesResiduals(int i, Matrix<double> *r)
{
    (*r) = m_procrustesResiduals[i];
}

void
GPA::GetProcrustesResiduals(int i, std::vector<std::vector<double> > *r)
{
    std::vector<std::vector<double> > rows;
    std::vector<double> cols;
    for (int j=0;j<m_landmarks;j++)
    {
        cols.clear();
        for (int k=0;k<m_dimensions;k++)
        {
            cols.push_back(m_procrustesResiduals[i][j][k]);
        }
        rows.push_back(cols);
    }
    (*r) = rows;
}

void
GPA::GetCentroidSizes(std::vector<double> *c)
{
    (*c) = m_centroidSizes;
}

void
GPA::SetProcrustesDistanceMethod(int method)
{
    m_method = method;
}

void
GPA::SetAlignment(int alignment)
{
    m_alignment = alignment;
}

int
GPA::GetNumberOfSpecimens()
{
    return m_individuals;
}

int
GPA::GetNumberOfLandmarks()
{
    return m_landmarks;
}

int
GPA::GetNumberOfDimensions()
{
    return m_dimensions;
}

bool
GPA::IsCalculated()
{
    return m_calculated;
}

void
GPA::GetUniformBasis(Matrix<double> *uniform)
{
    std::vector<double> lmks(m_dimensions*m_landmarks);
    std::vector<double> centroid(m_dimensions);
    std::vector<double> sum_squares(m_dimensions);
    std::vector<double> lmksaxes(m_landmarks*m_landmarks);

    int k = 0;
    for (int i=0;i<m_landmarks;i++)
    {
        for (int j=0;j<m_dimensions;j++)
        {
            lmks[k] = m_procrustesMean[i][j];
            k++;
        }
    }

    if (m_dimensions == 2)
    {
        std::vector<double> basis(2*m_dimensions*m_landmarks);

        ew::Tps2::principal_axes(&centroid[0], &sum_squares[0], &lmksaxes[0], &lmks[0], m_landmarks);
        ew::Tps2::uniform_basis(&basis[0],0,&centroid[0],&sum_squares[0],&lmksaxes[0],&lmks[0],m_landmarks);

        uniform->ReSize(2,m_landmarks*m_dimensions);
        for (int i=0;i<2;i++)
        {
            for (int j=0;j<m_dimensions*m_landmarks;j++)
            {
                (*uniform)[i][j] = basis[i*(m_dimensions*m_landmarks)+j];
            }
        }
    }
    else if (m_dimensions == 3)
    {

        std::vector<double> basis(5*m_dimensions*m_landmarks);

        ew::Tps3::principal_axes(&centroid[0], &sum_squares[0], &lmksaxes[0], &lmks[0], m_landmarks);
        ew::Tps3::uniform_basis(&basis[0],0,&centroid[0],&sum_squares[0],&lmksaxes[0],&lmks[0],m_landmarks);

        uniform->ReSize(5,m_landmarks*m_dimensions);
        for (int i=0;i<5;i++)
        {
            for (int j=0;j<m_dimensions*m_landmarks;j++)
            {
                (*uniform)[i][j] = basis[i*(m_dimensions*m_landmarks)+j];
            }
        }
    }
}
