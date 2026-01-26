/*
 * Test de la fonction creer_scan_avec_dim
 * 
 * Compilation:
 *   gcc test_creer_scan_avec_dim.c creer_scan_avec_dim.c -o test_creer_scan_avec_dim -Wall
 * 
 * Exécution:
 *   ./test_creer_scan_avec_dim
 */

#include <stdlib.h>
#include <stdio.h>

typedef unsigned char uint8;

int creer_scan_avec_dim(int idim, int jdim, int *i_scan, int *j_scan);

int main()
{
    // Dimensions de test
    int idim = 8;
    int jdim = 6;
    
    int lenscantotal = (jdim-1) * idim;
    
    // Allocation
    int* i_scan = (int*)malloc(lenscantotal * sizeof(int));
    int* j_scan = (int*)malloc(lenscantotal * sizeof(int));
    
    // Appel de la fonction
    creer_scan_avec_dim(idim, jdim, i_scan, j_scan);
    
    // Affichage des résultats
    printf("lenscantotal = %d\n", lenscantotal);
    printf("\nPremiers éléments:\n");
    for (int k = 0; k < 20 && k < lenscantotal; k++) {
        printf("k=%2d: i_scan=%d, j_scan=%d\n", k, i_scan[k], j_scan[k]);
    }
    
    printf("\n...\n\nDerniers éléments:\n");
    for (int k = lenscantotal-10; k < lenscantotal; k++) {
        printf("k=%2d: i_scan=%d, j_scan=%d\n", k, i_scan[k], j_scan[k]);
    }
    
    // Libération
    free(i_scan);
    free(j_scan);
    
    return 0;
}
