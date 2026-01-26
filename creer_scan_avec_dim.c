#include <stdlib.h>

int creer_scan_avec_dim(int idim, int jdim, int *i_scan, int *j_scan)
{
  // scan = [ (1:1:jdim) ,  ((jdim-1) : -1 : 2)]  ;
  int lenscan = 2*jdim - 2 ;
  int *scan = (int*)malloc(lenscan*sizeof(int)) ;
  
  // Première partie : 1 à jdim
  for (int i = 0; i < jdim; i++) {
    scan[i] = i + 1 ;
  }
  
  // Deuxième partie : (jdim-1) à 2 (décroissant)
  int idx = jdim ;
  for (int i = jdim-1; i >= 2; i--) {
    scan[idx] = i ;
    idx++ ;
  }

  // for n = 1 : 1 : floor(idim/2)
  //   j_scan = [ j_scan , scan] ;
  // end
  int nb_repeat = idim/2 ;
  int pos = 0 ; // position courante dans j_scan
  for (int n = 0; n < nb_repeat; n++) { //pour chaque répétition on copie le scan dans j_scan
    for (int k = 0; k < lenscan; k++) { 
      j_scan[pos] = scan[k] ;
      pos++ ;
    }
  }
  
  // pas = 1/(idim*(jdim-1)) ;
  // i_scan = 0 : pas : 1-pas ;
  // i_scan = i_scan*idim + 1 ;
  int lenscantotal = (jdim-1)*idim ; // lenscantotal = lenscan × nb_repeat
  double pas = 1.0 / (idim*(jdim-1)) ;
  for (int k = 0; k < lenscantotal; k++) {
    i_scan[k] = (int)(k * pas * idim + 1) ;
  }

  free(scan) ;
  return 0 ;
}