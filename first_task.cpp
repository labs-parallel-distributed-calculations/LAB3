#include <mpi.h>
#include <iostream>

using namespace std;

void first_task(int rank, int size) {
	if (size >= 2) {
		int message1;
		int message2;
		if (rank == size - 2) {
			message1 = 1010;
		}
		else if (rank == size - 1) {
			message2 = 998;
		}
		MPI_Bcast(&message1, 1, MPI_INT, size - 2, MPI_COMM_WORLD);
		MPI_Bcast(&message2, 1, MPI_INT, size - 1, MPI_COMM_WORLD);
		
		if (rank % 2 == 0) {
			cout << "RANK " << rank << " GET MESSAGE " << message1 << " AND " << message2 << endl;
		}
	}
}

int main(int argc, char *argv[]) {
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	first_task(rank, size);

	MPI_Finalize();
}
