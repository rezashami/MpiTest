#include <iostream>
#include "mpi.h"
#include <time.h>
#include <stdio.h>
using std::cout;
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

	/*Set value*/
	pioneer = 1000;

	/*Main buffer*/
	int* buf = NULL;

	/*All sums taje in this variable*/
	int* sums = NULL;

	/*Local buffer*/
	int*  mybuf = new int[pioneer];

	/*Master section*/
	if (myid == 0)
	{
		/*Use for take produced random data*/
		buf = new int[pioneer * world_size];
		fill_data(buf, (pioneer * world_size));
		sums = new int[world_size];
	}

	/*A barrier for move together proccesses*/
	MPI_Barrier(MPI_COMM_WORLD);

	/*Scatter main array to local arrays*/
	MPI_Scatter(buf, pioneer, MPI_INT, mybuf, pioneer, MPI_INT, 0, MPI_COMM_WORLD);

	/*Calculate local sums*/
	int mysum = 0;
	for (int i = 0; i < pioneer; i++)
	{
		mysum += mybuf[i];
	}
	cout << "Mysum is: " << mysum << " and my id is: " << myid << "\n";

	/*Send together sums to zero proccess*/
	int global_sum;
	MPI_Reduce(&mysum, &global_sum, 1, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);

	/*Compute all sums*/
	if (myid == 0)
	{
		cout << "The sum is: " << global_sum << endl;
	}

	/*Free memory*/
	delete[]buf;
	delete[] mybuf;

	/*Finilize all proccesses*/
	MPI_Finalize();
	return 0;
}
