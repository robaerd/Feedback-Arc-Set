/**
 * @file supervisor.c
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 09.01.19
 *
 * @brief Supervisor main program module.
 *
 * The supervisor sets up the shared memory and the semaphores and initializes the circular buffer required for the communication with the generators.
 * It then waits for the generators to write solutions to the circular buffer.
 * The supervisor program takes no arguments.
 * The supervisor reads the solutions from the circular buffer and remembers the best one.
 * Every time a better solution than the previous best solution is found, the supervisor writes the new solution to standard output.
 * If a generator writes a solution with 0 edges to the circular buffer, then the graph is acyclic and the supervisor terminates.
 * Otherwise the supervisor keeps reading results from the circular buffer until it receives a SIGINT or a SIGTERM signal.
 *
 * Before terminating, the supervisor notifies all generators that they should terminate as well.
 */

#include "supervisor.h"
#include "error.h" /* provides error_exit() and error_errno_exit() */


static int shmfd;               /* file descriptor from shared memory */
static struct myshm *myshm;     /* pointer to shared memrory */
static int read_pos = 0;        /* current postition to be read from circular buffer */
volatile sig_atomic_t quit = 0; /* gets set to 1 if SIGTERM or SIGINT occures */

/**
 * @brief sets global volatile quit var to 1 if SIGINT or SIGTERM received -> normal process termination
 */
static void handle_signal(int);

/**
 * @brief prints a feedback arc set solution to stdout in the following form: v1-u1 v2-u2 ...
 */
static void printEdges(edges);

/**
 * @brief reads next position in circular buffer if solution exists in circular buffer.
 *        Two semaphores are used to track the used and free space and wait for used space before read in case no space is used.
 * @return returns read feedback stract in the edges structure (contains the edges and edge amount)
 */
static edges circ_buf_read(void);

/**
 * @brief Creates, resizes and maps the shared memory and creates the semaphores
 */
static void allocate_resources(void);

/**
 * @brief sets state variable in shm to 0 -> all generators will terminate after they finished current task.
 *        Unmaps, closes and unlinks shared memory. Closes and unlinks all semaphores created previously.
 */
static void free_resources(void);


/**
 * First checks if not arguments are providen. No arguments are allowed.
 * Creates, resizes and maps the shared memory and creates the semaphores
 *
 */
int main(int argc, char **argv)
{
  //check if no arguments are provided
  if(argc > 1)  error_exit("do not provide arguments. arguments are not allowed!");

  /* all resources (sem and shm) are being freed on every program exit (error and normal), also the generators will terminate after exit */
  if (atexit(free_resources) != 0) error_exit("atexit: register function free_ressources.");

  allocate_resources(); /* allocates (creates) semaphores and shared memory */

  //initialize circular buffer where each edge has the initial value 0-0
  buf = myshm->data;
  int tt;
  int ss;
  for (tt = 0; tt < MAX_DATA; tt++) {
	for (ss = 0; ss < MAX_SOL_EDGES; ss++) {
	  buf[tt].edges[ss].v = 0;
	  buf[tt].edges[ss].u = 0;
	}
  }

  myshm->edgeAmount = 9; /* amount of edges of optimal solution. must be 8 or smaller 8, so initial value is 9 */
  myshm->state = 1; /* as long as this shm variable equals 1 the generators will generate solutions and write them into the circular buffer */
  myshm->write_pos = 0; /* inititalizes the write position for all generators to 0 */

  /* signal handling - SIGINT SIGTERM */
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handle_signal;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  /* all resources initialized - generators may now write into the buffer */
  if(sem_post(write_sem) == -1) error_errno_exit("incrementing write_sem");

  /* reads and prints the best solution (minimal feedback arc set) from circular buffer, as long as no SIGINT or SIGTERM is provided */
  while(!quit){
	edges temp; // solution read from the circular buffer is temporarily stored here
	temp = circ_buf_read(); // read next solution from circular buffer
	if(temp.amount == 0){   //check if the solution contains 0 edges -> graph is acylic -> terminate supervisor and generators
	  myshm->edgeAmount = temp.amount;
	  printf("The graph is acyclic!\n");
	  break;
	} else if(temp.amount < myshm->edgeAmount){ // if the edge amount in the solution is smaller than the previos solution
	  myshm->edgeAmount = temp.amount;          // -> remember smaller edgeamount and print current solution to stdout
	  printEdges(temp);
	}
  }

  /* free_sem deadlock prevention */
  int i;
  for(i = 0; i < 500; i++)
	sem_post(free_sem);
  exit(EXIT_SUCCESS);
}


