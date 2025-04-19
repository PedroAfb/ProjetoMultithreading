#include "queue.h"
#include <stdlib.h>

Node * create_node(sem_t semaphore) {
	Node* node = malloc(sizeof(Node));
	node->semaphore = semaphore;
	node->next = NULL;
	return node;
}

Queue* create_empty_queue() {
	Queue *queue = malloc(sizeof(Queue));
	queue->size = 0;
	queue->start = NULL;
	queue->end = NULL;
	return queue;
}

void free_queue(Queue* queue) {
	Node* node = queue->start;
	while (node != NULL) {
		free(node);
	}
	free(queue);
}

void push_back(Queue* queue, Node* node) {
	if (queue->end == NULL) {
		queue->start = node;
		queue->end = node;
	} else {
		queue->end->next = node;
		queue->end = node;
	}
	queue->size++;
}

Node* pop_start(Queue* queue) {
	if (queue->size == 0)
		return NULL;

	Node* node = queue->start;
	if (queue->start->next == NULL) 
		queue->start = NULL;
	else 
		queue->start = queue->start->next;
	
	queue->size--;
	return node;
}

