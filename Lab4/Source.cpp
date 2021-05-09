
#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>
using namespace std;
void PrintPolymon(int* polynom, int s, int r, string text)
{
	bool flag = false;
	printf("\nProcRank: %d Text: %s\n", r, text.c_str());
	for (int i = s-1; i >= 0; i--)
	{
		//printf("I: %d Result M: %d\n", i, polynom[i]);
		if (polynom[i] != 0)
		{
			flag = true;
			if (i == 1)
				printf("%dx", polynom[i]);
			else if (i != 0)
			{
				printf("%dx^%d", polynom[i], i);
			}
			else
				printf("%d", polynom[i]);
		}
		if (flag && i!=0 && polynom[i-1]>0)
			printf(" + ");
	}
	printf("\n");
}
int* MultiplyPolynoms(int* m1, int* m2, int size)
{
	int * ResultMultiply = new int[size];
	for (int i = 0; i < size; i++)
		ResultMultiply[i] = 0;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			//printf("I: %d J: %d m1[i]: %d m2[j]: %d\n", i, j, m1[i], m2[j]);
			if (m1[i] == 0 || m2[j] == 0)
				continue;
			else
			{
				//printf("Cell: %d Result M: %d\n", i+ j, m1[i] * m2[j]);
				ResultMultiply[i + j] += m1[i] * m2[j];
			}
		}
	}
	return ResultMultiply;
}
void DelProcs(vector <int>& Procs)
{
	for (int i = Procs.size()-1; i > 0; i--)
	{
		if (i % 2 != 0)
			Procs.erase(Procs.begin() + i);
	}
}
int main(int argc, char* argv[])
{
	int ProcNum, ProcRank,  BasicSize, NumProcs,  ResultSize;
	BasicSize = 3;
	NumProcs = 2;
	ResultSize = pow(BasicSize, NumProcs)+1;
	MPI_Status Status;
	MPI_Datatype ntype;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	MPI_Type_contiguous(ResultSize, MPI_INT, &ntype);
	MPI_Type_commit(&ntype);
	vector<int> ProcsRank;
	int* m1 = new int[ResultSize];
	int* m2 = new int[ResultSize];
	for (int i = 0; i < ProcNum; i++)
		ProcsRank.push_back(i);
	srand(time(NULL) - ProcRank);
	for (int i = 0; i < ResultSize; i++)
	{
		if (i >= BasicSize)
		{
			m1[i] = 0;
			m2[i] = 0;
		}
		else
		{
			m1[i] = rand() % 5 + 2;
			m2[i] = rand() % 5 + 2;
		}
	}
	PrintPolymon(m1, BasicSize, ProcRank,"M1");
	PrintPolymon(m2, BasicSize, ProcRank,"M2");
	int* resultsPolyM = MultiplyPolynoms(m1, m2, ResultSize);
	MPI_Barrier(MPI_COMM_WORLD);
	while (ProcsRank.size() > 1)
	{
		for (int i = 0; i < ProcsRank.size(); i++)
		{
			if (ProcRank == ProcsRank[i])
			{
				if (i % 2 == 0)
				{
					m1 = resultsPolyM;
				}
				else
				{
					//printf("Proc: %d ыутв Proc: %d\n", ProcRank, ProcsRank[i - 1]);
					MPI_Send(resultsPolyM, 1, ntype, ProcsRank[i - 1], 0, MPI_COMM_WORLD);
				}
			}
		}
		for (int i = 0; i < ProcsRank.size(); i++)
		{
			if (ProcRank == ProcsRank[i])
			{
				if (i % 2 == 0)
				{
					//printf("Proc: %d recv Proc: %d\n", ProcRank, ProcsRank[i + 1]);
					MPI_Recv(m2, 1, ntype, ProcsRank[i + 1], 0, MPI_COMM_WORLD, &Status);
					resultsPolyM = MultiplyPolynoms(m1, m2, ResultSize);
				}
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
		DelProcs(ProcsRank);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (ProcRank == ProcsRank[0])
	{
		printf("\n Result:");
		PrintPolymon(resultsPolyM, ResultSize, ProcsRank[0], "Results");
	}
	MPI_Finalize();
	delete[]m1, m2;
	return 0;
};
