#include "FemInternalSolver.h"

//#define WANT_STREAM
//#define WANT_MATH

#include <ofem/beam_load.h>

#include <logger.h>

#include <sstream>

using namespace ofem;

std::string float2str(double value)
{
    std::stringstream coordStream;
    coordStream << std::fixed << std::setw(10) << std::setprecision(2) << value;
    return coordStream.str();
}

void bar3e(
    RowVector& ex,
    RowVector& ey,
    RowVector& ez,
    RowVector& ep,
    double eq,
    Matrix& Ke,
    ColumnVector& fe)
{
    /*
    E = ep[0]
    A = ep[1]

    b = np.mat([
        [ex[1]-ex[0]],
        [ey[1]-ey[0]],
        [ez[1]-ez[0]]
    ])
    L = np.sqrt(b.T*b).item()

    n = np.asarray(b.T/L).reshape(3)

    G = np.mat([
        [n[0], n[1], n[2], 0.,   0.,   0.],
        [0.,   0.,   0.,   n[0], n[1], n[2]]
    ])

    Kle = E*A/L*np.mat([
        [1, -1],
        [-1, 1]
    ])

    return G.T*Kle*G
    */

    double E = ep(1);
    double A = ep(2);

    ColumnVector b(3);
    RowVector n(3);
    Matrix G(2, 6);
    Matrix Kle(2, 2);
    ColumnVector fle(2);

    b << ex(2) - ex(1)
      << ey(2) - ey(1)
      << ez(2) - ez(1);

    double L = sqrt((b.t() * b).AsScalar());

    n = b.t() / L;

    G << n(1) << n(2) << n(3) << 0.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 0.0 << n(1) << n(2) << n(3);

    double a = E * A / L;

    Kle << a << -a
        << -a << a;

    a = eq * L / 2.0;

    fle << a << a;

    Ke = G.t() * Kle * G;
    fe = G.t() * fle;
}

void bar3s(
    RowVector& ex,
    RowVector& ey,
    RowVector& ez,
    RowVector& ep,
    RowVector& ed,
    double eq,
    int n,
    ColumnVector& es,
    ColumnVector& edi,
    ColumnVector& eci)
{
    /*
    E = ep[0]
    A = ep[1]

    b = np.mat([
        [ex[1]-ex[0]],
        [ey[1]-ey[0]],
        [ez[1]-ez[0]]
    ])
    L = np.sqrt(b.T*b).item()

    n = np.asarray(b.T/L).reshape(3)

    G = np.mat([
        [n[0], n[1], n[2], 0., 0., 0.],
        [0., 0., 0., n[0], n[1], n[2]]
    ])

    #Kle = E*A/L*np.mat([
    #    [ 1,-1],
    #    [-1, 1]
    #])

    u = np.asmatrix(ed).T
    N = E*A/L*np.mat([[-1., 1.]])*G*u

    return N.item()
    */
    double E = ep(1);
    double A = ep(2);

    ColumnVector b(3);
    RowVector nn(3);
    Matrix G(2, 6);
    Matrix Kle(2, 2);
    ColumnVector fle(2);

    b << ex(2) - ex(1)
      << ey(2) - ey(1)
      << ez(2) - ez(1);

    double L = sqrt((b.t() * b).AsScalar());

    nn = b.t() / L;

    G << nn(1) << nn(2) << nn(3) << 0.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 0.0 << nn(1) << nn(2) << nn(3);

    double c1 = E * A / L;

    Kle << c1 << -c1
        << -c1 << c1;

    c1 = eq * L / 2.0;

    fle << c1 << c1;

    ColumnVector ae = ed.t();
    ColumnVector ale = G * ae;

    RowVector B(2);

    B << -1.0 / L << 1.0 / L;

    for (int i = 1; i <= n; i++)
    {
        eci(i) = (i - 1.0) * L / (n - 1.0);
        double x = eci(i);
        double up = -eq * (0.5 * pow(x, 2) - 0.5 * L * x) / E / A;
        double Np = -eq * (x - 0.5 * L);

        RowVector N(2);
        N << 1.0 - x / L << x / L;

        edi(i) = (N * ale + up).AsScalar(); // [1 x 2] * [2 x 1]
        es(i) = (E * A * B * ale + Np).AsScalar();
        eci(i) = x;
    }
}

void beam3e(
    RowVector& ex,
    RowVector& ey,
    RowVector& ez,
    RowVector& eo,
    RowVector& ep,
    RowVector& eq,
    Matrix& Ke,
    ColumnVector& fe)
{
    ColumnVector bb(3);
    bb << ex(2) - ex(1) << ey(2) - ey(1) << ez(2) - ez(1);
    double L = sqrt((bb.t() * bb).AsScalar());

    RowVector n1 = (bb / L).t();

    double lc = sqrt((eo * eo.t()).AsScalar());
    RowVector n3 = eo / lc;

    double qx = eq(1);
    double qy = eq(2);
    double qz = eq(3);
    double qw = eq(4);

    double E = ep(1);
    double Gs = ep(2);
    double A = ep(3);
    double Iy = ep(4);
    double Iz = ep(5);
    double Kv = ep(6);

    double a = E * A / L;
    double b = 12.0 * E * Iz / pow(L, 3);
    double c = 6.0 * E * Iz / pow(L, 2);
    double d = 12.0 * E * Iy / pow(L, 3);
    double e = 6.0 * E * Iy / pow(L, 2);
    double f = Gs * Kv / L;
    double g = 2.0 * E * Iy / L;
    double h = 2 * E * Iz / L;

    Matrix Kle(12, 12);

    Kle << a << 0 << 0 << 0 << 0 << 0 << -a << 0 << 0 << 0 << 0 << 0
        << 0 << b << 0 << 0 << 0 << c << 0 << -b << 0 << 0 << 0 << c
        << 0 << 0 << d << 0 << -e << 0 << 0 << 0 << -d << 0 << -e << 0
        << 0 << 0 << 0 << f << 0 << 0 << 0 << 0 << 0 << -f << 0 << 0
        << 0 << 0 << -e << 0 << 2 * g << 0 << 0 << 0 << e << 0 << g << 0
        << 0 << c << 0 << 0 << 0 << 2 * h << 0 << -c << 0 << 0 << 0 << h
        << -a << 0 << 0 << 0 << 0 << 0 << a << 0 << 0 << 0 << 0 << 0
        << 0 << -b << 0 << 0 << 0 << -c << 0 << b << 0 << 0 << 0 << -c
        << 0 << 0 << -d << 0 << e << 0 << 0 << 0 << d << 0 << e << 0
        << 0 << 0 << 0 << -f << 0 << 0 << 0 << 0 << 0 << f << 0 << 0
        << 0 << 0 << -e << 0 << g << 0 << 0 << 0 << e << 0 << 2 * g << 0
        << 0 << c << 0 << 0 << 0 << h << 0 << -c << 0 << 0 << 0 << 2 * h;

    ColumnVector fle(12);

    fle << qx
        << qy
        << qz
        << qw
        << -qz * L / 6.0
        << qy * L / 6.0
        << qx
        << qy
        << qz
        << qw
        << qz * L / 6.0
        << -qy * L / 6.0;
    fle = fle * L / 2.0;

    RowVector n2(3);

    n2 << n3(2) * n1(3) - n3(3) * n1(2)
       << -n1(3) * n3(1) + n1(1) * n3(3)
       << n3(1) * n1(2) - n1(1) * n3(2);

    Matrix An(3, 3);

    An.Row(1) = n1;
    An.Row(2) = n2;
    An.Row(3) = n3;

    Matrix G(12, 12);

    G = 0.0;

    G.SubMatrix(1, 3, 1, 3) = An;
    G.SubMatrix(4, 6, 4, 6) = An;
    G.SubMatrix(7, 9, 7, 9) = An;
    G.SubMatrix(10, 12, 10, 12) = An;

    Ke = G.t() * Kle * G;
    fe = G.t() * fle;
}

