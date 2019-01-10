#include "generator.h"
#include "error.h"
//#include "circularBuffer.h"


void printEdges(edge *Edges, int edgeAmount){
  int i;
  for (i = 0; i < edgeAmount; i++){
	printf("%ud-%ud\n", Edges[i].u, Edges[i].v);
  }
}

int write_pos = 0;
void circ_buf_write(edges val) {
  sem_wait(free_sem); // writing requires free space
  buf[write_pos] = val;
  sem_post(used_sem); // space is used by written data
  write_pos = (write_pos + 1) % MAX_DATA;
}


int main(int argc, char ** argv)
{
  int c;
  int edgeAmount;
  unsigned int vertexAmount = 0;
  edges solution;

  while((c = getopt(argc,argv,"")) != -1)
	switch(c) {
	case '?':
	  error_exit("invalid argument provided. only positional arguments allowed");
	default:
	  break;
	}

  int shmfd = shm_open(SHM_NAME, O_RDWR, 0600);
  if (shmfd == -1)  error_errno_exit("cannot open shared memory.");


  struct myshm *myshm;
  myshm = mmap(NULL, sizeof(*myshm), PROT_READ | PROT_WRITE, // need write to initialize all edges
			   MAP_SHARED, shmfd, 0);

  if (myshm == MAP_FAILED)  error_errno_exit("cannot map shared memory.");

  buf = myshm->data; //let buf point to circular buffer

  //set time seed for random number generator
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  srand((time_t)ts.tv_nsec); // using nano-seconds instead of seconds

  int optI = optind;
  edgeAmount = argc - optI;
  edge initEdges[edgeAmount];
  if(optI < argc){
	int i;
	for (i = 0; i < argc-optI; i++)
	  extractEdgeFromString( *(argv + optI + i), &initEdges[i], &vertexAmount);
	vertexAmount++; //because vertices start at 0
  }else {
	error_exit("invalid pattern.");
  }
  //verticeAmount = getAmountVertices(edges, edgeAmount);
  unsigned int vertices[vertexAmount];

  used_sem = sem_open(SEM_2, 0);
  if(used_sem == SEM_FAILED)  error_errno_exit("Opening semaphore used_sem failed.");
  free_sem = sem_open(SEM_1, 0);
  if(free_sem == SEM_FAILED)  error_errno_exit("Opening semaphore free_sem failed.");

  unsigned int solutionSize;
  while(myshm->state){
	randperm(vertexAmount, vertices);
	solutionSize = generateSolution(vertices, vertexAmount, initEdges, edgeAmount, solution.edges);
	if(solutionSize == 9) continue; // a solution with more than 8 edges was found
	solution.amount=solutionSize;
	circ_buf_write(solution);
  }


  // unmap shared memory:
  if (munmap(myshm, sizeof(*myshm)) == -1)  error_errno_exit("cannot munmap shared memory.");
  //close(shm)
  if (close(shmfd) == -1)  error_errno_exit("cannot close shared memory.");
  //closing semaphores
  if (sem_close(free_sem) == -1)  error_errno_exit("cannot close free_sem semaphore.");
  if (sem_close(used_sem) == -1)  error_errno_exit("cannot close used_sem semaphore.");
}
