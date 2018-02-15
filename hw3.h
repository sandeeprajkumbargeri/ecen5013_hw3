/*

File Name: hw3.h
Author: Sandeep Raj Kumbargeri
Description: Header file for hw3.c
Written for ECEN 5013 - Homework 3 - in Spring 2018 at University of Colorado Boulder

*/


#ifndef _HW3_H
#define _HW3_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

typedef struct pthread_args
{
  char filename[32];
}pthread_args_t;

#endif
