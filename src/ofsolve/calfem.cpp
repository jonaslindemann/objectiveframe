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

namespace calfem
{

/*
Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ",
", ", "", "", " << ", ";"); Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[",
"]"); Eigen::IOFormat OctaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "", "",
"[", "]"); Eigen::IOFormat HeavyFmt(Eigen::FullPrecision, 0, ", ", ";\n", "[",
"]", "[", "]"); Eigen::IOFormat CustomFmt(Eigen::StreamPrecision, 0, ", ",
";\n", "[", "]", "[", "]");
*/

void beam3e(const RowVec& ex, const RowVec& ey, const RowVec& ez,
    const RowVec& eo, const RowVec& ep, const RowVec& eq, Matrix& Ke,
    ColVec& fe)
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

    Kle << a, 0, 0, 0, 0, 0, -a, 0, 0, 0, 0, 0, 0, b, 0, 0, 0, c, 0, -b, 0, 0,
        0, c, 0, 0, d, 0, -e, 0, 0, 0, -d, 0, -e, 0, 0, 0, 0, f, 0, 0, 0, 0, 0,
        -f, 0, 0, 0, 0, -e, 0, 2 * g, 0, 0, 0, e, 0, g, 0, 0, c, 0, 0, 0, 2 * h,
        0, -c, 0, 0, 0, h, -a, 0, 0, 0, 0, 0, a, 0, 0, 0, 0, 0, 0, -b, 0, 0, 0,
        -c, 0, b, 0, 0, 0, -c, 0, 0, -d, 0, e, 0, 0, 0, d, 0, e, 0, 0, 0, 0, -f,
        0, 0, 0, 0, 0, f, 0, 0, 0, 0, -e, 0, g, 0, 0, 0, e, 0, 2 * g, 0, 0, c,
        0, 0, 0, h, 0, -c, 0, 0, 0, 2 * h;

    ColVec fle(12);

    fle << qx, qy, qz, qw, -qz * L / 6.0, qy * L / 6.0, qx, qy, qz, qw,
        qz * L / 6.0, -qy * L / 6.0;

    fle = fle * L / 2.0;

    RowVec n2(3);

    n2 << n3(1) * n1(2) - n3(2) * n1(1), -n1(2) * n3(0) + n1(0) * n3(2),
        n3(0) * n1(1) - n1(0) * n3(1);

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

void beam3s(const RowVec& ex, const RowVec& ey, const RowVec& ez,
    const RowVec& eo, const RowVec& ep, const RowVec& ed,
    const RowVec& eq, const int n, Matrix& es, Matrix& edi, ColVec& eci)
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

    n2 << n3(1) * n1(2) - n3(2) * n1(1), -n1(2) * n3(0) + n1(0) * n3(2),
        n3(0) * n1(1) - n1(0) * n3(1);

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

    diffSol << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -qx * pow(L, 2) / 2.0 / E / A,
        qy * pow(L, 4) / 24.0 / E / Iz, qz * pow(L, 4) / 24.0 / E / Iy,
        -qw * pow(L, 2) / 2.0 / Gs / Kv, -qz * pow(L, 3) / 6.0 / E / Iy,
        qy * pow(L, 3) / 6.0 / E / Iz;

    // u = G*ed.AsColumn() - diffSol;
    u = G * ed.transpose() - diffSol;

    Matrix C(12, 12);

    C << 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, pow(L, 3), pow(L, 2), L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, pow(L, 3), pow(L, 2), L, 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, L, 1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, -3.0 * pow(L, 2), -2.0 * L, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        3.0 * pow(L, 2), 2.0 * L, 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0.0;

    ColVec m(12);

    // m = C.i()*u;
    m = C.inverse() * u;

    // es.ReSize(n, 6);
    es.resize(n, 6);
    es.setZero();

    // edi.ReSize(n,4);
    edi.resize(n, 4);
    edi.setZero();

    // eci.ReSize(n);
    eci.resize(n);
    eci.setZero();

    for (auto i = 0; i < n; i++)
    {
        eci(i) = double(i) * L / double(n - 1);
        double x = eci(i);

        Matrix T1(6, 12);

        T1 << E * A, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, -6.0 * E * Iz, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0 * E * Iy, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Gs * Kv, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0 * E * Iy * x, -2.0 * E * Iy, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 6.0 * E * Iz * x, 2.0 * E * Iz, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0;

        ColVec T2(6);

        T2 << -qx * x, -qy * x, -qz * x, -qw * x, -qz * pow(x, 2) / 2.0,
            qy * pow(x, 2) / 2.0;

        es.row(i) = (T1 * m + T2).transpose();

        Matrix T3(4, 12);

        T3 << x, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, pow(x, 3), pow(x, 2), x, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(x, 3), pow(x, 2), x, 1.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, x, 1.0;

        ColVec T4(4);

        T4 << -qx * pow(x, 2) / 2.0 / E / A, qy * pow(x, 4) / 24.0 / E / Iz,
            qz * pow(x, 4) / 24.0 / E / Iy, -qw * pow(x, 2) / 2.0 / Gs / Kv;

        edi.row(i) = (T3 * m + T4).transpose();
    }
}

