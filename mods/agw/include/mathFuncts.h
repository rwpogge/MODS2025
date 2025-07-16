
#ifndef _MATHFUNCT_H_
#define _MATHFUNCT_H

#define DIM_N 2
#define DIM_M 2


int MatrixMul(double A[DIM_M][DIM_N], int m_A, int n_A,
	double B[DIM_M][DIM_N], int m_B, int n_B,
	double C[DIM_M][DIM_N], int m_C, int n_C);

int MatrixAdd(double A[DIM_M][DIM_N], int m_A, int n_A,
	double B[DIM_M][DIM_N], int m_B, int n_B,
	double C[DIM_M][DIM_N], int m_C, int n_C);

int MatrixMulSkalar(double A[DIM_M][DIM_N], int m_A, int n_A,
	double skalar,
	double C[DIM_M][DIM_N], int m_C, int n_C);

#endif