void beam3s(
    RowVector& ex,
    RowVector& ey,
    RowVector& ez,
    RowVector& eo,
    RowVector& ep,
    RowVector& ed,
    RowVector& eq,
    int n,
    Matrix& es,
    Matrix& edi,
    ColumnVector& eci)
{
    ColumnVector bb(3);
    bb << ex(2) - ex(1) << ey(2) - ey(1) << ez(2) - ez(1);
    double L = sqrt((bb.t() * bb).AsScalar());

    RowVector n1 = (bb / L).t();

    double lc = sqrt((eo * eo.t()).AsScalar());
    RowVector n3 = eo / lc;

    double qx = eq(1);
    double qy = eq(2);
    double qz = eq(3);
    double qw = eq(4);

    double E = ep(1);
    double Gs = ep(2);
    double A = ep(3);
    double Iy = ep(4);
    double Iz = ep(5);
    double Kv = ep(6);

    RowVector n2(3);

    n2 << n3(2) * n1(3) - n3(3) * n1(2)
       << -n1(3) * n3(1) + n1(1) * n3(3)
       << n3(1) * n1(2) - n1(1) * n3(2);

    Matrix An(3, 3);

    An.Row(1) = n1;
    An.Row(2) = n2;
    An.Row(3) = n3;

    Matrix G(12, 12);

    G = 0.0;

    G.SubMatrix(1, 3, 1, 3) = An;
    G.SubMatrix(4, 6, 4, 6) = An;
    G.SubMatrix(7, 9, 7, 9) = An;
    G.SubMatrix(10, 12, 10, 12) = An;

    ColumnVector u(12);
    ColumnVector diffSol(12);

    diffSol << 0.0
            << 0.0
            << 0.0
            << 0.0
            << 0.0
            << 0.0
            << -qx * pow(L, 2) / 2.0 / E / A
            << qy * pow(L, 4) / 24.0 / E / Iz
            << qz * pow(L, 4) / 24.0 / E / Iy
            << -qw * pow(L, 2) / 2.0 / Gs / Kv
            << -qz * pow(L, 3) / 6.0 / E / Iy
            << qy * pow(L, 3) / 6.0 / E / Iz;

    u = G * ed.AsColumn() - diffSol;

    Matrix C(12, 12);

    C << 0.0 << 1.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 1.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 1.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 1.0
      << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << -1.0 << 0.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 0.0 << 0.0 << 1.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
      << L << 1.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
      << 0.0 << 0.0 << pow(L, 3) << pow(L, 2) << L << 1.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << pow(L, 3) << pow(L, 2) << L << 1.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << L << 1.0
      << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << -3.0 * pow(L, 2) << -2.0 * L << -1.0 << 0.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 3.0 * pow(L, 2) << 2.0 * L << 1 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0 << 0.0;

    ColumnVector m(12);

    m = C.i() * u;

    es.ReSize(n, 6);
    es = 0.0;
    edi.ReSize(n, 4);
    edi = 0.0;
    eci.ReSize(n);
    eci = 0.0;

    int i;
    for (i = 1; i <= n; i++)
    {
        eci(i) = (i - 1.0) * L / (n - 1.0);
        double x = eci(i);

        Matrix T1(6, 12);

        T1 << E * A << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
           << 0.0 << 0.0 << -6.0 * E * Iz << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
           << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << -6.0 * E * Iy << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
           << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << Gs * Kv << 0.0
           << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << -6.0 * E * Iy * x << -2.0 * E * Iy << 0.0 << 0.0 << 0.0 << 0.0
           << 0.0 << 0.0 << 6.0 * E * Iz * x << 2.0 * E * Iz << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

        ColumnVector T2(6);

        T2 << -qx * x
           << -qy * x
           << -qz * x
           << -qw * x
           << -qz * pow(x, 2) / 2.0
           << qy * pow(x, 2) / 2.0;

        es.Row(i) = (T1 * m + T2).t();

        /*
        for (j=1; j<=6; j++)
                so_print(es(i,j) << ", ";
        so_print(endl;
        */

        Matrix T3(4, 12);

        T3 << x << 1.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
           << 0.0 << 0.0 << pow(x, 3) << pow(x, 2) << x << 1.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
           << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << pow(x, 3) << pow(x, 2) << x << 1.0 << 0.0 << 0.0
           << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << x << 1.0;

        ColumnVector T4(4);

        T4 << -qx * pow(x, 2) / 2.0 / E / A
           << qy * pow(x, 4) / 24.0 / E / Iz
           << qz * pow(x, 4) / 24.0 / E / Iy
           << -qw * pow(x, 2) / 2.0 / Gs / Kv;

        edi.Row(i) = (T3 * m + T4).t();

        /*
        for (j=1; j<=4; j++)
                so_print(edi(i,j) << ", ";
        so_print(endl;
        */
    }
}

