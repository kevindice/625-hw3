#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unrolled_int_linked_list.c"

// Test various functions of the linked list implimentation
int main()
{
  initNodePools();

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

  cleanUpNodePools();

  return 0;
}
