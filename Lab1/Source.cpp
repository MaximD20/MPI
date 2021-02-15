#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
using namespace std;
int main(int argc, char* argv[])
{
	int ProcNum, ProcRank, RecvData,count = 0, SendData = 0, dataEnd = 0;
	MPI_Status Status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	if (ProcRank == 0)
	{
		// Действия, выполняемые только процессом с рангом 0
		// Осуществляется последовательное получение сообщений при помощи функции MPI_Recv от остальных процессов
		// Если полученное сообщение не -1, то счётчик (count) увеличивается на 1 и выводится сообщение об этом
		// Иначе процесс выводит окончательное значение счётчика и завершает работу
		printf("\n Hello from process %3d\n", ProcRank);
		for (int i = 1; i < ProcNum; i++)
		{
			MPI_Recv(&RecvData, sizeof(SendData), MPI_INT, MPI_ANY_SOURCE,
				MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
			if (dataEnd != 1)
			{
				if (RecvData == -1)
				{
					printf("Recv is -1. End Programm count = %d \n", count);
					dataEnd = 1;
				}
				else
				{
					printf("Data Recv is %d\n", RecvData);
					count++;
				}
			}
		}
		if(dataEnd != 1)
			printf("Recv is -1. End Programm count = %d \n", count);
	}
	else
	{
		// Действия, выполняемые остальными процессами (кроме процесса с рангом 0)
		// Генерируется случайное число, которое отправляется процессу с рангом 0 при помощи функции MPI_Send

		srand(time(NULL));
		SendData = rand() %ProcNum -ProcRank;
		printf("Generate ProcRank: %d data is %d\n", ProcRank, SendData);
		MPI_Send(&SendData, sizeof(SendData), MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}
