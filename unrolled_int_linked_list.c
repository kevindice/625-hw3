#include<stdio.h>
#include<stdlib.h>
#define MAX_ELEMENTS 16

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

// Covert linked list to array
