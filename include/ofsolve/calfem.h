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

#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>
#include <iostream>
#include <string>
#include <vector>
#include <set>

namespace calfem
{

typedef Eigen::Matrix<double, 1, Eigen::Dynamic> RowVec;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> ColVec;
typedef Eigen::MatrixXd Matrix;
typedef Eigen::MatrixXd Mat;
typedef Eigen::SparseMatrix<double> SpMatrix;

typedef Eigen::Matrix<int, 1, Eigen::Dynamic> IntRowVec;
typedef Eigen::Matrix<int, Eigen::Dynamic, 1> IntColVec;
typedef Eigen::MatrixXi IntMatrix;
typedef Eigen::MatrixXi IntMat;
typedef Eigen::SparseMatrix<int> SpIntMatrix;

typedef Eigen::Triplet<double, std::ptrdiff_t> Triplet;
typedef std::vector<Triplet> TripletList;

void bar3e(
    const RowVec& ex,
    const RowVec& ey,
    const RowVec& ez,
    const RowVec& ep,
    double eq,
    Matrix& Ke,
    ColVec& fe);

void bar3s(
    const RowVec& ex,
    const RowVec& ey,
    const RowVec& ez,
    const RowVec& ep,
    const RowVec& ed,
    double eq,
    int n,
    ColVec& es,
    ColVec& edi,
    ColVec& eci);

void beam3e(
    const RowVec& ex,
    const RowVec& ey,
    const RowVec& ez,
    const RowVec& eo,
    const RowVec& ep,
    const RowVec& eq,
    Matrix& Ke,
    ColVec& fe);

void beam3s(
    const RowVec& ex,
    const RowVec& ey,
    const RowVec& ez,
    const RowVec& eo,
    const RowVec& ep,
    const RowVec& ed,
    const RowVec& eq,
    const int n,
    Matrix& es,
    Matrix& edi,
    ColVec& eci);

template <typename T>
void print(T& A)
{
    std::cout << A.format(Eigen::IOFormat(Eigen::StreamPrecision, 0, ", ", ";\n", "[", "]", "[", "]")) << "\n";
}

void spy(const SpMatrix& matrix);

std::vector<Eigen::Index> toZeroIndex(const std::vector<Eigen::Index>& vec);

void extractAllDofs(Eigen::Index nDofs, const std::vector<Eigen::Index>& bcDofs, std::vector<Eigen::Index>& freeDofs, std::vector<Eigen::Index>& allDofs);

void extractElDisp(const IntMatrix& edof, const ColVec& a, Matrix& ed);

void assem(IntRowVec& Topo, Matrix& K, Matrix& Ke, ColVec& f, ColVec& fe);

void spassem(const IntRowVec& Topo, TripletList& Ktriplets, const Matrix& Ke, ColVec& f, const ColVec& fe);

void extractEdof(
    const IntMatrix& dofs,
    const Matrix& coords,
    const IntMatrix& topo,
    IntMatrix& edof,
    Matrix& ex,
    Matrix& ey,
    Matrix& ez);

bool solveq(const Matrix& K, const ColVec& f, const IntColVec& bcDofs, const ColVec& bcVals, ColVec& a, ColVec& Q);
bool spsolveq(const SpMatrix& K, const ColVec& f, const IntColVec& bcDofs, const ColVec& bcVals, ColVec& a, ColVec& Q);

class SparseSolver
{
private:
    Eigen::SimplicialLLT<SpMatrix> m_solver;
    SpMatrix m_Ksys;
    SpMatrix m_Ksysf;

    Eigen::Index m_nFreeDofs;
    Eigen::Index m_nDofs;

    TripletList m_Ksyslist;
    TripletList m_Ksysflist;
    ColVec m_fsys;
    Eigen::VectorXd m_asys;
    Eigen::VectorXd m_a;

    std::vector<Eigen::Index> m_allDofsVec;
    std::vector<Eigen::Index> m_freeDofsVec;
    std::vector<Eigen::Index> m_bcDofsVec;

    std::vector<Eigen::Index> m_ind;
    std::vector<Eigen::Index> m_pind;

    const SpMatrix* m_K;
    ColVec* m_f;
    const IntColVec* m_bcDofs;
    const ColVec* m_bcVals;

public:
    SparseSolver();

    static std::shared_ptr<SparseSolver> create();

    bool setup(const SpMatrix& K, const IntColVec& bcDofs, const ColVec& bcVals);
    bool solve(const ColVec& f, ColVec& a, ColVec& Q);
    bool recompute(const ColVec& f, ColVec& a, ColVec& Q);
};

typedef std::shared_ptr<SparseSolver> SparseSolverPtr;

/*
void writeRowVec(const std::string& name, RowVec& v, std::ostream& out, bool matlab = true);
void writeColVec(const std::string& name, ColVec& v, std::ostream& out, bool matlab = true);
void writeMatrix(const std::string& name, Matrix& m, std::ostream& out, bool matlab = true);
// void writeMatrix(const std::string& name, SymmetricBandMatrix &m, std::ostream &out, bool matlab=true);
*/

}
