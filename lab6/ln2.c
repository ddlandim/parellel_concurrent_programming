#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define N 100000000
#define u 2.0
int main(int argc, char *argv[])
{
    int id, numprocs, i, rc;
    double localSum = 0.0, passo = ((u-1) / (double) N), x, globalSum = 0.0,  tmili;
    struct timeval ini, fim;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&id);

    if(id == 0) gettimeofday(&ini,NULL);

    for (i = id + 1; i <= N; i += numprocs){
        x = 1 + ((double)i * passo);
        localSum += 0.5*(1/x+1/(x+passo));
    }
    localSum *= passo;
    MPI_Reduce(&localSum, &globalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (id == 0) {
        printf("ln 2: %.5f\n", globalSum);
        gettimeofday(&fim,NULL);
        tmili = (double)(1000.0*(fim.tv_sec-ini.tv_sec)+(fim.tv_usec-ini.tv_usec)/1000.0);
        printf("%d processos: tempo - %.3lf ms\n", numprocs, tmili);
    }

    MPI_Finalize();
    return 0;
}

//ssh @172.20.22.5
