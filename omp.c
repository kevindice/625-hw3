#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include "unrolled_int_linked_list.c"

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
   int i, k, n, err, *count, nthreads = 4;
   double nchars = 0;
   double tstart, ttotal;
   FILE *fd;
   char **word, **line;
   struct Node** hithead;
   struct Node** hitend;

   if(argc != 3){
      printf("Usage: %s <job id> <input size>", argv[0]);
      return -1;
   }

// Malloc space for the word list and lines

   word = (char **) malloc( maxwords * sizeof( char * ) );
   count = (int *) malloc( maxwords * sizeof( int ) );
   hithead = (struct Node**) malloc( maxwords * sizeof(struct Node *) );
   hitend = (struct Node**) malloc( maxwords * sizeof(struct Node *) );
   for( i = 0; i < maxwords; i++ ) {
      word[i] = malloc( 10 );
      hithead[i] = hitend[i] = (struct Node *) calloc(1, sizeof(struct Node));
      count[i] = 0;
   }

   line = (char **) malloc( maxlines * sizeof( char * ) );
   for( i = 0; i < maxlines; i++ ) {
      line[i] = malloc( 2001 );
   }


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


// Loop over the word list

   tstart = myclock();  // Set the zero time
   tstart = myclock();  // Start the clock

   omp_set_num_threads( nthreads );

#pragma omp parallel for schedule( dynamic ) private(i,k)
   for( k = 0; k < nlines; k++ ) {
      for( i = 0; i < nwords; i++ ) {
         if( strstr( line[k], word[i] ) != NULL ) {
	    count[i]++;
	    hitend[i] = add(hitend[i], k);
	 } 
      }

   }

   ttotal = myclock() - tstart;
   printf( "The flipped serial run took %lf seconds for %d words over %d lines\n",
           ttotal, nwords, nlines);

// Dump out the word counts

   char *output_file = (char*)malloc(50 * sizeof(char));
   sprintf(output_file, "/homes/kmdice/625/hw3/output/wiki-%s.out", argv[1]);
   fd = fopen( output_file, "w" );
   for( i = 0; i < nwords; i++ ) {
      if(count[i] != 0){
         fprintf( fd, "%d %s (%d): ", i, word[i], count[i] );
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
   fprintf( fd, "The flipped serial run took %lf seconds for %d words over %d lines on %d\n",
           ttotal, nwords, nlines, nthreads);
   fclose( fd );

// Clean up after ourselves

   for(i = 0; i < maxwords; i++ ) {
      free(word[i]);
      destroy(hithead[i]);
   }
   free(word);
   free(hithead);
   free(hitend);

   for(i = 0; i < maxlines; i++ ) {
      free(line[i]);
   }
   free(line);

}

double myclock() {
   static time_t t_start = 0;  // Save and subtract off each time

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   if( t_start == 0 ) t_start = ts.tv_sec;

   return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}
