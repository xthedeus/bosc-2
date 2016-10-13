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