#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>

#ifdef USE_EIGEN

#include <Eigen/Dense>
using namespace Eigen;

void beam3e(
	RowVectorXd &ex,
	RowVectorXd &ey,
	RowVectorXd &ez,
	RowVectorXd &eo,
	RowVectorXd &ep,
	RowVectorXd &eq,
	MatrixXd &Ke,
	VectorXd &fe);

void beam3s(
	RowVectorXd &ex,
	RowVectorXd &ey,
	RowVectorXd &ez,
	RowVectorXd &eo,
	RowVectorXd &ep,
	RowVectorXd &ed,
	RowVectorXd &eq,
	int n,
	MatrixXd &es,
	MatrixXd &edi,
	VectorXd &eci);

void assign_submatrix(MatrixXd& Mt, VectorXi& rowIdx, VectorXi& colIdx, MatrixXd& Ms);

/*
void assem(
	RowVector &Topo,
	SymmetricBandMatrix &K,
	Matrix &Ke,
	ColumnVector &f,
	ColumnVector &fe);

double max(RowVector& rowVector);

double min(RowVector& rowVector);

*/

#else

#include <newmat.h>
#include <newmatio.h>
#include <newmatap.h>
#include <include.h>

void beam3e(
	RowVector &ex,
	RowVector &ey,
	RowVector &ez,
	RowVector &eo,
	RowVector &ep,
	RowVector &eq,
	Matrix &Ke,
	ColumnVector &fe);

void beam3s(
	RowVector &ex,
	RowVector &ey,
	RowVector &ez,
	RowVector &eo,
	RowVector &ep,
	RowVector &ed,
	RowVector &eq,
	int n,
	Matrix &es,
	Matrix &edi,
	ColumnVector &eci);

void assem(
	RowVector &Topo,
	SymmetricBandMatrix &K,
	Matrix &Ke,
	ColumnVector &f,
	ColumnVector &fe);

double max(RowVector& rowVector);
double min(RowVector& rowVector);

#endif