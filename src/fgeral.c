#include "define.h"

/* Funções cliente */


/* Funções server */

void log_message(const char *message) {
  time_t now;
  time(&now);
  struct tm *local_time = localtime(&now);
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);

  printf("%s - %s\n", timestamp, message);
}

/* Funções Network **/

/* Funções Network */