void assem(
    RowVector& Topo,
    SymmetricBandMatrix& K,
    Matrix& Ke,
    ColumnVector& f,
    ColumnVector& fe)
{
    int i, j;

    for (i = 1; i <= Ke.Nrows(); i++)
        for (j = i; j <= Ke.Ncols(); j++)
            K((int)Topo(i), (int)Topo(j)) = K((int)Topo(i), (int)Topo(j)) + Ke(i, j);

    for (i = 1; i <= fe.Nrows(); i++)
        f((int)Topo(i)) = f((int)Topo(i)) + fe(i);
}

double max(RowVector& rowVector)
{
    int i;
    double maxValue = -1e300;

    for (i = 1; i <= rowVector.Ncols(); i++)
        if (rowVector(i) > maxValue)
            maxValue = rowVector(i);

    return maxValue;
}

double min(RowVector& rowVector)
{
    int i;
    double minValue = 1e300;

    for (i = 1; i <= rowVector.Ncols(); i++)
        if (rowVector(i) < minValue)
            minValue = rowVector(i);

    return minValue;
}

FrameSolver::FrameSolver()
{
    m_beamModel = NULL;
    m_maxNodeValue = -1.0e300;
    m_X = NULL;
    m_forceNode = NULL;
    m_errorStatus = BS_NO_ERROR;
    m_resultInfo = NULL;
}

FrameSolver::~FrameSolver()
{
    if (m_X != NULL)
        delete m_X;
}

void FrameSolver::setBeamModel(ofem::BeamModel* model)
{
    m_beamModel = model;
}

