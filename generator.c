#include "generator.h"
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
	  fprintf(stderr,"invalid option. only postitional arguments expected\n");
	  exit(EXIT_FAILURE);
	default:
	  break;
	}

  int shmfd = shm_open(SHM_NAME, O_RDWR, 0600);
  if (shmfd == -1){
	fprintf(stderr, "ERROR int shm open.\n");
	exit(EXIT_FAILURE);
	//error handling
  }

  struct myshm *myshm;
  myshm = mmap(NULL, sizeof(*myshm), PROT_READ | PROT_WRITE, // need write to initialize all edges
			   MAP_SHARED, shmfd, 0);

  if (myshm == MAP_FAILED) {
	fprintf(stderr, "error: shm map failed");
	exit(EXIT_FAILURE);
  }

  buf = myshm->data;
  //set time seed for random number generator
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  // using nano-seconds instead of seconds
  srand((time_t)ts.tv_nsec);

  int optI = optind;
  edgeAmount = argc - optI;
  //char *edges[edgeAmount];
  edge initEdges[edgeAmount];
  if(optI < argc){
	int i;
	for (i = 0; i < argc-optI; i++){
	  //check if valid; better check inside extractEdge..()
	  extractEdgeFromString( *(argv + optI + i), &initEdges[i], &vertexAmount);
	  //	  edges[i] = *(argv + optI + i);
	  //problem: zahlen groesser 9 werden nicht gehandlet!
	  /* if (*(edges[i] +1) != '-' || !isdigit(*(edges[i])) || !isdigit(*(edges[i] + 2))){
		 fprintf(stderr,"Wrong edge pattern: %s. Should be: u-v", edges[i]);
		 exit(EXIT_FAILURE);
		 }*/
	}
	vertexAmount++; //because vertices start at 0
  }else {
	fprintf(stderr, "Invalid pattern!\n");
	exit(EXIT_FAILURE);
  }
  //verticeAmount = getAmountVertices(edges, edgeAmount);
  unsigned int vertices[vertexAmount];
  //  memset(vertices, 0, sizeof(unsigned int)*vertexAmount);
  //printf("verticeamount: %d\n", vertexAmount);
  

  used_sem = sem_open("11708475-used_sem", 0);
  free_sem = sem_open("11708475-free_sem", 0);

 unsigned int solutionSize;
  while(myshm->state){
	randperm(vertexAmount, vertices);
  solutionSize = generateSolution(vertices, vertexAmount, initEdges, edgeAmount, solution.edges);
  if(solutionSize == 9) continue; // a solution with more than 8 edges was found
  solution.amount=solutionSize;
  circ_buf_write(solution);
  }

  //printEdges(solution.edges, solutionSize);


  // unmap shared memory:
  if (munmap(myshm, sizeof(*myshm)) == -1){
	fprintf(stderr, "ERROR in munmap\n");
	exit(EXIT_FAILURE);
	// error
  }

  //close(shm)
  if (close(shmfd) == -1){
	fprintf(stderr, "ERROR closing shmfd\n");
	exit(EXIT_FAILURE);
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
}
