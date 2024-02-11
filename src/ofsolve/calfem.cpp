//
// CALFEM C++ libray
// Copyright (C) 1999-2023 Division of Structural Mecahnics, Lund University
//
// Written by Jonas Lindemann
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Comments and suggestions to jonas.lindemann@byggmek.lth.se

#include <ofsolve/calfem.h>

namespace calfem {

void bar3e(const RowVec &ex, const RowVec &ey, const RowVec &ez, const RowVec &ep, double eq, Matrix &Ke, ColVec &fe)
{
    double E = ep(0);
    double A = ep(1);

    ColVec b(3);
    RowVec n(3);
    Matrix G(2, 6);
    Matrix Kle(2, 2);
    ColVec fle(2);

    b << ex(1) - ex(0), ey(1) - ey(0), ez(1) - ez(0);

    double L = sqrt((b.transpose() * b));

    n = b.transpose() / L;

    G << n(0), n(1), n(2), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, n(0), n(1), n(2);

    double a = E * A / L;

    Kle << a, -a, -a, a;

    a = eq * L / 2.0;

    fle << a, a;

    Ke = G.transpose() * Kle * G;
    fe = G.transpose() * fle;
}

void bar3s(const RowVec &ex, const RowVec &ey, const RowVec &ez, const RowVec &ep, const RowVec &ed, double eq, int n,
           ColVec &es, ColVec &edi, ColVec &eci)
{
    double E = ep(0);
    double A = ep(1);

    ColVec b(3);
    RowVec nn(3);
    Matrix G(2, 6);
    Matrix Kle(2, 2);
    ColVec fle(2);

    b << ex(1) - ex(0), ey(1) - ey(0), ez(1) - ez(0);

    double L = sqrt((b.transpose() * b));

    nn = b.transpose() / L;

    G << nn(0), nn(1), nn(2), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, nn(0), nn(1), nn(2);

    double c1 = E * A / L;

    Kle << c1, -c1, -c1, c1;

    c1 = eq * L / 2.0;

    fle << c1, c1;

    ColVec ae = ed.transpose();
    ColVec ale = G * ae;

    RowVec B(2);

    B << -1.0 / L, 1.0 / L;

    for (int i = 1; i <= n; i++)
    {
        eci(i - 1) = (i - 1.0) * L / (n - 1.0);
        double x = eci(i - 1);
        double up = -eq * (0.5 * pow(x, 2) - 0.5 * L * x) / E / A;
        double Np = -eq * (x - 0.5 * L);

        RowVec N(2);
        N << 1.0 - x / L, x / L;

        edi(i - 1) = N * ale + up; // [1 x 2] * [2 x 1]
        es(i - 1) = E * A * B * ale + Np;
        eci(i - 1) = x;
    }
}

void beam3e(const RowVec &ex, const RowVec &ey, const RowVec &ez, const RowVec &eo, const RowVec &ep, const RowVec &eq,
            Matrix &Ke, ColVec &fe)
{
    ColVec bb(3);
    bb << ex(1) - ex(0), ey(1) - ey(0), ez(1) - ez(0);

    double L = sqrt((bb.transpose() * bb));

    RowVec n1 = (bb / L).transpose();

    double lc = sqrt((eo * eo.transpose()));
    RowVec n3 = eo / lc;

    double qx = eq(0);
    double qy = eq(1);
    double qz = eq(2);
    double qw = eq(3);

    double E = ep(0);
    double Gs = ep(1);
    double A = ep(2);
    double Iy = ep(3);
    double Iz = ep(4);
    double Kv = ep(5);

    double a = E * A / L;
    double b = 12.0 * E * Iz / pow(L, 3);
    double c = 6.0 * E * Iz / pow(L, 2);
    double d = 12.0 * E * Iy / pow(L, 3);
    double e = 6.0 * E * Iy / pow(L, 2);
    double f = Gs * Kv / L;
    double g = 2.0 * E * Iy / L;
    double h = 2 * E * Iz / L;

    Matrix Kle(12, 12);

    Kle << a, 0, 0, 0, 0, 0, -a, 0, 0, 0, 0, 0, 0, b, 0, 0, 0, c, 0, -b, 0, 0, 0, c, 0, 0, d, 0, -e, 0, 0, 0, -d, 0, -e,
        0, 0, 0, 0, f, 0, 0, 0, 0, 0, -f, 0, 0, 0, 0, -e, 0, 2 * g, 0, 0, 0, e, 0, g, 0, 0, c, 0, 0, 0, 2 * h, 0, -c, 0,
        0, 0, h, -a, 0, 0, 0, 0, 0, a, 0, 0, 0, 0, 0, 0, -b, 0, 0, 0, -c, 0, b, 0, 0, 0, -c, 0, 0, -d, 0, e, 0, 0, 0, d,
        0, e, 0, 0, 0, 0, -f, 0, 0, 0, 0, 0, f, 0, 0, 0, 0, -e, 0, g, 0, 0, 0, e, 0, 2 * g, 0, 0, c, 0, 0, 0, h, 0, -c,
        0, 0, 0, 2 * h;

    ColVec fle(12);

    fle << qx, qy, qz, qw, -qz * L / 6.0, qy * L / 6.0, qx, qy, qz, qw, qz * L / 6.0, -qy * L / 6.0;

    fle = fle * L / 2.0;

    RowVec n2(3);

    n2 << n3(1) * n1(2) - n3(2) * n1(1), -n1(2) * n3(0) + n1(0) * n3(2), n3(0) * n1(1) - n1(0) * n3(1);

    Matrix An(3, 3);

    An.row(0) = n1;
    An.row(1) = n2;
    An.row(2) = n3;

    Matrix G = Matrix::Zero(12, 12);

    G.block(0, 0, 3, 3) = An;
    G.block(3, 3, 3, 3) = An;
    G.block(6, 6, 3, 3) = An;
    G.block(9, 9, 3, 3) = An;

    Ke = G.transpose() * Kle * G;
    fe = G.transpose() * fle;
}

void beam3s(const RowVec &ex, const RowVec &ey, const RowVec &ez, const RowVec &eo, const RowVec &ep, const RowVec &ed,
            const RowVec &eq, const int n, Matrix &es, Matrix &edi, ColVec &eci)
{
    ColVec bb(3);
    bb << ex(1) - ex(0), ey(1) - ey(0), ez(1) - ez(0);

    double L = sqrt((bb.transpose() * bb));

    RowVec n1 = (bb / L).transpose();

    double lc = sqrt((eo * eo.transpose()));
    RowVec n3 = eo / lc;

    double qx = eq(0);
    double qy = eq(1);
    double qz = eq(2);
    double qw = eq(3);

    double E = ep(0);
    double Gs = ep(1);
    double A = ep(2);
    double Iy = ep(3);
    double Iz = ep(4);
    double Kv = ep(5);

    RowVec n2(3);

    n2 << n3(1) * n1(2) - n3(2) * n1(1), -n1(2) * n3(0) + n1(0) * n3(2), n3(0) * n1(1) - n1(0) * n3(1);

    Matrix An(3, 3);

    An.row(0) = n1;
    An.row(1) = n2;
    An.row(2) = n3;

    Matrix G = Matrix::Zero(12, 12);

    G.block(0, 0, 3, 3) = An;
    G.block(3, 3, 3, 3) = An;
    G.block(6, 6, 3, 3) = An;
    G.block(9, 9, 3, 3) = An;

    ColVec u(12);
    ColVec diffSol(12);

    diffSol << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -qx * pow(L, 2) / 2.0 / E / A, qy * pow(L, 4) / 24.0 / E / Iz,
        qz * pow(L, 4) / 24.0 / E / Iy, -qw * pow(L, 2) / 2.0 / Gs / Kv, -qz * pow(L, 3) / 6.0 / E / Iy,
        qy * pow(L, 3) / 6.0 / E / Iz;

    // u = G*ed.AsColumn() - diffSol;
    u = G * ed.transpose() - diffSol;

    Matrix C(12, 12);

    C << 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(L, 3),
        pow(L, 2), L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(L, 3), pow(L, 2), L, 1.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        -3.0 * pow(L, 2), -2.0 * L, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0 * pow(L, 2), 2.0 * L, 1, 0.0, 0.0, 0.0, 0.0, 0.0,
        0, 0.0;

    ColVec m(12);

    m = C.inverse() * u;

    es.resize(n, 6);
    es.setZero();

    edi.resize(n, 4);
    edi.setZero();

    eci.resize(n);
    eci.setZero();

    for (auto i = 0; i < n; i++)
    {
        eci(i) = double(i) * L / double(n - 1);
        double x = eci(i);

        Matrix T1(6, 12);

        T1 << E * A, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0 * E * Iz, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0 * E * Iy, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Gs * Kv, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0 * E * Iy * x,
            -2.0 * E * Iy, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0 * E * Iz * x, 2.0 * E * Iz, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0;

        ColVec T2(6);

        T2 << -qx * x, -qy * x, -qz * x, -qw * x, -qz * pow(x, 2) / 2.0, qy * pow(x, 2) / 2.0;

        es.row(i) = (T1 * m + T2).transpose();

        Matrix T3(4, 12);

        T3 << x, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(x, 3), pow(x, 2), x, 1.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(x, 3), pow(x, 2), x, 1.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, x, 1.0;

        ColVec T4(4);

        T4 << -qx * pow(x, 2) / 2.0 / E / A, qy * pow(x, 4) / 24.0 / E / Iz, qz * pow(x, 4) / 24.0 / E / Iy,
            -qw * pow(x, 2) / 2.0 / Gs / Kv;

        edi.row(i) = (T3 * m + T4).transpose();
    }
}

void spy(const SpMatrix &matrix)
{
    auto rows = matrix.rows();
    auto cols = matrix.cols();

    for (auto i = 0; i < rows; i++)
    {
        for (auto j = 0; j < cols; j++)
        {
            if (abs(matrix.coeff(i, j)) > 0.0)
                std::cout << "o ";
            else
                std::cout << ". ";
        }
        std::cout << "\n";
    }
}

std::vector<Eigen::Index> toZeroIndex(const std::vector<Eigen::Index> &vec)
{
    std::vector<Eigen::Index> idx;

    for (auto &dof : vec)
        idx.push_back(dof - 1);

    return idx;
}

void extractAllDofs(Eigen::Index nDofs, const std::vector<Eigen::Index> &bcDofs, std::vector<Eigen::Index> &freeDofs,
                    std::vector<Eigen::Index> &allDofs)
{
    allDofs.clear();
    freeDofs.clear();

    for (auto i = 1; i <= nDofs; i++)
        allDofs.push_back(i);

    std::set_difference(allDofs.begin(), allDofs.end(), bcDofs.begin(), bcDofs.end(),
                        std::inserter(freeDofs, freeDofs.begin()));
}

void extractElDisp(const IntMatrix &edof, const ColVec &a, Matrix &ed)
{
    auto nElements = edof.rows();
    auto nDofs = edof.cols();

    ed.resize(nElements, nDofs);

    for (auto i = 0; i < nElements; i++)
    {
        IntColVec ind = edof.row(i).array() - 1;
        ed.row(i) = a(ind);
    }
}

void extractEdof(const IntMatrix &dofs, const Matrix &coords, const IntMatrix &topo, IntMatrix &edof, Matrix &ex,
                 Matrix &ey, Matrix &ez)
{
    auto dofsPerNode = dofs.cols();
    auto nElements = topo.rows();
    auto nodesPerElement = topo.cols();
    auto nNodes = coords.rows();

    edof.resize(nElements, nodesPerElement * dofsPerNode);
    edof.setZero();

    ex.resize(nElements, nodesPerElement);
    ex.setZero();

    ey.resize(nElements, nodesPerElement);
    ey.setZero();

    ez.resize(nElements, nodesPerElement);
    ez.setZero();

    for (auto i = 0; i < nElements; i++)
    {
        for (auto j = 0; j < nodesPerElement; j++)
        {
            auto k = topo(i, j);
            auto x = coords(k - 1, 0);
            auto y = coords(k - 1, 1);
            auto z = coords(k - 1, 2);
            ex(i, j) = x;
            ey(i, j) = y;
            ez(i, j) = z;
            edof.block(i, j * dofsPerNode, 1, dofsPerNode) = dofs.row(k - 1);
        }
    }
}

void assem(IntRowVec &Topo, Matrix &K, Matrix &Ke, ColVec &f, ColVec &fe)
{
    int i, j, r, c;

    for (i = 0; i < Ke.rows(); i++)
        for (j = i; j < Ke.cols(); j++)
        {
            r = Topo(i) - 1;
            c = Topo(j) - 1;

            K(r, c) = K(r, c) + Ke(i, j);
        }

    for (i = 0; i < fe.rows(); i++)
    {
        r = Topo(i) - 1;
        f(r) = f(r) + fe(i);
    }
}

bool solveq(const Matrix &K, const ColVec &f, const IntColVec &bcDofs, const ColVec &bcVals, ColVec &a, ColVec &Q)
{
    auto nDofs = K.rows();

    std::vector<Eigen::Index> allDofsVec;
    std::vector<Eigen::Index> freeDofsVec;
    std::vector<Eigen::Index> bcDofsVec;

    a.setZero();

    calfem::extractAllDofs(nDofs, bcDofsVec, freeDofsVec, allDofsVec);

    auto nFreeDofs = freeDofsVec.size();

    Matrix Ksys(nFreeDofs, nFreeDofs);
    Ksys.setZero();

    ColVec fsys(nFreeDofs);

    auto ind = calfem::toZeroIndex(freeDofsVec);
    auto pind = calfem::toZeroIndex(bcDofsVec);

    Ksys = K(ind, ind);
    fsys = f(ind) - K(ind, pind) * bcVals; // [ ind x 1 ] + [ ind x pind ] x [ pind ]

    ColVec asys = Ksys.llt().solve(fsys);

    a(ind) = asys;
    a(pind) = bcVals;

    Q = K * a - f;

    return true;
}

void spassem(const IntRowVec &Topo, TripletList &Ktriplets, const Matrix &Ke, ColVec &f, const ColVec &fe)
{
    int i, j, r, c;

    for (i = 0; i < Ke.rows(); i++)
        for (j = 0; j < Ke.cols(); j++)
        {
            r = Topo(i) - 1;
            c = Topo(j) - 1;

            Ktriplets.push_back(Triplet(r, c, Ke(i, j)));
        }

    for (i = 0; i < fe.rows(); i++)
    {
        r = Topo(i) - 1;
        f(r) = f(r) + fe(i);
    }
}

bool spsolveq(const SpMatrix &K, const ColVec &f, const IntColVec &bcDofs, const ColVec &bcVals, ColVec &a, ColVec &Q)
{
    auto nDofs = K.rows();

    std::vector<Eigen::Index> allDofsVec;
    std::vector<Eigen::Index> freeDofsVec;
    std::vector<Eigen::Index> bcDofsVec;

    a.setZero();

    for (auto i = 0; i < bcDofs.rows(); i++)
    {
        bcDofsVec.push_back(bcDofs(i));
        a(bcDofs(i) - 1) = bcVals(i);
    }

    calfem::extractAllDofs(nDofs, bcDofsVec, freeDofsVec, allDofsVec);

    auto nFreeDofs = freeDofsVec.size();
    auto nBcDofs = bcDofsVec.size();

    SpMatrix Ksys(nFreeDofs, nFreeDofs);
    SpMatrix Ksysf(nFreeDofs, nBcDofs);

    TripletList Ksyslist;
    TripletList Ksysflist;

    std::cout << "K non zeros = " << K.nonZeros() << "\n";
    std::cout << "K size = " << K.rows() << "\n";

    Ksyslist.reserve(K.nonZeros());
    Ksysflist.reserve(K.nonZeros());

    auto ind = calfem::toZeroIndex(freeDofsVec);
    auto pind = calfem::toZeroIndex(bcDofsVec);

    std::set<Eigen::Index> indSet(ind.begin(), ind.end());
    std::set<Eigen::Index> pindSet(pind.begin(), pind.end());

    std::map<Eigen::Index, Eigen::Index> indMap;
    std::map<Eigen::Index, Eigen::Index> pindMap;

    Eigen::Index i = 0;

    for (auto &idx : ind)
        indMap[idx] = i++;

    i = 0;

    for (auto &idx : pind)
        pindMap[idx] = i++;

    for (Eigen::Index k = 0; k < K.outerSize(); ++k)
        for (SpMatrix::InnerIterator it(K, k); it; ++it)
        {
            auto v = it.value();
            auto r = it.row();
            auto c = it.col();

            if ((indSet.find(r) != indSet.end()) && (indSet.find(c) != indSet.end()))
            {
                Ksyslist.push_back(Triplet(indMap[r], indMap[c], v));
            }

            if ((indSet.find(r) != indSet.end()) && (pindSet.find(c) != pindSet.end()))
            {
                Ksysflist.push_back(Triplet(indMap[r], pindMap[c], v));
            }
        }

    Ksys.setFromTriplets(Ksyslist.begin(), Ksyslist.end());
    Ksysf.setFromTriplets(Ksysflist.begin(), Ksysflist.end());

    std::cout << "Ksys non zeros = " << Ksys.nonZeros() << "\n";
    std::cout << "Ksys size = " << Ksys.rows() << "\n";

    ColVec fsys(nFreeDofs);

    fsys = f(ind) - Ksysf * bcVals;

    std::cout << fsys.maxCoeff() << "\n";
    std::cout << f.maxCoeff() << "\n";

    // Eigen::SparseLU<SpMatrix> solver;
    Eigen::SimplicialLLT<SpMatrix> solver;

    solver.compute(Ksys);

    if (solver.info() != Eigen::Success)
    {
        std::cout << "Solver failed.\n";
        return false;
    }
    Eigen::VectorXd asys = solver.solve(fsys);

    a(ind) = asys;
    a(pind) = bcVals;

    Q = K * a - f;

    return true;
}

SparseSolver::SparseSolver()
{}

std::shared_ptr<SparseSolver> SparseSolver::create()
{
    return std::make_shared<SparseSolver>();
}

bool SparseSolver::setup(const SpMatrix &K, const IntColVec &bcDofs, const ColVec &bcVals)
{
    m_K = &K;
    m_bcDofs = &bcDofs;
    m_bcVals = &bcVals;

    m_nDofs = K.rows();

    m_allDofsVec.clear();
    m_freeDofsVec.clear();
    m_bcDofsVec.clear();

    m_a.resize(m_nDofs);
    m_a.setZero();

    for (auto i = 0; i < bcDofs.rows(); i++)
    {
        m_bcDofsVec.push_back(bcDofs(i));
        m_a(bcDofs(i) - 1) = bcVals(i);
    }

    calfem::extractAllDofs(m_nDofs, m_bcDofsVec, m_freeDofsVec, m_allDofsVec);

    m_nFreeDofs = m_freeDofsVec.size();
    auto nBcDofs = m_bcDofsVec.size();

    m_Ksys.resize(m_nFreeDofs, m_nFreeDofs);
    m_Ksysf.resize(m_nFreeDofs, nBcDofs);

    m_Ksyslist.clear();
    m_Ksysflist.clear();

    std::cout << "K non zeros = " << K.nonZeros() << "\n";
    std::cout << "K size = " << K.rows() << "\n";

    m_Ksyslist.reserve(K.nonZeros());
    m_Ksysflist.reserve(K.nonZeros());

    m_ind = calfem::toZeroIndex(m_freeDofsVec);
    m_pind = calfem::toZeroIndex(m_bcDofsVec);

    std::set<Eigen::Index> indSet(m_ind.begin(), m_ind.end());
    std::set<Eigen::Index> pindSet(m_pind.begin(), m_pind.end());

    std::map<Eigen::Index, Eigen::Index> indMap;
    std::map<Eigen::Index, Eigen::Index> pindMap;

    Eigen::Index i = 0;

    for (auto &idx : m_ind)
        indMap[idx] = i++;

    i = 0;

    for (auto &idx : m_pind)
        pindMap[idx] = i++;

    for (int k = 0; k < (*m_K).outerSize(); ++k)
        for (SpMatrix::InnerIterator it((*m_K), k); it; ++it)
        {
            auto v = it.value();
            auto r = it.row();
            auto c = it.col();

            if ((indSet.find(r) != indSet.end()) && (indSet.find(c) != indSet.end()))
            {
                m_Ksyslist.push_back(Triplet(indMap[r], indMap[c], v));
            }

            if ((indSet.find(r) != indSet.end()) && (pindSet.find(c) != pindSet.end()))
            {
                m_Ksysflist.push_back(Triplet(indMap[r], pindMap[c], v));
            }
        }

    m_Ksys.setFromTriplets(m_Ksyslist.begin(), m_Ksyslist.end());
    m_Ksysf.setFromTriplets(m_Ksysflist.begin(), m_Ksysflist.end());

    std::cout << "Ksys non zeros = " << m_Ksys.nonZeros() << "\n";
    std::cout << "Ksys size = " << m_Ksys.rows() << "\n";

    return true;
}

bool SparseSolver::solve(const ColVec &f, ColVec &a, ColVec &Q)
{
    a.resize(m_nDofs);
    a.setZero();

    for (auto i = 0; i < (*m_bcDofs).rows(); i++)
    {
        m_bcDofsVec.push_back((*m_bcDofs)(i));
        a((*m_bcDofs)(i)-1) = (*m_bcVals)(i);
    }

    m_fsys.resize(m_nFreeDofs);

    m_fsys = f(m_ind) - m_Ksysf * (*m_bcVals);

    std::cout << m_fsys.maxCoeff() << "\n";
    std::cout << f.maxCoeff() << "\n";

    m_solver.compute(m_Ksys);
    if (m_solver.info() != Eigen::Success)
    {
        std::cout << "Solver failed.\n";
        return false;
    }
    m_asys = m_solver.solve(m_fsys);

    a(m_ind) = m_asys;
    a(m_pind) = (*m_bcVals);

    Q = (*m_K) * a - f;

    return true;
}

bool SparseSolver::recompute(const ColVec &f, ColVec &a, ColVec &Q)
{
    a.resize(m_nDofs);
    a.setZero();

    for (auto i = 0; i < (*m_bcDofs).rows(); i++)
    {
        m_bcDofsVec.push_back((*m_bcDofs)(i));
        a((*m_bcDofs)(i)-1) = (*m_bcVals)(i);
    }

    m_fsys.resize(m_nFreeDofs);

    m_fsys = f(m_ind) - m_Ksysf * (*m_bcVals);

    std::cout << m_fsys.maxCoeff() << "\n";
    std::cout << (*m_f).maxCoeff() << "\n";

    if (m_solver.info() != Eigen::Success)
    {
        std::cout << "Solver failed.\n";
        return false;
    }

    m_asys = m_solver.solve(m_fsys);

    a(m_ind) = m_asys;
    a(m_pind) = (*m_bcVals);

    Q = (*m_K) * a - f;

    return true;
}

} // namespace calfem
