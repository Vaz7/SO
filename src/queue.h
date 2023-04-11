#ifndef Queue_included
#define Queue_included

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


// Node structure for the linked list
typedef struct Nodo {
    int pos;
    struct Nodo* prox;     
} Nodo;

// Queue structure
typedef struct Queue {
    Nodo* front;           
    Nodo* rear;            
} Queue;


void initQueue(Queue* q);
void enqueue(Queue* q, int pos);
int dequeue(Queue* q);
int isEmpty(Queue* q);
void clearQueue(Queue* q);


#endif