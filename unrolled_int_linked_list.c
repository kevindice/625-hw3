#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#ifndef MAX_ELEMENTS
#define MAX_ELEMENTS 128
#endif

#define MEMORY_POOL_SIZE 100000
#define MAX_NUM_MEMORY_POOLS 1000

/*
 * Unrolled integer linked list
 *
 * This linked list implimentation is read and write
 * only. No delete operations are implimented to make
 * my life eaiser.
 *
 * This is an attempt at a cache and locality friendly,
 * reasonably space-efficient way to store a sparce
 * matrix of keyword search hits.
 *
 */


int num_node_pools = 0;
struct Node **node_pools;
int current_node_count = 0;
int nodes_in_use = 0;

// Unrolled Linked List Node
struct Node
{
    int numElements;
    int array[MAX_ELEMENTS];
    struct Node *next;
};

// Allocat a node pool
void allocateNewPool()
{
   node_pools[num_node_pools++] = malloc(MEMORY_POOL_SIZE * sizeof(struct Node));
   current_node_count = 0;
}

// Init node pool array and allocate first pool
void initNodePools()
{
    // Allocate our pool of nodes
    num_node_pools = 0;
    current_node_count = 0;
    node_pools = (struct Node **) malloc(MAX_NUM_MEMORY_POOLS * sizeof(struct Node *));
    allocateNewPool();
}

// Deal out space for a node from contiguous memory pool
struct Node* node_alloc()
{
    if(current_node_count == MEMORY_POOL_SIZE)
    {
        allocateNewPool();
    }
    nodes_in_use++;
    return &(node_pools[num_node_pools - 1][current_node_count++]);
}

// Cleanup after ourselves
void cleanUpNodePools()
{
    int i;
    for(i = 0; i < num_node_pools; i++)
    {
        free(node_pools[i]);
        node_pools[i] = 0;
        printf("%d\n", num_node_pools); fflush(stdout);
    }
    free(node_pools);
}


// Print function for testing
void printUnrolledList(struct Node *n)
{
    while (n != NULL)
    {
        int i;
        for (i=0; i< n->numElements; i++)
	{
	    printf("%d\n", n->array[i]);
	}
	n = n->next;
    }
}

// Find number of elements stored
int getLength(struct Node *n)
{
    int len = 0;
    while(n != NULL)
    {
        len += n->numElements;
	n = n->next;
    }
    return len;
}

// Covert linked list to array
void toArray(struct Node *n, int **array, int *length)
{
    *length = getLength(n);
    *array =  (int *) calloc(*length, sizeof(int));

    int pos = 0;
    while (n != NULL)
    {
	memcpy(*array + pos, n->array, n->numElements * sizeof(int));
	pos += n->numElements;
	n = n->next;
    }
}

// Add an integer to the list, returning the current node
// if not yet full or the new node if one is created
struct Node* add(struct Node *n, int x)
{
    // We are given an empty list, so create the head node
    if(n == NULL)
    {
        struct Node* new_head_node = NULL;
	      new_head_node = node_alloc();
	      return add(new_head_node, x);
    }

    // We are given a full list, so create a new node insert into it
    if(n->numElements == MAX_ELEMENTS)
    {
        struct Node* new_node = NULL;
	      new_node = node_alloc();
	      n->next = new_node;
	      return add(new_node, x);
    }

    n->array[n->numElements] = x;
    n->numElements += 1;
    return n;
}

// Destroy a list given the head node
void destroy(struct Node *n)
{
    while (n != NULL)
    {
        struct Node* next = n->next;
	//printf("Destroying node with first element %d\n", n->array[0]);
	free(n);
	n = next;
    }
}

// Test various functions of the linked list implimentation
int testing_stuff()
{

    struct Node* head = NULL;
    struct Node* second = NULL;
    struct Node* third = NULL;

    head = node_alloc();
    second = node_alloc();
    third = node_alloc();

    head->numElements = 4;
    head->array[0] = 3;
    head->array[1] = 7;
    head->array[2] = 6;
    head->array[3] = 9;

    second->numElements = 7;
    second->array[0] = 1;
    second->array[1] = 2;
    second->array[2] = 3;
    second->array[3] = 4;
    second->array[4] = 5;
    second->array[5] = 6;
    second->array[6] = 7;

    third->numElements = 6;
    third->array[0] = 3;
    third->array[1] = 7;
    third->array[2] = 6;
    third->array[3] = 9;
    third->array[4] = 13;
    third->array[5] = 19;

    // Test the add function
    int j;
    struct Node* current = third;
    for (j = 25; j < 52; j++)
    {
        current = add(current, j);
    }

    // Link together
    head->next = second;
    second->next = third;

    printf("Print linked list directly:\n\n");
    printUnrolledList(head);

    int *my_array;
    int length;
    toArray(head, &my_array, &length);

    printf("The length of my array is %d.\n", length);
    printf("The contents of my array is:\n");
    int i;
    for(i=0; i < length; i ++)
    {
        printf("%d\n", my_array[i]);
    }
    free(my_array);

    destroy(head);

    return 0;
}
