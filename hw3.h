/*

File Name: hw3.h
Author: Sandeep Raj Kumbargeri
Description: Header file for hw3.c
Written for ECEN 5013 - Homework 3 - in Spring 2018 at University of Colorado Boulder

*/

#ifndef _HW3_H
#define _HW3_H

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>

#include "dll.h"

//User defined data types declaration
typedef struct pthread_args
{
  char filename[32];
}pthread_args_t;

#define TARGET_OCCURANCES 3
#define CLOCK_ID          CLOCK_MONOTONIC

void update_time(char *, size_t);
void secure_log(char *, const char *);
void *first_thread(void *);
void *second_thread(void *);
void signal_handler(int);
long double cpu_load(void);
void timer_expiry_handler(union sigval);

payload_t* search_character_entry(node_t *, char);
void print_characters_by_count(node_t *, unsigned int, char *);

#endif
