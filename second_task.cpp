#include <mpi.h>
#include <iterator>
#include <vector>
#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>

using namespace std;


MPI_Status status;
MPI_Request request;
const int size_of_array_to_sort = 36;
const int allowed_number_of_processes = 9;
const int size_of_separated_part = size_of_array_to_sort / allowed_number_of_processes;
vector<int> random_array;
vector<int> separated_parts_of_array;
vector<int> sorted_array;
vector<int> temp_buffer_for_sorting;

vector<int> generate_random_array(int size, int from, int to) {
	vector<int> rand_arr;
	int rand_num;
	for (int i = 0; i < size; ++i) {
		rand_num = rand() % (to - from) + from;
		rand_arr.push_back(rand_num);
	}
	return rand_arr;
}
void bubblesort(int *a, int n){
	for (int i = n - 1; i >= 0; --i){
		for (int j = 0; j < i; ++j){
			if (a[j] > a[j + 1]) {
				swap(a[j], a[j + 1]);
			}
		}
	}
}
void distributed_algorithm(int rank, int p1, int p2)
{
	if (rank == p2)
	{
		MPI_Issend(separated_parts_of_array.data(), size_of_separated_part, MPI_INT, p1, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		MPI_Irecv(separated_parts_of_array.data(), size_of_separated_part, MPI_INT, p1, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
	}
	if (rank == p1)
	{
		MPI_Irecv(temp_buffer_for_sorting.data(), size_of_separated_part, MPI_INT, p2, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		for (int i = size_of_separated_part; i < size_of_separated_part * 2; ++i)
		{
			temp_buffer_for_sorting[i] = separated_parts_of_array[i - size_of_separated_part];
		}
		bubblesort(temp_buffer_for_sorting.data(), size_of_separated_part * 2);
		for (int i = 0; i < size_of_separated_part; ++i)
		{
			separated_parts_of_array[i] = temp_buffer_for_sorting[i];
		}

		MPI_Issend(temp_buffer_for_sorting.data() + size_of_separated_part, size_of_separated_part, MPI_INT, p2, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
	}
}
int main(int argc, char *argv[])
{
	int rank;
	int size;
	srand(time(0));
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	random_array = generate_random_array(size_of_array_to_sort, 0, 50);
	sorted_array.resize(size_of_array_to_sort);
	separated_parts_of_array.resize(size_of_separated_part);
	temp_buffer_for_sorting.resize(size_of_separated_part * 2);

	if (size == allowed_number_of_processes)
	{
		MPI_Scatter(random_array.data(), size_of_separated_part, MPI_INT, separated_parts_of_array.data(), size_of_separated_part, MPI_INT, 0, MPI_COMM_WORLD);
		for (int i = 0; i < size; ++i) {
			for (int j = i+1; j < size; ++j) {
				distributed_algorithm(rank, i, j);
			}
		}
		MPI_Gather(separated_parts_of_array.data(), size_of_separated_part, MPI_INT, sorted_array.data(), size_of_separated_part, MPI_INT, 0, MPI_COMM_WORLD);
		if (rank == 0)
		{
			cout << "Random array:" << endl;
			copy(random_array.begin(), random_array.end(), ostream_iterator<int>(std::cout, " "));
			cout << endl << "Sorted array:" << endl;
			copy(sorted_array.begin(), sorted_array.end(), ostream_iterator<int>(std::cout, " "));
		}
	}
	MPI_Finalize();
	return 0;
}
