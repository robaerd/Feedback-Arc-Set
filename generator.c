#include "generator.h"
#include "error.h"


static int shmfd;
static struct myshm *myshm;

static void allocate_resources(void);

static void free_resources(void);

static circ_buf_write(edges, int);


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

  if (atexit(free_resources) != 0) error_exit("atexit: register function free_ressources.");

  allocate_resources(); // allocates sem and shm

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

  //initialize vertices in ascending order from 0 to vertexAmount
  unsigned int vertices[vertexAmount];
  unsigned int i;
  for(i = 0; i < vertexAmount; i++)
	vertices[i] = i;

  unsigned int solutionSize;

  while(myshm->state == 1){
	randperm(vertexAmount, vertices);
	solutionSize = generateSolution(vertices, vertexAmount, initEdges, edgeAmount, solution.edges);
	if(solutionSize == 9) continue; // a solution with more than 8 edges was found
	solution.amount=solutionSize;
	if(sem_wait(write_sem) == -1){
	  if (errno == EINTR) // interrupted by signal?
		continue;
	  error_errno_exit("waiting for write_sem"); // implement wait error handling with signal errno ==eintr
	}
	myshm->write_pos = circ_buf_write(solution, myshm->write_pos);
	if(sem_post(write_sem) == -1) error_errno_exit("incrementing write_sem"); //error handling
  }

  exit(EXIT_SUCCESS);
}

void allocate_resources(void)
{
  shmfd = shm_open(SHM_NAME, O_RDWR, 0600);
  if (shmfd == -1)  error_errno_exit("cannot open shared memory.");

  myshm = mmap(NULL, sizeof(*myshm), PROT_READ | PROT_WRITE, // need write to initialize all edges
			   MAP_SHARED, shmfd, 0);

  if (myshm == MAP_FAILED)  error_errno_exit("cannot map shared memory.");

  free_sem = sem_open(SEM_1, 0);
  if(free_sem == SEM_FAILED)  error_errno_exit("Opening semaphore free_sem failed.");
  used_sem = sem_open(SEM_2, 0);
  if(used_sem == SEM_FAILED)  error_errno_exit("Opening semaphore used_sem failed.");
  write_sem = sem_open(SEM_3, 0);
  if(write_sem == SEM_FAILED)  error_errno_exit("Opening semaphore write_sem failed.");
}

void free_resources(void)
{
  // unmap shared memory:
  if (munmap(myshm, sizeof(*myshm)) == -1)  error_errno_exit("cannot munmap shared memory.");
  //close(shm)
  if (close(shmfd) == -1)  error_errno_exit("cannot close shared memory.");
  //closing semaphores
  if (sem_close(free_sem) == -1)  error_errno_exit("cannot close free_sem semaphore.");
  if (sem_close(used_sem) == -1)  error_errno_exit("cannot close used_sem semaphore.");
  if (sem_close(write_sem) == -1)  error_errno_exit("cannot close write_sem semaphore.");
}

//int write_pos = 0;
int circ_buf_write(edges val, int pos)
{
  sem_wait(free_sem); // writing requires free space
  buf[pos] = val;
  sem_post(used_sem); // space is used by written data
  return (pos + 1) % MAX_DATA;
}