void spy(const SpMatrix& matrix)
{
    auto rows = matrix.rows();
    auto cols = matrix.cols();

    for (auto i=0; i<rows; i++)
    {
        for (auto j=0; j<cols; j++)
        {
            if (abs(matrix.coeff(i, j)) > 0.0)
                std::cout << "o ";
            else
                std::cout << ". ";
        }
        std::cout << "\n";
    }
}

std::vector<int> toZeroIndex(const std::vector<int>& vec)
{
    std::vector<int> idx;

    for (auto& dof : vec)
        idx.push_back(dof - 1);

    return idx;
}

void extractAllDofs(int nDofs, const std::vector<int>& bcDofs, std::vector<int>& freeDofs, std::vector<int>& allDofs)
{
    allDofs.clear();
    freeDofs.clear();

    for (auto i = 1; i <= nDofs; i++)
        allDofs.push_back(i);

    std::set_difference(
        allDofs.begin(), allDofs.end(),
        bcDofs.begin(), bcDofs.end(),
        std::inserter(freeDofs, freeDofs.begin()));
}

void extractElDisp(const IntMatrix& edof, const ColVec& a, Matrix& ed)
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

void extractEdof(const IntMatrix& dofs, const Matrix& coords, const IntMatrix& topo, IntMatrix& edof, Matrix& ex, Matrix& ey, Matrix& ez)
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

void assem(IntRowVec& Topo, Matrix& K, Matrix& Ke, ColVec& f, ColVec& fe)
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

void solveq(const Matrix& K, const ColVec& f, const IntColVec& bcDofs, const ColVec& bcVals, ColVec& a, ColVec& Q)
{
    auto nDofs = K.rows();

    std::vector<int> allDofsVec;
    std::vector<int> freeDofsVec;
    std::vector<int> bcDofsVec;

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

    // ColVec asys = Ksys.ldlt().solve(fsys);
    ColVec asys = Ksys.llt().solve(fsys);
    // ColVec asys = Ksys.fullPivHouseholderQr().solve(fsys);

    a(ind) = asys;
    a(pind) = bcVals;

    Q = K * a - f;
}

void spassem(const IntRowVec& Topo, TripletList& Ktriplets, const Matrix& Ke, ColVec& f, const ColVec& fe)
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

void spsolveq(const SpMatrix& K, const ColVec& f, const IntColVec& bcDofs, const ColVec& bcVals, ColVec& a, ColVec& Q)
{
    auto nDofs = K.rows();

    std::vector<int> allDofsVec;
    std::vector<int> freeDofsVec;
    std::vector<int> bcDofsVec;

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

    std::set<int> indSet(ind.begin(), ind.end());
    std::set<int> pindSet(pind.begin(), pind.end());

    std::map<int, int> indMap;
    std::map<int, int> pindMap;

    int i = 0;

    for (auto& idx : ind)
        indMap[idx] = i++;

    i = 0;

    for (auto& idx : pind)
        pindMap[idx] = i++;

    for (int k = 0; k < K.outerSize(); ++k)
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

    fsys = f(ind) - Ksysf*bcVals;

    std::cout << fsys.maxCoeff() << "\n";
    std::cout << f.maxCoeff() << "\n";

    //Eigen::SparseLU<SpMatrix> solver; 
    Eigen::SimplicialLLT<SpMatrix> solver;
    solver.compute(Ksys);
    if (solver.info() != Eigen::Success)
    {
        std::cout << "Solver failed.\n";
        return;
    }
    Eigen::VectorXd asys = solver.solve(fsys); 

    //calfem::print(asys);
    
    //fsys = f(ind) - K(ind, pind) * bcVals; // [ ind x 1 ] + [ ind x pind ] x [

    // ColVec asys = Ksys.ldlt().solve(fsys);
    // ColVec asys = Ksys.llt().solve(fsys);
    // ColVec asys = Ksys.fullPivHouseholderQr().solve(fsys);

    a(ind) = asys;
    a(pind) = bcVals;

    Q = K * a - f;
}

SparseSolver::SparseSolver()
{
}

std::shared_ptr<SparseSolver> SparseSolver::create()
{
    return std::make_shared<SparseSolver>();
}

void SparseSolver::setup(const SpMatrix& K, const IntColVec& bcDofs, const ColVec& bcVals)
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

    std::set<int> indSet(m_ind.begin(), m_ind.end());
    std::set<int> pindSet(m_pind.begin(), m_pind.end());

    std::map<int, int> indMap;
    std::map<int, int> pindMap;

    int i = 0;

    for (auto& idx : m_ind)
        indMap[idx] = i++;

    i = 0;

    for (auto& idx : m_pind)
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
}

