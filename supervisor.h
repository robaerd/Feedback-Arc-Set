/**
 * @file supervisor.h
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 09.01.19
 *
 * @brief provide all necessary header files for superivsor.h
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

#include "circularBuffer.h"
