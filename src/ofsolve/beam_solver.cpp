#include <ofsolve/beam_solver.h>

#include <ofem/beam_load.h>

#include <logger.h>

#include <sstream>

#include <Eigen/Eigenvalues>

using namespace ofem;
using namespace calfem;
using namespace ofsolver;

BeamSolver::BeamSolver()
    : m_beamModel{nullptr}, m_maxNodeValue{-1.0e300}, m_forceNode{nullptr}, m_modelState{ModelState::Ok},
      m_maxN{-1e300}, m_minN{1e300}, m_maxT{-1e300}, m_minT{1e300}, m_maxM{-1e300}, m_minM{1e300}, m_maxV{-1e300},
      m_minV{1e300}, m_maxNavier{-1e300}, m_minNavier{1e300}, m_force{0.0, 0.0, 0.0}, m_nDof{0},
      m_hasEigenModes{false}, m_numEigenModes{0}
{}

BeamSolver::~BeamSolver()
{}

void BeamSolver::setBeamModel(ofem::BeamModel *model)
{
    m_beamModel = model;
}

void BeamSolver::execute()
{
    double E, G, A, Iy, Iz, Kv;
    int i, j, k;

    //
    // Reset error status
    //

    m_modelState = ModelState::Ok;

    Logger::instance()->log(LogLevel::Error, "\nSimple 3D beam solver");
    Logger::instance()->log(LogLevel::Error, "---------------------\n");

    //
    // Retrieve fem model
    //

    BeamModel *femModel = m_beamModel;

    if (femModel == NULL)
    {
        Logger::instance()->log(LogLevel::Error, "Invalid model.");
        m_modelState = ModelState::Invalid;
        return;
    }

    //
    // Retrieve individual parts of fem model
    //

    BeamSet *elementSet = static_cast<BeamSet *>(femModel->getElementSet());
    NodeSet *nodeSet = femModel->getNodeSet();
    MaterialSet *materialSet = femModel->getMaterialSet();
    NodeBCSet *bcSet = femModel->getNodeBCSet();
    NodeLoadSet *nodeLoadSet = femModel->getNodeLoadSet();
    ElementLoadSet *elementLoadSet = femModel->getElementLoadSet();

    //
    // Check if we have a valid model
    //

    if (nodeSet->getSize() == 0)
    {
        Logger::instance()->log(LogLevel::Error, "No nodes defined.");
        m_modelState = ModelState::NoNodes;
        return;
    }

    if (elementSet->getSize() == 0)
    {
        Logger::instance()->log(LogLevel::Error, "No elements defined.");
        m_modelState = ModelState::NoElements;
        return;
    }

    if (bcSet->getSize() == 0)
    {
        Logger::instance()->log(LogLevel::Error, "No boundary conditions defined.");
        m_modelState = ModelState::NoBC;
        return;
    }

    if ((nodeLoadSet->getSize() == 0) && (elementLoadSet->getSize() == 0) && (m_forceNode == NULL))
    {
        Logger::instance()->log(LogLevel::Error, "No node loads defined.");
        m_modelState = ModelState::NoLoads;
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

    RowVec Ex(2);
    RowVec Ey(2);
    RowVec Ez(2);
    Matrix Eq(static_cast<int>(elementSet->getSize()), 4);
    Eq.setZero();
    RowVec Eo(3);
    RowVec Ep(6);
    Ep.setZero();
    Matrix Ke(12, 12);
    Matrix Ke_b(6, 6);
    ColVec fe(12);
    ColVec fe_b(6);
    m_f.resize(m_nDof, 1);
    m_f.setZero();
    IntRowVec DofTopo(12);
    IntRowVec DofTopo_b(6);

    //
    // Element loads
    //

    Logger::instance()->log(LogLevel::Info, "Setting up element loads.");

    for (i = 0; i < elementLoadSet->getSize(); i++)
    {
        double vx, vy, vz;
        double value;

        BeamLoad *elementLoad = (BeamLoad *)elementLoadSet->getLoad(i);

        elementLoad->getLocalDirection(vx, vy, vz);
        value = -elementLoad->getValue();

        for (j = 0; j < elementLoad->getElementsSize(); j++)
        {
            Element *element = elementLoad->getElement(j);

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

    m_Ks.resize(m_nDof, m_nDof);
    m_Ks.setZero();

    calfem::TripletList Ktriplets;
    Ktriplets.reserve(elementSet->getSize() * 144);

    Logger::instance()->log(LogLevel::Info, "Assembling system matrix.");

    for (i = 1; i <= elementSet->getSize(); i++)
    {
        double x1, y1, z1;
        double x2, y2, z2;
        double ex, ey, ez;

        Beam *beam = (Beam *)elementSet->getElement(i - 1);

        beam->getNode(0)->getCoord(x1, y1, z1);
        beam->getNode(1)->getCoord(x2, y2, z2);

        Ex(0) = x1;
        Ey(0) = y1;
        Ez(0) = z1;
        Ex(1) = x2;
        Ey(1) = y2;
        Ez(1) = z2;

        beam->getOrientationZ(ex, ey, ez);
        Eo(0) = ex;
        Eo(1) = ey;
        Eo(2) = ez;

        if (beam->getMaterial() != NULL)
        {
            beam->getMaterial()->getProperties(E, G, A, Iy, Iz, Kv);
            Ep(0) = E;
            Ep(1) = G;
            Ep(2) = A;
            Ep(3) = Iy;
            Ep(4) = Iz;
            Ep(5) = Kv;

            if (beam->beamType() == ofem::btBeam)
            {
                for (j = 0; j < 6; j++)
                    DofTopo(j) = beam->getNode(0)->getDof(j)->getNumber();

                for (j = 0; j < 6; j++)
                    DofTopo(j + 6) = beam->getNode(1)->getDof(j)->getNumber();

                RowVec RowEq(4);
                RowEq = Eq.row(i - 1);

                beam3e(Ex, Ey, Ez, Eo, Ep, RowEq, Ke, fe);
                spassem(DofTopo, Ktriplets, Ke, m_f, fe);
                // assem(DofTopo, Kf, Ke, m_f, fe);
            }
            else
            {
                Ep(0) = E;
                Ep(1) = A;

                for (j = 0; j < 3; j++)
                    DofTopo_b(j) = beam->getNode(0)->getDof(j)->getNumber();

                for (j = 0; j < 3; j++)
                    DofTopo_b(j + 3) = beam->getNode(1)->getDof(j)->getNumber();

                double eq = 0.0;

                bar3e(Ex, Ey, Ez, Ep, eq, Ke_b, fe_b);
                spassem(DofTopo_b, Ktriplets, Ke_b, m_f, fe_b);
            }
        }
        else
        {
            Logger::instance()->log(LogLevel::Error, "Element with undefined material.");
            m_modelState = ModelState::UndefinedMaterial;
        }

        if (m_modelState != ModelState::Ok)
            break;
    }

    if (m_modelState != ModelState::Ok)
        return;

    //
    // Node loads
    //

    Logger::instance()->log(LogLevel::Info, "Defining load vector.");

    for (i = 0; i < nodeLoadSet->getSize(); i++)
    {
        double vx, vy, vz;
        double value;
        double scale;

        BeamNodeLoad *nodeLoad = (BeamNodeLoad *)nodeLoadSet->getLoad(i);

        nodeLoad->getDirection(vx, vy, vz);
        value = nodeLoad->getValue();
        scale = nodeLoad->getScale();

        for (j = 0; j < (int)nodeLoad->getNodeSize(); j++)
        {
            Node *node = nodeLoad->getNode(j);
            m_f(node->getDof(0)->getNumber() - 1) += vx * value * scale;
            m_f(node->getDof(1)->getNumber() - 1) += vy * value * scale;
            m_f(node->getDof(2)->getNumber() - 1) += vz * value * scale;
        }
    }

    //
    // Boundary conditions
    //

    Logger::instance()->log(LogLevel::Info, "Setting up boundary conditions.");

    Matrix Bc(m_nDof, 2);
    Bc.setZero();

    std::map<int, double> bcMap;

    int bcCount = 0;

    for (i = 0; i < bcSet->getSize(); i++)
    {
        BeamNodeBC *nodeBC = (BeamNodeBC *)bcSet->getBC(i);

        for (j = 0; j < nodeBC->getNodeSize(); j++)
        {
            Node *node = nodeBC->getNode(j);

            if (node->getKind() != nkNotConnected)
            {
                for (k = 0; k < 6; k++)
                {
                    if (nodeBC->isPrescribed(k + 1))
                    {
                        if (node->getDof(k) != nullptr)
                        {
                            bcCount++;
                            bcMap[node->getDof(k)->getNumber()] = nodeBC->getPrescribedValue(k);
                        }
                    }
                }
            }
        }
    }

    m_bcDofs.resize(bcMap.size());
    m_bcDofs.setZero();
    m_bcVals.resize(bcMap.size());
    m_bcVals.setZero();

    int idx = 0;

    for (auto &kv : bcMap)
    {
        m_bcDofs(idx) = kv.first;
        m_bcVals(idx) = kv.second;
        idx++;
    }

    ColVec f = m_f;

    if (m_forceNode != nullptr)
    {
        int ldof1 = m_forceNode->getDof(0)->getNumber();
        int ldof2 = m_forceNode->getDof(1)->getNumber();
        int ldof3 = m_forceNode->getDof(2)->getNumber();
        if ((ldof1 > 0) && (ldof2 > 0) && (ldof3 > 0))
        {
            f(ldof1 - 1) += m_force[0];
            f(ldof2 - 1) += m_force[1];
            f(ldof3 - 1) += m_force[2];
        }
        else
            Logger::instance()->log(LogLevel::Error, "Somethings wrong...");
    }

    if (f.isZero())
    {
        Logger::instance()->log(LogLevel::Error, "No effective loads applied.");
        m_modelState = ModelState::NoLoads;
        return;
    }

    //
    // Create global displacement vector
    //

    Logger::instance()->log(LogLevel::Info, "Solving system. ");

    m_globalA.resize(m_nDof, 1);
    m_globalA.setZero();

    m_Ks.setFromTriplets(Ktriplets.begin(), Ktriplets.end());

    Logger::instance()->log(LogLevel::Info, "Calling solveq...");

    if (!m_sparseSolver.setup(m_Ks, m_bcDofs, m_bcVals))
    {
        Logger::instance()->log(LogLevel::Error, "Solver setup failed...");
        m_modelState = ModelState::SetupFailed;
        return;
    }

    if (!m_sparseSolver.solve(f, m_globalA, m_globalQ))
    {
        Logger::instance()->log(LogLevel::Error, "Solve failed...");
        m_modelState = ModelState::SolveFailed;
        return;
    }

    Logger::instance()->log(LogLevel::Info, "solveq done...");

    m_maxNodeValue = m_globalA.maxCoeff();

    //
    // Store displacements in nodes
    //

    Logger::instance()->log(LogLevel::Info, "Storing results in nodes and elements.");

    nodeSet->clearNodeValues();

    double nodeValue;

    m_maxReactionForce = 0.0;
    m_maxReactionMoment = 0.0;

    for (i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);

        if (node->getKind() != nkNotConnected)
        {
            if (node->getKind() == nk6Dof)
            {
                node->setValueSize(12);
                for (j = 0; j < 6; j++)
                {
                    nodeValue = m_globalA(node->getDof(j)->getNumber() - 1);
                    node->setValue(j, nodeValue);
                }

                // Store reaction forces in nodes

                for (j = 0; j < 6; j++)
                {
                    nodeValue = m_globalQ(node->getDof(j)->getNumber() - 1);
                    node->setValue(j + 6, nodeValue);

                    if (j < 3)
                    {
                        if (abs(nodeValue) > m_maxReactionForce)
                            m_maxReactionForce = abs(nodeValue);
                    }
                    else
                    {
                        if (abs(nodeValue) > m_maxReactionMoment)
                            m_maxReactionMoment = abs(nodeValue);
                    }
                }
            }
            else
            {
                node->setValueSize(6);

                for (j = 0; j < 3; j++)
                {
                    nodeValue = m_globalA(node->getDof(j)->getNumber() - 1);
                    node->setValue(j, nodeValue);
                }

                // Store reaction forces in nodes

                for (j = 0; j < 3; j++)
                {
                    nodeValue = m_globalQ(node->getDof(j)->getNumber() - 1);
                    node->setValue(j + 3, nodeValue);

                    if (abs(nodeValue) > m_maxReactionForce)
                        m_maxReactionForce = abs(nodeValue);
                }
            }
        }
    }
    //
    // Store element forces in elements
    //

    int n;
    RowVec Ed(12);
    RowVec Ed_b(6);
    Matrix Es;
    Matrix Edi;
    ColVec Eci;

    double N, T, Vy, Vz, My, Mz, Navier;

    initMaxMin();

    for (i = 1; i <= elementSet->getSize(); i++)
    {

        double x1, y1, z1;
        double x2, y2, z2;
        double ex, ey, ez;

        Beam *beam = (Beam *)elementSet->getElement(i - 1);
        n = beam->getEvaluationPoints();
        beam->setValueSize(n * 11);

        beam->getNode(0)->getCoord(x1, y1, z1);
        beam->getNode(1)->getCoord(x2, y2, z2);

        Ex(0) = x1;
        Ey(0) = y1;
        Ez(0) = z1;
        Ex(1) = x2;
        Ey(1) = y2;
        Ez(1) = z2;

        beam->getOrientationZ(ex, ey, ez);
        Eo(0) = ex;
        Eo(1) = ey;
        Eo(2) = ez;

        beam->getMaterial()->getProperties(E, G, A, Iy, Iz, Kv);
        Ep(0) = E;
        Ep(1) = G;
        Ep(2) = A;
        Ep(3) = Iy;
        Ep(4) = Iz;
        Ep(5) = Kv;

        if (beam->beamType() == btBeam)
        {
            for (j = 0; j < 6; j++)
            {
                DofTopo(j) = beam->getNode(0)->getDof(j)->getNumber();
                DofTopo(j + 6) = beam->getNode(1)->getDof(j)->getNumber();
                Ed(j) = m_globalA(DofTopo(j) - 1);
                Ed(j + 6) = m_globalA(DofTopo(j + 6) - 1);
            }

            RowVec RowEq(4);
            RowEq = Eq.row(i - 1);
            beam3s(Ex, Ey, Ez, Eo, Ep, Ed, RowEq, n, Es, Edi, Eci);

            int pos = 0;

            for (k = 0; k < n; k++)
            {
                N = Es(k, 0);
                T = Es(k, 1);
                Vy = Es(k, 2);
                Vz = Es(k, 3);
                My = Es(k, 4);
                Mz = Es(k, 5);

                Navier = calcNavier(N, My, Mz, beam);
                updateMaxMin(N, T, Vy, Vz, My, Mz, Navier);

                for (j = 0; j < 6; j++)
                    beam->setValue(pos++, Es(k, j));
            }

            for (k = 0; k < n; k++)
                for (j = 0; j < 4; j++)
                    beam->setValue(pos++, Edi(k, j));
        }
        else
        {
            for (j = 0; j < 3; j++)
            {
                DofTopo_b(j) = beam->getNode(0)->getDof(j)->getNumber();
                DofTopo_b(j + 3) = beam->getNode(1)->getDof(j)->getNumber();
                Ed_b(j) = m_globalA(DofTopo_b(j) - 1);
                Ed_b(j + 3) = m_globalA(DofTopo_b(j + 3) - 1);
            }

            double eq = 0.0;

            ColVec Es_b(n);
            ColVec Edi_b(n);
            ColVec Eci_b(n);

            Ep(0) = E;
            Ep(1) = A;

            bar3s(Ex, Ey, Ez, Ep, Ed_b, eq, n, Es_b, Edi_b, Eci_b);

            int pos = 0;

            for (k = 0; k < n; k++)
            {
                N = Es_b(k);
                T = 0.0;
                Vy = 0.0;
                Vz = 0.0;
                My = 0.0;
                Mz = 0.0;

                Navier = 0.0;
                updateMaxMin(N, T, Vy, Vz, My, Mz, Navier);

                for (j = 0; j < 6; j++)
                    if (j == 0)
                        beam->setValue(pos++, Es_b(k));
                    else
                        beam->setValue(pos++, 0.0);
            }

            for (k = 0; k < n; k++)
                for (j = 0; j < 4; j++)
                    beam->setValue(pos++, 0.0);
        }
    }

    printMaxMin();

    Logger::instance()->log(LogLevel::Info, "Solver completed.");
    
    // Optional: Perform stability check after solve
    // Uncomment to enable automatic eigenvalue analysis
    /*
    Logger::instance()->log(LogLevel::Info, "Performing stability check...");
    if (computeEigenModes(5))
    {
        double minEigenvalue = getEigenValue(0);
        if (minEigenvalue < -1e-6)
        {
            Logger::instance()->log(LogLevel::Warning, 
                "Structure is UNSTABLE. Check eigenmode visualization.");
        }
        else if (minEigenvalue < 1e-3)
        {
            Logger::instance()->log(LogLevel::Warning, 
                "Structure may be near instability (small positive eigenvalue).");
        }
    }
    */
}

double BeamSolver::getMaxNodeValue()
{
    return m_maxNodeValue;
}

double ofsolver::BeamSolver::getMaxReactionForce()
{
    return m_maxReactionForce;
}

double ofsolver::BeamSolver::getMaxReactionMoment()
{
    return m_maxReactionMoment;
}

void BeamSolver::recompute()
{
    if (this->modelState() == ModelState::Ok)
    {
        int i, j;

        BeamModel *femModel = m_beamModel;

        if (femModel == NULL)
        {
            Logger::instance()->log(LogLevel::Error, "Invalid model.");
            m_modelState = ModelState::Invalid;
            return;
        }

        //
        // Retrieve individual parts of fem model
        //

        NodeSet *nodeSet = femModel->getNodeSet();

        ColVec f = m_f;
        f.setZero();

        if (m_forceNode != nullptr)
        {
            int ldof1 = m_forceNode->getDof(0)->getNumber();
            int ldof2 = m_forceNode->getDof(1)->getNumber();
            int ldof3 = m_forceNode->getDof(2)->getNumber();
            if ((ldof1 > 0) && (ldof2 > 0) && (ldof3 > 0))
            {
                f(ldof1 - 1) += m_force[0];
                f(ldof2 - 1) += m_force[1];
                f(ldof3 - 1) += m_force[2];
            }
            else
                Logger::instance()->log(LogLevel::Error, "Somethings wrong...");
        }

        Logger::instance()->log(LogLevel::Info, "Defining load vector.");

        auto nodeLoadSet = femModel->getNodeLoadSet();

        for (i = 0; i < nodeLoadSet->getSize(); i++)
        {
            double vx, vy, vz;
            double value;
            double scale;

            BeamNodeLoad *nodeLoad = (BeamNodeLoad *)nodeLoadSet->getLoad(i);

            nodeLoad->getDirection(vx, vy, vz);
            value = nodeLoad->getValue();
            scale = nodeLoad->getScale();

            for (j = 0; j < (int)nodeLoad->getNodeSize(); j++)
            {
                Node *node = nodeLoad->getNode(j);
                f(node->getDof(0)->getNumber() - 1) += vx * value * scale;
                f(node->getDof(1)->getNumber() - 1) += vy * value * scale;
                f(node->getDof(2)->getNumber() - 1) += vz * value * scale;
            }
        }

        if (f.isZero())
        {
            Logger::instance()->log(LogLevel::Error, "No effective loads applied.");
        }

        //
        // Create global displacement vector
        //

        m_globalA.resize(m_nDof, 1);
        m_globalA.setZero();

        Logger::instance()->log(LogLevel::Info, "Calling solveq...");

        if (!m_sparseSolver.recompute(f, m_globalA, m_globalQ))
        {
            Logger::instance()->log(LogLevel::Error, "Recompute solve failed");
            m_modelState = ModelState::RecomputeFailed;
            return;
        }

        Logger::instance()->log(LogLevel::Info, "solveq done...");

        m_maxNodeValue = m_globalA.maxCoeff();

        //
        // Store displacements in nodes
        //

        Logger::instance()->log(LogLevel::Info, "Storing results in nodes and elements.");

        nodeSet->clearNodeValues();

        double nodeValue;

        m_maxReactionForce = 0.0;
        m_maxReactionMoment = 0.0;

        for (i = 0; i < nodeSet->getSize(); i++)
        {
            auto node = nodeSet->getNode(i);

            if (node->getKind() != nkNotConnected)
            {
                if (node->getKind() == nk6Dof)
                {
                    node->setValueSize(12);
                    for (j = 0; j < 6; j++)
                    {
                        nodeValue = m_globalA(node->getDof(j)->getNumber() - 1);
                        node->setValue(j, nodeValue);
                    }

                    // Store reaction forces in nodes

                    for (j = 0; j < 6; j++)
                    {
                        nodeValue = m_globalQ(node->getDof(j)->getNumber() - 1);
                        node->setValue(j + 6, nodeValue);

                        if (j < 3)
                        {
                            if (abs(nodeValue) > m_maxReactionForce)
                                m_maxReactionForce = abs(nodeValue);
                        }
                        else
                        {
                            if (abs(nodeValue) > m_maxReactionMoment)
                                m_maxReactionMoment = abs(nodeValue);
                        }
                    }
                }
                else
                {
                    node->setValueSize(6);

                    for (j = 0; j < 3; j++)
                    {
                        nodeValue = m_globalA(node->getDof(j)->getNumber() - 1);
                        node->setValue(j, nodeValue);
                    }

                    // Store reaction forces in nodes

                    for (j = 0; j < 3; j++)
                    {
                        nodeValue = m_globalQ(node->getDof(j)->getNumber() - 1);
                        node->setValue(j + 3, nodeValue);

                        for (j = 0; j < 3; j++)
                        {
                            nodeValue = m_globalQ(node->getDof(j)->getNumber() - 1);
                            node->setValue(j + 3, nodeValue);

                            if (abs(nodeValue) > m_maxReactionForce)
                                m_maxReactionForce = abs(nodeValue);
                        }
                    }
                }
            }
        }
    }
}

void BeamSolver::update()
{
    //
    // Retrieve fem model
    //

    auto femModel = m_beamModel;

    if (femModel == nullptr)
    {
        Logger::instance()->log(LogLevel::Error, "Invalid model.");
        m_modelState = ModelState::Invalid;
        return;
    }

    //
    // Retrieve individual parts of fem model
    //

    auto elementSet = femModel->getElementSet();
    auto elementLoadSet = femModel->getElementLoadSet();

    //
    // Store element forces in elements
    //

    int n;
    int i, j, k;
    double N, T, Vy, Vz, My, Mz, Navier;
    double E, G, A, Iy, Iz, Kv;

    RowVec Ex(2);
    RowVec Ey(2);
    RowVec Ez(2);
    Matrix Eq(static_cast<int>(elementSet->getSize()), 4);
    Eq.setZero();
    RowVec Eo(3);
    RowVec Ep(6);
    Ep.setZero();
    IntRowVec DofTopo(12);
    IntRowVec DofTopo_b(6);

    RowVec Ed(12);
    RowVec Ed_b(6);
    Matrix Es;
    Matrix Edi;
    ColVec Eci;

    initMaxMin();

    for (i = 1; i <= elementSet->getSize(); i++)
    {

        double x1, y1, z1;
        double x2, y2, z2;
        double ex, ey, ez;

        Beam *beam = (Beam *)elementSet->getElement(i - 1);
        n = beam->getEvaluationPoints();
        beam->setValueSize(n * 11);

        beam->getNode(0)->getCoord(x1, y1, z1);
        beam->getNode(1)->getCoord(x2, y2, z2);

        Ex(0) = x1;
        Ey(0) = y1;
        Ez(0) = z1;
        Ex(1) = x2;
        Ey(1) = y2;
        Ez(1) = z2;

        beam->getOrientationZ(ex, ey, ez);
        Eo(0) = ex;
        Eo(1) = ey;
        Eo(2) = ez;

        beam->getMaterial()->getProperties(E, G, A, Iy, Iz, Kv);
        Ep(0) = E;
        Ep(1) = G;
        Ep(2) = A;
        Ep(3) = Iy;
        Ep(4) = Iz;
        Ep(5) = Kv;

        if (beam->beamType() == btBeam)
        {
            for (j = 0; j < 6; j++)
            {
                DofTopo(j) = beam->getNode(0)->getDof(j)->getNumber();
                DofTopo(j + 6) = beam->getNode(1)->getDof(j)->getNumber();
                Ed(j) = m_globalA(DofTopo(j) - 1);
                Ed(j + 6) = m_globalA(DofTopo(j + 6) - 1);
            }

            RowVec RowEq(4);
            RowEq = Eq.row(i - 1);
            beam3s(Ex, Ey, Ez, Eo, Ep, Ed, RowEq, n, Es, Edi, Eci);

            int pos = 0;

            for (k = 0; k < n; k++)
            {
                N = Es(k, 0);
                T = Es(k, 1);
                Vy = Es(k, 2);
                Vz = Es(k, 3);
                My = Es(k, 4);
                Mz = Es(k, 5);

                Navier = calcNavier(N, My, Mz, beam);
                updateMaxMin(N, T, Vy, Vz, My, Mz, Navier);

                for (j = 0; j < 6; j++)
                    beam->setValue(pos++, Es(k, j));
            }

            for (k = 0; k < n; k++)
                for (j = 0; j < 4; j++)
                    beam->setValue(pos++, Edi(k, j));
        }
        else
        {
            for (j = 0; j < 3; j++)
            {
                DofTopo_b(j) = beam->getNode(0)->getDof(j)->getNumber();
                DofTopo_b(j + 3) = beam->getNode(1)->getDof(j)->getNumber();
                Ed_b(j) = m_globalA(DofTopo_b(j) - 1);
                Ed_b(j + 3) = m_globalA(DofTopo_b(j + 3) - 1);
            }

            double eq = 0.0;

            ColVec Es_b(n);
            ColVec Edi_b(n);
            ColVec Eci_b(n);

            Ep(0) = E;
            Ep(1) = A;

            bar3s(Ex, Ey, Ez, Ep, Ed_b, eq, n, Es_b, Edi_b, Eci_b);

            int pos = 0;

            for (k = 0; k < n; k++)
            {
                N = Es_b(k);
                T = 0.0;
                Vy = 0.0;
                Vz = 0.0;
                My = 0.0;
                Mz = 0.0;

                Navier = 0.0;
                updateMaxMin(N, T, Vy, Vz, My, Mz, Navier);

                for (j = 0; j < 6; j++)
                    if (j == 0)
                        beam->setValue(pos++, Es_b(k));
                    else
                        beam->setValue(pos++, 0.0);
            }

            for (k = 0; k < n; k++)
                for (j = 0; j < 4; j++)
                    beam->setValue(pos++, 0.0);
        }
    }

    printMaxMin();

    Logger::instance()->log(LogLevel::Info, "Update completed.");
}

void BeamSolver::setFeedbackForce(Node *node, double fx, double fy, double fz)
{
    m_forceNode = node;
    m_force[0] = fx;
    m_force[1] = fy;
    m_force[2] = fz;
}

void BeamSolver::updateMaxMin(double N, double T, double Vy, double Vz, double My, double Mz, double Navier)
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

    if (m_beamModel != nullptr)
    {
        m_beamModel->setMaxN(m_maxN);
        m_beamModel->setMaxT(m_maxT);
        m_beamModel->setMaxV(m_maxV);
        m_beamModel->setMaxM(m_maxM);
        m_beamModel->setMinN(m_minN);
        m_beamModel->setMinT(m_minT);
        m_beamModel->setMinV(m_minV);
        m_beamModel->setMinM(m_minM);
        m_beamModel->setMaxNavier(m_maxNavier);
        m_beamModel->setMinNavier(m_minNavier);
    }
}

void BeamSolver::initMaxMin()
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

    if (m_beamModel != nullptr)
    {
        m_beamModel->setMaxN(m_maxN);
        m_beamModel->setMaxT(m_maxT);
        m_beamModel->setMaxV(m_maxV);
        m_beamModel->setMaxM(m_maxM);
        m_beamModel->setMinN(m_minN);
        m_beamModel->setMinT(m_minT);
        m_beamModel->setMinV(m_minV);
        m_beamModel->setMinM(m_minM);
        m_beamModel->setMaxNavier(m_maxNavier);
        m_beamModel->setMinNavier(m_minNavier);
    }
}

