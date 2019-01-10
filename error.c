/**
 * @file error.c
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 10.01.19
 *
 * @brief containts two error-exit functions
 *        error_exit(..) prints a provided error message to stderr and exits with exit code EXIT_FAILURE
 *        error_errno_exit(..) additionaly prints the errno message
 *
 */
#include "error.h"

/**
 * @brief prints provided error message to stderr and exits with exit code EXIT_FAILURE
 * @param error_message error message to be printed to stderr
 */
void error_exit(char *error_message){
  fprintf(stderr,"ERROR: %s\n", error_message);
  exit(EXIT_FAILURE);
}

/**
 * @brief prints provided error message with the errno messge to stderr and exits with exit code EXIT_FAILURE
 * @param error_message error message to be printed to stderr
 */
void error_errno_exit(char *error_message){
  fprintf(stderr,"ERROR: %s, %s\n", error_message, strerror(errno));
  exit(EXIT_FAILURE);
}
