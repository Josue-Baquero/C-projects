#include <stdlib.h>
#include <math.h>

typedef unsigned char uint8;

/**
   calcul la variance en enlevant les bords
  */
double var_vec_bord(uint8** in, int bord, int idim, int jdim)
{
  double var = 0;
  double mean = 0;
  int count = 0;
  
  // Calcul de la moyenne (en excluant les bords)
  for (int i = bord; i < idim - bord; i++) {
    for (int j = bord; j < jdim - bord; j++) {
      mean += in[i][j];
      count++;
    }
  }
  
  if (count == 0) return 0;
  
  mean = mean / count;
  
  // Calcul de la variance
  for (int i = bord; i < idim - bord; i++) {
    for (int j = bord; j < jdim - bord; j++) {
      double diff = in[i][j] - mean;
      var += diff * diff;
    }
  }
  
  var = var / count;
  
  return var;
}
