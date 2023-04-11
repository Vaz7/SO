#include "queue.h"

// Initialize the queue
void initQueue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
}

// Enqueue an element to the queue
void enqueue(Queue* q, int pos) {
    Nodo* newNodo = (Nodo*)malloc(sizeof(Nodo)); // Create a new Nodo
    newNodo->pos = pos;
    newNodo->prox = NULL;

    if (q->rear == NULL) {
        // If the queue is empty, both front and rear point to the new Nodo
        q->front = newNodo;
        q->rear = newNodo;
    } else {
        // Otherwise, add the new Nodo to the rear of the queue
        q->rear->prox = newNodo;
        q->rear = newNodo;
    }
}

// Dequeue an element from the queue
int dequeue(Queue* q) {
    if (q->front == NULL) {
        // If the queue is empty, return an error value or throw an error
        printf("Queue is empty.\n");
        return -1; // or any other sentinel value to indicate queue is empty
    }

    Nodo* frontNodo = q->front;
    int pos = frontNodo->pos;

    q->front = q->front->prox; // Move front to the prox Nodo

    // If the front becomes NULL, it means the queue is now empty
    if (q->front == NULL) {
        q->rear = NULL; // Set rear to NULL as well
    }

    free(frontNodo); // Free memory of the dequeued Nodo

    return pos;
}

// Check if the queue is empty
int isEmpty(Queue* q) {
    return q->front == NULL;
}

// Clean up the queue and free all memory
void clearQueue(Queue* q) {
    while (!isEmpty(q)) {
        dequeue(q);
    }
}