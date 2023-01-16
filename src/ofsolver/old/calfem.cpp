#include "calfem.h"

#ifdef USE_EIGEN

void beam3e(
	RowVectorXd &ex,
	RowVectorXd &ey,
	RowVectorXd &ez,
	RowVectorXd &eo,
	RowVectorXd &ep,
	RowVectorXd &eq,
	MatrixXd &Ke,
	VectorXd &fe)
{
	VectorXd bb(3);
	bb << ex(1) - ex(0), ey(1) - ey(0), ez(1) - ez(0);

	//double L = sqrt((bb.transpose() * bb).AsScalar());
	double L = sqrt(bb.transpose() * bb);

	RowVectorXd n1 = (bb / L).transpose();

	double lc = sqrt(eo*eo.transpose());
	RowVectorXd n3 = eo / lc;

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
	double b = 12.0*E*Iz / pow(L, 3);
	double c = 6.0*E*Iz / pow(L, 2);
	double d = 12.0*E*Iy / pow(L, 3);
	double e = 6.0*E*Iy / pow(L, 2);
	double f = Gs * Kv / L;
	double g = 2.0*E*Iy / L;
	double h = 2 * E*Iz / L;

	MatrixXd Kle(12, 12);

	Kle << a, 0, 0, 0, 0, 0, -a, 0, 0, 0, 0, 0
		, 0, b, 0, 0, 0, c, 0, -b, 0, 0, 0, c
		, 0, 0, d, 0, -e, 0, 0, 0, -d, 0, -e, 0
		, 0, 0, 0, f, 0, 0, 0, 0, 0, -f, 0, 0
		, 0, 0, -e, 0, 2 * g, 0, 0, 0, e, 0, g, 0
		, 0, c, 0, 0, 0, 2 * h, 0, -c, 0, 0, 0, h
		, -a, 0, 0, 0, 0, 0, a, 0, 0, 0, 0, 0
		, 0, -b, 0, 0, 0, -c, 0, b, 0, 0, 0, -c
		, 0, 0, -d, 0, e, 0, 0, 0, d, 0, e, 0
		, 0, 0, 0, -f, 0, 0, 0, 0, 0, f, 0, 0
		, 0, 0, -e, 0, g, 0, 0, 0, e, 0, 2 * g, 0
		, 0, c, 0, 0, 0, h, 0, -c, 0, 0, 0, 2 * h;

	VectorXd fle(12);

	fle <<
		qx,
		qy,
		qz,
		qw,
		-qz * L / 6.0,
		qy * L / 6.0,
		qx,
		qy,
		qz,
		qw,
		qz * L / 6.0,
		-qy * L / 6.0;

	fle = fle * L / 2.0;

	VectorXd n2(3);

	n2 << n3(1)*n1(2) - n3(2)*n1(1),
		-n1(2)*n3(0) + n1(0)*n3(2),
		n3(0)*n1(1) - n1(0)*n3(1);

	MatrixXd An(3, 3);

	An.row(0) = n1;
	An.row(1) = n2;
	An.row(2) = n3;

	MatrixXd G(12, 12);

	G.setZero();

	VectorXi idx(2);

	idx << 0, 2;
	assign_submatrix(G, idx, idx, An);
	idx << 3, 5;
	assign_submatrix(G, idx, idx, An);
	idx << 6, 8;
	assign_submatrix(G, idx, idx, An);
	idx << 9, 11;
	assign_submatrix(G, idx, idx, An);

	/*
	G.SubMatrix(1, 3, 1, 3) = An;
	G.SubMatrix(4, 6, 4, 6) = An;
	G.SubMatrix(7, 9, 7, 9) = An;
	G.SubMatrix(10, 12, 10, 12) = An;
	*/

	Ke = G.transpose() * Kle * G;
	fe = G.transpose() * fle;
}

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
	VectorXd &eci)
{
	VectorXd bb(3);
	bb << ex(1) - ex(0), 
		ey(1) - ey(0), 
		ez(1) - ez(0);

	double L = sqrt(bb.transpose() * bb);

	RowVectorXd n1 = (bb / L).transpose();

	double lc = sqrt(eo*eo.transpose());
	RowVectorXd n3 = eo / lc;

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

	RowVectorXd n2(3);

	n2 << n3(1)*n1(2) - n3(2)*n1(1),
		-n1(2)*n3(0) + n1(0)*n3(2),
		n3(0)*n1(1) - n1(0)*n3(1);

	MatrixXd An(3, 3);

	An.row(0) = n1;
	An.row(1) = n2;
	An.row(2) = n3;

	MatrixXd G(12, 12);

	G.setZero();

	VectorXi idx(2);

	idx << 0, 2;
	assign_submatrix(G, idx, idx, An);
	idx << 3, 5;
	assign_submatrix(G, idx, idx, An);
	idx << 6, 8;
	assign_submatrix(G, idx, idx, An);
	idx << 9, 11;
	assign_submatrix(G, idx, idx, An);

	VectorXd u(12);
	VectorXd diffSol(12);

	diffSol << 0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		-qx * pow(L, 2) / 2.0 / E / A,
		qy * pow(L, 4) / 24.0 / E / Iz,
		qz * pow(L, 4) / 24.0 / E / Iy,
		-qw * pow(L, 2) / 2.0 / Gs / Kv,
		-qz * pow(L, 3) / 6.0 / E / Iy,
		qy * pow(L, 3) / 6.0 / E / Iz;

	u = G * ed.transpose() - diffSol;

	MatrixXd C(12, 12);

	C << 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, pow(L, 3), pow(L, 2), L, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(L, 3), pow(L, 2), L, 1.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, L, 1.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -3.0*pow(L, 2), -2.0*L, -1.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 3.0*pow(L, 2), 2.0*L, 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0.0;

	VectorXd m(12);

	m = C.inverse()*u;

	es.resize(n, 6);
	es.setZero();
	edi.resize(n, 4);
	edi.setZero();
	eci.resize(n);
	eci.setZero();

	int i;
	for (i = 0; i < n; i++)
	{
		eci(i) = (i - 1.0)*L / (n - 1.0);
		double x = eci(i);

		MatrixXd T1(6, 12);

		T1 << E * A, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, -6.0*E*Iz, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0*E*Iy, 0.0, 0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Gs * Kv, 0.0,
			0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -6.0*E*Iy*x, -2.0*E*Iy, 0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 6.0*E*Iz*x, 2.0*E*Iz, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;

		VectorXd T2(6);

		T2 << -qx * x,
			-qy * x,
			-qz * x,
			-qw * x,
			-qz * pow(x, 2) / 2.0,
			qy * pow(x, 2) / 2.0;

		es.row(i) = (T1*m + T2).transpose();

		/*
		for (j=1; j<=6; j++)
		so_print(es(i,j) << ", ";
		so_print(endl;
		*/

		MatrixXd T3(4, 12);

		T3 << x, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, pow(x, 3), pow(x, 2), x, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 0.0, 0.0, 0.0, pow(x, 3), pow(x, 2), x, 1.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, x, 1.0;

		VectorXd T4(4);

		T4 << -qx * pow(x, 2) / 2.0 / E / A,
			qy * pow(x, 4) / 24.0 / E / Iz,
			qz * pow(x, 4) / 24.0 / E / Iy,
			-qw * pow(x, 2) / 2.0 / Gs / Kv;

		edi.row(i) = (T3*m + T4).transpose();

		/*
		for (j=1; j<=4; j++)
		so_print(edi(i,j) << ", ";
		so_print(endl;
		*/
	}
}