void FrameSolver::execute()
{
    double E, G, A, Iy, Iz, Kv;
    int i, j, k;

    //
    // Reset error status
    //

    m_errorStatus = BS_NO_ERROR;

    Logger::instance()->log(LogLevel::Error, "\nSimple 3D beam solver");
    Logger::instance()->log(LogLevel::Error, "---------------------\n");

    //
    // Retrieve fem model
    //

    BeamModel* femModel = m_beamModel;

    if (femModel == NULL)
    {
        Logger::instance()->log(LogLevel::Error, "Invalid model.");
        m_errorStatus = BS_INVALID_MODEL;
        return;
    }

    //
    // Retrieve individual parts of fem model
    //

    BeamSet* elementSet = static_cast<BeamSet*>(femModel->getElementSet());
    NodeSet* nodeSet = femModel->getNodeSet();
    MaterialSet* materialSet = femModel->getMaterialSet();
    NodeBCSet* bcSet = femModel->getNodeBCSet();
    NodeLoadSet* nodeLoadSet = femModel->getNodeLoadSet();
    ElementLoadSet* elementLoadSet = femModel->getElementLoadSet();

    //
    // Check if we have a valid model
    //

    if (nodeSet->getSize() == 0)
    {
        Logger::instance()->log(LogLevel::Error, "No nodes defined.");
        m_errorStatus = BS_NO_NODES;
        return;
    }

    if (elementSet->getSize() == 0)
    {
        Logger::instance()->log(LogLevel::Error, "No elements defined.");
        m_errorStatus = BS_NO_ELEMENTS;
        return;
    }

    if (bcSet->getSize() == 0)
    {
        Logger::instance()->log(LogLevel::Error, "No boundary conditions defined.");
        m_errorStatus = BS_NO_BC;
        return;
    }

    if ((nodeLoadSet->getSize() == 0) && (elementLoadSet->getSize() == 0) && (m_forceNode == NULL))
    {
        Logger::instance()->log(LogLevel::Error, "No node loads defined.");
        m_errorStatus = BS_NO_LOADS;
        return;
    }

    //
    // Enumerate everything
    //

    nodeSet->resetNodeKind(nkNotConnected);
    elementSet->updateNodeKinds();

    nodeSet->enumerateNodes();
    materialSet->enumerateMaterials();
    elementSet->enumerateElements();
    nodeLoadSet->enumerateLoads();
    elementLoadSet->enumerateLoads();
    bcSet->enumerateBCs();
    m_nDof = nodeSet->enumerateDofs() - 1;

    //
    // Assemble system
    //

    RowVector Ex(2);
    RowVector Ey(2);
    RowVector Ez(2);
    Matrix Eq(static_cast<int>(elementSet->getSize()), 4);
    Eq = 0.0;
    RowVector Eo(3);
    RowVector Ep(6);
    Ep = 0.0;
    Matrix Ke(12, 12);
    Matrix Ke_b(6, 6);
    ColumnVector fe(12);
    ColumnVector fe_b(6);
    m_f.ReSize(m_nDof, 1);
    m_f = 0.0;
    RowVector DofTopo(12);
    RowVector DofTopo_b(6);

    //
    // Element loads
    //

    Logger::instance()->log(LogLevel::Info, "Setting up element loads.");

    for (i = 0; i < elementLoadSet->getSize(); i++)
    {
        double vx, vy, vz;
        double value;

        BeamLoad* elementLoad = (BeamLoad*)elementLoadSet->getLoad(i);

        elementLoad->getLocalDirection(vx, vy, vz);
        value = -elementLoad->getValue();

        for (j = 0; j < elementLoad->getElementsSize(); j++)
        {
            Element* element = elementLoad->getElement(j);

            Eq(element->getNumber(), 1) = Eq(element->getNumber(), 1) + vx * value;
            Eq(element->getNumber(), 2) = Eq(element->getNumber(), 2) + vy * value;
            Eq(element->getNumber(), 3) = Eq(element->getNumber(), 3) + vz * value;
            Eq(element->getNumber(), 4) = 0.0;
        }
    }

    //
    // Calculate bandwidth
    //

    Logger::instance()->log(LogLevel::Info, "Calculating bandwidth.");

    int maxBandwidth = 0;
    int bandwidth;

    for (i = 1; i <= elementSet->getSize(); i++)
    {
        auto beam = static_cast<ofem::Beam*>(elementSet->getElement(i - 1));

        if (beam->beamType() == btBeam)
        {
            for (j = 0; j < 6; j++)
                DofTopo(j + 1) = beam->getNode(0)->getDof(j)->getNumber();

            for (j = 0; j < 6; j++)
                DofTopo(j + 7) = beam->getNode(1)->getDof(j)->getNumber();

            bandwidth = (int)max(DofTopo) - (int)min(DofTopo);
            if (bandwidth > maxBandwidth)
                maxBandwidth = bandwidth;
        }
        else
        {
            for (j = 0; j < 3; j++)
                DofTopo_b(j + 1) = beam->getNode(0)->getDof(j)->getNumber();

            for (j = 0; j < 3; j++)
                DofTopo_b(j + 4) = beam->getNode(1)->getDof(j)->getNumber();

            bandwidth = (int)max(DofTopo_b) - (int)min(DofTopo_b);
            if (bandwidth > maxBandwidth)
                maxBandwidth = bandwidth;
        }
    }

    // so_print(endl << "\tBandwidth = " << maxBandwidth);
    // so_print("\tDegrees of freedom = " << m_nDof << endl);

    // BandMatrix K(m_nDof,maxBandwidth,maxBandwidth);
    SymmetricBandMatrix K(m_nDof, maxBandwidth);
    K = 0.0;

    Logger::instance()->log(LogLevel::Info, "Assembling system matrix.");

    for (i = 1; i <= elementSet->getSize(); i++)
    {
        double x1, y1, z1;
        double x2, y2, z2;
        double ex, ey, ez;

        Beam* beam = (Beam*)elementSet->getElement(i - 1);

        beam->getNode(0)->getCoord(x1, y1, z1);
        beam->getNode(1)->getCoord(x2, y2, z2);

        Ex(1) = x1;
        Ey(1) = y1;
        Ez(1) = z1;
        Ex(2) = x2;
        Ey(2) = y2;
        Ez(2) = z2;

        beam->getOrientationZ(ex, ey, ez);
        Eo(1) = ex;
        Eo(2) = ey;
        Eo(3) = ez;

        if (beam->getMaterial() != NULL)
        {
            beam->getMaterial()->getProperties(E, G, A, Iy, Iz, Kv);
            Ep(1) = E;
            Ep(2) = G;
            Ep(3) = A;
            Ep(4) = Iy;
            Ep(5) = Iz;
            Ep(6) = Kv;

            if (beam->beamType() == ofem::btBeam)
            {
                for (j = 0; j < 6; j++)
                    DofTopo(j + 1) = beam->getNode(0)->getDof(j)->getNumber();

                for (j = 0; j < 6; j++)
                    DofTopo(j + 7) = beam->getNode(1)->getDof(j)->getNumber();

                RowVector RowEq(4);
                RowEq = Eq.Row(i);

                beam3e(Ex, Ey, Ez, Eo, Ep, RowEq, Ke, fe);
                assem(DofTopo, K, Ke, m_f, fe);
            }
            else
            {
                Ep(1) = E;
                Ep(2) = A;

                for (j = 0; j < 3; j++)
                    DofTopo_b(j + 1) = beam->getNode(0)->getDof(j)->getNumber();

                for (j = 0; j < 3; j++)
                    DofTopo_b(j + 4) = beam->getNode(1)->getDof(j)->getNumber();

                double eq = 0.0;

                bar3e(Ex, Ey, Ez, Ep, eq, Ke_b, fe_b);
                assem(DofTopo_b, K, Ke_b, m_f, fe_b);
            }
        }
        else
        {
            Logger::instance()->log(LogLevel::Error, "Element with undefined material.");
            m_errorStatus = BS_UNDEFINED_MATERIAL;
        }

        if (m_errorStatus != BS_NO_ERROR)
            break;
    }

    if (m_errorStatus != BS_NO_ERROR)
        return;

    //
    // Node loads
    //

    Logger::instance()->log(LogLevel::Info, "Defining load vector.");

    for (i = 0; i < nodeLoadSet->getSize(); i++)
    {
        double vx, vy, vz;
        double value;

        BeamNodeLoad* nodeLoad = (BeamNodeLoad*)nodeLoadSet->getLoad(i);

        nodeLoad->getDirection(vx, vy, vz);
        value = nodeLoad->getValue();

        for (j = 0; j < (int)nodeLoad->getNodeSize(); j++)
        {
            Node* node = nodeLoad->getNode(j);
            m_f(node->getDof(0)->getNumber()) = vx * value;
            m_f(node->getDof(1)->getNumber()) = vy * value;
            m_f(node->getDof(2)->getNumber()) = vz * value;
        }
    }

    //
    // Boundary conditions
    //

    Logger::instance()->log(LogLevel::Info, "Setting up boundary conditions.");

    Matrix Bc(m_nDof, 2);
    Bc = 0.0;
    int bcCount = 0;

    for (i = 0; i < bcSet->getSize(); i++)
    {
        BeamNodeBC* nodeBC = (BeamNodeBC*)bcSet->getBC(i);

        for (j = 0; j < nodeBC->getNodeSize(); j++)
        {
            Node* node = nodeBC->getNode(j);

            if (node->getKind() != nkNotConnected)
            {
                for (k = 0; k < 6; k++)
                {
                    if (nodeBC->isPrescribed(k + 1))
                    {
                        if (node->getDof(k) != nullptr)
                        {
                            bcCount++;
                            Bc(bcCount, 1) = node->getDof(k)->getNumber();
                            Bc(bcCount, 2) = nodeBC->getPrescribedValue(k);
                        }
                    }
                }
            }
        }
    }

    Bc = Bc.Rows(1, bcCount);

    //
    // Remove boundary conditions from full system matrix
    // This is not very effective!
    // WARNING!! routine does not handle prescribed displacements!=0.0
    //

    Logger::instance()->log(LogLevel::Info, "Removing boundary conditions from system matrix.");

    RowVector Idx(m_nDof);
    Idx = 0.0;

    for (i = 1; i <= Bc.Nrows(); i++)
        Idx((int)Bc(i, 1)) = 1.0;

    m_nVars = K.Nrows() - Bc.Nrows();

    // so_print(endl << "\tFree variables = " << m_nVars << endl);

    if (maxBandwidth > m_nVars)
        maxBandwidth = m_nVars;
    // BandMatrix Ksys(m_nVars,maxBandwidth,maxBandwidth);
    SymmetricBandMatrix Ksys(m_nVars, maxBandwidth);

    m_fsys.ReSize(K.Nrows() - Bc.Nrows(), 1);
    // ColumnVector fsys(K.Nrows() - Bc.Nrows());
    m_gdof.ReSize(K.Nrows() - Bc.Nrows(), 1);
    m_ldof.ReSize(K.Nrows(), 1);

    Ksys = 0.0;
    m_fsys = 0.0;
    m_gdof = 0.0;
    m_ldof = -1.0;

    int row = 1;
    int col = 1;

    Logger::instance()->log(LogLevel::Info, "Creating Ksys.");

    for (i = 1; i <= K.Nrows(); i++)
    {
        if (Idx(i) < 0.5)
        {
            col = row;
            for (j = i; j <= K.Ncols(); j++)
            {
                if (Idx(j) < 0.5)
                {
                    if (abs(row - col) <= maxBandwidth)
                        if (abs(i - j) <= maxBandwidth)
                            Ksys(row, col) = K(i, j);
                    col++;
                }
            }
            m_fsys(row) = m_f(i);
            m_gdof(row) = i;
            m_ldof(i) = row;
            row++;
        }
    }

    //
    // Solve system
    //

    m_a.ReSize(m_nVars, 1);
    m_a = 0.0;

    //
    // Apply feedback force
    //

    Logger::instance()->log(LogLevel::Info, "Applying feedback force.");

    ColumnVector fsys = m_fsys;

    if (m_forceNode != NULL)
    {
        // cout<< "\tdof1 = " << m_forceNode->getDof(0)->getNumber());
        // cout<< "\tdof2 = " << m_forceNode->getDof(1)->getNumber());
        // cout<< "\tdof3 = " << m_forceNode->getDof(2)->getNumber());
        int ldof1 = (int)m_ldof(m_forceNode->getDof(0)->getNumber());
        int ldof2 = (int)m_ldof(m_forceNode->getDof(1)->getNumber());
        int ldof3 = (int)m_ldof(m_forceNode->getDof(2)->getNumber());
        if ((ldof1 > 0.0) && (ldof2 > 0.0) && (ldof3 > 0.0))
        {
            fsys(ldof1) += m_force[0];
            fsys(ldof2) += m_force[1];
            fsys(ldof3) += m_force[2];
        }
        else
            Logger::instance()->log(LogLevel::Error, "Somethings wrong...");
    }

    if (fsys.IsZero())
    {
        Logger::instance()->log(LogLevel::Error, "No effective loads applied.");
        m_errorStatus = BS_NO_LOADS;
        return;
    }

    Logger::instance()->log(LogLevel::Info, "Solving system. Keeping LU factorisation.");

    if (m_X != NULL)
        delete m_X;

    auto logDetSign = Ksys.LogDeterminant().Sign();

    Logger::instance()->log(LogLevel::Info, "logDetSign = " + float2str(logDetSign));

    if (logDetSign < 0)
    {
        Logger::instance()->log(LogLevel::Error, "System unstable.");
        m_errorStatus = BS_UNSTABLE;
        return;
    }

    if (logDetSign == 0)
    {
        Logger::instance()->log(LogLevel::Error, "Matrix singular.");
        m_errorStatus = BS_SINGULAR;
        return;
    }

    m_X = new LinearEquationSolver(Ksys);
    m_a = m_X->i() * fsys;

    //
    // Create global displacement vector
    //

    m_GlobalA.ReSize(m_nDof);
    m_GlobalA = 0.0;
    m_maxNodeValue = -1.0e300;

    for (i = 1; i <= Ksys.Nrows(); i++)
    {
        m_GlobalA((int)m_gdof(i)) = m_a(i);
        if (fabs(m_a(i)) > m_maxNodeValue)
            m_maxNodeValue = fabs(m_a(i));
    }

    //
    // Store displacements in nodes
    //

    Logger::instance()->log(LogLevel::Error, "Storing results in nodes and elements.");

    nodeSet->clearNodeValues();

    double nodeValue;

    for (i = 0; i < nodeSet->getSize(); i++)
    {
        Node* node = nodeSet->getNode(i);

        if (node->getKind() != nkNotConnected)
        {
            node->setValueSize(3);
            for (j = 0; j < 3; j++)
            {
                nodeValue = m_GlobalA(node->getDof(j)->getNumber());
                node->setValue(j, nodeValue);
            }
        }
    }
    //
    // Store element forces in elements
    //

    int n;
    RowVector Ed(12);
    RowVector Ed_b(6);
    Matrix Es;
    Matrix Edi;
    ColumnVector Eci;

    double N, T, Vy, Vz, My, Mz, Navier;

    initMaxMin();

    for (i = 1; i <= elementSet->getSize(); i++)
    {

        double x1, y1, z1;
        double x2, y2, z2;
        double ex, ey, ez;

        Beam* beam = (Beam*)elementSet->getElement(i - 1);
        n = beam->getEvaluationPoints();
        beam->setValueSize(n * 11);

        beam->getNode(0)->getCoord(x1, y1, z1);
        beam->getNode(1)->getCoord(x2, y2, z2);

        Ex(1) = x1;
        Ey(1) = y1;
        Ez(1) = z1;
        Ex(2) = x2;
        Ey(2) = y2;
        Ez(2) = z2;

        beam->getOrientationZ(ex, ey, ez);
        Eo(1) = ex;
        Eo(2) = ey;
        Eo(3) = ez;

        beam->getMaterial()->getProperties(E, G, A, Iy, Iz, Kv);
        Ep(1) = E;
        Ep(2) = G;
        Ep(3) = A;
        Ep(4) = Iy;
        Ep(5) = Iz;
        Ep(6) = Kv;

        if (beam->beamType() == btBeam)
        {
            for (j = 0; j < 6; j++)
            {
                DofTopo(j + 1) = beam->getNode(0)->getDof(j)->getNumber();
                DofTopo(j + 7) = beam->getNode(1)->getDof(j)->getNumber();
                Ed(j + 1) = m_GlobalA((int)DofTopo(j + 1));
                Ed(j + 7) = m_GlobalA((int)DofTopo(j + 7));
            }

            RowVector RowEq(4);
            RowEq = Eq.Row(i);
            beam3s(Ex, Ey, Ez, Eo, Ep, Ed, RowEq, n, Es, Edi, Eci);

            int pos = 0;

            for (k = 1; k <= n; k++)
            {
                N = Es(k, 1);
                T = Es(k, 2);
                Vy = Es(k, 3);
                Vz = Es(k, 4);
                My = Es(k, 5);
                Mz = Es(k, 6);

                Navier = calcNavier(N, My, Mz, beam);
                updateMaxMin(N, T, Vy, Vz, My, Mz, Navier);

                for (j = 1; j <= 6; j++)
                    beam->setValue(pos++, Es(k, j));
            }

            for (k = 1; k <= n; k++)
                for (j = 1; j <= 4; j++)
                    beam->setValue(pos++, Edi(k, j));
        }
        else
        {
            for (j = 0; j < 3; j++)
            {
                DofTopo_b(j + 1) = beam->getNode(0)->getDof(j)->getNumber();
                DofTopo_b(j + 4) = beam->getNode(1)->getDof(j)->getNumber();
                Ed_b(j + 1) = m_GlobalA((int)DofTopo_b(j + 1));
                Ed_b(j + 4) = m_GlobalA((int)DofTopo_b(j + 4));
            }

            double eq = 0.0;

            ColumnVector Es_b(n);
            ColumnVector Edi_b(n);
            ColumnVector Eci_b(n);

            Ep(1) = E;
            Ep(2) = A;

            bar3s(Ex, Ey, Ez, Ep, Ed_b, eq, n, Es_b, Edi_b, Eci_b);

            int pos = 0;

            for (k = 1; k <= n; k++)
            {
                N = Es_b(k);
                T = 0.0;
                Vy = 0.0;
                Vz = 0.0;
                My = 0.0;
                Mz = 0.0;

                Navier = 0.0;
                updateMaxMin(N, T, Vy, Vz, My, Mz, Navier);

                for (j = 1; j <= 6; j++)
                    if (j == 1)
                        beam->setValue(pos++, Es_b(k));
                    else
                        beam->setValue(pos++, 0.0);
            }

            for (k = 1; k <= n; k++)
                for (j = 1; j <= 4; j++)
                {
                    beam->setValue(pos++, 0.0);
                }
        }
    }

    printMaxMin();
}

