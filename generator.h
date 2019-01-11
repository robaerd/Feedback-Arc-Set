/**
 * @file generator.h
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 07.01.19
 *
 * @brief provide all necessary header files for generator.c and topologicalSol.c.
 *         Declares all vertices-edge-graph related functions from topological.c
 */

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

/**
 * @brief shuffles provided vertices
 */
void randperm (unsigned int, unsigned int *);

/**
 * @brief extracts (parses) a string containing edges. Also provides vertex amount
 */
void extractEdgeFromString(char *, edge *, unsigned int *);

/**
 * @brief generates a valid feedback arc set
 * @return returns size of the generated feedback arc set
 */
unsigned int generateSolution(unsigned int *, unsigned int, edge *, int, edge[]);

