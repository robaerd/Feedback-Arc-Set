#include <semaphore.h>

#define SHM_NAME "/11708475-shm" /* shared memory name */
#define MAX_DATA (60)            /* size of circular buffer */
#define MAX_SOL_EDGES (8)        /* max amount of edges in a feedback arc set solution */

/* semaphore names */
#define SEM_1 "/11708475-free_sem"
#define SEM_2 "/11708475-used_sem"
#define SEM_3 "/11708475-write-sem"

/**
 * A structure to represent an edge
 */
typedef struct edge {
  unsigned int v; /** representing vertex indice where an directed edge is going out */
  unsigned int u; /** correspoinding vertex indice where the directed edge from u is coming in */
} edge;

/**
 * A structure to represent an Feedback Arc Set
 */
typedef struct edgeSol {
  edge edges[MAX_SOL_EDGES]; /** a feedback arc set solution */
  unsigned int amount;       /** amount of edges in the solution */
} edges;

/**
 * Shared memory. Currently has a size of 4kB
 */
struct myshm {
  unsigned int state;        /** state of supervisor. 1 if running -> generator keeps producing new solutions.
                                 0 if supervisor is about to terminate -> all generator should also terminate */
  unsigned int edgeAmount;   /** amount of edges the most optimal solution found so far consists of  */
  edges data[MAX_DATA];      /** circular edges buffer */
  int write_pos;             /** next free write position in buffer */
};

//circular buffer
edges *buf; /* points to shared memory mapped with mmap(2) */
sem_t *free_sem, /* tracks free space, initialized to BUF_LEN */
      *used_sem, /* tracks used space, initialized to 0 */
      *write_sem; /* for write mutex, only one generator can write at a time*/




