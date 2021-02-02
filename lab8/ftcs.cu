#include <stdio.h>
#include <stdlib.h>

#define tam 1.0
#define dx 0.00001
#define dt 0.000001
#define T  0.01
#define kappa 0.000045
#define MAX_THREADS 512

__global__ void run_ftcs(double *u, double *prev, double k, double t, double x, long int n)
{
	long int i =  blockDim.x * blockIdx.x + threadIdx.x;
	if(i>0 && i<(n-1))
		u[i] = prev[i] + k*t/(x*x)*(prev[i-1]-2*prev[i]+prev[i+1]);
}

__global__ void borders(double *u, long int n)
{
	long int i =  blockDim.x * blockIdx.x + threadIdx.x;
	if(i==1) u[0] = u[n] = 0.0;
}


int main(void) 
{

    /********* HOST VARIABLES *********/
    double *tmp, *u, *u_prev;
    double x = 0, t = 0;
    long int i, n, maxloc = 0, blocks;

    n = tam/dx;
    u = malloc((n+1)*sizeof(double));
    u_prev = malloc((n+1)*sizeof(double));
    blocks = ceil((n+1)/MAX_THREADS);

    /********* CUDA VARIABLES *********/
    double *d_u, *d_u_prev;

    cudaMalloc((void **)&d_u, n + 1);
	cudaMalloc((void **)&d_u_prev, n + 1);
    cudaMemcpy(d_u_prev, u_prev, n + 1, cudaMemcpyHostToDevice);

    
    /********* FILLING U_PREV *********/
  
    for (i = 0; i < n + 1; i++) {
        if (x <= 0.5) u_prev[i] = 200*x;
        else        u_prev[i] = 200*(1.-x);
        x += dx;
    }

  

    
    while (t < T) {
        x = dx;
        run_ftcs<<<nblocks,THREADS_PER_BLOCK>>>(d_u, d_u_prev, kappa, dt, dx, n+1);
        cudaDeviceSynchronize();
        borders<<<nblocks,THREADS_PER_BLOCK>>>(u_u, n);

        tmp = u_prev; 
        u_prev = u;
        u = tmp; /* troca entre ponteiros */
        t += dt;
    }

    cudaMemcpy(u, d_u, n + 1, cudaMemcpyDeviceToHost);

  /* Calculando o maior valor e sua localizacao */
 
    for (i = 1; i < n + 1; i++) 
        if (u[i] > u[maxloc]) maxloc = i;
    
    printf("Maior valor u[%ld] = %g\n", maxloc, u[maxloc]);


    free(u);
    free(u_prev);
    cudaFree(u_u);
    cudaFree(u_prev_prev);

    return 0;

}
