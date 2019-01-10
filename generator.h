#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <semaphore.h>
#include <time.h>

#include "circularBuffer.h"

void swap(unsigned int * ,unsigned int *);

void randperm (unsigned int, unsigned int *);

void extractEdgeFromString(char *, edge *, unsigned int *);

//void extractVertFromEdge(char *, int *, int *);

int validEdge(edge, unsigned int *, unsigned int);

unsigned int generateSolution(unsigned int *, unsigned int, edge *, int, edge[]);

