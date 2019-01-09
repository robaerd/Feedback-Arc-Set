#include "supervisor.h"


#define SHM_NAME "/myshm"
#define MAX_DATA (50)

//maybe struct array which contains one integer with u and one with v
struct edge{
  unsigned int v;
  unsigned int u;
};

struct edge solutionEdges[8];
//shared mem
struct myshm {
  unsigned int state;
  unsigned int edgeAmount;
  struct edge data[MAX_DATA];
};

//circular buffer
#define BUF_LEN 8
int *buf; // points to shared memory mapped with mmap(2)
sem_t *free_sem, // tracks free space, initialized to BUF_LEN
  *used_sem; // tracks used space, initialized to 0

int read_pos = 0;
int circ_buf_read() {
  sem_wait(used_sem); // reading requires data (used space)
  int val = buf[read_pos];
  sem_post(free_sem); // reading frees up space
  read_pos = (read_pos + 1) % BUF_LEN;
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
	//error handling
  }

  // set the size of the shared memory:
  if (ftruncate(shmfd, sizeof(struct myshm)) < 0){
	//ERROr handling
  }
  // map shared memory object:
  struct myshm *myshm;
  myshm = mmap(NULL, sizeof(*myshm), PROT_READ | PROT_WRITE, //should only read
			   MAP_SHARED, shmfd, 0);

  //test
  //test end

  // unmap shared memory:
  if (munmap(myshm, sizeof(*myshm)) == -1){
	// error
  }
  // remove shared memory object:
  if (shm_unlink(SHM_NAME) == -1){
	// error
  }
}
