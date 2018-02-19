/*

File Name: dll.c
Author: Sandeep Raj Kumbargeri
Description: Linux Doubly Linked Lists Functions .c file
Written for ECEN 5013 - Homework 2 - in Spring 2018 at University of Colorado Boulder

*/

#include "dll.h"

node_t* insert_at_beginning(node_t *head_node, payload_t data)
{
  node_t *next_node;
  item_t *entry = (item_t *) malloc(sizeof(item_t));
  bzero(entry, sizeof(item_t));

  if(head_node->prev != NULL)
  {
    free(entry);
    return NULL;
  }

  //If theere are no nodes in the linked list
  if(head_node->next == NULL)
  {
    //printf("Linked list doesn't exist. Creating head node.\n");

    head_node->next = &(entry->link);

    entry->link.prev = head_node;
    entry->link.next = NULL;

    entry->data = data;

    //printf("Head: %lu\tPrev: %lu\tNext: %lu\n",(unsigned long) head_node, (unsigned long) head_node->prev, (unsigned long) head_node->next);

    return (head_node);
  }

  //Adding a new node the new node after the head node
  else
  {
    //printf("Adding new node..\n");
    //printf("Head: %lu\tPrev: %lu\tNext: %lu\n",(unsigned long) head_node, (unsigned long) head_node->prev, (unsigned long) head_node->next);

    next_node = head_node->next;
    head_node->next = &(entry->link);

    entry->link.prev = head_node;
    entry->link.next = next_node;

    next_node->prev = &(entry->link);
    entry->data = data;
    return (head_node);
  }
}

node_t* insert_at_end(node_t *head_node, payload_t data)
{
  node_t *next_node;
  item_t *entry = (item_t *) malloc(sizeof(item_t));
  bzero(entry, sizeof(item_t));

  if(head_node->prev != NULL)
  {
    free(entry);
    return NULL;
  }

  if(head_node->next == NULL)
  {
    //printf("Linked list doesn't exist. Creating head node.\n");

    head_node->next = &(entry->link);

    entry->link.prev = head_node;
    entry->link.next = NULL;

    entry->data = data;

    //printf("Head: %lu\tPrev: %lu\tNext: %lu\n",(unsigned long) head_node, (unsigned long) head_node->prev, (unsigned long) head_node->next);

    return (head_node);
  }

  //If the head node received is not a head node at all
  else
  {
    //printf("Error: Parameter 1 is not pointing to a head node.\n");
    next_node = head_node;
    while(next_node->next != NULL)
      next_node = next_node->next;

    next_node->next = &(entry->link);

    entry->link.prev = next_node;
    entry->link.next = NULL;

    entry->data = data;
    return(head_node);
  }
}


node_t* insert_at_position(node_t *head_node, payload_t data, unsigned int offset_count)
{
  node_t *next_node;
  item_t *entry = (item_t *) malloc(sizeof(item_t));
  bzero(entry, sizeof(item_t));
  unsigned char offset = 0;

  if((head_node->prev != NULL) || (offset_count == 0))
  {
    free(entry);
    return NULL;
  }

  else
  {
    next_node = head_node;
    for(offset = 1; offset <= offset_count; offset++)
    {
      if(next_node->next != NULL)
        next_node = next_node->next;
      else
        break;
    }

    offset--;

    if(offset != offset_count)
    {
      printf("offset = %u -- offset count = %u\n", offset, offset_count);
      free(entry);
      return NULL;
    }

    else
    {
      if(next_node->next == NULL)
      {
        next_node->next = &(entry->link);

        entry->link.prev = next_node;
        entry->link.next = NULL;

        entry->data = data;

        return head_node;
      }

      else
      {
        (next_node->prev)->next = &(entry->link);

        entry->link.prev = next_node->prev;
        entry->link.next = next_node;

        next_node->prev = &(entry->link);
        entry->data = data;

        return head_node;
      }
    }
  }
}

