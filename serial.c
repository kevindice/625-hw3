#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "unrolled_int_linked_list.c"

#define MAX_KEYWORD_LENGTH 10
#define MAX_LINE_LENGTH 2001

#ifdef VIPER
#define WIKI_FILE "/home/k/kmdice/625/hw3/test10-%s.txt"
#define KEYWORD_FILE "/home/k/kmdice/625/hw3/keywords.txt"
#define OUTPUT_FILE "/home/k/kmdice/625/hw3/output/wiki-%s.out"
#elif PERSONAL
#define WIKI_FILE "/home/kevin/625/hw3/test10-%s.txt"
#define KEYWORD_FILE "/home/kevin/625/hw3/keywords.txt"
#define OUTPUT_FILE "/home/kevin/625/hw3/output/wiki-%s.out"
#else
#define WIKI_FILE "/homes/kmdice/625/hw3/test10-%s.txt"
#define KEYWORD_FILE "/homes/kmdice/625/hw3/keywords.txt"
#define OUTPUT_FILE "/homes/kmdice/625/hw3/output/wiki-%s.out"
#endif


double myclock();

int compare(const void* a, const void* b) {
  const char **ia = (const char **)a;
  const char **ib = (const char **)b;
  return strcmp(*ia, *ib);
}

int main(int argc, char * argv[])
{
  int nwords, maxwords = 50000;
  int nlines, maxlines = 1000000;
  int i, k, n, err, *count;
  double nchars = 0;
  double tstart, ttotal;
  FILE *fd;
  char *wordmem, **word, *linemem, **line, *tempwordmem;
  struct Node** hithead;
  struct Node** hittail;

  if(argc != 3){
    printf("Usage: %s <job id> <input size>", argv[0]);
    return -1;
  }

  // Malloc space for the word list and lines

  count = (int *) calloc( maxwords, sizeof( int ) );

  // Allocate node pools
  allocateNodePools();

  // Contiguous memory for words
  wordmem = malloc(maxwords * MAX_KEYWORD_LENGTH * sizeof(char));
  word = (char **) malloc( maxwords * sizeof( char * ) );
  for(i = 0; i < maxwords; i++){
    word[i] = wordmem + i * MAX_KEYWORD_LENGTH;
  }

  // Allocate arrays for the heads and tails of the lists
  hithead = (struct Node**) malloc( maxwords * sizeof(struct Node *) );
  hittail = (struct Node**) malloc( maxwords * sizeof(struct Node *) );
  for( i = 0; i < maxwords; i++ ) {
    hithead[i] = hittail[i] = node_alloc();
  }

  // Contiguous memory...yay
  linemem = malloc(maxlines * MAX_LINE_LENGTH * sizeof(char));
  line = (char **) malloc( maxlines * sizeof( char * ) );
  for( i = 0; i < maxlines; i++ ) {
    line[i] = linemem + i * MAX_LINE_LENGTH;
  }


  // Read in the dictionary words

  fd = fopen( KEYWORD_FILE, "r" );
  nwords = -1;
  do {
    err = fscanf( fd, "%[^\n]\n", word[++nwords] );
  } while( err != EOF && nwords < maxwords );
  fclose( fd );

  printf( "Read in %d words\n", nwords);

  // sort and copy back to original contiguous memory
  //
  // qsort throws your pointers around and doesn't sort your
  // strings in place.  To do this, we sort, copy over to temp memory
  // by accessing our shuffled/sorted (depends on your perspective)
  // pointers, memcpy back to original memory block

  qsort(word, nwords, sizeof(char *), compare);
  tempwordmem = malloc(maxwords * MAX_KEYWORD_LENGTH * sizeof(char));
  for(i = 0; i < maxwords; i++){
    for(k = 0; k < MAX_KEYWORD_LENGTH; k++){
      *(tempwordmem + i * MAX_KEYWORD_LENGTH + k) = word[i][k];
    }
    word[i] = wordmem + i * MAX_KEYWORD_LENGTH;
  }
  memcpy(wordmem, tempwordmem, maxwords * MAX_KEYWORD_LENGTH);
  free(tempwordmem); tempwordmem = NULL;

  // Read in the lines from the data file

  char *input_file = (char*)malloc(50 * sizeof(char));
  sprintf(input_file, WIKI_FILE, argv[2]);
  fd = fopen( input_file, "r" );
  nlines = -1;
  do {
    err = fscanf( fd, "%[^\n]\n", line[++nlines] );
    if( line[nlines] != NULL ) nchars += (double) strlen( line[nlines] );
  } while( err != EOF && nlines < maxlines);
  fclose( fd );
  free(input_file); input_file = NULL;

  printf( "Read in %d lines averaging %.0lf chars/line\n", nlines, nchars / nlines);

  // Loop over the word list

  tstart = myclock();  // Set the zero time
  tstart = myclock();  // Start the clock


  for( k = 0; k < nlines; k++ ) {
    for( i = 0; i < nwords; i++ ) {
      if( strstr( line[k], word[i] ) != NULL ) {
        count[i]++;
        hittail[i] = add(hittail[i], k);
      }
    }

  }

  ttotal = myclock() - tstart;
  printf( "The serial run took %lf seconds for %d words over %d lines\n",
  ttotal, nwords, nlines);

  // Dump out the word counts

  char *output_file = (char*) malloc(50 * sizeof(char));
  sprintf(output_file, OUTPUT_FILE, argv[1]);

  fd = fopen( output_file, "w" );
    for( i = 0; i < nwords; i++ ) {
      if(count[i] != 0){
        fprintf( fd, "%s: ", word[i] );
        int *line_numbers;
        int len;
        toArray(hithead[i], &line_numbers, &len);
        for (k = 0; k < len - 1; k++) {
          fprintf( fd, "%d, ", line_numbers[k]);
        }
        fprintf( fd, "%d\n", line_numbers[len - 1]);
        free(line_numbers);  line_numbers = NULL;
      }
    }
  fclose( fd );

  free(output_file);  output_file = NULL;

  printf("\n\n========\n"
    "Unrolled linked list stats:\n\n"
    "Node Pools: %d\n"
    "Current Node Count: %d\n"
    "Total Nodes Allocated: %d\n"
    "Nodes in Use: %d\n=========\n",
    _num_node_pools,
    _current_node_count,
    _num_node_pools * MEMORY_POOL_SIZE,
    nodes_in_use
  );

  // Clean up after ourselves

  // Linked list counts
  destroyNodePools();
  free(hithead);  hithead = NULL;
  free(hittail);  hittail = NULL;

  // Words
  free(word);     word = NULL;
  free(wordmem);  wordmem = NULL;

  // Lines
  free(line);     line = NULL;
  free(linemem);  linemem = NULL;
}

double myclock() {
  static time_t t_start = 0;  // Save and subtract off each time

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  if( t_start == 0 ) t_start = ts.tv_sec;

  return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}
