/*

File Name: hw3.c
Author: Sandeep Raj Kumbargeri
Description: Demonstrates working with pThreads and logging information
Written for ECEN 5013 - Homework 3 - in Spring 2018 at University of Colorado Boulder

*/

#include "hw3.h"

void update_time(char *, size_t);
void secure_log(char *, const char *);
void *first_thread(void *);
void *second_thread(void *);
void signal_handler(int);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
  FILE *file;
  pthread_t thread_id[2];
  pthread_args_t args;

  if (argc != 2)
  {
    printf ("USAGE: %s <log filename>\n", argv[0]);
    exit(1);
  }

  file = fopen(argv[1], "w");
  if(file == NULL)
  {
    printf ("\n## ERROR ## Cannot create the file \"%s\". Exiting.\n", argv[1]);
    exit(1);
  }

  fclose(file);
  strcpy(args.filename, argv[1]);

  secure_log(args.filename, "## MASTER ## Successfully created log file.\n");

  signal(SIGUSR1, signal_callback_handler);
  signal(SIGUSR2, signal_callback_handler);

  secure_log(args.filename, "## MASTER ## Successfully initialized SIGUSR callback handlers.\n");

  pthread_create(&thread_id[0], NULL, first_thread, (void *) &args);
  pthread_create(&thread_id[1], NULL, second_thread, (void *) &args);
  return 0;
}

void *first_thread(void *args)
{

}

void *second_thread(void *args)
{

}

void signal_handler(int signal)
{
  if((signal == SIGUSR1) || (signal == SIGUSR2))
  {

  }
}

void update_time(char *current_time, size_t length)
{
  time_t present_time;
  struct tm *local_time;

  bzero(current_time, length);
  time(&present_time);
  local_time = localtime(&present_time);
  strcpy(current_time, asctime(local_time));
  current_time[strlen(current_time) - 1] = '\0';
}

void secure_log(char *filename, const char *message)
{
  FILE *file;
  char current_time[32] = {0};

  pthread_mutex_lock(&mutex);
  file = fopen(filename, "a");
  update_time(current_time, sizeof(current_time));
  fprintf(file, "%s  %s\n", current_time, message);
  fclose(file);
  pthread_mutex_unlock(&mutex);
}
