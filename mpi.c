#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>
#include "unrolled_int_linked_list.c"

#define MAX_KEYWORD_LENGTH 10
#define MAX_LINE_LENGTH 2001

double myclock();

int compare(const void* a, const void* b) {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
}

int main(int argc, char * argv[])
{
 /*  int nwords, maxwords = 50000;
   int nwords, maxwords = 50000; */
   int nwords, maxwords = 50000;
   int nlines, maxlines = 1000000;
   int i, k, n, err, *count;
   int start, end;
   double nchars = 0;
   double tstart, ttotal;
   FILE *fd;
   char *wordmem, **word, *linemem, **line;
   struct Node** hithead;
   struct Node** hitend;

   // MPI related
   int numtasks, rank, len, rc;
   char hostname[MPI_MAX_PROCESSOR_NAME];

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Get_processor_name(hostname, &len);
   printf("Number of tasks= %d, My rank= %d, Running on %s\n", numtasks, rank, hostname);

   if(argc != 3){
      printf("Usage: %s <job id> <input size>", argv[0]);
      return -1;
   }

// Malloc space for the word list and lines

   count = (int *) calloc( maxwords, sizeof( int ) );

   // Init node pools
   initNodePools();

   // Contiguous memory ftw
   wordmem = malloc(maxwords * MAX_KEYWORD_LENGTH * sizeof(char));
   word = (char **) malloc( maxwords * sizeof( char * ) );
   for(i = 0; i < maxwords; i++){
       word[i] = wordmem + i * MAX_KEYWORD_LENGTH;
   }

   hithead = (struct Node**) malloc( maxwords * sizeof(struct Node *) );
   hitend = (struct Node**) malloc( maxwords * sizeof(struct Node *) );
   for( i = 0; i < maxwords; i++ ) {
      hithead[i] = hitend[i] = node_alloc();
   }

   // Contiguous memory...yay
   linemem = malloc(maxlines * MAX_LINE_LENGTH * sizeof(char));
   line = (char **) malloc( maxlines * sizeof( char * ) );
   for( i = 0; i < maxlines; i++ ) {
      line[i] = linemem + i * MAX_LINE_LENGTH;
   }


// Read in the dictionary words
if(rank == 0)
{
   fd = fopen( "/homes/kmdice/625/hw3/keywords.txt", "r" );
   nwords = -1;
   do {
      err = fscanf( fd, "%[^\n]\n", word[++nwords] );
   } while( err != EOF && nwords < maxwords );
   fclose( fd );

   qsort(word, nwords, sizeof(char *), compare);

   char *tempmem = malloc( maxwords * MAX_KEYWORD_LENGTH * sizeof(char) );
   int i;
   for(i = 0; i < nwords; i++)
   {
       memcpy(tempmem + i * MAX_KEYWORD_LENGTH, word[i], MAX_KEYWORD_LENGTH);
       word[i] = wordmem + i * MAX_KEYWORD_LENGTH; // Fix pointers after qsort sorted them
   }
   memcpy(wordmem, tempmem, maxwords * MAX_KEYWORD_LENGTH * sizeof(char));
   free(tempmem);

   printf( "Read in %d words in proc %d\n", nwords, rank);
}

   MPI_Bcast(wordmem, maxwords * MAX_KEYWORD_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);

   printf("After bcast, proc %d says %s, %s, %s\n", rank, word[0], word[1], word[2]);

// Read in the lines from the data file
if(rank == 0)
{
   char *input_file = (char*)malloc(50 * sizeof(char));
   sprintf(input_file, "/homes/kmdice/625/hw3/test10-%s.txt", argv[2]);
   fd = fopen( input_file, "r" );
   nlines = -1;
   do {
      err = fscanf( fd, "%[^\n]\n", line[++nlines] );
      if( line[nlines] != NULL ) nchars += (double) strlen( line[nlines] );
   } while( err != EOF && nlines < maxlines);
   fclose( fd );
   free(input_file);

   printf( "Read in %d lines averaging %.0lf chars/line\n", nlines, nchars / nlines);
}

   MPI_Bcast(linemem, maxlines * MAX_LINE_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);

   printf("After bcast, proc %d says %c\n", rank, line[4][25]);

// Set up timing
   if(rank == 0)
   {
      tstart = myclock();  // Set the zero time
      tstart = myclock();  // Start the clock
   }


// Division of work
   start = rank * (nwords/numtasks);
   end = (rank + 1) * (nwords/numtasks);
   if(rank == numtasks - 1) end = nwords;

// Loop over the word list
   for( k = 0; k < nlines; k++ ) {
      for( i = start; i < end; i++ ) {
         if( strstr( line[k], word[i] ) != NULL ) {
	    count[i]++;
	    hitend[i] = add(hitend[i], k);
	 } 
      }

   }

if(rank == 0)
{
   ttotal = myclock() - tstart;
   printf( "The mpi run took %lf seconds for %d words over %d lines\n",
           ttotal, nwords, nlines);
}

// Dump out the word counts

   char *output_file = (char*)malloc(50 * sizeof(char));
   sprintf(output_file, "/homes/kmdice/625/hw3/output/wiki-%s-part-%d.out", argv[1], rank);
   fd = fopen( output_file, "w" );
   for( i = start; i < end; i++ ) {
      if(count[i] != 0){
         fprintf( fd, "%s: ", word[i] );
         int *line_numbers;
         int len;
         toArray(hithead[i], &line_numbers, &len);
         for (k = 0; k < len - 1; k++) {
            fprintf( fd, "%d, ", line_numbers[k]);
         }
         fprintf( fd, "%d\n", line_numbers[len - 1]);
         free(line_numbers);
      }
   }
   fclose( fd );

// Clean up after ourselves

   // Linked list counts
   cleanUpNodePools();
   free(hithead);
   free(hitend);

   // Words
   free(word);
   free(wordmem);

   // Lines
   free(line);
   free(linemem);

  // printf("\n\n\nUnrolled linked list stats:\n\nNode Pools: %d\nCurrent Node Count: %d\nTotal Nodes Allocated: %d\nNodes in Use: %d", num_node_pools, current_node_count, num_node_pools * MEMORY_POOL_SIZE, nodes_in_use);

   MPI_Finalize();
}

double myclock() {
   static time_t t_start = 0;  // Save and subtract off each time

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   if( t_start == 0 ) t_start = ts.tv_sec;

   return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}
