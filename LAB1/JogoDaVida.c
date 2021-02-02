//Luiz Otavio Passos RA: 122039
#define _XOPEN_SOURCE 600 // X/Open 6, incorporating POSIX 2004
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#define DEAD 0
#define ALIVE 1
#define NUM_THREADS 8
#define SRAND_VALUE 1985
#define GENERATIONS 2000
#define SIZE 2048

struct thread{
	int inicio;
	int fim;
	int index;
	pthread_t ti;
};

pthread_barrier_t barWait;

int **grid, **newGrid, count[NUM_THREADS];

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

void print_count(){
	int total = 0, i;
	for (i = 0; i < NUM_THREADS; i++){
		total += count[i];	
	}
	printf("%d\n", total);
	
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

void *new_generation(void *args)
{
	struct thread *t = (struct thread *)args;
	for (int i = t->inicio; i < t->fim; i++){
		for (int j = 0; j < SIZE; j++)
			if(grid[i][j] == ALIVE) count[t->index]++;
		pthread_barrier_wait(&barWait);
	}
	


int g;
for (g = 0; g < GENERATIONS; g++) {
	/* code */

	for (int i = t->inicio; i < t->fim; i++){
		for (int j = 0; j < SIZE; j++){
			/* living cells with less than 2 neighbors die of abandonment
			living cells with 4 or more neighbors die of overpopulation */
			if(grid[i][j] == ALIVE){
				if(get_neighbors(grid, i, j) < 2) {
					newGrid[i][j] = DEAD;
					count[t->index]--;
				} else if(get_neighbors(grid, i, j) >= 4){
					newGrid[i][j] = DEAD;
					count[t->index]--;
				}else if(get_neighbors(grid, i, j) == 2 || get_neighbors(grid, i, j) == 3) {
					newGrid[i][j] = ALIVE;
				}
			} else {
				if(get_neighbors(grid, i, j) == 3){
					newGrid[i][j] = ALIVE;
					count[t->index]++;

				}
			}


		}
		
	}
	pthread_barrier_wait(&barWait);
	

	for (int i = t->inicio; i < t->fim; i++){
		for (int j = 0; j < SIZE; j++){
			grid[i][j] = newGrid[i][j];
		}
	}
	pthread_barrier_wait(&barWait);

	
	
}

}

int main(int argc, char *argv[])
{
	//------ CLOCK------//
	clock_t start, end;
  	struct timeval inicio, inicioLaco, final2, finalLaco;
  	int tmili, tempoLaco;
  	start = clock();
  	gettimeofday(&inicio, NULL);
	//-----------------//

	int  i;

	grid = malloc(SIZE * sizeof(int*));
    for(i = 0; i < SIZE; i++) grid[i] = malloc(SIZE * sizeof(int));

    newGrid = malloc(SIZE * sizeof(int*));
    for(i = 0; i < SIZE; i++) newGrid[i] = malloc(SIZE * sizeof(int));

	for ( i = 0; i < NUM_THREADS; i++){
		count[i] = 0;
	}
	

	

	start_generation(grid);

	pthread_barrier_init(&barWait, NULL, NUM_THREADS);
	struct thread threadArray[NUM_THREADS];
	threadArray[0].inicio = 0;
	threadArray[0].fim = SIZE/NUM_THREADS;
	threadArray[0].index = 0;

	for (i = 1; i < NUM_THREADS; i++) {
		threadArray[i].inicio = threadArray[i-1].fim;
		threadArray[i].fim = threadArray[i].inicio + (SIZE/NUM_THREADS);
		threadArray[i].index = i;
	}

	/*for (i = 0; i < NUM_THREADS; i++) {
		printf("t[%d] inicio %d fim %d\n",i, threadArray[i].inicio, threadArray[i].fim);
	} */



	gettimeofday(&inicioLaco, NULL);
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threadArray[i].ti, NULL, new_generation, &threadArray[i]);
	}
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threadArray[i].ti, NULL);
	}
	pthread_barrier_destroy(&barWait);
	print_count();
	gettimeofday(&finalLaco, NULL);
	tempoLaco = (int) (1000 * (finalLaco.tv_sec - inicioLaco.tv_sec) + (finalLaco.tv_usec - inicioLaco.tv_usec) / 1000);
	printf("tempo decorrido LACO: %d milisegundos\n", tempoLaco);

	

	gettimeofday(&final2, NULL);
	tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);

  	printf("tempo decorrido: %d milisegundos\n", tmili);
  return 0;
}


 // 1 thread 149321/149372
 // 2 thread 77426/77475
 // 4 thread 44039/44086
 // 8 thread 42758/42804