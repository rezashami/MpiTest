#include <iostream>
#include "mpi.h"
#include <time.h>
using  std::cout;
using std::endl;

/*Fill array with random element*/
void fill_data(int * start, int size)
{
	srand(time(NULL));
	for (int i = 0; i < size; i++)
	{
		start[i] = i;
	}
}

int main(int argc, char** argv)
{
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
	int* buf;
	int* sums;
	int*  mybuf = new int[pioneer];
	/*Master section*/
	
		/*Use for take produced random data*/
	buf = new int[pioneer * world_size];
	fill_data(buf, (pioneer * world_size));
	sums = new int[world_size];
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Scatter(buf, pioneer, MPI_INT, mybuf, pioneer, MPI_INT, 0, MPI_COMM_WORLD);
	int mysum = 0;
	for (int i = 0; i < pioneer; i++)
	{
		mysum += mybuf[i];
	}
	cout << "Mysum is: " << mysum << " and my id is: " << myid << "\n";
	MPI_Gather(&mysum, 1, MPI_INT,sums, 1, MPI_INT, 0,MPI_COMM_WORLD);
	if (myid == 0)
	{
		int totalSum = 0;
		for (int i = 0; i < world_size; i++)
		{
			totalSum += sums[i];
		}
		cout << "The sum is: " << totalSum << endl;
	}

	/*Free memory*/
	delete[]buf;
	delete[] mybuf;

	MPI_Finalize();
	return 0;
}
