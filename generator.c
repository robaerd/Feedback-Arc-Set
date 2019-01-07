#include "generator.h"
#include <time.h>

void swap(char** firstElem, char **secondElem)
{
  printf("befor: %s .. %s\n", *firstElem, *secondElem);
  char temp[4];
  strcpy(temp, *firstElem);
  strcpy(*firstElem, *secondElem);
  strcpy(*secondElem, temp);
  printf("after: %s .. %s\n", *firstElem, *secondElem);
}

void randperm (int n, char **N)
{
  int i;
  srand(time(NULL)); // initializes rand function
  for(i = n-1; i > 0; i--) {
	int r = rand() % n;
	if (r == i) continue;
	swap(&N[i], &N[r]);
	}
  }

int main(int argc, char ** argv)
{
  int c;
  int edgeAmount;

  while((c = getopt(argc,argv,"")) != -1)
	switch(c) {
	case '?':
	  fprintf(stderr,"invalid option. only postitional arguments expected\n");
	  exit(EXIT_FAILURE);
	default:
	  break;
	}


	int optI = optind;
	edgeAmount = argc - optI;
	char *edges[edgeAmount];
	if(optI < argc){
	  int i;
	  for (i = 0; i < argc-optI; i++)
		edges[i] = *(argv + optI + i);
	} else {
	  fprintf(stderr, "Invalid pattern!\n");
	  exit(EXIT_FAILURE);
	}

	//test
	//swap(&edges[0], &edges[1]);
	randperm(edgeAmount, edges);
	int t;
	for (t = 0; t < edgeAmount; t++)
	  printf("%s\n", edges[t]);

}
