/******************************************************************************
   main.c

   Implementation of a simple FIFO buffer as a linked list defined in list.h.

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "list.h"

#define NTHREADS 2

// FIFO list;
List *fifo;
typedef struct Job
{
  int from;
  int to;
} Job;
double sum; /* this data is shared by the thread(s) */
double result;
void *runner(void *param); /* threads call this function */
int main(int argc, char *argv[])
{
  int n = atoi(argv[1])/NTHREADS;
  pthread_t thread_id[NTHREADS];
  Job jobs[NTHREADS];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
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
    threadtask.from = i*n+1;
    threadtask.to = n+(i*n);
    jobs[i] = threadtask;
    pthread_create(&thread_id[i],&attr,runner,&jobs[i]);
  }
  for (i = 0; i < NTHREADS; ++i)
  {
    printf("waiting for thread %d\n", i);
    pthread_join(thread_id[i],NULL); 
    printf("sum = %f\n",sum);
    result+=sum;
  }
  printf("Result: %f\n", result);
}

/* The thread will begin control in this function */ 
void *runner(void *param)
{
  int i;
  Job *job = param;
  printf("From: %d\n", job->from);
  printf("To: %d\n", job->to);
  sum = 0;
  for (i = job->from; i <= job->to; i++) {
    sum += sqrt(i);
  }
  pthread_exit(0);
}

/*int main(int argc, char* argv[])
{
  fifo = list_new();

  list_add(fifo, node_new_str("s1"));
  list_add(fifo, node_new_str("s2"));

  Node *n1 = list_remove(fifo);
  if (n1 == NULL) { printf("Error no elements in list\n"); exit(-1);}
  Node *n2 = list_remove(fifo);
  if (n2 == NULL) { printf("Error no elements in list\n"); exit(-1);}
  printf("%s\n%s\n", n1->elm, n2->elm);

  return 0;
}*/

