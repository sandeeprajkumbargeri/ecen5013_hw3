/*

File Name: hw3.c
Author: Sandeep Raj Kumbargeri
Description: Demonstrates working with pThreads and logging information
Written for ECEN 5013 - Homework 3 - in Spring 2018 at University of Colorado Boulder

*/

//Includes
#include "hw3.h"

//Global Variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_t1, sem_t2;
bool exit_thread = false;


int main(int argc, char *argv[])
{
  FILE *file;
  pthread_t thread_id[2];
  pthread_args_t args;
  char message[128];

  if (argc != 2)
  {
    printf ("USAGE: %s <log filename>\n", argv[0]);
    exit(1);
  }

  //Open the log file in the write mode
  file = fopen(argv[1], "w");
  if(file == NULL)
  {
    printf ("\n## ERROR ## Cannot create the file \"%s\". Exiting.\n", argv[1]);
    exit(1);
  }

  fclose(file);
  strcpy(args.filename, argv[1]);

  bzero(message, sizeof(message));
  sprintf(message, "##  MASTER  ## Started. POSIX Thread_ID: %lu. Linux Thread ID: %ld.\n", pthread_self(), syscall(SYS_gettid));
  secure_log(args.filename, message);

  secure_log(args.filename, "##  MASTER  ## Successfully created log file.\n");

  //Initializing the USR1 and USR2 signals
  signal(SIGUSR1, signal_handler);
  signal(SIGUSR2, signal_handler);

  secure_log(args.filename, "##  MASTER  ## Successfully initialized SIGUSR callback handlers.\n");

  //Initializing thread 1 and thread 2 related semaphores
  sem_init(&sem_t1, 0, 0);
  sem_init(&sem_t2, 0, 0);

  //Creating the threads and passing the log file name to the threads
  pthread_create(&thread_id[0], NULL, first_thread, (void *) &args);
  pthread_create(&thread_id[1], NULL, second_thread, (void *) &args);

  //Wait for the threads to end
  pthread_join(thread_id[0], NULL);
  pthread_join(thread_id[1], NULL);

  secure_log(args.filename, "##  MASTER  ## SIGUSR1 or SIGUSR2 received. Exiting normally.\n");
  return 0;
}

//This thread finds the characters with "TARGET OCCURANCES" (see the macro) in the vday.txt and logs it. Stays alive until SIG -USR1 or SIG -USR2 is received.
void *first_thread(void *args)
{
  FILE *file;
  node_t head_node;
  payload_t data;
  payload_t *data_p;
  char rchar = 0;
  pthread_args_t *thread_args = (pthread_args_t *) args;
  char message[128];

  sprintf(message, "## THREAD 1 ## Started. POSIX Thread_ID: %lu. Linux Thread ID: %ld.\n", pthread_self(), syscall(SYS_gettid));
  secure_log(thread_args->filename, message);

  head_node.prev = NULL;
  head_node.next = NULL;

  file = fopen("vday.txt", "r");

  //If the vday.txt file is not found, then exit the thread logging that the file was not found.
  if(file == NULL)
  {
    secure_log(thread_args->filename, "## THREAD 1 ## Error in opening the file vday.txt.\n");
    pthread_exit(NULL);
  }

  //Loop through the file character by character until the end of file is reached
  while(feof(file) == 0)
  {
    fread((void *) &rchar, sizeof(char), 1, file);  //read character at the file pointer
    data_p = search_character_entry(&head_node, rchar); //search for the character in the linked list

    //If the character is not found in the linked list..
    if(data_p == NULL)
    {
      data.rchar = rchar;
      data.count = 1;
      insert_at_end(&head_node, data);    //Add it at the end of the linked list
    }

    else
      data_p->count++;  //Otherwise, increment the count of the character occurance by 1 time
  }

  fclose(file);
  print_characters_by_count(&head_node, TARGET_OCCURANCES, thread_args->filename);  //log the characters with three occurances
  destroy(&head_node);  //destroy all the nodes in the linked list

  sem_wait(&sem_t1);    //wait for the SIG -USR1 os SIG -USR2 to exit the thread
  secure_log(thread_args->filename, "## THREAD 1 ## SIGUSR1 or SIGUSR2 received. Exiting normally.\n");
  pthread_exit(NULL);
}

