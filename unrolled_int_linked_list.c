#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#ifndef MAX_ELEMENTS
#define MAX_ELEMENTS 128
#endif

#define MEMORY_POOL_SIZE 25600
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

// Allocate a node pool
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
    node_pools[i] = NULL;
    printf("%d\n", num_node_pools); fflush(stdout);
  }
  free(node_pools);
  node_pools = NULL;
  num_node_pools = 0;
  current_node_count = 0;
  nodes_in_use = 0;
}


// Print function for testing
void printUnrolledList(struct Node *n)
{
  while (n != NULL)
  {
    int i;
    for (i = 0; i < n->numElements; i++)
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
  *array =  (int *) malloc(*length * sizeof(int));

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
