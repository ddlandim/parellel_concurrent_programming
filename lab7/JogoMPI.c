//Luiz Otavio Passos RA: 122039
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DEAD 0
#define ALIVE 1
#define SRAND_VALUE 1985
#define GENERATIONS 2000
#define SIZE 2048



int **grid, **newGrid, *upper_ghost, *lower_ghost;

void start_generation(int *grid[], int size, int id, int num_proc, int start, int end)
{
    int lixo;
    srand(SRAND_VALUE);
	for(int p = 0; p < num_proc; p++){
		MPI_Barrier(MPI_COMM_WORLD);
		if(p == id) {
			for(int i = 1; i <= SIZE+1; i++) {
				for(int j = 0; j< SIZE; j++) {
                    if(i >= start && i <= end){
						grid[i-start+1][j] = rand() % 2;
                    } else lixo = rand() % 2;
				}
			}
		}
	}
}


void update_borders(int *grid[], int this, int prev, int next, int size, MPI_Status s)
{

	MPI_Sendrecv(grid[1], SIZE, MPI_INT, this, 10,
            	 grid[size+1], SIZE, MPI_INT, prev, 10,
                 MPI_COMM_WORLD, &s);

    MPI_Sendrecv(grid[size], SIZE, MPI_INT, this, 11,
                 grid[0], SIZE, MPI_INT, next, 11,
                 MPI_COMM_WORLD, &s);
}


int get_neighbors(int *grid[], int i, int j)
{
	int count = 0;

	if(grid[i-1][j] == ALIVE) count++; //upstairs neighbor
    if(grid[i+1][j] == ALIVE) count++; //downstairs neighbor

	/*----FIRST COLUMN----*/
	if(j == 0) {

		//left neighbors
		if(grid[i-1][SIZE-1] == ALIVE) count++;
		if(grid[i][SIZE-1] == ALIVE) count++;
		if(grid[i+1][SIZE-1] == ALIVE) count++;

		//right neighbors
		if(grid[i-1][j+1] == ALIVE) count++;
		if(grid[i][j+1] == ALIVE) count++;
		if(grid[i+1][j+1] == ALIVE) count++;

		return count;
	}

	if(j == SIZE - 1) {

		//left neighbors
		if(grid[i-1][j-1] == ALIVE) count++;
		if(grid[i][j-1] == ALIVE) count++;
		if(grid[i+1][j-1] == ALIVE) count++;

		//right neighbors
		if(grid[i-1][0] == ALIVE) count++;
		if(grid[i][0] == ALIVE) count++;
		if(grid[i+1][0] == ALIVE) count++;

		return count;
	}



	/*----NOT IN THE BORDERS----*/

	//left neighbors
	if(grid[i-1][j-1] == ALIVE) count++;
	if(grid[i][j-1] == ALIVE) count++;
	if(grid[i+1][j-1] == ALIVE) count++;

	//right neighbors
	if(grid[i-1][j+1] == ALIVE) count++;
	if(grid[i][j+1] == ALIVE) count++;
	if(grid[i+1][j+1] == ALIVE) count++;

	return count;
}