/**
 * @brief sets global volatile quit var to 1 if SIGINT or SIGTERM received and prints kind of signal-> normal process termination
 * @param signal received signal
 */
void handle_signal(int signal)
{
  quit = 1;
  printf("Interrupted by signal: %s\n", strsignal(signal));
}

/**
 * @brief Creates, resizes and maps the shared memory and creates the semaphores
 */
void allocate_resources(void)
{
  shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600);
  if (shmfd == -1)  error_errno_exit("cannot open shared memory.");

  // set the size of the shared memory:
  if (ftruncate(shmfd, sizeof(struct myshm)) < 0)  error_errno_exit("cannot resize shared memory.");

  // map shared memory object:
  myshm = mmap(NULL, sizeof(*myshm), PROT_READ | PROT_WRITE, // need write to initialize all edges
			   MAP_SHARED, shmfd, 0);
  if (myshm == MAP_FAILED)  error_errno_exit("cannot map shared memory.");

  //sem_open - free, used
  free_sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0600, MAX_DATA);
  if(free_sem == SEM_FAILED)  error_errno_exit("Opening semaphore free_sem failed.");
  used_sem = sem_open(SEM_2, O_CREAT | O_EXCL, 0600, 0);
  if(used_sem == SEM_FAILED)  error_errno_exit("Opening semaphore used_sem failed.");
  //sem_open - write mutex
  write_sem = sem_open(SEM_3, O_CREAT | O_EXCL, 0600, 0);
  if(write_sem == SEM_FAILED)  error_errno_exit("Opening semaphore write_sem failed.");
}

/**
 * @brief sets state variable in shm to 0 -> all generators will terminate after they finished current task.
 *       Unmaps, closes and unlinks shared memory. Closes and unlinks all semaphores created previously.
*/
void free_resources(void)
{
  myshm->state = 0; // setting state variable to 0 so all generators leave while loop

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

  //unlink semaphores
  if(sem_unlink(SEM_1) == -1)  error_errno_exit("cannot unlink semaphore SEM_1");
  if(sem_unlink(SEM_2) == -1)  error_errno_exit("cannot unlink semaphore SEM_2");
  if(sem_unlink(SEM_3) == -1)  error_errno_exit("cannot unlink semaphore SEM_3");
}

/**
 * @brief reads next position in circular buffer if solution exists in circular buffer.
 *        Two semaphores are used to track the used and free space and wait for used space before read in case no space is used.
 * @return returns read feedback stract in the edges structure (contains the edges and edge amount)
 */
edges circ_buf_read(void)
{
  if(sem_wait(used_sem) == -1){
	if (errno == EINTR) // interrupted by signal?
	  exit(EXIT_SUCCESS);
	error_errno_exit("cannot wait used_sem."); // reading requires data (used space)
  }
  edges val;
  val = buf[read_pos];
  if(sem_post(free_sem) == -1) error_errno_exit("incrementing free_sem");
  read_pos = (read_pos + 1) % MAX_DATA;
  return val;
}

/**
 * @brief prints a feedback arc set solution to stdout in the following form: v1-u1 v2-u2 ...
 * @param edges feedback arc set solution containing the edges and the edge amount
 */
void printEdges(edges solution)
{
  unsigned int i;
  printf("Solution with %d edges: ", solution.amount);
  for (i = 0; i < solution.amount; i++){
	printf("%d-%d ", solution.edges[i].v, solution.edges[i].u);
  }
  printf("\n");
}
