#include <iostream>
#include "mpi.h"
#include <time.h>
using  std::cout;
using std::endl;

/*Fill array with random element*/
void fill_data(int * start, int size)
{
	/*for save data in .csv file*/
	FILE *fp;
	fp = fopen("datas.csv", "w+");
	fprintf(fp, "value,index\n");
	srand(time(NULL));
	for (int i = 0; i < size; i++)
	{
		start[i] = rand();
		fprintf(fp, "%d,%d\n", start[i], i);
	}
}

int main(int argc, char** argv)
{
	/*Use for keep information about proccesses*/
	int myid;
	int world_size;

	/*Use for forward pointer to send separate element*/
	int pioneer;

	/*Initialize mpi*/
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	double t1, t2;
	

	/*Set value*/
	pioneer = 1000;

	/*Master section*/
	if (myid == 0)
	{
		/*Use for take produced random data*/
		int* buf;
		buf = new int[pioneer * world_size];
		fill_data(buf, (pioneer * world_size));
		int offset = 0;

		t1 = MPI_Wtime();

		/*start from one because self messaging force to use MPI_Isend() function. To avoid use this, start from one and zero proccess calculate separatly*/
		for (int dest = 1; dest < world_size; dest++)
		{
			/*Use for partition data into ranks*/
			offset += (pioneer);

			/*Move pointer to other section of array that we want send to other proccesses*/
			MPI_Send(buf+offset, pioneer, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}

		/*Calculate the local sum for zero proccess*/
		int totalSum = 0;
		for (int i = 0; i < pioneer; i++)
		{
			totalSum += buf[i];
		}
		cout << "Mysum is: " << totalSum << " and my id is: " << myid << "\n";
		MPI_Status st;

		/*Recieve and calculate the sum*/
		for (int src = 1; src < world_size; src++)
		{
			int temp;
			MPI_Recv(&temp, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &st);
			totalSum += temp;
		}
		cout << "The sum is: " << totalSum << endl;

		/*Free memory*/
		delete[]buf;
	}
	else {
		/*Allocate a buffer for proccess*/
		int* mybuf = new int[pioneer];

		/*This variable keep local sum*/
		int mysum = 0;

		t1 = MPI_Wtime();

		/*Recieve section*/
		MPI_Status st;
		MPI_Recv(mybuf, pioneer, MPI_INT, 0, 0, MPI_COMM_WORLD, &st);

		/*Calculate the local sum*/
		for (int i = 0; i < pioneer; i++)
		{
			mysum += mybuf[i];
		}
		cout << "Mysum is: " << mysum << " and my id is: " << myid << "\n";

		/*Send local sum to zero proccess*/
		MPI_Send(&mysum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

		/*Free memory*/
		delete[] mybuf;
	}

	t2 = MPI_Wtime();
	printf("Elapsed time is %f\n", t2 - t1);
	
	/*Finalize section*/
	MPI_Finalize();
	return 0;
}
