#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mathFuncts.h"

int printMatrix(char* bez, double A[DIM_M][DIM_N], int m, int n)
{
	int i,j;

	printf("Matrix %s:\n",bez);
	for (i=0;i<m;i++)
	{
		printf("[");
		for (j=0;j<n;j++)
		{
			printf("%f\t",A[i][j]);
		}
		printf(" ]\n");
	}
	printf("\n");
}

int MatrixMul(double A[DIM_M][DIM_N], int m_A, int n_A,
	double B[DIM_M][DIM_N], int m_B, int n_B,
	double C[DIM_M][DIM_N], int m_C, int n_C)
{
	int i, j, k;
	if (n_A!=m_B || m_A!=m_C || n_B!=n_C) 
	{ // falsche Dimensionen
		fprintf(stderr,"Wrong dimension\n");
		return(-1);
	}
	for (i=0; i<m_C; i++)
	{   // Initialisierung
		for (j=0; j<n_C; j++) 
		{
			C[i][j] = 0.0;
		}
	}
	for (i=0; i<m_C; i++) 
	{
		for (j=0; j<n_C; j++) 
		{
			for (k=0; k<n_A; k++) 	
			{   // Multiplikation
				C[i][j] += A[i][k]*B[k][j];
    			}
		}
	}
	return 0;
}

int MatrixAdd(double A[DIM_M][DIM_N], int m_A, int n_A,
	double B[DIM_M][DIM_N], int m_B, int n_B,
	double C[DIM_M][DIM_N], int m_C, int n_C)
{
	int i, j;
	if (n_A!=n_B || n_A!=n_C || m_B!=m_C || m_A!=m_C) 
	{ // falsche Dimensionen
		fprintf(stderr,"Wrong dimension\n");
		return(-1);
	}
	for (i=0; i<m_C; i++)
	{   // Initialisierung
		for (j=0; j<n_C; j++) 
		{
			C[i][j] = 0.0;
		}
	}
	for (i=0; i<m_C; i++) 
	{
		for (j=0; j<n_C; j++) 
		{
			C[i][j] = A[i][j]+B[i][j];
		}
	}
	return 0;
}


int MatrixMulSkalar(double A[DIM_M][DIM_N], int m_A, int n_A,
	double skalar,
	double C[DIM_M][DIM_N], int m_C, int n_C)
{
	int i,j;

	if (n_A!=n_C || m_A!=m_C) 
	{ // falsche Dimensionen
		fprintf(stderr,"Wrong dimension\n");
		return(-1);
	}
	for (i=0; i<m_C; i++)
	{   // Initialisierung
		for (j=0; j<n_C; j++) 
		{
			C[i][j] = 0.0;
		}
	}
	for (i=0;i<m_C;i++)
	{
		for (j=0;j<n_C;j++)
		{
			C[i][j]=skalar*A[i][j];
		}
	}

	return(0);
}