double FrameSolver::getMaxNodeValue()
{
    return m_maxNodeValue;
}

void FrameSolver::recompute()
{
    if (this->getLastError() == BS_NO_ERROR)
    {
        int i, j;

        BeamModel* femModel = m_beamModel;

        if (femModel == NULL)
        {
            Logger::instance()->log(LogLevel::Error, "Invalid model.");
            m_errorStatus = BS_INVALID_MODEL;
            return;
        }

        //
        // Retrieve individual parts of fem model
        //

        NodeSet* nodeSet = femModel->getNodeSet();

        //
        // Apply feedback force
        //

        ColumnVector fsys = m_fsys;

        if (m_forceNode != NULL)
        {
            fsys((int)m_ldof(m_forceNode->getDof(0)->getNumber())) += m_force[0];
            fsys((int)m_ldof(m_forceNode->getDof(1)->getNumber())) += m_force[1];
            fsys((int)m_ldof(m_forceNode->getDof(2)->getNumber())) += m_force[2];
        }

        //
        // Calculate new solution
        //

        m_a = m_X->i() * fsys;

        //
        // Create global displacement vector
        //

        m_maxNodeValue = -1.0e300;
        m_GlobalA = 0.0;

        for (i = 1; i <= m_nVars; i++)
        {
            m_GlobalA((int)m_gdof(i)) = m_a(i);
            if (fabs(m_a(i)) > m_maxNodeValue)
                m_maxNodeValue = fabs(m_a(i));
        }

        //
        // Store displacements in nodes
        //

        double nodeValue;

        nodeSet->clearNodeValues();

        for (i = 0; i < nodeSet->getSize(); i++)
        { 
            Node* node = nodeSet->getNode(i);
            node->setValueSize(3);
            for (j = 0; j < 3; j++)
            {
                if (node->getDof(j) != nullptr)
                {
                    nodeValue = m_GlobalA(node->getDof(j)->getNumber());
                    node->setValue(j, nodeValue);
                }
            }
        }
    }
}

