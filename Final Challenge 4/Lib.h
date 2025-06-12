#include <stdio.h>
#include <stdlib.h>




void readEdge (int *i, int *j)
/* Scans input to read a blocked edge.
   Returns the indices i and j in m[13][13] of an edge.
   Call this function with pointers to the variables
   that should receive the values, e.g.
       readEdge (&x, &y);
*/
{
   int cx, cy;
   char d;
   scanf ("%d %d %c", &cy, &cx, &d);
   if (d == 's') {
      *i = 2 + cy * 2 + 1;
      *j = 2 + cx * 2;
   }
   else if (d == 'e') {
      *i = 2 + cy * 2;
      *j = 2 + cx * 2 + 1;
   }
   else {
      fprintf (stderr, "error on direction: %c\n", d);
      exit (1);
   }
}

void readStation (int *i, int *j, int Station)
/* Scans input to read a station.
   Returns the indices i and j in m[13][13] of a station.
 */
{
   switch (Station) {
      case 1: *i = 12, *j = 4; break;
      case 2: *i = 12, *j = 6; break;
      case 3: *i = 12, *j = 8; break;
      case 4: *i = 8, *j = 12; break;
      case 5: *i = 6, *j = 12; break;
      case 6: *i = 4, *j = 12; break;
      case 7: *i = 0, *j = 8; break;
      case 8: *i = 0, *j = 6; break;
      case 9: *i = 0, *j = 4; break;
      case 10: *i = 4, *j = 0; break;
      case 11: *i = 6, *j = 0; break;
      case 12: *i = 8, *j = 0; break;
      default: fprintf (stderr, "Illegal station\n"); exit (-1);
   }
}

void printCrossingName (int i, int j)
/* Print the name of the crossing with indices i and j in m[13][13]/ */
{
   if ((i-2)%2 == 0 && (j-2)%2 == 0)
      printf ("c%d%d ",  (i-2)/2, (j-2)/2);
}


char* getCrossingName(int i, int j) {
   if ((i - 2) % 2 == 0 && (j - 2) % 2 == 0) {
       int row = (i - 2) / 2;
       int col = (j - 2) / 2;
       char* crossingName = (char*)malloc(5 * sizeof(char));
       if (crossingName == NULL) {
           perror("Failed to allocate memory");
           return NULL;
       }
       sprintf(crossingName, "c%d%d ", row, col);
       return crossingName;
   } else {
       return NULL;
   }
}

void printMatrix (int m[][13])
/* Print the elements of the matrix m[13][13]. */
{
   int i, j;

   for (i = 0; i < 13; i++) {
      for (j = 0; j < 13; j++) {
         printf ("%2d ", m[i][j]);
      }
      printf ("\n");
   }
}
