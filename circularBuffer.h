#include <semaphore.h>

#define SHM_NAME "/11708475-shm"
#define MAX_DATA (50)
#define MAX_SOL_EDGES (8)
// struct array which contains one integer with u and one with v
typedef struct edge{
  unsigned int v;
  unsigned int u;
} edge;

typedef struct edgeSol{
  edge edges[MAX_SOL_EDGES];
  unsigned int amount;
} edges;

//shared mem
struct myshm {
  unsigned int state;
  unsigned int edgeAmount;
  edges data[MAX_DATA];
};

//circular buffer
edges *buf; // points to shared memory mapped with mmap(2)
sem_t *free_sem, // tracks free space, initialized to BUF_LEN
  *used_sem; // tracks used space, initialized to 0
