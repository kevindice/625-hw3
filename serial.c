#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

double myclock();

int compare(const void* a, const void* b) {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
}

int main()
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

// Malloc space for the word list and lines

   word = (char **) malloc( maxwords * sizeof( char * ) );
   count = (int *) malloc( maxwords * sizeof( int ) );
   for( i = 0; i < maxwords; i++ ) {
      word[i] = malloc( 10 );
      count[i] = 0;
   }

   line = (char **) malloc( maxlines * sizeof( char * ) );
   for( i = 0; i < maxlines; i++ ) {
      line[i] = malloc( 2001 );
   }


// Read in the dictionary words

   fd = fopen( "./keywords.txt", "r" );
   nwords = -1;
   do {
      err = fscanf( fd, "%[^\n]\n", word[++nwords] );
   } while( err != EOF && nwords < maxwords );
   fclose( fd );

   printf( "Read in %d words\n", nwords);


// Read in the lines from the data file

   fd = fopen( "./test10-4.txt", "r" );
   nlines = -1;
   do {
      err = fscanf( fd, "%[^\n]\n", line[++nlines] );
      if( line[nlines] != NULL ) nchars += (double) strlen( line[nlines] );
   } while( err != EOF && nlines < maxlines);
   fclose( fd );

   printf( "Read in %d lines averaging %.0lf chars/line\n", nlines, nchars / nlines);


// sort

   qsort(word, nwords, sizeof(char *), compare);


// Loop over the word list

   tstart = myclock();  // Set the zero time
   tstart = myclock();  // Start the clock

   for( i = 0; i < nwords; i += 125 ) {

      for( k = 0; k < nlines; k++ ) {
         #include "unrolled_loop_body_125.c"
      }

   }

   ttotal = myclock() - tstart;
   printf( "The serial run took %lf seconds for %d words\n",
           ttotal, nwords);

// Dump out the word counts

   fd = fopen( "wiki.out", "w" );
   for( i = 0; i < nwords; i++ ) {
      fprintf( fd, "%d %s %d\n", i, word[i], count[i] );
   }
   fprintf( fd, "The serial run took %lf seconds for %d words\n",
           ttotal, nwords);
   fclose( fd );

}

double myclock() {
   static time_t t_start = 0;  // Save and subtract off each time

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   if( t_start == 0 ) t_start = ts.tv_sec;

   return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}
