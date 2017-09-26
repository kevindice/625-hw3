#include<stdio.h>
#include<stdlib.h>
#define MAX_ELEMENTS 16

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


// Unrolled Linked List Node
struct Node
{
    int numElements;
    int array[maxElements];
    struct Node *next;
};


// Print function for testing
void printUnrolledList(struct Node *n)
{
    while (n != NULL)
    {
        for (int i=0; i< n->numElements; i++)
	{
	    printf("%d", n->array[i]);
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
    }
    return len;
}

// Covert linked list to array
void toArray(struct Node *n, int **array, int *length)
{
    length = getLength(n);
    array = malloc(length * sizeof(int));

    int pos = 0;
    while (n != NULL)
    {
	memcpy(array + pos, n->array, n->numElements * sizeof(int));
	pos += n->numElements;
	n = n->next;
    }
}
