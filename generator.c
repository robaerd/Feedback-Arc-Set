/**
 * @file generator.c
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 07.01.19
 *
 * @brief Generator main program module.
 *
 * The generator program takes a graph as input (set of edges of the graph).
 * Each positional argument is one edge; at least one edge must be given.
 * An edge is specified by a string, with the indices of the two nodes it connects separated by a -.
 * Since the graph is directed, the edge starts at the first node and leads to the second node.
 * The amount of vertices is implicitly provided through the indices in the edges. Where the highest indice equals
 * amount of indices -1 (indices from vertices start at 0).
 *
 * The program repeatedly generates a random solution using an monte-carlo-algroithm.
 * First the vertices are shuffled then all edges (u,v) for which u > v in the ordering are selected.
 * These edges form a feedback arc set. The result is written to a circular buffer.
 * It repeats this procedure until it is notified by the supervisor to terminate.
 *
 * These steps are executed repeatedly and the smallest feedback arc set so far is retained.
 */

#include "generator.h"
#include "error.h" /* provides error_exit() and error_errno_exit() */


static int shmfd; /* file descriptor from shared memory */
static struct myshm *myshm; /* pointer to shared memrory */

/**
 * @brief opens semaphores and shared memory used and maps the shared memory for current generator process
 */
static void allocate_resources(void);

/**
 * @brief unmaps shared memory, closes shared memory file descriptor and closes all opened semaphores
 */
static void free_resources(void);

/**
 * @brief writes feedback arc set into circular buffer. Two semaphores are used to track the used and free space
 *   and wait for free space in case no space is left.
*/
static int circ_buf_write(edges, int);

/**
 * First checks if only positional arguments are provided.
 * The resource_free() function is being called every time the program exit with error or normally
 * Then the resources are alloated (shared memory and semaphores opened)
 * For random values which are needed for the vertices shuffle, the current time in nano-seconds is used as seed-value
 * The edges provided as positional arguments are being stored in an edge struct array, where each edge struct stores two vertices.
 * In the same step the vertice amaount is determined.
 * As long as the supervisor hasn't changed the state in the shared memory, the generator is searching for a solution with < 8 edges
 *   and is writing them into the circular buffer.
 * Only one process can write at a time into the buffer. This is done with a single semaphore. Prevents overwriting from values.
 *
 * @brief reads edges (positional Arguments), determines feedback arc set from topological sorting and writes it to circular buffer
 */
int main(int argc, char ** argv)
{

  int c;
  int edgeAmount; /* amount of edges provided as positional arguments */
  unsigned int vertexAmount = 0;  /* amount of vertices. is implicitly provided by the input edges */
  edges solution; /* a valid feedback arc set is stored here, containing the edges and the edge amount */

  /* checks if only positional arguments are provided, otherwise exit with error */
  while((c = getopt(argc,argv,"")) != -1)
	switch(c) {
	case '?':
	  error_exit("invalid argument provided. only positional arguments allowed");
	default:
	  break;
	}
    
  /* all resources (sem and shm) are being freed on every program exit (error and normal) */
  if (atexit(free_resources) != 0) error_exit("atexit: register function free_ressources.");

  allocate_resources(); /* allocates (opens) semaphores and shared memory */

  buf = myshm->data; //let buf point to circular buffer

  /* set time seed for random number generator */
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  srand((time_t)ts.tv_nsec); // using nano-seconds for uniformer distribution

  /* extracts edges provided as positional arguments into edge struct array.
     also determines vertice amount and edge amount */
  int optI = optind;
  edgeAmount = argc - optI;
  edge initEdges[edgeAmount];
  if(optI < argc){
	int i;
	for (i = 0; i < argc-optI; i++)
	  extractEdgeFromString( *(argv + optI + i), &initEdges[i], &vertexAmount);
	vertexAmount++; //because vertex indices start at 0
  }else {
	error_exit("invalid pattern.");
  }

  /* initialize vertices in ascending order from 0 to vertexAmount */
  unsigned int vertices[vertexAmount];
  unsigned int i;
  for(i = 0; i < vertexAmount; i++)
	vertices[i] = i;

  unsigned int solutionSize; /* edge amount from determined feedback arc set */

  /* as long as state not changed from supervisor, determines valid feedback arc sets with size < 8
       and writes them into circular buffer */
  while(myshm->state == 1){
	randperm(vertexAmount, vertices); // shuffles vertices
	solutionSize = generateSolution(vertices, vertexAmount, initEdges, edgeAmount, solution.edges); // find valid feedback arc set
	if(solutionSize == MAX_SOL_EDGES+1) continue; // a solution with more than 8 edges was found -> throw away
	solution.amount=solutionSize; // writes amount of edges of current solution into edges struct solution
	if(sem_wait(write_sem) == -1){
	  if (errno == EINTR) // interrupted by signal?
		continue;
	  error_errno_exit("waiting for write_sem");
	}
	myshm->write_pos = circ_buf_write(solution, myshm->write_pos); /* write found set into circular buffer */
	if(sem_post(write_sem) == -1) error_errno_exit("incrementing write_sem");
  }

  exit(EXIT_SUCCESS);
}

/**
 * @brief opens semaphores and shared memory used and maps the shared memory for current generator process
 */
void allocate_resources(void)
{
  shmfd = shm_open(SHM_NAME, O_RDWR, 0600);
  if (shmfd == -1)  error_errno_exit("cannot open shared memory.");

  myshm = mmap(NULL, sizeof(*myshm), PROT_READ | PROT_WRITE,
			   MAP_SHARED, shmfd, 0);

  if (myshm == MAP_FAILED)  error_errno_exit("cannot map shared memory.");

  free_sem = sem_open(SEM_1, 0);
  if(free_sem == SEM_FAILED)  error_errno_exit("Opening semaphore free_sem failed.");
  used_sem = sem_open(SEM_2, 0);
  if(used_sem == SEM_FAILED)  error_errno_exit("Opening semaphore used_sem failed.");
  write_sem = sem_open(SEM_3, 0);
  if(write_sem == SEM_FAILED)  error_errno_exit("Opening semaphore write_sem failed.");
}

/**
 * @brief unmaps shared memory, closes shared memory file descriptor and closes all opened semaphores
 */
void free_resources(void)
{
  // unmap shared memory
  if (munmap(myshm, sizeof(*myshm)) == -1)  error_errno_exit("cannot munmap shared memory.");
  //close shared memory
  if (close(shmfd) == -1)  error_errno_exit("cannot close shared memory.");
  //closing semaphores
  if (sem_close(free_sem) == -1)  error_errno_exit("cannot close free_sem semaphore.");
  if (sem_close(used_sem) == -1)  error_errno_exit("cannot close used_sem semaphore.");
  if (sem_close(write_sem) == -1)  error_errno_exit("cannot close write_sem semaphore.");
}

/**
 * @brief writes feedback arc set into circular buffer. Two semaphores are used to track the used and free space
 *   and wait for free space in case no space is left.
 * @param val edges struct to be written into circular buffer
 * @param pos the position in the circular buffer, the edges solutin is written into
 * @return next free position in the circular buffer. Return value is used to set the write_pos var in the shared memory
 */
int circ_buf_write(edges val, int pos)
{
  sem_wait(free_sem); // writing requires free space
  buf[pos] = val;
  sem_post(used_sem); // space is used by written data
  return (pos + 1) % MAX_DATA;
}