void FrameSolver::update()
{
    Logger::instance()->log(LogLevel::Info, "Updating results.");

    //
    // Retrieve fem model
    //

    BeamModel* femModel = m_beamModel;

    if (femModel == NULL)
    {
        Logger::instance()->log(LogLevel::Error, "Invalid model.");
        m_errorStatus = BS_INVALID_MODEL;
        return;
    }

    //
    // Retrieve individual parts of fem model
    //

    ElementSet* elementSet = femModel->getElementSet();
    /*
    CFemNodeSet* nodeSet = femModel->getNodeSet();
    CFemMaterialSet* materialSet = femModel->getMaterialSet();
    CFemNodeBCSet* bcSet = femModel->getNodeBCSet();
    CFemNodeLoadSet* nodeLoadSet = femModel->getNodeLoadSet();
    */
    ElementLoadSet* elementLoadSet = femModel->getElementLoadSet();

    RowVector Ex(2);
    RowVector Ey(2);
    RowVector Ez(2);
    Matrix Eq(static_cast<int>(elementSet->getSize()), 4);
    Eq = 0.0;
    RowVector Eo(3);
    RowVector Ep(6);
    Ep = 0.0;
    RowVector DofTopo(12);
    RowVector DofTopo_b(6);
    int i, j, k;
    double E, G, A, Iy, Iz, Kv;

    //
    // Element loads
    //

    for (i = 0; i < elementLoadSet->getSize(); i++)
    {
        double vx, vy, vz;
        double value;

        BeamLoad* elementLoad = (BeamLoad*)elementLoadSet->getLoad(i);

        elementLoad->getLocalDirection(vx, vy, vz);
        value = -elementLoad->getValue();

        for (j = 0; j < elementLoad->getElementsSize(); j++)
        {
            Element* element = elementLoad->getElement(j);

            Eq(element->getNumber(), 1) = Eq(element->getNumber(), 1) + vx * value;
            Eq(element->getNumber(), 2) = Eq(element->getNumber(), 2) + vy * value;
            Eq(element->getNumber(), 3) = Eq(element->getNumber(), 3) + vz * value;
            Eq(element->getNumber(), 4) = 0.0;
        }
    }

    //
    // Store element forces in elements
    //

    int n;
    RowVector Ed(12);
    RowVector Ed_b(6);
    Matrix Es;
    Matrix Edi;
    ColumnVector Eci;

    double N, T, Vy, Vz, My, Mz, Navier;

    initMaxMin();

    /*
    for (i = 1; i <= elementSet->getSize(); i++)
    {

        double x1, y1, z1;
        double x2, y2, z2;
        double ex, ey, ez;

        Beam* beam = (Beam*)elementSet->getElement(i - 1);
        n = beam->getEvaluationPoints();
        beam->setValueSize(n * 10);

        beam->getNode(0)->getCoord(x1, y1, z1);
        beam->getNode(1)->getCoord(x2, y2, z2);

        Ex(1) = x1;
        Ey(1) = y1;
        Ez(1) = z1;
        Ex(2) = x2;
        Ey(2) = y2;
        Ez(2) = z2;

        beam->getOrientationZ(ex, ey, ez);
        Eo(1) = ex;
        Eo(2) = ey;
        Eo(3) = ez;

        beam->getMaterial()->getProperties(E, G, A, Iy, Iz, Kv);
        Ep(1) = E;
        Ep(2) = G;
        Ep(3) = A;
        Ep(4) = Iy;
        Ep(5) = Iz;
        Ep(6) = Kv;

        for (j = 0; j < 6; j++)
        {
            DofTopo(j + 1) = beam->getNode(0)->getDof(j)->getNumber();
            DofTopo(j + 7) = beam->getNode(1)->getDof(j)->getNumber();
            Ed(j + 1) = m_GlobalA((int)DofTopo(j + 1));
            Ed(j + 7) = m_GlobalA((int)DofTopo(j + 7));
        }

        RowVector RowEq(4);
        RowEq = Eq.Row(i);
        beam3s(Ex, Ey, Ez, Eo, Ep, Ed, RowEq, n, Es, Edi, Eci);

        int pos = 0;

        for (k = 1; k <= n; k++)
        {
            N = Es(k, 1);
            Vy = Es(k, 2);
            Vz = Es(k, 3);
            T = Es(k, 4);
            My = Es(k, 5);
            Mz = Es(k, 6);

            Navier = calcNavier(N, My, Mz, beam);
            updateMaxMin(N, T, Vy, Vz, My, Mz, Navier);

            for (j = 1; j <= 6; j++)
                beam->setValue(pos++, Es(k, j));
        }

        for (k = 1; k <= n; k++)
            for (j = 1; j <= 4; j++)
                beam->setValue(pos++, Edi(k, j));
    }
    */
    for (i = 1; i <= elementSet->getSize(); i++)
    {

        double x1, y1, z1;
        double x2, y2, z2;
        double ex, ey, ez;

        Beam* beam = (Beam*)elementSet->getElement(i - 1);
        n = beam->getEvaluationPoints();
        beam->setValueSize(n * 11);

        beam->getNode(0)->getCoord(x1, y1, z1);
        beam->getNode(1)->getCoord(x2, y2, z2);

        Ex(1) = x1;
        Ey(1) = y1;
        Ez(1) = z1;
        Ex(2) = x2;
        Ey(2) = y2;
        Ez(2) = z2;

        beam->getOrientationZ(ex, ey, ez);
        Eo(1) = ex;
        Eo(2) = ey;
        Eo(3) = ez;

        beam->getMaterial()->getProperties(E, G, A, Iy, Iz, Kv);
        Ep(1) = E;
        Ep(2) = G;
        Ep(3) = A;
        Ep(4) = Iy;
        Ep(5) = Iz;
        Ep(6) = Kv;

        if (beam->beamType() == btBeam)
        {
            for (j = 0; j < 6; j++)
            {
                DofTopo(j + 1) = beam->getNode(0)->getDof(j)->getNumber();
                DofTopo(j + 7) = beam->getNode(1)->getDof(j)->getNumber();
                Ed(j + 1) = m_GlobalA((int)DofTopo(j + 1));
                Ed(j + 7) = m_GlobalA((int)DofTopo(j + 7));
            }

            RowVector RowEq(4);
            RowEq = Eq.Row(i);
            beam3s(Ex, Ey, Ez, Eo, Ep, Ed, RowEq, n, Es, Edi, Eci);

            int pos = 0;

            for (k = 1; k <= n; k++)
            {
                N = Es(k, 1);
                T = Es(k, 2);
                Vy = Es(k, 3);
                Vz = Es(k, 4);
                My = Es(k, 5);
                Mz = Es(k, 6);

                Navier = calcNavier(N, My, Mz, beam);
                updateMaxMin(N, T, Vy, Vz, My, Mz, Navier);

                for (j = 1; j <= 6; j++)
                    beam->setValue(pos++, Es(k, j));
            }

            for (k = 1; k <= n; k++)
                for (j = 1; j <= 4; j++)
                    beam->setValue(pos++, Edi(k, j));
        }
        else
        {
            for (j = 0; j < 3; j++)
            {
                DofTopo_b(j + 1) = beam->getNode(0)->getDof(j)->getNumber();
                DofTopo_b(j + 4) = beam->getNode(1)->getDof(j)->getNumber();
                Ed_b(j + 1) = m_GlobalA((int)DofTopo_b(j + 1));
                Ed_b(j + 4) = m_GlobalA((int)DofTopo_b(j + 4));
            }

            double eq = 0.0;

            ColumnVector Es_b(n);
            ColumnVector Edi_b(n);
            ColumnVector Eci_b(n);

            Ep(1) = E;
            Ep(2) = A;

            bar3s(Ex, Ey, Ez, Ep, Ed_b, eq, n, Es_b, Edi_b, Eci_b);

            int pos = 0;

            for (k = 1; k <= n; k++)
            {
                N = Es_b(k);
                T = 0.0;
                Vy = 0.0;
                Vz = 0.0;
                My = 0.0;
                Mz = 0.0;

                Navier = 0.0;
                updateMaxMin(N, T, Vy, Vz, My, Mz, Navier);

                for (j = 1; j <= 6; j++)
                    if (j == 1)
                        beam->setValue(pos++, Es_b(k));
                    else
                        beam->setValue(pos++, 0.0);
            }

            for (k = 1; k <= n; k++)
                for (j = 1; j <= 4; j++)
                {
                    beam->setValue(pos++, 0.0);
                }
        }
    }

    printMaxMin();
}

