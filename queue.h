
#ifndef QUEUE_H
#define QUEUE_H

#include <semaphore.h>
#include <stdlib.h>

typedef struct Node {
	sem_t semaphore;
	struct Node* next;
} Node;

typedef struct Queue {
	int size;
	Node* start;
	Node* end;
} Queue; 

Node *create_node(sem_t semaphore);

Queue *create_empty_queue();

void free_queue(Queue* queue);

void push_back(Queue* queue, Node* node);

Node* pop_start(Queue* queue);

#endif

