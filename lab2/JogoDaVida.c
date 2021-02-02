//Luiz Otavio Passos RA: 122039
#define _XOPEN_SOURCE 600 // X/Open 6, incorporating POSIX 2004
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#define DEAD 0
#define ALIVE 1
#define NUM_THREADS 4
#define SRAND_VALUE 1985
#define GENERATIONS 5
#define SIZE 2048

pthread_barrier_t barWait;

int **grid, **newGrid, countVivas, mortas;

void printa_matriz(int *grid[])
{
	for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            printf("%d ",grid[i][j]);
        }
        printf("\n");
    }

}

void start_generation(int *grid[])
{
    srand(SRAND_VALUE);
    for(int i = 0; i < SIZE; i++) { //laço sobre as células do tabuleiro sem contar com um eventual halo
        for(int j = 0; j< SIZE; j++) {
            grid[i][j] = rand() % 2;
        }
    }
}

int get_neighbors(int *grid[], int i, int j)
{
	int count = 0;
	/*----TOP ROW----*/
    if(i == 0) {

		if(grid[SIZE-1][j] == ALIVE) count++; //upstairs neighbor
		if(grid[i+1][j] == ALIVE) count++; //downstairs neighbor

		//top left corner
		if(j == 0){
			//left neighbors
			if(grid[SIZE-1][SIZE-1] == ALIVE) count++;
			if(grid[i][SIZE-1] == ALIVE) count++;
			if(grid[i+1][SIZE-1] == ALIVE) count++;

			//right neighbors
			if(grid[SIZE-1][j+1] == ALIVE) count++;
			if(grid[i][j+1] == ALIVE) count++;
			if(grid[i+1][j+1] == ALIVE) count++;

			return count;
		}

		//top right corner
		if(j == SIZE - 1){
			//left neighbors
			if(grid[SIZE-1][j-1] == ALIVE) count++;
			if(grid[i][j-1] == ALIVE) count++;
			if(grid[i+1][j-1] == ALIVE) count++;

			//right neighbors
			if(grid[SIZE-1][0] == ALIVE) count++;
			if(grid[i][0] == ALIVE) count++;
			if(grid[i+1][0] == ALIVE) count++;

			return count;
		}

		//not in the corners
		else {


			//left neighbors
			if(grid[SIZE-1][j-1] == ALIVE) count++;
			if(grid[i][j-1] == ALIVE) count++;
			if(grid[i+1][j-1] == ALIVE) count++;

			//right neighbors
			if(grid[SIZE-1][j+1] == ALIVE) count++;
			if(grid[i][j+1] == ALIVE) count++;
			if(grid[i+1][j+1] == ALIVE) count++;

			return count;
		}
	}

	/*/*----BOTTOM ROW----*/
	if(i == SIZE - 1) {

		if(grid[i-1][j] == ALIVE) count++; //upstairs neighbor
		if(grid[0][j] == ALIVE) count++; //downstairs neighbor

		//bottom left corner
		if(j == 0){
			//left neighbors
			if(grid[i-1][SIZE-1] == ALIVE) count++;
			if(grid[i][SIZE-1] == ALIVE) count++;
			if(grid[0][SIZE-1] == ALIVE) count++;


			//right neighbors
			if(grid[i-1][j+1] == ALIVE) count++;
			if(grid[i][j+1] == ALIVE) count++;
			if(grid[0][j+1] == ALIVE) count++;
			return count;
		}

		//bottom right corner
		if(j == SIZE - 1){

			//left neighbors
			if(grid[i-1][j-1] == ALIVE) count++;
			if(grid[i][j-1] == ALIVE) count++;
			if(grid[0][j-1] == ALIVE) count++;

			//right neighbors
			if(grid[i-1][0] == ALIVE) count++;
			if(grid[i][0] == ALIVE) count++;
			if(grid[0][0] == ALIVE) count++;


			return count;
		}

		//not in the corners
		else {

			//left neighbors
			if(grid[i-1][j-1] == ALIVE) count++;
			if(grid[i][j-1] == ALIVE) count++;
			if(grid[0][j-1] == ALIVE) count++;

			//right neighbors
			if(grid[i-1][j+1] == ALIVE) count++;
			if(grid[i][j+1] == ALIVE) count++;
			if(grid[0][j+1] == ALIVE) count++;

			return count;
		}
	}

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

	/*----LAST COLUMN----*/
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

void first_generation()
{
	int i, j;

	#pragma omp parallel private(j) shared(grid) reduction(+:countVivas)
  {
	#pragma omp for
	for (i = 0; i < SIZE; i++){
		for (j = 0; j < SIZE; j++){
			if(grid[i][j] == ALIVE) countVivas++;
			}
		}
	}
}

void new_generation()
{
	int g, i, j;

    for (g = 0; g < GENERATIONS; g++) {
      #pragma omp parallel private(j) shared(grid, newGrid) reduction(+:countVivas,mortas)
      {
    	#pragma omp  for
    	for (int i = 0; i < SIZE; i++){
    		for (int j = 0; j < SIZE; j++){
    			/* living cells with less than 2 neighbors die of abandonment
    			living cells with 4 or more neighbors die of overpopulation */
    			if(grid[i][j] == ALIVE){
    				if(get_neighbors(grid, i, j) < 2) {
    					newGrid[i][j] = DEAD;
    					mortas++;
    				} else if(get_neighbors(grid, i, j) >= 4){
    					newGrid[i][j] = DEAD;
    					mortas++;
    				}else if(get_neighbors(grid, i, j) == 2 || get_neighbors(grid, i, j) == 3) {
    					newGrid[i][j] = ALIVE;
    				}
    			} else {
    				if(get_neighbors(grid, i, j) == 3){
    					newGrid[i][j] = ALIVE;
    					countVivas++;
    				}
    			}
    		}
    	}
    }
        /*if(omp_get_thread_num() == 0){
            printf("%d\n", mortas);
            countVivas = countVivas - mortas;
            mortas = 0;
            printf("Geração %d : %d\n", g, countVivas);
        } */
        #pragma omp parallel private(j) shared(grid, newGrid) reduction(+:countVivas,mortas)
        {
        #pragma omp for
    	for (int i = 0; i < SIZE; i++){
    		for (int j = 0; j < SIZE; j++){
    			grid[i][j] = newGrid[i][j];
    		}
    	}
    }
  }
}







int main(int argc, char *argv[])
{
    double tmili, tempoLaco;
    omp_set_num_threads(NUM_THREADS);
    countVivas = 0, mortas = 0;
    tmili = omp_get_wtime();
    
    int  i;
    grid = malloc(SIZE * sizeof(int*));
    for(i = 0; i < SIZE; i++) grid[i] = malloc(SIZE * sizeof(int));
    newGrid = malloc(SIZE * sizeof(int*));
    for(i = 0; i < SIZE; i++) newGrid[i] = malloc(SIZE * sizeof(int));

    start_generation(grid);
    tempoLaco = omp_get_wtime();
    first_generation();
    printf("%d\n", countVivas);
    new_generation();
    printf("tempo decorrido LACO: %.2f milisegundos\n", (omp_get_wtime() - tempoLaco)*1000);
    printf("tempo decorrido: %.2f milisegundos\n", (omp_get_wtime() - tmili)*1000);
    return 0;
}


 // 1 thread 149321/149372
 // 2 thread 77426/77475
 // 4 thread 44039/44086
 // 8 thread 42758/42804
