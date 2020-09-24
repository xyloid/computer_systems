// =================== Support Code =================
// Stack
//
//
//
// - Implement each of the functions to create a working stack.
// - Do not change any of the function declarations
//   - (i.e. stack_t* create_stack() should not have additional arguments)
// - You should not have any 'printf' statements in your stack functions.
//   - (You may consider using these printf statements to debug, but they should
//   be removed from your final version)
// ==================================================
#ifndef MYSTACK_H
#define MYSTACK_H

#include<time.h>
#include<limits.h>
#include<stdlib.h>

#ifndef TIME_UNIT
#define TIME_UNIT time_t
#endif

// Stores the maximum 'depth' of our stack.
// Our implementation enforces a maximum depth of our stack.
// (i.e. capacity cannot exceed MAX_DEPTH for any stack)
#define MAX_DEPTH INT_MAX

// Create a node data structure to store data within
// our stack. In our case, we will stores 'integers'
typedef struct node {
	TIME_UNIT data;
	struct node *next;
} node_t;

// Create a stack data structure
// Our stack holds a single pointer to a node, which
// is a linked list of nodes.
typedef struct stack {
	int count;             // count keeps track of how many items
	                       // are in the stack.
	unsigned int capacity; // Stores the maximum size of our stack
	node_t *head;          // head points to a node on the top of our stack.
} stack_t;

// Creates a stack
// Returns a pointer to a newly created stack.
// The stack should be initialized with data on the heap.
// (Think about what the means in terms of memory allocation)
// The stacks fields should also be initialized to default values.
stack_t *create_stack(unsigned int capacity) {
	// Modify the body of this function as needed.
	if (capacity < 1) {
		return NULL;
	}
	stack_t *myStack = NULL;
	myStack = (stack_t *)malloc(sizeof(stack_t));
	(*myStack).count = 0;

	unsigned int MAX_INT = ((1 << (sizeof(int) * 8 - 2)) - 1) * 2 + 1;

	// Restrict capacity to MAX_INT to avoid int overflow.
	capacity = capacity > MAX_INT ? MAX_INT : capacity;

	// Restrict capacity according to MAX_DEPTH.
	(*myStack).capacity = MAX_DEPTH > capacity ? capacity : MAX_DEPTH;

	(*myStack).head = NULL;
	return myStack;
}

// Free stack
// Removes a stack and ALL of its elements from memory.
// This should be called before the program terminates.
void free_stack(stack_t *s) {
	if (s == NULL) {
		return;
	} else {
		node_t *head = (*s).head;
		node_t *temp = NULL;
		while (head != NULL) {
			temp = (*head).next;
			free(head);
			head = temp;
		}
		free(s);
	}
}

// Stack Empty
// Check if the stack is empty
// Returns 1 if true (The stack is completely empty)
// Returns 0 if false (the stack has at least one element enqueued)
int stack_empty(stack_t *s) {
	if (s == NULL) {
		return -1;
	} else {
		return (*s).count > 0 ? 0 : 1;
	}
}

// Stack Full
// Check if the stack is full
// Returns 1 if true (The Stack is completely full, i.e. equal to capacity)
// Returns 0 if false (the Stack has more space available to enqueue items)
int stack_full(stack_t *s) {
	if (s == NULL) {
		return -1;
	} else {
		return (*s).capacity > (*s).count ? 0 : 1;
	}
}

// Enqueue a new item
// i.e. push a new item into our data structure
// Returns a -1 if the operation fails (otherwise returns 0 on success).
// (i.e. if the Stack is full that is an error, but does not crash the program).
int stack_enqueue(stack_t *s, TIME_UNIT item) {
	if ((*s).count >= (*s).capacity) {
		return -1;
	} else {
		node_t *new_head = (node_t *)malloc(sizeof(node_t));
		(*new_head).data = item;
		(*new_head).next = (*s).head;
		(*s).head = new_head;
		(*s).count++;
		return 0;
	}
}

// Dequeue an item
// Returns the item at the front of the stack and
// removes an item from the stack.
// Removing from an empty stack should crash the program, call exit(1).
TIME_UNIT stack_dequeue(stack_t *s) {
	if (s == NULL) {
		exit(1);
	} else if ((*s).count <= 0) {
		free_stack(s);
		exit(1);
	} else {
		TIME_UNIT ret = (*(*s).head).data;
		node_t *temp = (*s).head;
		(*s).head = (*(*s).head).next;
		free(temp);
		(*s).count--;
		return ret;
	}
}

// Stack Size
// Queries the current size of a stack
// A stack that has not been previously created will crash the program.
// (i.e. A NULL stack cannot return the size)
unsigned int stack_size(stack_t *s) {
	if (s == NULL) {
		return 0;
	} else if ((*s).count < 0) {
		return 0;
	} else {
		return (unsigned int)(*s).count;
	}
}

#endif