void FrameSolver::setFeedbackForce(Node* node, double fx, double fy, double fz)
{
    m_forceNode = node;
    m_force[0] = fx;
    m_force[1] = fy;
    m_force[2] = fz;
}

int FrameSolver::getLastError()
{
    return m_errorStatus;
}

void FrameSolver::updateMaxMin(double N, double T, double Vy, double Vz, double My, double Mz, double Navier)
{
    double V, M;

    V = sqrt(pow(Vy, 2) + pow(Vz, 2));
    M = sqrt(pow(My, 2) + pow(Mz, 2));

    if (N > m_maxN)
        m_maxN = N;

    if (N < m_minN)
        m_minN = N;

    if (fabs(T) > m_maxT)
        m_maxT = fabs(T);

    if (fabs(T) < m_minT)
        m_minT = fabs(T);

    if (fabs(M) > m_maxM)
        m_maxM = fabs(M);

    if (fabs(M) < m_minM)
        m_minM = fabs(M);

    if (fabs(V) > m_maxV)
        m_maxV = fabs(V);

    if (fabs(V) < m_minV)
        m_minV = fabs(V);

    if (fabs(Navier) > m_maxNavier)
        m_maxNavier = fabs(Navier);

    if (fabs(Navier) < m_minNavier)
        m_minNavier = fabs(Navier);

    if (m_resultInfo != NULL)
    {
        m_resultInfo->setMaxN(m_maxN);
        m_resultInfo->setMaxT(m_maxT);
        m_resultInfo->setMaxV(m_maxV);
        m_resultInfo->setMaxM(m_maxM);
        m_resultInfo->setMinN(m_minN);
        m_resultInfo->setMinT(m_minT);
        m_resultInfo->setMinV(m_minV);
        m_resultInfo->setMinM(m_minM);
        m_resultInfo->setMaxNavier(m_maxNavier);
        m_resultInfo->setMinNavier(m_minNavier);
    }
}