void new_generation(int size, int total_sum, int id, int prev, int next)
{
    int count = 0;



    /*********************** conta filhos da 1 geração ***********************/
	for (int i = 1; i <= size; i++){
		for (int j = 0; j < SIZE; j++)
			if(grid[i][j] == ALIVE) count++;
	}


    MPI_Reduce(&count, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (id == 0) printf("%d alive cells\n", total_sum);
    MPI_Barrier(MPI_COMM_WORLD);
    /*********************** conta filhos da 1 geração ***********************/



    MPI_Status s;


    for (int g = 0; g < GENERATIONS; g++) {
		update_borders(grid, prev, id, next, size, s);
		MPI_Barrier(MPI_COMM_WORLD);

        /*********************** conta filhos da i-ésima geração ***********************/
    	for (int i = 1; i <= size; i++){
    		for (int j = 0; j < SIZE; j++){

    			if(grid[i][j] == ALIVE){
    				if(get_neighbors(grid, i, j) < 2) {
    					newGrid[i][j] = DEAD;
    					count--;
    				} else if(get_neighbors(grid, i, j) >= 4){
    					newGrid[i][j] = DEAD;
    					count--;
    				} else if(get_neighbors(grid, i, j) == 2 || get_neighbors(grid, i, j) == 3) {
    					newGrid[i][j] = ALIVE;
    				}
    			} else {
    				if(get_neighbors(grid, i, j) == 3){
    					newGrid[i][j] = ALIVE;
    					count++;
    				}
    			}
    		}
    	}
        MPI_Barrier(MPI_COMM_WORLD);
        /*********************** conta filhos da i-ésima geração ***********************/





        /*********************** cria i-ésima geração ***********************/
    	for (int i = 1; i <= size; i++){
    		for (int j = 0; j < SIZE; j++)
    			grid[i][j] = newGrid[i][j];
    	}
        MPI_Reduce(&count, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (id == 0) printf("%d alive cells\n", total_sum, GENERATIONS);
        fflush(stdout);
        MPI_Barrier(MPI_COMM_WORLD);
        /*********************** cria i-ésima geração ***********************/


    }

}

int main(int argc, char *argv[])
{

	int i, proc_id, prev_proc, next_proc, num_proc, ierr, ferr, ratio, start, end, range_start, range_end, resto, total_sum = 0;


	ierr = MPI_Init(&argc, &argv);

    /*********************** iniciando matrizes ***********************/
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
        ratio = SIZE/num_proc;
    if(SIZE % num_proc == 0){
        range_start = ratio*proc_id + 1;
        grid = malloc((ratio + 2) * sizeof(int*));
        for(i = 0; i < ratio+2; i++) grid[i] = malloc(SIZE * sizeof(int));
        newGrid = malloc((ratio + 2) * sizeof(int*));
        for(i = 0; i < ratio+2; i++) newGrid[i] = malloc(SIZE * sizeof(int));
    } else {
        resto = SIZE % num_proc;
        if(proc_id == num_proc - 1){
            range_start = ratio*proc_id + 1;
            ratio = ratio + resto;
            grid = malloc((ratio + 2)  * sizeof(int*));
            for(i = 0; i < ratio + 2; i++) grid[i] = malloc(SIZE * sizeof(int));
            newGrid = malloc((ratio + 2) * sizeof(int*));
            for(i = 0; i < ratio + 2 ; i++) newGrid[i] = malloc(SIZE * sizeof(int));
        } else {
            range_start = ratio*proc_id + 1;
            grid = malloc((ratio + 2) * sizeof(int*));
            for(i = 0; i < ratio+2; i++) grid[i] = malloc(SIZE * sizeof(int));
            newGrid = malloc((ratio + 2) * sizeof(int*));
            for(i = 0; i < ratio+2; i++) newGrid[i] = malloc(SIZE * sizeof(int));

        }

    }
    range_end = range_start + ratio;

    upper_ghost = malloc(SIZE * sizeof(int));
    lower_ghost = malloc(SIZE * sizeof(int));






    grid = malloc((ratio + 2) * sizeof(int*));
    for(i = 0; i < ratio+2; i++) grid[i] = malloc(SIZE * sizeof(int));
    newGrid = malloc((ratio + 2) * sizeof(int*));
    for(i = 0; i < ratio+2; i++) newGrid[i] = malloc(SIZE * sizeof(int));
	start_generation(grid, ratio, proc_id, num_proc, range_start, range_end);
    /*********************** iniciando matrizes ***********************/




    /*********************** definindo limites ***********************/
	next_proc = (proc_id + 1) % num_proc;
    prev_proc = ((proc_id == 0) ? num_proc : proc_id) - 1;


    MPI_Barrier(MPI_COMM_WORLD);
    /*********************** definindo limites ***********************/


    /*********************** novas gerações ***********************/
    new_generation(ratio, total_sum, proc_id, prev_proc, next_proc);

    /*********************** novas gerações ***********************/



    ferr = MPI_Finalize();
	return 0;
}
