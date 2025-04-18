#include "queue.h"
#include <stdio.h>
#include <stdlib.h>


int main() {
	Queue* queue = create_empty_queue();
	for (int i = 0; i < 4; i++) {
		sem_t sem;
		sem_init(&sem, 0, 0);
		Node* node = create_node(sem);
		push_back(queue, node);
		printf("%d\n", queue->size);
	}	
	for (int i = 0; i < 4; i++) {
		Node* node = pop_start(queue);
		printf("%p", (void*)node->next);
		printf("%d\n", queue->size);
	}
	free_queue(queue);

	return 0;
}

