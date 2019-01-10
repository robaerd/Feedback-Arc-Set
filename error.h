/**
 * @file error.h
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 10.01.19
 *
 * @brief provide all necessary header files for error.c.
 *        Declares all error_*exit functions.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/**
 * @brief prints provided error message to stderr and exits with exit code EXIT_FAILURE
 */
void error_exit(char *);

/**
 * @brief prints provided error message with the errno messge to stderr and exits with exit code EXIT_FAILURE
 */
void error_errno_exit(char *);
