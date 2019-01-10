#include "error.h"

void error_exit(char *error_message){
  fprintf(stderr,"ERROR: %s\n", error_message);
  exit(EXIT_FAILURE);
}


void error_errno_exit(char *error_message){
  fprintf(stderr,"ERROR: %s, %s\n", error_message, strerror(errno));
  exit(EXIT_FAILURE);
}
