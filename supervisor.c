#include "supervisor.h"
//#include "circularBuffer.h"

void printEdges(edge *Edges, int edgeAmount){
  int i;
  printf("Solution with %d edges: ", edgeAmount);
  for (i = 0; i < edgeAmount; i++){
	printf("%d-%d ", Edges[i].u, Edges[i].v);
  }
  printf("\n");
}


int read_pos = 0;
edges circ_buf_read() {
  sem_wait(used_sem); // reading requires data (used space)
  edges val;
  val = buf[read_pos];
  //memcpy(&val, &buf[read_pos], sizeof(buf[read_pos]));
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
	fprintf(stderr, "error: shm map failed\n");
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

  //initialize myshm amount to 9 (can only hold 8 so any value over 9 would be ok)
  myshm->edgeAmount = 9;
  //initialize state - 1 equals running
  myshm->state = 1;

  //signal handling - SIGINT SIGTERM
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handle_signal;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  //sem_open()
  free_sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0600, MAX_DATA);
  used_sem = sem_open(SEM_2, O_CREAT | O_EXCL, 0600, 0);


  while(!quit){
	edges temp;
	temp = circ_buf_read();
	if(temp.amount == 0){
	  myshm->edgeAmount = temp.amount;
	  printf("The graph is acyclic!\n");
	  //now terminate supervisor and all other generators
	  myshm->state = 0;
	  break;
	} else if(temp.amount < myshm->edgeAmount){
	  myshm->edgeAmount = temp.amount;
	  printEdges(temp.edges, temp.amount);
	  //now terminate supervisor and all other generator
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

  //closing semaphores
  if (sem_close(free_sem) == -1){
	fprintf(stderr, "ERROR closing free_sem\n");
	exit(EXIT_FAILURE);
	//errno
  }
  if (sem_close(used_sem) == -1){
	fprintf(stderr, "ERROR closing used_sem\n");
	exit(EXIT_FAILURE);
	//errno
  }
  //unlink semaphores
  if(sem_unlink(SEM_1) == -1){
	fprintf(stderr, "ERROR unlinking free_sem\n");
	exit(EXIT_FAILURE);
	//errno
  }
    if(sem_unlink(SEM_2) == -1){
	fprintf(stderr, "ERROR unlinking used_sem\n");
	exit(EXIT_FAILURE);
	//errno
  }

}
