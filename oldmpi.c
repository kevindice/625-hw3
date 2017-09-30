#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>
#include "unrolled_int_linked_list.c"

#define MAX_KEYWORD_LENGTH 10
#define MAX_LINE_LENGTH 2001
#define MAX_WORDS 50000
#define MAX_LINES 1000000

#ifdef VIPER
   #define WIKI_FILE "/home/k/kmdice/625/hw3/test10-%s.txt"
   #define KEYWORD_FILE "/home/k/kmdice/625/hw3/keywords.txt"
   #define OUTPUT_FILE "/home/k/kmdice/625/hw3/output/wiki-%s-part-%03d.out"
#else
   #define WIKI_FILE "/homes/kmdice/625/hw3/test10-%s.txt"
   #define KEYWORD_FILE "/homes/kmdice/625/hw3/keywords.txt"
   #define OUTPUT_FILE "/homes/kmdice/625/hw3/output/wiki-%s-part-%03d.out"
#endif

double myclock();
int compare(const void* a, const void* b);
void read_keywords();
void sort_keywords();
void read_wiki_data();

int main(int argc, char * argv[])
{
   int nwords = 0;
   int nlines = 0;
   int i, k, n, err, *count;
   int start, end;
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

   if(argc != 6){
      printf("Usage: %s <job id> <input size> <parallel environment> <nslots> <nhosts>", argv[0]);
      return -1;
   }

   // Set up timing
   if(rank == 0)
   {
      tstart = myclock();  // Set the zero time
      tstart = myclock();  // Start the clock
   }

// Malloc space for the word list and lines

   count = (int *) calloc( MAX_WORDS, sizeof( int ) );

   // Init node pools
   allocateNodePools();

   // Contiguous memory ftw
   wordmem = malloc(MAX_WORDS * MAX_KEYWORD_LENGTH * sizeof(char));
   word = (char **) malloc( MAX_WORDS * sizeof( char * ) );
   for(i = 0; i < MAX_WORDS; i++){
       word[i] = wordmem + i * MAX_KEYWORD_LENGTH;
   }

   hithead = (struct Node**) malloc( MAX_WORDS * sizeof(struct Node *) );
   hitend = (struct Node**) malloc( MAX_WORDS * sizeof(struct Node *) );
   for( i = 0; i < MAX_WORDS; i++ ) {
      hithead[i] = hitend[i] = node_alloc();
   }

   // Contiguous memory...yay
   linemem = malloc(MAX_LINES * MAX_LINE_LENGTH * sizeof(char));
   line = (char **) malloc( MAX_LINES * sizeof( char * ) );
   for( i = 0; i < MAX_LINES; i++ ) {
      line[i] = linemem + i * MAX_LINE_LENGTH;
   }


// Read in the dictionary words
if(rank == 0)
{
   read_keywords(word, &nwords);
   sort_keywords(wordmem, word, nwords);
   printf( "Read in %d words in proc %d\n", nwords, rank);
}

// Read in the lines from the data file
if(rank == 0)
{
   double nchars = 0;
   read_wiki_data(line, &nlines, &nchars, argv[2]);
   printf( "Read in %d lines averaging %.0lf chars/line\n", nlines, nchars / nlines);
}


// Broadcast data
   MPI_Bcast(&nwords, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&nlines, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(wordmem, MAX_WORDS * MAX_KEYWORD_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);
   MPI_Bcast(linemem, MAX_LINES * MAX_LINE_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);


// Division of work
   start = rank * (nwords/numtasks);
   end = (rank + 1) * (nwords/numtasks);
   if(rank == numtasks - 1) end = nwords;

   printf("------- Proc: %d, Start: %d, End: %d, Nwords: %d, Num tasks: %d --------\n", rank, start, end, nwords, numtasks);


// Loop over the word list
   for( k = 0; k < nlines; k++ ) {
      for( i = start; i < end; i++ ) {
         if( strstr( line[k], word[i] ) != NULL ) {
	          count[i]++;
	          hitend[i] = add(hitend[i], k);
	       }
      }
   }

   printf("Done with computation %d \n", rank);


// Dump out the word counts

   printf("Proc %d starting output\n", rank);
   char *output_file = malloc(50 * sizeof(char));
   sprintf(output_file, OUTPUT_FILE, argv[1], rank);
   printf("outputfile: %s\n", output_file);
   fd = fopen( output_file, "w" );
   //printf("!!!!!!!!!!!!!!! File pointer: %p\n", fd);
   //printf("ASDFASDF 1\n"); fflush(stdin);
   for( i = start; i < end; i++ ) {
     //printf("ASDFASDF 2\n"); fflush(stdin);
      if(count[i] != 0){
        //printf("ASDFASDF 3\n"); fflush(stdin);
         fprintf( fd, "%s: ", "cake" );
         int *line_numbers;
         int len;
         //printf("ASDFASDF 4\n"); fflush(stdin);
         toArray(hithead[i], &line_numbers, &len);
         //printf("ASDFASDF 5\n"); fflush(stdin);
         for (k = 0; k < len - 1; k++) {
            //printf("ASDFASDF 6\n"); fflush(stdin);
            fprintf( fd, "%d, ", line_numbers[k]);
         }
         fprintf( fd, "%d\n", line_numbers[len - 1]);
         //free(line_numbers); line_numbers = NULL;
      }
   }
   fclose( fd );
   //free(output_file); output_file = NULL;

// Take end time when all are finished writing the file
   MPI_Barrier(MPI_COMM_WORLD);

   if(rank == 0)
   {
      ttotal = myclock() - tstart;
      printf( "DATA\t%lf\t%d\t%d\t%s\t%s\t%s\n",
         ttotal, nwords, nlines, argv[3], argv[4], argv[5]); fflush(stdout);
   }


// Clean up after ourselves

   MPI_Finalize();

   // Linked list counts
   printf("Freeing stuff\n"); fflush(stdout);
   destroyNodePools();
   printf("Node pools freed\n"); fflush(stdout);
   //free(hithead); hithead = NULL;
   printf("Head list free\n"); fflush(stdout);
   //free(hitend); hitend = NULL;
   printf("End list free\n"); fflush(stdout);

   // Words
   //free(word); word = NULL;
   printf("Word freed\n"); fflush(stdout);
   //free(wordmem); wordmem = NULL;
   printf("Word mem freed\n"); fflush(stdout);

   // Lines
   //free(line); line = NULL;
   printf("Line freed\n"); fflush(stdout);
   //free(linemem); linemem = NULL;
   printf("Line mem freed\n"); fflush(stdout);

  // printf("\n\n\nUnrolled linked list stats:\n\nNode Pools: %d\nCurrent Node Count: %d\nTotal Nodes Allocated: %d\nNodes in Use: %d", num_node_pools, current_node_count, num_node_pools * MEMORY_POOL_SIZE, nodes_in_use);


}

double myclock() {
   static time_t t_start = 0;  // Save and subtract off each time

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   if( t_start == 0 ) t_start = ts.tv_sec;

   return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}

int compare(const void* a, const void* b) {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
}

void read_keywords(char **word, int *nwords)
{
  int err;
  FILE *fd;

  fd = fopen( KEYWORD_FILE, "r" );
  if(fd == NULL)
  {
      printf("File not found!"); fflush(stdout); exit(1);
  }
  *nwords = -1;
  do {
     err = fscanf( fd, "%[^\n]\n", word[++(*nwords)] );
  } while( err != EOF && (*nwords) < MAX_WORDS );
  fclose( fd );
}

void sort_keywords(char *wordmem, char **word, int nwords)
{
  qsort(word, nwords, sizeof(char *), compare);

  char *tempmem = malloc( MAX_WORDS * MAX_KEYWORD_LENGTH * sizeof(char) );
  int i;
  for(i = 0; i < nwords; i++)
  {
      memcpy(tempmem + i * MAX_KEYWORD_LENGTH, word[i], MAX_KEYWORD_LENGTH);
      word[i] = wordmem + i * MAX_KEYWORD_LENGTH; // Fix pointers after qsort sorted them
  }
  memcpy(wordmem, tempmem, MAX_WORDS * MAX_KEYWORD_LENGTH * sizeof(char));
  //free(tempmem); tempmem = NULL;
}

void read_wiki_data(char **line, int *nlines, double *nchars, char *input_size)
{
  FILE *fd;
  int err;
  char *input_file = (char*)malloc(500 * sizeof(char));
  sprintf(input_file, WIKI_FILE, input_size);
  fd = fopen( input_file, "r" );
  if(fd == NULL)
  {
      printf("File not found!"); fflush(stdout); exit(1);
  }

  *nlines = -1;
  do {
     err = fscanf( fd, "%[^\n]\n", line[++(*nlines)] );
     if( line[*nlines] != NULL ) *nchars += (double) strlen( line[*nlines] );
  } while( err != EOF && *nlines < MAX_LINES);
  fclose( fd );
  //free(input_file); input_file = NULL;
}