void BeamSolver::printMaxMin()
{
#ifdef DEBUG_OUTPUT
    cout << "Nmax,min = " << m_maxN << ", " << m_minN << endl;
    cout << "Tmax,min = " << m_maxT << ", " << m_minT << endl;
    cout << "Vmax,min = " << m_maxV << ", " << m_minV << endl;
    cout << "Mmax,min = " << m_maxM << ", " << m_minM << endl;
    cout << "Navier,max,min = " << m_maxNavier << ", " << m_minNavier << endl;
#endif
}

ModelState BeamSolver::modelState()
{
    return m_modelState;
}

double BeamSolver::calcNavier(double N, double My, double Mz, Beam *beam)
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

// Eigenvalue analysis implementation

Eigen::MatrixXd BeamSolver::extractFreeStiffness()
{
    // Extract the free (unconstrained) portion of the stiffness matrix
    // This removes rows and columns corresponding to prescribed DOFs
    
    std::set<int> bcDofSet;
    for (int i = 0; i < m_bcDofs.size(); i++)
    {
        bcDofSet.insert(m_bcDofs(i) - 1); // Convert to 0-based indexing
    }
    
    // Count free DOFs
    int numFreeDofs = m_nDof - bcDofSet.size();
    
    // Create mapping from free DOF indices to original indices
    std::vector<int> freeDofs;
    freeDofs.reserve(numFreeDofs);
    
    for (int i = 0; i < m_nDof; i++)
    {
        if (bcDofSet.find(i) == bcDofSet.end())
        {
            freeDofs.push_back(i);
        }
    }
    
    // Extract the free portion of the stiffness matrix
    Eigen::MatrixXd Kfree(numFreeDofs, numFreeDofs);
    Kfree.setZero();
    
    for (int i = 0; i < numFreeDofs; i++)
    {
        for (int j = 0; j < numFreeDofs; j++)
        {
            int ri = freeDofs[i];
            int rj = freeDofs[j];
            // For symmetric matrices, access using proper ordering
            // Since stiffness matrix is typically stored upper/full, this ensures correct access
            if (ri <= rj)
                Kfree(i, j) = m_Ks.coeff(ri, rj);
            else
                Kfree(i, j) = m_Ks.coeff(rj, ri);
        }
    }
    
    return Kfree;
}