//This thread logs the total CPU utilization every 100ms until SIG -USR1 or SIG -USR2 is received
void *second_thread(void *args)
{
  pthread_args_t *thread_args = (pthread_args_t *) args;
  timer_t timer_id;
  struct sigevent sevp;
  struct itimerspec tspec;
  char message[128];

  sprintf(message, "## THREAD 2 ## Started. POSIX Thread_ID: %lu. Linux Thread ID: %ld.\n", pthread_self(), syscall(SYS_gettid));
  secure_log(thread_args->filename, message);

  //Set the timer configuration
  sevp.sigev_notify = SIGEV_THREAD;
  sevp.sigev_value.sival_ptr = &timer_id;
  sevp.sigev_notify_function = timer_expiry_handler;
  sevp.sigev_notify_attributes = NULL;

  //Set the timer value to 100ms
  tspec.it_value.tv_sec = 0;
  tspec.it_value.tv_nsec = 100000000;
  tspec.it_interval.tv_sec = 0;
  tspec.it_interval.tv_nsec = 100000000;

  timer_create(CLOCK_ID, &sevp, &timer_id);
  timer_settime(timer_id, 0, &tspec, 0);
  secure_log(thread_args->filename, "## THREAD 2 ## Timer set up successfully.\n");

  //Loop until exit_thread becomes true
  while(exit_thread == false)
  {
    sem_wait(&sem_t2);  //wait on semaphore semt2 (which is posted by the timer handler)
    bzero(message, sizeof(message));
    sprintf(message, "## THREAD 2 ## CPU Usage: %Lf.\n", cpu_load()); //Log the CPU usage
    secure_log(thread_args->filename, message);
  }

  timer_delete(&timer_id);

  //As the exit_thread becomes true, the above loop is exited and the threads exits after logging that it is exiting
  secure_log(thread_args->filename, "## THREAD 2 ## SIGUSR1 or SIGUSR2 received. Exiting normally.\n");
  pthread_exit(NULL);
}

//Handler for the POSIX timer created in the second thread. Posts the semaphore everytime the timer triggers
void timer_expiry_handler(union sigval arg)
{
  sem_post(&sem_t2);
}

//The following function is taken from stack-overflow. I do not have any rights over it.
//URL: https://stackoverflow.com/questions/3769405/determining-cpu-utilization
//Calculates the CPU usage of all the cores and returns it to the caller function
long double cpu_load(void)
{
  long double a[4], b[4], loadavg;
   FILE *fp;
   char dump[50];

   fp = fopen("/proc/stat","r");
   fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
   fclose(fp);
   usleep(40000);

   fp = fopen("/proc/stat","r");
   fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
   fclose(fp);

   loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
   return loadavg;
}

//Executes when an external signal -USR1 or -USR2 is received.
//Posts the semaphore to thread 1 and sets the exit_thread of thread 2 to true.
void signal_handler(int signal)
{
  if((signal == SIGUSR1) || (signal == SIGUSR2))
  {
    sem_post(&sem_t1);
    exit_thread = true;
  }
}

//Called by the secure_log function to update the time during logging a message.
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

//Logs a message the specified file with the timestamp.
void secure_log(char *filename, const char *message)
{
  FILE *file;
  char current_time[32] = {0};

  //protect the writes using the mutex - so that only one function can write at a time
  pthread_mutex_lock(&mutex);
  file = fopen(filename, "a");
  update_time(current_time, sizeof(current_time));
  fprintf(file, "%s  %s\n", current_time, message);
  fclose(file);
  pthread_mutex_unlock(&mutex);
}

//Searches through the linked list with the head pointed by head_node for characted rchar.
//Returns NULL if the character is not found in the linked Lists
//Returns the address of the payload if the character is found in the payload
payload_t* search_character_entry(node_t *head_node, char rchar)
{
  item_t *entry = NULL;
  node_t *next_node = NULL;
  node_t *current_node = NULL;

  //If the received node address isn't head node's or the linked list is empty, rreturn NULL
  if((head_node->prev != NULL) || (head_node->next == NULL))
    return NULL;

  else
  {
    current_node = head_node->next;
    while(current_node->next != NULL) //traverse the linked list till the last node
    {
      next_node = current_node->next;

      entry = GET_LIST_CONTAINER(current_node, item_t, link);

      //check if the character is present, if yes, send its payload address
      if((entry->data).rchar == rchar)
        return &(entry->data);

      current_node = next_node;
    }

    //do the same for the last node
    entry = GET_LIST_CONTAINER(current_node, item_t, link);

    if((entry->data).rchar == rchar)
      return &(entry->data);

    else
      return NULL;  //return NULL if the character is not found in the linked list
  }
}

//Traverses the linked list to find the characters with target count in the payload.
//Prints the information to the log file
void print_characters_by_count(node_t *head_node, unsigned int target_count, char *filename)
{
  item_t *entry = NULL;
  node_t *next_node = NULL;
  node_t *current_node = NULL;

  char message[128];

  //If the node provided is not a head node or the linked list is empty, print the following
  if((head_node->prev != NULL) || (head_node->next == NULL))
  {
    sprintf(message,"## THREAD 1 ## No characters found with %u total occurances.\n", target_count);
    secure_log(filename, message);
  }

  else
  {
    current_node = head_node->next;
    while(current_node->next != NULL) //traverse the linked list until the last node
    {
      next_node = current_node->next;

      //get the address to the entry from the node_t
      entry = GET_LIST_CONTAINER(current_node, item_t, link);

      //if it character's count matches the targer count, log it
      if((entry->data).count == target_count)
      {
        bzero(message, sizeof(message));
        sprintf(message, "## THREAD 1 ## %c found with %u total occurances.\n", entry->data.rchar, target_count);
        secure_log(filename, message);
      }

      current_node = next_node;
    }

    //repeat the above just for the final node
    entry = GET_LIST_CONTAINER(current_node, item_t, link);

    if((entry->data).count == target_count)
    {
      bzero(message, sizeof(message));
      sprintf(message, "## THREAD 1 ## %c found with %u total occurances.\n", entry->data.rchar, target_count);
      secure_log(filename, message);
    }
  }
}
