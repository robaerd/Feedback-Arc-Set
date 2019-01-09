#include "generator.h"
//#include "circularBuffer.h"

/*
int getAmountVertices (char **edges, int amountEdges)
{
  int max = 0;
  int i;
  int u,v;
  for(i = 0; i < amountEdges; i++){
	extractVertFromEdge(edges[i], &u, &v);
	if(u > max) max = u;
	if(v > max) max = v;
  }
  return max+1;
}
*/

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
  sem_post(used_sem); // space is used by written data write_pos = (write_pos + 1) % BUF_LEN;
}


int main(int argc, char ** argv)
{
  int c;
  int edgeAmount;
  int vertexAmount;
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
	  extractEdgeFromString( *(argv + optI + i), initEdges[i], &vertexAmount);
	  //	  edges[i] = *(argv + optI + i);
	  //problem: zahlen groesser 9 werden nicht gehandlet!
	  /* if (*(edges[i] +1) != '-' || !isdigit(*(edges[i])) || !isdigit(*(edges[i] + 2))){
		 fprintf(stderr,"Wrong edge pattern: %s. Should be: u-v", edges[i]);
		 exit(EXIT_FAILURE);
		 }*/
	}
  }else {
	fprintf(stderr, "Invalid pattern!\n");
	exit(EXIT_FAILURE);
  }
  //verticeAmount = getAmountVertices(edges, edgeAmount);
  unsigned int vertices[vertexAmount];
  printf("verticeamount: %d\n", vertexAmount);
  randperm(vertexAmount, vertices);

  //test
  //  char *solution = (char*) (malloc(sizeof(char) * 8*3));
  //  memset(solution, '\0', sizeof(char) * 8*3);
  int solutionSize = generateSolution(vertices, vertexAmount, initEdges, edgeAmount, solution.edges);
  int t;
  //for(t = 0; t < solutionSize; t++)
  int z;
  //for(z = 0; z < vertexAmount; z++) printf("%d,", vertices[z]);
  //printf("solution: %s\n", solution);
  printEdges(solution.edges, solutionSize);


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
}
