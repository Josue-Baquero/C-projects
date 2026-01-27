#include <stdlib.h>
#include <math.h>

typedef unsigned char uint8;

/**
   calcul la variance en enlevant les bords
  */
double var_vec_bord(uint8** in, int bord, int idim, int jdim)
{
  double sum = 0;      // Somme des valeurs
  double sum_sq = 0;   // Somme des carrés
  int count = 0;
  
  // Une seule passe : calcul de somme et somme des carrés
  for (int i = bord; i < idim - bord; i++) {
    for (int j = bord; j < jdim - bord; j++) {
      double val = in[i][j];
      sum += val;
      sum_sq += val * val;
      count++;
    }
  }
  
  if (count == 0) return 0;
  
  // Variance = E[X²] - E[X]²
  double mean = sum / count;
  double var = sum_sq / count - mean * mean;
  
  return var;
}