void assign_submatrix(MatrixXd& Mt, VectorXi& rowIdx, VectorXi& colIdx, MatrixXd& Ms)
{
	for (int row = 0; row < rowIdx.size(); row++)
		for (int col = 0; col < colIdx.size(); col++)
			Mt(rowIdx(row), colIdx(col)) = Ms(row, col);
}

/*
void assem(
	RowVector &Topo,
	SymmetricBandMatrix &K,
	Matrix &Ke,
	ColumnVector &f,
	ColumnVector &fe)
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
		if (rowVector(i)>maxValue)
			maxValue = rowVector(i);

	return maxValue;
}

double min(RowVector& rowVector)
{
	int i;
	double minValue = 1e300;

	for (i = 1; i <= rowVector.Ncols(); i++)
		if (rowVector(i)<minValue)
			minValue = rowVector(i);

	return minValue;
}

*/

#else

void beam3e(
	RowVector &ex,
	RowVector &ey,
	RowVector &ez,
	RowVector &eo,
	RowVector &ep,
	RowVector &eq,
	Matrix &Ke,
	ColumnVector &fe)
{
	ColumnVector bb(3);
	bb << ex(2) - ex(1) << ey(2) - ey(1) << ez(2) - ez(1);
	double L = sqrt((bb.t() * bb).AsScalar());

	RowVector n1 = (bb / L).t();

	double lc = sqrt((eo*eo.t()).AsScalar());
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
	double b = 12.0*E*Iz / pow(L, 3);
	double c = 6.0*E*Iz / pow(L, 2);
	double d = 12.0*E*Iy / pow(L, 3);
	double e = 6.0*E*Iy / pow(L, 2);
	double f = Gs * Kv / L;
	double g = 2.0*E*Iy / L;
	double h = 2 * E*Iz / L;

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

	n2 << n3(2)*n1(3) - n3(3)*n1(2)
		<< -n1(3)*n3(1) + n1(1)*n3(3)
		<< n3(1)*n1(2) - n1(1)*n3(2);

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
	ColumnVector &eci)
{
	ColumnVector bb(3);
	bb << ex(2) - ex(1) << ey(2) - ey(1) << ez(2) - ez(1);
	double L = sqrt((bb.t() * bb).AsScalar());

	RowVector n1 = (bb / L).t();

	double lc = sqrt((eo*eo.t()).AsScalar());
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

	n2 << n3(2)*n1(3) - n3(3)*n1(2)
		<< -n1(3)*n3(1) + n1(1)*n3(3)
		<< n3(1)*n1(2) - n1(1)*n3(2);

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
		<< 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << -3.0*pow(L, 2) << -2.0*L << -1.0 << 0.0 << 0.0 << 0.0
		<< 0.0 << 0.0 << 3.0*pow(L, 2) << 2.0*L << 1 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0 << 0.0;

	ColumnVector m(12);

	m = C.i()*u;

	es.ReSize(n, 6);
	es = 0.0;
	edi.ReSize(n, 4);
	edi = 0.0;
	eci.ReSize(n);
	eci = 0.0;

	int i;
	for (i = 1; i <= n; i++)
	{
		eci(i) = (i - 1.0)*L / (n - 1.0);
		double x = eci(i);

		Matrix T1(6, 12);

		T1 << E * A << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
			<< 0.0 << 0.0 << -6.0*E*Iz << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
			<< 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << -6.0*E*Iy << 0.0 << 0.0 << 0.0 << 0.0 << 0.0
			<< 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << Gs * Kv << 0.0
			<< 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << -6.0*E*Iy*x << -2.0*E*Iy << 0.0 << 0.0 << 0.0 << 0.0
			<< 0.0 << 0.0 << 6.0*E*Iz*x << 2.0*E*Iz << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

		ColumnVector T2(6);

		T2 << -qx * x
			<< -qy * x
			<< -qz * x
			<< -qw * x
			<< -qz * pow(x, 2) / 2.0
			<< qy * pow(x, 2) / 2.0;

		es.Row(i) = (T1*m + T2).t();

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

		edi.Row(i) = (T3*m + T4).t();

		/*
		for (j=1; j<=4; j++)
		so_print(edi(i,j) << ", ";
		so_print(endl;
		*/
	}
}

void assem(
	RowVector &Topo,
	SymmetricBandMatrix &K,
	Matrix &Ke,
	ColumnVector &f,
	ColumnVector &fe)
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
		if (rowVector(i)>maxValue)
			maxValue = rowVector(i);

	return maxValue;
}

double min(RowVector& rowVector)
{
	int i;
	double minValue = 1e300;

	for (i = 1; i <= rowVector.Ncols(); i++)
		if (rowVector(i)<minValue)
			minValue = rowVector(i);

	return minValue;
}

#endif
