#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
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
   double nchars = 0;
   double tstart, ttotal;
   FILE *fd;
   char **word, **line;
   struct Node** hithead;
   struct Node** hitend;

   int rank;
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if(rank == 0 && argc != 3){
      printf("Usage: %s <job id> <input size>", argv[0]);
      return -1;
   }

// Malloc space for the word list and lines

   count = (int *) malloc( maxwords * sizeof( int ) );

   // Contiguous memory ftw
   word = (char **) malloc( maxwords * sizeof( char * ) );
   word[0] = (char *) malloc(sizeof(char) * maxwords * MAX_KEYWORD_LENGTH);
   for(i = 0; i < maxwords; i++){
       word[i] = (*word + i * MAX_KEYWORD_LENGTH);
   }

   hithead = (struct Node**) malloc( maxwords * sizeof(struct Node *) );
   hitend = (struct Node**) malloc( maxwords * sizeof(struct Node *) );
   for( i = 0; i < maxwords; i++ ) {
      word[i] = malloc( 10 );
      hithead[i] = hitend[i] = (struct Node *) calloc(1, sizeof(struct Node));
      count[i] = 0;
   }

   // Contiguous memory...yay
   line = (char **) malloc( maxlines * sizeof( char * ) );
   line[0] = (char *) malloc(sizeof(char) * maxlines * MAX_LINE_LENGTH);
   for( i = 0; i < maxlines; i++ ) {
      line[i] = (*line + i * MAX_LINE_LENGTH);
   }


   if (rank == 0){

     // Read in the dictionary words
 
     fd = fopen( "/homes/kmdice/625/hw3/keywords.txt", "r" );
     nwords = -1;
     do {
        err = fscanf( fd, "%[^\n]\n", word[++nwords] );
     } while( err != EOF && nwords < maxwords );
     fclose( fd );

     printf( "Read in %d words\n", nwords);


     // Read in the lines from the data file

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


     // sort

     qsort(word, nwords, sizeof(char *), compare);

   } // End read in stuff (only root node)

   // B CAST arrays here
   MPI_Bcast(word[0], maxwords * MAX_KEYWORD_LENGTH * sizeof(char), MPI_CHAR, 0, MPI_COMM_WORLD);
   MPI_Bcast(line[0], maxlines * MAX_LINE_LENGTH * sizeof(char), MPI_CHAR, 0, MPI_COMM_WORLD);



   // sanity check
   printf("Everyone should print this:  %s\n", word[0]);

// Loop over the word list

   tstart = myclock();  // Set the zero time
   tstart = myclock();  // Start the clock


//   for( k = 0; k < nlines; k++ ) {
//      for( i = 0; i < nwords; i++ ) {
//         if( strstr( line[k], word[i] ) != NULL ) {
//	    count[i]++;
//	    hitend[i] = add(hitend[i], k);
//	 } 
//      }
//
//   }

   ttotal = myclock() - tstart;
   printf( "The serial run took %lf seconds for %d words over %d lines\n",
           ttotal, nwords, nlines);

// Dump out the word counts

   char *output_file = (char*)malloc(50 * sizeof(char));
   sprintf(output_file, "/homes/kmdice/625/hw3/output/wiki-%s.out", argv[1]);
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
         free(line_numbers);
      }
   }
   fprintf( fd, "The serial run took %lf seconds for %d words over %d lines\n",
           ttotal, nwords, nlines);
   fclose( fd );

// Clean up after ourselves

   for(i = 0; i < maxwords; i++ ) {
      destroy(hithead[i]);
   }
   free(word[0]);
   free(word);
   free(hithead);
   free(hitend);

   free(line[0]);
   free(line);
}

double myclock() {
   static time_t t_start = 0;  // Save and subtract off each time

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   if( t_start == 0 ) t_start = ts.tv_sec;

   return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}
