//#include "supervisor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
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
} edges;

//shared mem
struct myshm {
  unsigned int state;
  unsigned int edgeAmount;
  edges data[MAX_DATA];
};

//circular buffer
#define BUF_LEN 8
edges *buf; // points to shared memory mapped with mmap(2)
sem_t *free_sem, // tracks free space, initialized to BUF_LEN
  *used_sem; // tracks used space, initialized to 0

int read_pos = 0;
edges circ_buf_read() {
  sem_wait(used_sem); // reading requires data (used space)
  edges val;
  val = buf[read_pos];
  memcpy(&val, &buf[read_pos], sizeof(buf[read_pos]));
  read_pos = (read_pos + 1) % MAX_DATA;
  return val;
}


//signal handling
volatile sig_atomic_t quit = 0;
void handle_signal(int signal) { quit = 1; }

int main(int argc, char **argv)
{
	
  //check if no arguments are provided
  if(argc > 1){
	fprintf(stderr, "ERROR: Arguments provided. Do not provide Arguments.\n");
	exit(EXIT_FAILURE);
  }
	
  // create and/or open the shared memory object:
  int shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600);
  if (shmfd == -1){
	fprintf(stderr, "ERROR int shm open.\n");
	exit(EXIT_FAILURE);
	//error handling
  }
	
  // set the size of the shared memory:
  if (ftruncate(shmfd, sizeof(struct myshm)) < 0){
	fprintf(stderr, "ERROR int ftruncate.\n");
	exit(EXIT_FAILURE);
	//ERROr handling
  }
  // map shared memory object:
  struct myshm *myshm;
  myshm = mmap(NULL, sizeof(*myshm), PROT_READ | PROT_WRITE, //should only read
			   MAP_SHARED, shmfd, 0);
	
  //just a test fix
  //init buf with 0-0 edges
  buf = myshm->data;
  int tt;
  int ss;
  for (tt = 0; tt < MAX_DATA; tt++) {
	for (ss = 0; ss < MAX_SOL_EDGES; ss++) {
	  buf[tt].edges[ss].u = 0;
	  buf[tt].edges[ss].v = 0;
	}
  }

  edges test = circ_buf_read();
	

  // unmap shared memory:
  if (munmap(myshm, sizeof(*myshm)) == -1){
		fprintf(stderr, "ERROR int munmap\n");
	exit(EXIT_FAILURE);
	// error
  }
  // remove shared memory object:
  if (shm_unlink(SHM_NAME) == -1){
	fprintf(stderr, "ERROR int unlink\n");
	exit(EXIT_FAILURE);
	// error
  }
}
