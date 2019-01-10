#include "supervisor.h"
#include "error.h"

void printEdges(edge *Edges, int edgeAmount){
  int i;
  printf("Solution with %d edges: ", edgeAmount);
  for (i = 0; i < edgeAmount; i++){
	printf("%d-%d ", Edges[i].v, Edges[i].u);
  }
  printf("\n");
}


int read_pos = 0;
edges circ_buf_read() {
  if(sem_wait(used_sem) == -1) error_errno_exit("cannot wait used_sem."); // reading requires data (used space)
  edges val;
  val = buf[read_pos];
  sem_post(free_sem); // reading frees up space
  read_pos = (read_pos + 1) % MAX_DATA;
  return val;
}


//signal handling
volatile sig_atomic_t quit = 0;
void handle_signal(int signal) { quit = 1; }

int main(int argc, char **argv)
{

  //check if no arguments are provided
  if(argc > 1)  error_exit("do not provide arguments. arguments are not allowed!");

  // create and/or open the shared memory object:
  int shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600);
  if (shmfd == -1)  error_errno_exit("cannot open shared memory.");

  // set the size of the shared memory:
  if (ftruncate(shmfd, sizeof(struct myshm)) < 0)  error_errno_exit("cannot resize shared memory.");
  // map shared memory object:
  struct myshm *myshm;
  myshm = mmap(NULL, sizeof(*myshm), PROT_READ | PROT_WRITE, // need write to initialize all edges
			   MAP_SHARED, shmfd, 0);

  if (myshm == MAP_FAILED)  error_errno_exit("cannot map shared memory.");

  //init buf with 0-0 edges
  buf = myshm->data;
  int tt;
  int ss;
  for (tt = 0; tt < MAX_DATA; tt++) {
	for (ss = 0; ss < MAX_SOL_EDGES; ss++) {
	  buf[tt].edges[ss].v = 0;
	  buf[tt].edges[ss].u = 0;
	}
  }

  //initialize myshm amount to 9 (can only hold 8 so any value over 9 would be ok)
  myshm->edgeAmount = 9;
  //initialize state -> 1 equals running
  myshm->state = 1;
  //initially sets amount of generators in shm to 0
  myshm->generatorAmount = 0;
  //write position initialize for all generators to 0;
  myshm->write_pos = 0;

  //signal handling - SIGINT SIGTERM
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handle_signal;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  //sem_open - free, used
  free_sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0600, MAX_DATA);
  if(free_sem == SEM_FAILED)  error_errno_exit("Opening semaphore free_sem failed.");
  used_sem = sem_open(SEM_2, O_CREAT | O_EXCL, 0600, 0);
  if(used_sem == SEM_FAILED)  error_errno_exit("Opening semaphore used_sem failed.");
  //sem_open - write mutex
  write_sem = sem_open(SEM_3, O_CREAT | O_EXCL, 0600, 1);
  if(write_sem == SEM_FAILED)  error_errno_exit("Opening semaphore write_sem failed.");
  //sem_open - alive


  while(!quit){
	edges temp;
	temp = circ_buf_read();
	if(temp.amount == 0){
	  myshm->edgeAmount = temp.amount;
	  printf("The graph is acyclic!\n");
	  //now terminate supervisor and all other generators
	  //myshm->state = 0;
	  break;
	} else if(temp.amount < myshm->edgeAmount){
	  myshm->edgeAmount = temp.amount;
	  printEdges(temp.edges, temp.amount);
	}
  }

  //int generatorDef = myshm->generatorAmount * (-1);
  //  alive_sem = sem_open(SEM_4, O_CREAT | O_EXCL, 0600, 1);
  //if(alive_sem == SEM_FAILED)  error_errno_exit("Opening semaphore alive_sem failed.");
  //  printf("Terminating all generators\n");
  myshm->state = 0; // setting state variable to 0 so all generators leave while loop

  //wait for all generators to terminate before unmapping shm
  //if(sem_wait(alive_sem) == -1) error_errno_exit("waiting for alive_sem.");
  printf("All generators terminated, now terminating supervisor\n");

  // unmap shared memory:
  if (munmap(myshm, sizeof(*myshm)) == -1)  error_errno_exit("cannot munmap shared memory.");
  //close shared memeory
  if (close(shmfd) == -1)  error_errno_exit("cannot close shared memory.");
  // remove shared memory object:
  if (shm_unlink(SHM_NAME) == -1) error_errno_exit("cannot unlink shared memory.");

  //closing semaphores
  if (sem_close(free_sem) == -1)  error_errno_exit("cannot close free_sem semaphore.");
  if (sem_close(used_sem) == -1)  error_errno_exit("cannot close used_sem semaphore.");
  if (sem_close(write_sem) == -1)  error_errno_exit("cannot close write_sem semaphore.");
  //  if (sem_close(alive_sem) == -1)  error_errno_exit("cannot close alive_sem semaphore.");
  //unlink semaphores
  if(sem_unlink(SEM_1) == -1)  error_errno_exit("cannot unlink semaphore SEM_1");
  if(sem_unlink(SEM_2) == -1)  error_errno_exit("cannot unlink semaphore SEM_2");
  if(sem_unlink(SEM_3) == -1)  error_errno_exit("cannot unlink semaphore SEM_3");
  //if(sem_unlink(SEM_4) == -1)  error_errno_exit("cannot unlink semaphore SEM_4");

}
