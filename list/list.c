// list/list.c
// 
// Implementation for linked list.
//
// <Author>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list_t *list_alloc() { 
  list_t* mylist =  (list_t *) malloc(sizeof(list_t)); 
  mylist->head = NULL;
}

void list_free(list_t *l) {

  if (l == NULL){
    return;
  }

  node_t *curr = l->head;
  node_t *next_node;

  while (curr!=NULL){
    next_node = curr->next;
    free(curr);
    curr = next_node;
  }
  l->head = NULL;
}

void list_print(list_t *l) {
  if(l == NULL){
    printf("No value");
    return;
  }

  node_t *curr = l->head;
  while (curr!=NULL){
    printf("%d ->", curr->value);
    curr = curr->next;
  }

  printf("NULL\n");
}

char * listToString(list_t *l) {
  char* buf = (char *) malloc(sizeof(char) * 10024);
  char tbuf[20];

	node_t* curr = l->head;
  while (curr != NULL) {
    sprintf(tbuf, "%d->", curr->value);
    curr = curr->next;
    strcat(buf, tbuf);
  }
  strcat(buf, "NULL");
  return buf;
}

int list_length(list_t *l) { 
  if (l == NULL) {
    return 0;
  }
  int counter = 0; 
  node_t *curr = l->head;
  while (curr != NULL) {
    curr = curr->next;
    counter = counter + 1; 
  }
  return counter;
}


void list_add_to_back(list_t *l, elem value) {

  node_t *cur_node = (node_t *) getNode(value);

  
  if (l==NULL){
    l->head = cur_node;
  }
  else{
    node_t *curr = l->head;
    while (curr != NULL){
      curr = curr->next;
    }
    curr->next = cur_node;
  }
  }


void list_add_to_front(list_t *l, elem value) {
     node_t *cur_node = (node_t *) getNode(value);

     /* Insert to front */
     node_t *head = l->head;  // get head of list

     cur_node->next = head;
     l->head = cur_node;
}

node_t * getNode(elem value) {
  node_t *mynode;

  mynode = (node_t *) malloc(sizeof(node_t));
  mynode->value = value;
  mynode->next = NULL;

  return mynode;
}

void list_add_at_index(list_t *l, elem value, int index) {

    node_t *cur_node = getNode(value);
    if (index == 0) {
        list_add_to_front(l, value);
        return;
    }

    node_t *curr = l->head;
    int counter = 0;
    while (curr != NULL && counter < index - 1) {
        curr = curr->next;
        counter++;
    }
    if (curr == NULL) {
        printf("Index out of bounds\n");
        return;
    }
    cur_node->next = curr->next;
    curr->next = cur_node;
}


elem list_remove_from_back(list_t *l) {
    if (l == NULL || l->head == NULL) {
        return -1;
    }

    if (l->head->next == NULL) {
        elem value = l->head->value;
        free(l->head);
        l->head = NULL;
        return value;
    }

    node_t *curr = l->head;
    while (curr->next->next != NULL) {
        curr = curr->next;
    }

    elem value = curr->next->value;
    node_t *node_to_remove = curr->next;
    curr->next = NULL;
    free(node_to_remove);

    return value;
}


elem list_remove_from_front(list_t *l) {
    if (l == NULL || l->head == NULL) {
        return -1; 
    }
    node_t *node_to_remove = l->head; 
    elem value = node_to_remove->value; 
    l->head = l->head->next; 

    free(node_to_remove);
    return value; 
}

elem list_remove_at_index(list_t *l, int index) {
    if (index == 1) {
        return list_remove_from_front(l); // If index is 1, remove from front
    } else {
        node_t *curr = l->head;
        int counter = 1;

        while (curr != NULL && counter < index - 1) {
            curr = curr->next;
            counter++;
        }

        if (curr == NULL || curr->next == NULL) {
            return -1; 
        }
        elem value = curr->next->value;
        node_t *node_to_remove = curr->next;
        curr->next = curr->next->next;
        free(node_to_remove);
        return value;
    }
}

bool list_is_in(list_t *l, elem value) {
    if (l == NULL) {
        return false;
    }
    node_t *curr = l->head;
    while (curr != NULL) {
        if (curr->value == value) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}


elem list_get_elem_at(list_t *l, int index) {
  if (l==NULL || index < 1){
    return -1;
  }
  node_t *curr = l->head;
  int counter = 1;
  while (curr!=NULL){
    if (counter == index){
      return curr->value;
      }
    curr = curr->next;
    counter++;
  }
  return -1;
}
int list_get_index_of(list_t *l, elem value) { 
  if (l==NULL){
    return -1;
  }
  node_t *curr = l->head;
  int counter = 1;
  while (curr!=NULL){

    if (curr->value == value){
      return counter;
      }
    curr = curr->next;
    counter++;
  }
  return -1;
}

