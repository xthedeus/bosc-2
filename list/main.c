/******************************************************************************
   main.c

   Implementation of a simple FIFO buffer as a linked list defined in list.h.

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include "list.h"
#include <unistd.h>
#include <semaphore.h>

#define NTHREADS 4

struct timeval tvalBefore, tvalAfter;
struct sched_param param;

// FIFO list;
List *fifo;
typedef struct Job
{
  int from;
  int to;
} Job;

double sum; /* this data is shared by the thread(s) */
void *runner(void *param); /* threads call this function */
void *produce(void *param);
void *consume(void *param);
void Sleep(float wait_time_ms);

sem_t semvar;

int numOfProductsToProduce;
int producedProducts;
int bufferSize;


// TEST OF LIST IMPLEMENTATION //
/*
int main(int argc, char *argv[])
{
  gettimeofday( &tvalBefore, NULL );

  int n = atoi(argv[1])/NTHREADS;

  pthread_t thread_id[NTHREADS];
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  Job jobs[NTHREADS];

  fifo = list_new();

  int i;

  if(argc != 2) {
    fprintf(stderr,"usage: a.out <integer value>\n"); 
    return -1;
  }

  if (atoi(argv[1]) < 0) {
    fprintf(stderr,"%d must be >= 0\n",atoi(argv[1])); 
    return -1;
  }

  for (i = 0; i < NTHREADS; ++i)
  {
    Job threadtask;
    if(i==0) {
      threadtask.from = i*n;
    } else {
      threadtask.from = i*n+1;
    }
    threadtask.to = n+(i*n);
    
    jobs[i] = threadtask;
    pthread_create(&thread_id[i],&attr,runner,&jobs[i]);
  }

  for (i = 0; i < NTHREADS; ++i)
  {
    pthread_join(thread_id[i],NULL); 
  }
  int k = 0;
  while(list_remove(fifo)->next != NULL) k++;
  printf("Expected length: %d\n", atoi(argv[1]));
  printf("Actual length: %d\n", k);
  //printf("Result: %f\n", sum);
  //gettimeofday( &tvalAfter, NULL );
  //printf("Time: %ld milliseconds\n",(((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L + tvalAfter.tv_usec) - tvalBefore.tv_usec) / 1000);
}

/* The thread will begin control in this function */ 
void *runner(void *param)
{
  int i;
  Job *job = param;
  for (i = job->from; i <= job->to; i++) {
    list_add(fifo, node_new_str("" + i));
  }
  pthread_exit(0);
}

int main(int argc, char *argv[]) 
{
  int numOfProducers = atoi(argv[1]);
  int numOfConsumers = atoi(argv[2]);
  bufferSize = atoi(argv[3]);
  numOfProductsToProduce = atoi(argv[4]);

  int numOfThreads = numOfProducers+numOfConsumers;
  producedProducts = 0;

  fifo = list_new();

  sem_init(&semvar, 0, 1);

  pthread_t thread_id[numOfThreads];
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  int jobs[numOfThreads];

  int i;
  int k;
  for (i = 0; i < numOfProducers; ++i)
  {
    int num = i;
    jobs[i] = num;
    pthread_create(&thread_id[i],&attr,produce,&jobs[i]);
  }

  for (k = numOfProducers; k < (numOfConsumers+numOfProducers); ++k)
  {
    int num = k;
    jobs[k] = num;
    pthread_create(&thread_id[k],&attr,consume,&jobs[k]);
  }

  for (i = 0; i < numOfProducers; ++i)
  {
    pthread_join(thread_id[i],NULL); 
  }

  for (k = numOfProducers; k < (numOfConsumers+numOfProducers); ++k)
  {
    pthread_join(thread_id[k],NULL); 
  }

}

void *produce(void *param) 
{

  if(producedProducts < numOfProductsToProduce && fifo->len < bufferSize)
  {
    sem_wait(&semvar);
    int producerNum = *(int*)param;
    char producedName[24];
    sprintf(producedName, "%d", producedProducts);
    //Node *n = node_new_str(producedName);
    list_add(fifo, node_new_str(producedName));
    printf("Producer %d produced Item_%s. Items in buffer: %d (out of %d)\n", producerNum, producedName, fifo->len, bufferSize);
    producedProducts += 1;
    sem_post(&semvar);
  }

  if(producedProducts < numOfProductsToProduce)
  {
    Sleep(5000);
    produce(param);
  } 
  else 
  {
    pthread_exit(0);
  }

}

void *consume(void *param)
{
  if(fifo->len > 0)
  {
    sem_wait(&semvar);
    int consumerNum = *(int*)param;
    Node *n = list_remove(fifo);
    printf("Consumer %d consumed Item_%s. Items in buffer: %d (out of %d)\n", consumerNum, (char *)n->elm, fifo->len, bufferSize);
    sem_post(&semvar);
  }

  if(producedProducts < numOfProductsToProduce)
  {
    Sleep(5000);
    consume(param);
  } 
  else 
  {
    pthread_exit(0);
  }
  
}

/* Random sleep function */
void Sleep(float wait_time_ms)
{
  wait_time_ms = ((float)rand())*wait_time_ms / (float)RAND_MAX;
  usleep((int) (wait_time_ms * 1e3f)); // convert from ms to us
}