bool BeamSolver::computeEigenModes(int numModes)
{
    try
    {
        Logger::instance()->log(LogLevel::Info, "Computing eigenvalues and eigenvectors...");
        
        // Clear previous results
        clearEigenModes();
        
        // Check if system has been assembled
        if (m_Ks.rows() == 0 || m_Ks.cols() == 0)
        {
            Logger::instance()->log(LogLevel::Error, "Stiffness matrix not assembled. Run execute() first.");
            return false;
        }
        
        // Extract the free (unconstrained) stiffness matrix
        Eigen::MatrixXd Kfree = extractFreeStiffness();
        
        int matrixSize = Kfree.rows();
        
        if (matrixSize == 0)
        {
            Logger::instance()->log(LogLevel::Error, "No free degrees of freedom for eigenvalue analysis.");
            return false;
        }
        
        Logger::instance()->log(LogLevel::Info, 
            "Free stiffness matrix size: " + std::to_string(matrixSize) + "x" + std::to_string(matrixSize));
        
        // Check if matrix size is reasonable for dense solver
        if (matrixSize > 1000)
        {
            Logger::instance()->log(LogLevel::Warning, 
                "Large matrix (" + std::to_string(matrixSize) + " DOFs). Eigenvalue computation may be slow.");
        }
        
        // Compute eigenvalues and eigenvectors using self-adjoint solver
        // Note: Kfree should be symmetric for structural stiffness matrices
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(Kfree);
        
        if (solver.info() != Eigen::Success)
        {
            Logger::instance()->log(LogLevel::Error, "Eigenvalue computation failed.");
            return false;
        }
        
        // Get eigenvalues and eigenvectors
        Eigen::VectorXd eigenvalues = solver.eigenvalues();
        Eigen::MatrixXd eigenvectors = solver.eigenvectors();
        
        // Store the requested number of modes (sorted by eigenvalue magnitude)
        // Smallest eigenvalues indicate instability
        m_numEigenModes = std::min(numModes, static_cast<int>(eigenvalues.size()));
        
        // Check for negative eigenvalues (indicates instability)
        bool hasNegativeEigenvalues = false;
        double minEigenvalue = eigenvalues(0);
        const double EIGENVALUE_TOLERANCE = 1e-8;  // Stricter tolerance for numerical stability
        
        if (minEigenvalue < -EIGENVALUE_TOLERANCE)
        {
            hasNegativeEigenvalues = true;
            Logger::instance()->log(LogLevel::Warning, 
                "UNSTABLE STRUCTURE DETECTED: Negative eigenvalue = " + std::to_string(minEigenvalue));
        }
        else if (minEigenvalue < EIGENVALUE_TOLERANCE)
        {
            Logger::instance()->log(LogLevel::Warning, 
                "NEAR-SINGULAR STRUCTURE: Smallest eigenvalue = " + std::to_string(minEigenvalue));
        }
        
        // Store eigenvalues and eigenvectors
        for (int i = 0; i < m_numEigenModes; i++)
        {
            m_eigenValues.push_back(eigenvalues(i));
            
            // Expand eigenvector back to full DOF space
            Eigen::VectorXd fullEigenvector(m_nDof);
            fullEigenvector.setZero();
            
            // Map free DOFs back to original numbering
            std::set<int> bcDofSet;
            for (int j = 0; j < m_bcDofs.size(); j++)
            {
                bcDofSet.insert(m_bcDofs(j) - 1);
            }
            
            int freeIdx = 0;
            for (int j = 0; j < m_nDof; j++)
            {
                if (bcDofSet.find(j) == bcDofSet.end())
                {
                    fullEigenvector(j) = eigenvectors(freeIdx, i);
                    freeIdx++;
                }
            }
            
            m_eigenVectors.push_back(fullEigenvector);
            
            std::stringstream ss;
            ss << "Mode " << (i+1) << ": eigenvalue = " << eigenvalues(i);
            if (eigenvalues(i) < -EIGENVALUE_TOLERANCE)
                ss << " (UNSTABLE)";
            else if (eigenvalues(i) < EIGENVALUE_TOLERANCE)
                ss << " (NEAR-SINGULAR)";
            Logger::instance()->log(LogLevel::Info, ss.str());
        }
        
        m_hasEigenModes = true;
        
        Logger::instance()->log(LogLevel::Info, 
            "Successfully computed " + std::to_string(m_numEigenModes) + " eigen modes.");
        
        // Update model state if unstable
        if (hasNegativeEigenvalues)
        {
            m_modelState = ModelState::Unstable;
        }
        
        return true;
        
    }
    catch (const std::exception &e)
    {
        Logger::instance()->log(LogLevel::Error, 
            "Exception in eigenmode computation: " + std::string(e.what()));
        return false;
    }
}

void BeamSolver::clearEigenModes()
{
    m_eigenValues.clear();
    m_eigenVectors.clear();
    m_hasEigenModes = false;
    m_numEigenModes = 0;
}

bool BeamSolver::hasEigenModes() const
{
    return m_hasEigenModes;
}

int BeamSolver::getNumEigenModes() const
{
    return m_numEigenModes;
}

double BeamSolver::getEigenValue(int mode) const
{
    if (mode >= 0 && mode < m_numEigenModes)
    {
        return m_eigenValues[mode];
    }
    return 0.0;
}

void BeamSolver::getEigenVector(int mode, Eigen::VectorXd &eigenVector) const
{
    if (mode >= 0 && mode < m_numEigenModes)
    {
        eigenVector = m_eigenVectors[mode];
    }
    else
    {
        eigenVector = Eigen::VectorXd::Zero(m_nDof);
    }
}

