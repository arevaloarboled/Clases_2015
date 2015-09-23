#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


/*
	El programa original presentaba una zona crítica a la cual accedían todos los hilos sin ninguna
	dificultad, cambiando el valor de la variable sum. Se agregó un mutex que controle el acceso 
	a la variable sum dando como resultado lo esperado por el programa, que es que cada hilo sume
	1000 al valor actual de la variable
*/

/* Define global data where everyone can see them */
#define NUMTHRDS 8
#define VECLEN 100000
int *a, *b; 
long sum=0;
pthread_mutex_t bloqueo;

void *dotprod(void *arg)
{
	/* 
	 * Each thread works on a different set of data.
	 * The offset is specified by the arg parameter. 
	 * The size of the data for each thread is indicated by VECLEN.
	 */
	int i, start, end, offset, len;
	long tid = (long)arg;
	offset = tid;
	len = VECLEN;
	start = offset*len;
	end   = start + len;

	/* Perform my section of the dot product */
	printf("thread: %ld starting. start=%d end=%d\n",tid,start,end-1);
	pthread_mutex_lock(&bloqueo);
	for (i=start; i<end ; i++) 
	  sum += (a[i] * b[i]);
	pthread_mutex_unlock(&bloqueo);
	printf("thread: %ld done. Global sum now is=%li\n",tid,sum);

	pthread_exit((void*) 0);
}

int main (int argc, char *argv[])
{
	long i;
	void *status;
	pthread_t threads[NUMTHRDS];
	pthread_mutex_init(&bloqueo,NULL);

	/* Assign storage and initialize values */
	a = (int*) malloc (NUMTHRDS*VECLEN*sizeof(int));
	b = (int*) malloc (NUMTHRDS*VECLEN*sizeof(int));
	  
	for (i=0; i<VECLEN*NUMTHRDS; i++)
	  a[i]= b[i]=1;

	/* Create threads as joinable, each of which will execute the dot product
	 * routine. Their offset into the global vectors is specified by passing
	 * the "i" argument in pthread_create().
	 */
	for(i=0; i<NUMTHRDS; i++) 
	   pthread_create(&threads[i], NULL, dotprod, (void *)i); 

	/* Wait on the threads for final result */
	for(i=0; i<NUMTHRDS; i++) 
	  pthread_join(threads[i], &status);

	pthread_mutex_destroy(&bloqueo);
	/* After joining, print out the results and cleanup */
	printf ("Final Global Sum=%li\n",sum);
	free (a);
	free (b);

	pthread_exit(NULL);
}