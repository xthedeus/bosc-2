#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct state {
  int *resource;
  int *available;
  int **max;
  int **allocation;
  int **need;
} State;
int **allocate_double_matrix(int m, int n);
// Global variables
int m, n;
State *s = NULL;

// Mutex for access to state.
pthread_mutex_t state_mutex;

/* Random sleep function */
void Sleep(float wait_time_ms)
{
  // add randomness
  wait_time_ms = ((float)rand())*wait_time_ms / (float)RAND_MAX;
  usleep((int) (wait_time_ms * 1e3f)); // convert from ms to us
}

int **allocate_double_matrix(int m, int n)
{
 /* Allocate memory for the elements */
 int *mem = malloc(m * n * sizeof(int));
 /* Allocate memory for the matrix array */
 int **mat = malloc(m * sizeof(int *));
 /* Setup array */
 if (mem != NULL && mat != NULL) {
 int i;
 for (i = 0; i < m; ++i) {
 mat[i] = &mem[i * n];
 }
 } else {
 printf("Out of memory!\n"); exit(-1);
 }
 return mat;
}

/* Allocate resources in request for process i, only if it 
   results in a safe state and return 1, else return 0 */
int resource_request(int i, int *request)
{
  printf("Start resource request\n");
  int *work = s->available;
  int finish[m];
  int j;
  bool safe = true;
  a:for (j = 0; j < n; ++j)
    {
      if(!finish[i]) {
        safe = false;
      
        if(request[j] <= work[j]) {
          finish[i] = true;
          work[j] = work[j]+s->allocation[i][j];
          //s->available[j] -= request[j];
          //s->allocation[k][j] += request[j];
          //s->need[k][j] -= request[j];
          goto a;
        }
      }
    }
  printf("Safe: %d\n", safe);
  return safe;
}

/* Release the resources in request for process i */
void resource_release(int i, int *request)
{
  int j;
  for (j = 0; j < n; ++j)
  {
    s->allocation[i][j] -= request[j];
  }
  
}

/* Generate a request vector */
void generate_request(int i, int *request)
{
  int j, sum = 0;
  while (!sum) {
    for (j = 0;j < n; j++) {
      request[j] = s->need[i][j] * ((double)rand())/ (double)RAND_MAX;
      sum += request[j];
    }
  }
  printf("Process %d: Requesting resources.\n",i);
}

/* Generate a release vector */
void generate_release(int i, int *request)
{
  int j, sum = 0;
  while (!sum) {
    for (j = 0;j < n; j++) {
      request[j] = s->allocation[i][j] * ((double)rand())/ (double)RAND_MAX;
      sum += request[j];
    }
  }
  printf("Process %d: Releasing resources.\n",i);
}

/* Threads starts here */
void *process_thread(void *param)
{
  /* Process number */
  int i = (int) (long) param, j;
  /* Allocate request vector */
  int *request = malloc(n*sizeof(int));
  while (1) {
    /* Generate request */
    generate_request(i, request);
    while (!resource_request(i, request)) {
      /* Wait */
      Sleep(100);
    }
    /* Generate release */
    generate_release(i, request);
    /* Release resources */
    resource_release(i, request);
    /* Wait */
    Sleep(1000);
  }
  free(request);
}

int main(int argc, char* argv[])
{
  /* Get size of current state as input */
  int i, j;
  printf("Number of processes: ");
  scanf("%d", &m);
  printf("Number of resources: ");
  scanf("%d", &n);

  /* Allocate memory for state */
  int *res = malloc(n * sizeof(int *));
  int *avail = malloc(n * sizeof(int *));
  int **max = allocate_double_matrix(m,n);
  int **allo = allocate_double_matrix(m,n);
  int **need = allocate_double_matrix(m,n);
  State s1 = { res, avail, max, allo, need };
  s = &s1;
  if (s == NULL) { printf("\nYou need to allocate memory for the state!\n"); exit(0); };

  /* Get current state as input */
  printf("Resource vector: ");
  for(i = 0; i < n; i++)
    scanf("%d", &s->resource[i]);
  printf("Enter max matrix: ");
  for(i = 0;i < m; i++)
    for(j = 0;j < n; j++)
      scanf("%d", &s->max[i][j]);
  printf("Enter allocation matrix: ");
  for(i = 0; i < m; i++)
    for(j = 0; j < n; j++) {
      scanf("%d", &s->allocation[i][j]);
    }
  printf("\n");

  /* Calcuate the need matrix */
  for(i = 0; i < m; i++)
    for(j = 0; j < n; j++)
      s->need[i][j] = s->max[i][j]-s->allocation[i][j];

  /* Calcuate the availability vector */
  for(j = 0; j < n; j++) {
    int sum = 0;
    for(i = 0; i < m; i++)
      sum += s->allocation[i][j];
    s->available[j] = s->resource[j] - sum;
  }

  /* Output need matrix and availability vector */
  printf("Need matrix:\n");
  for(i = 0; i < n; i++)
    printf("R%d ", i+1);
  printf("\n");
  for(i = 0; i < m; i++) {
    for(j = 0; j < n; j++)
      printf("%d  ",s->need[i][j]);
    printf("\n");
  }
  printf("Availability vector:\n");
  for(i = 0; i < n; i++)
    printf("R%d ", i+1);
  printf("\n");
  for(j = 0; j < n; j++)
    printf("%d  ",s->available[j]);
  printf("\n");

  /* If initial state is unsafe then terminate with error */

  /* Seed the random number generator */
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
  
  /* Create m threads */
  pthread_t *tid = malloc(m*sizeof(pthread_t));
  for (i = 0; i < m; i++)
    pthread_create(&tid[i], NULL, process_thread, (void *) (long) i);
  
  /* Wait for threads to finish */
  pthread_exit(0);
  free(tid);

  /* Free state memory */
}