void SparseSolver::solve(const ColVec& f, ColVec& a, ColVec& Q)
{
    a.resize(m_nDofs);
    a.setZero();

    for (auto i = 0; i < (*m_bcDofs).rows(); i++)
    {
        m_bcDofsVec.push_back((*m_bcDofs)(i));
        a((*m_bcDofs)(i) - 1) = (*m_bcVals)(i);
    }

    m_fsys.resize(m_nFreeDofs);

    m_fsys = f(m_ind)-m_Ksysf * (*m_bcVals);

    std::cout << m_fsys.maxCoeff() << "\n";
    std::cout << (*m_f).maxCoeff() << "\n";

    // Eigen::SparseLU<SpMatrix> solver;
    // Eigen::SimplicialLLT<SpMatrix> solver;
    m_solver.compute(m_Ksys);
    if (m_solver.info() != Eigen::Success)
    {
        std::cout << "Solver failed.\n";
        return;
    }
    m_asys = m_solver.solve(m_fsys);

    a(m_ind) = m_asys;
    a(m_pind) = (*m_bcVals);

    Q = (*m_K) * m_a - f;
}

void SparseSolver::recompute(const ColVec& f, ColVec& a, ColVec& Q)
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

    // Eigen::SparseLU<SpMatrix> solver;
    // Eigen::SimplicialLLT<SpMatrix> solver;
    //m_solver.compute(m_Ksys);
    if (m_solver.info() != Eigen::Success)
    {
        std::cout << "Solver failed.\n";
        return;
    }
    m_asys = m_solver.solve(m_fsys);

    a(m_ind) = m_asys;
    a(m_pind) = (*m_bcVals);

    Q = (*m_K) * m_a - f;
}

/*
void writeRowVec(const std::string& name, RowVec &v, std::ostream &out, bool
matlab)
{
        if (matlab)
                out << name << " = [" << std::endl;
        else
                out << name << " = matrix([[" << std::endl;

        int i;

        for (i=1; i<=v.Ncols(); i++)
                        if (i<v.Nrows())
                                out << "\t " << v(i) << " ," << std::endl;
                        else
                                out << "\t " << v(i) << std::endl;

        if (matlab)
                out << "]';" << endl;
        else
                out << "]])" << endl;
}

void writeColVector(const std::string& name, ColVec &v, std::ostream &out, bool
matlab)
{
        if (matlab)
                out << name << " = [" << std::endl;
        else
                out << name << " = matrix([" << std::endl;

        int i;

        for (i=1; i<=v.Nrows(); i++)
                if (matlab)
                        out << v(i) << std::endl;
                else
                        if (i<v.Nrows())
                                out << "\t[ " << v(i) << " ]," << std::endl;
                        else
                                out << "\t[ " << v(i) << "]" << std::endl;

        if (matlab)
                out << "];" << endl;
        else
                out << "])" << endl;
}

void writeMatrix(const std::string& name, Matrix &m, std::ostream &out, bool
matlab)
{
        if (matlab)
                out << name << " = [" << std::endl;
        else
                out << name << " = matrix([" << std::endl;

        int i, j;

        for (i=1; i<=m.Nrows(); i++)
        {
                for (j=1; j<=m.Ncols(); j++)
                {
                        //std::cout << "row = " << i << " col = " << j <<
std::endl; if (matlab) out << m(i,j) << " "; else
                        {
                                if (j==1)
                                        out << "\t[ ";

                                if (j<m.Ncols())
                                        out << m(i,j) << ", ";
                                else
                                        out << m(i,j) << " ],";
                        }
                }
                if (matlab)
                        out << ";" << std::endl;
                else
                        out << std::endl;
        }
        if (matlab)
                out << "];" << endl;
        else
                out << "])" << endl;
}
*/

/*
void writeMatrix(const std::string& name, SymmetricBandMatrix &m, std::ostream
&out, bool matlab)
{
        if (matlab)
                out << name << " = [" << std::endl;
        else
                out << name << " = matrix([" << std::endl;

        int i, j;

        for (i=1; i<=m.Nrows(); i++)
        {
                for (j=1; j<=m.Ncols(); j++)
                {
                        //std::cout << "row = " << i << " col = " << j <<
std::endl; if (matlab) out << m(i,j) << " "; else
                        {
                                if (j==1)
                                        out << "\t[ ";

                                if (j<m.Ncols())
                                        out << m(i,j) << ", ";
                                else
                                        out << m(i,j) << " ],";
                        }
                }
                if (matlab)
                        out << ";" << std::endl;
                else
                        out << std::endl;
        }
        if (matlab)
                out << "];" << endl;
        else
                out << "])" << endl;
}
*/

} // namespace calfem