void FrameSolver::initMaxMin()
{
    m_maxN = -1.0e300;
    m_minN = 1.0e300;
    m_maxT = -1.0e300;
    m_minT = 1.0e300;
    m_maxM = -1.0e300;
    m_minM = 1.0e300;
    m_maxV = -1.0e300;
    m_minV = 1.0e300;
    m_maxNavier = -1.0e300;
    m_minNavier = 1.0e300;

    if (m_resultInfo != NULL)
    {
        m_resultInfo->setMaxN(m_maxN);
        m_resultInfo->setMaxT(m_maxT);
        m_resultInfo->setMaxV(m_maxV);
        m_resultInfo->setMaxM(m_maxM);
        m_resultInfo->setMinN(m_minN);
        m_resultInfo->setMinT(m_minT);
        m_resultInfo->setMinV(m_minV);
        m_resultInfo->setMinM(m_minM);
        m_resultInfo->setMaxNavier(m_maxNavier);
        m_resultInfo->setMinNavier(m_minNavier);
    }
}

void FrameSolver::printMaxMin()
{
#ifdef DEBUG_OUTPUT
    cout << "Nmax,min = " << m_maxN << ", " << m_minN << endl;
    cout << "Tmax,min = " << m_maxT << ", " << m_minT << endl;
    cout << "Vmax,min = " << m_maxV << ", " << m_minV << endl;
    cout << "Mmax,min = " << m_maxM << ", " << m_minM << endl;
    cout << "Navier,max,min = " << m_maxNavier << ", " << m_minNavier << endl;
#endif
}

void FrameSolver::setResultInfo(ResultInfo* resultInfo)
{
    m_resultInfo = resultInfo;
}

double FrameSolver::calcNavier(double N, double My, double Mz, Beam* beam)
{
    double E, G, A, Iy, Iz, Kv;
    double eyMax, eyMin, ezMax, ezMin;
    double sig[4];
    double sigN;
    double maxSig;
    int i;

    beam->getMaterial()->getProperties(E, G, A, Iy, Iz, Kv);
    beam->getMaterial()->getSection()->getExcY(eyMax, eyMin);
    beam->getMaterial()->getSection()->getExcZ(ezMax, ezMin);

    sigN = N / A;

    for (i = 0; i < 4; i++)
        sig[i] = sigN;

    sig[0] += Mz * ezMax / Iz;
    sig[1] += Mz * ezMax / Iz;
    sig[2] -= Mz * ezMin / Iz;
    sig[3] -= Mz * ezMin / Iz;

    sig[0] += My * eyMax / Iy;
    sig[1] -= My * eyMin / Iy;
    sig[2] += My * eyMax / Iy;
    sig[3] -= My * eyMax / Iy;

    maxSig = -1.0e300;

    for (i = 0; i < 4; i++)
        if (fabs(sig[i]) > maxSig)
            maxSig = fabs(sig[i]);

    return maxSig;
}
