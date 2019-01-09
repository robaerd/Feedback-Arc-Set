#include "supervisor.h"
#include "circularBuffer.h"


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
  myshm = mmap(NULL, sizeof(*myshm), PROT_READ | PROT_WRITE, // need write to initialize all edges
			   MAP_SHARED, shmfd, 0);

  if (myshm == MAP_FAILED) {
	fprintf(stderr, "error: shm map failed");
	exit(EXIT_FAILURE);
	}
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

  //  edges test = circ_buf_read();


  // unmap shared memory:
  if (munmap(myshm, sizeof(*myshm)) == -1){
		fprintf(stderr, "ERROR in munmap\n");
	exit(EXIT_FAILURE);
	// error
  }

  if (close(shmfd) == -1){
	fprintf(stderr, "ERROR closing shmfd\n");
	exit(EXIT_FAILURE);
  }
 
  // remove shared memory object:
  if (shm_unlink(SHM_NAME) == -1){
	fprintf(stderr, "ERROR int unlink\n");
	exit(EXIT_FAILURE);
	// error
  }
}
