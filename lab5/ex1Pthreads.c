#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define TP 4
#define TC 2
#define SIZE 100
#define VAZIO 0


sem_t mutex_prod; //1
sem_t mutex_cons; //1

//pthread_barrier_t barreira;

int i; // offset de escrita p/ produtores
int j; // offset de leitura p/ consumidores

int buffer[SIZE];
int count = 0;
int negativosGerados = 0;



int gera_dado(void)
{
	return rand()%100000000 + 1;
}

int eh_primo(int x)
{
    if(x < 0) return 0;
    for(int i = 2; i <= 100000000; i++){
        if(x != i && x % i == 0){
           printf("%d não é primo\n", x);
            fflush(stdout);
            return 0;
        } 
    }
    printf("%d é primo\n", x);
    fflush(stdout);
    return 1;
}

void *produtor(void *args)
{
	int pid = *(int *)args;
	int item;
	
	while(1){
		item = gera_dado();
        
		
		sem_wait(&mutex_prod);
		if(count > SIZE) {
			while (negativosGerados < TC){
				buffer[i] = -1;
				i = (i+1)%SIZE;
				negativosGerados++;
				printf("-> TP %d gerou num negativo\n", pid);
			}
			sem_post(&mutex_prod);
			break;
        }
        count++;
		buffer[i] = item;
		printf("TP_%d: produziu buffer[%d] = %d\n", pid, i, item);
		i = (i+1)%SIZE;
		sem_post(&mutex_prod);
		
       
	}
    printf("---------> TP_%d: finalizou\n", pid);
    pthread_exit(NULL);
}

void *consumidor(void *args)
 {
	int cid = *(int *)args;
	int item;
	while(1) {
		
		sem_wait(&mutex_cons);
		
		if(buffer[j] == VAZIO){
			while (buffer[j] == VAZIO){
				//printf("vazio\n");
				j = (j+1)%SIZE;
			}		
		}
		item = buffer[j];
		
		if(item < 0) {
			printf("---------- num negativo-------------- em %d\n", cid);
			buffer[j] = VAZIO;
			j = (j+1)%SIZE;
			sem_post(&mutex_cons);
            break;
		}
		buffer[j] = VAZIO;		
        eh_primo(item);
		j = (j+1)%SIZE;
		sem_post(&mutex_cons);
		
        
	}
    printf("-------->TC_%d encontrou numero negativo e foi desligada\n", cid);
    pthread_exit(NULL);
}




void inicializa(void)
{

	sem_init(&mutex_prod, 0, 1);
	sem_init(&mutex_cons, 0, 1);
	i = 0;
	j = 0;
	srand(time(NULL));
	for (int k = 0; k < SIZE; k++){
		buffer[k] = VAZIO;
		
	}
	
}



int main(void) 
{
    inicializa();
    pthread_t tp[TP], tc[TC];
    int p, c, pid[TP], cid[TC];
    for (p=0; p<TP; p++){
		pid[p] = p+1;
		pthread_create(&tp[p], NULL, produtor, &pid[p]);
	}
	for (c=0; c<TC; c++) {
		cid[c] = c+1;
		pthread_create(&tc[c], NULL, consumidor, &cid[c]);
	}
	for (p=0; p<TP; p++) {
		pthread_join(tp[p], NULL);
	}
	for (c=0; c<TC; c++) {
		pthread_join(tc[c], NULL);
	}
    
    return(0);
}

//a 19s 13s 11s
//b 11s 7s 7s
//c 13s 11s 13s