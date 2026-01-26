#include <stdlib.h>

typedef unsigned char uint8;

int reconstruction_scan(uint8* vec_illu, int* i_scan, int* j_scan, int len, 
			uint8** reconst, int offset, int idim, int jdim)
{
  // Initialiser l'image à 0
  for (int i = 0; i < idim; i++) {
    for (int j = 0; j < jdim; j++) {
      reconst[i][j] = 0;
    }
  }
  
  // for n = 1:len
  //   out(floor(i_scan(n)) , floor(j_scan(n))) = vec_illu(n) ;
  // end
  
  for (int n = 0; n < len; n++) {
    int idx = (n + offset) % len;  // Position dans vec_illu avec décalage
    int i = i_scan[n] - 1;  // -1 car MATLAB indexe à partir de 1
    int j = j_scan[n] - 1;  // -1 car MATLAB indexe à partir de 1
    
    reconst[i][j] = vec_illu[idx];
  }
 
  return 0;
}
