#include <stdlib.h>

typedef unsigned char uint8;

/**
   filtrer localement par au kernel 3x3 : ones(3,3)/9
   filtre pas les bords
 */
void filter2D_local(uint8** in, uint8** out, int idim, int jdim)
{
  // Filtre moyenneur 3x3
  // Ne traite pas les bords (i=0, i=idim-1, j=0, j=jdim-1)
  
  for (int i = 1; i < idim-1; i++) {
    for (int j = 1; j < jdim-1; j++) {
      // Somme des 9 pixels voisins
      int sum = 0;
      for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
          sum += in[i+di][j+dj];
        }
      }
      // Moyenne : division par 9
      out[i][j] = sum / 9;
    }
  }
  
  // Copier les bords sans modification
  for (int j = 0; j < jdim; j++) {
    out[0][j] = in[0][j];           // première ligne
    out[idim-1][j] = in[idim-1][j]; // dernière ligne
  }
  for (int i = 0; i < idim; i++) {
    out[i][0] = in[i][0];           // première colonne
    out[i][jdim-1] = in[i][jdim-1]; // dernière colonne
  }
}
