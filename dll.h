/*

File Name: dll.h
Author: Sandeep Raj Kumbargeri
Description: Linux Doubly Linked Lists Functions .h file
Written for ECEN 5013 - Homework 2 - in Spring 2018 at University of Colorado Boulder

*/


#ifndef _DLL_H
#define _DLL_H

//Headers
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

//MACRO to return the adddress of the container structure
#define GET_LIST_CONTAINER(addr, type, member) ({\
	const __typeof__( ( (type * )0)->member) *__ptr = (addr);\
	(type *) (( (char *) __ptr ) - offsetof(type, member)); }\
)

//Defining the node structure
typedef struct node{
  struct node *next;
  struct node *prev;
} node_t;

typedef struct payload
{
	char rchar;
	unsigned int count;
} payload_t;

//Defining the container structure
typedef struct item{
  payload_t data;
  node_t link;
} item_t;


//Declaring the functions
node_t* destroy(node_t *);
node_t* insert_at_beginning(node_t *, payload_t);
node_t* insert_at_end(node_t *, payload_t);
node_t* insert_at_position(node_t *, payload_t, unsigned int);
node_t* delete_from_beginning(node_t *);
node_t* delete_from_end(node_t *);
node_t* delete_from_position(node_t *, unsigned int);
size_t size(node_t *);


#endif