node_t* delete_from_beginning(node_t *head_node)
{
  item_t *entry = NULL;
  node_t *next_node = NULL;

  //If theere are no nodes in the linked list
  if((head_node->prev != NULL) || (head_node->next == NULL))
    return NULL;

  //If the received head node is a valid head node, delete it.
  else
  {
    next_node = head_node->next;

    head_node->next = next_node->next;
    (next_node->next)->prev = head_node;

    next_node->next = NULL;
    next_node->prev = NULL;

    entry = GET_LIST_CONTAINER(next_node, item_t, link);
    free(entry);

    return (head_node);
  }
}

node_t* delete_from_end(node_t *head_node)
{
  item_t *entry = NULL;
  node_t *next_node = NULL;

  //If theere are no nodes in the linked list
  if((head_node->prev != NULL) || (head_node->next == NULL))
    return NULL;


  //If the head node received is a valiod head node
  else
  {
    next_node = head_node;
    while(next_node->next != NULL)
      next_node = next_node->next;

    (next_node->prev)->next = NULL;
    next_node->prev = NULL;

    entry = GET_LIST_CONTAINER(next_node, item_t, link);
    free(entry);

    return (head_node);
  }
}

node_t* delete_from_position(node_t *head_node, unsigned int offset_count)
{
  item_t *entry;
  node_t *next_node;
  unsigned char offset = 0;

  if((head_node->prev != NULL) || (offset_count == 0) || (head_node->next == NULL))
    return NULL;

  else
  {
    next_node = head_node;
    for(offset = 1; offset <= offset_count; offset++)
    {
      if(next_node->next != NULL)
        next_node = next_node->next;
      else
        break;
    }

    offset--;

    if(offset != offset_count)
      return NULL;

    else
    {
      if(next_node->next == NULL)
      {
        (next_node->prev)->next = NULL;
        next_node->prev = NULL;

        entry = GET_LIST_CONTAINER(next_node, item_t, link);
        free(entry);

        return head_node;
      }

      else
      {
        (next_node->prev)->next = next_node->next;
        (next_node->next)->prev = next_node->prev;

        next_node->next = NULL;
        next_node->prev = NULL;

        entry = GET_LIST_CONTAINER(next_node, item_t, link);
        free(entry);

        return head_node;
      }
    }
  }
}

size_t size(node_t *head_node)
{
  size_t length = 0;
  node_t *next_node;

  if(head_node->prev != NULL)
    return 0;

  else
  {
    next_node = head_node;
    while(next_node->next != NULL)
    {
      next_node = next_node->next;
      length++;
    }
  }

  return length;
}

node_t* destroy(node_t *head_node)
{
  item_t *entry = NULL;
  node_t *next_node = NULL;
  node_t *current_node = NULL;

  if((head_node->prev != NULL) || (head_node->next == NULL))
    return NULL;

  else
  {
    current_node = head_node->next;
    while(current_node->next != NULL)
    {
      next_node = current_node->next;

      current_node->prev = NULL;
      current_node->next = NULL;

      entry = GET_LIST_CONTAINER(current_node, item_t, link);
      free(entry);

      current_node = next_node;
    }

    current_node->prev = NULL;
    entry = GET_LIST_CONTAINER(current_node, item_t, link);
    free(entry);
    head_node->next = NULL;
    current_node = NULL;
    next_node = NULL;

    return head_node;
  }
}

// void main(void)
// {
//   node_t head_node;
//   payload_t data;
//
//   head_node.prev = NULL;
//   head_node.next = NULL;
//
//   data.a = 20;
//
//   insert_at_beginning(&head_node, data);
//   printf("\nSize = %lu.\n", size(&head_node));
//   insert_at_end(&head_node, data);
//   printf("\nSize = %lu.\n", size(&head_node));
//   insert_at_position(&head_node, data, 1);
//   printf("\nSize = %lu.\n", size(&head_node));
//   delete_from_position(&head_node, 1);
//   printf("\nSize = %lu.\n", size(&head_node));
//   insert_at_end(&head_node, data);
//   printf("\nSize = %lu.\n", size(&head_node));
//   delete_from_beginning(&head_node);
//   printf("\nSize = %lu.\n", size(&head_node));
//   delete_from_end(&head_node);
//   printf("\nSize = %lu.\n", size(&head_node));
//   destroy(&head_node);
//   printf("\nSize = %lu.\n", size(&head_node));
// }
