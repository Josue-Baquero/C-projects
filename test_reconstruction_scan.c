/*
 * Test de la fonction reconstruction_scan
 * 
 * Compilation:
 *   gcc test_reconstruction_scan.c creer_scan_avec_dim.c reconstruction_scan.c -o test_reconstruction_scan -Wall
 * 
 * Exécution:
 *   ./test_reconstruction_scan
 */

#include <stdlib.h>
#include <stdio.h>

typedef unsigned char uint8;

int creer_scan_avec_dim(int idim, int jdim, int *i_scan, int *j_scan);
int reconstruction_scan(uint8* vec_illu, int* i_scan, int* j_scan, int len, 
			uint8** reconst, int offset, int idim, int jdim);

uint8** alloue_2D_char(int ligne, int colonne) {
    uint8 **tmp = malloc(ligne*sizeof(char*));
    uint8 *data = calloc(ligne*colonne, sizeof(char));
    for (int i=0; i<ligne; i++)
        tmp[i] = &(data[i*colonne]);
    return tmp;
}

void free_2D_char(uint8** tab) {
    free(tab[0]);
    free(tab);
}

int main()
{
    int idim = 8;
    int jdim = 6;
    int lenscantotal = (jdim-1) * idim;
    
    // Allocation
    int* i_scan = (int*)malloc(lenscantotal * sizeof(int));
    int* j_scan = (int*)malloc(lenscantotal * sizeof(int));
    uint8* vec_illu = (uint8*)malloc(lenscantotal * sizeof(uint8));
    uint8** reconst = alloue_2D_char(idim, jdim);
    
    // Créer le parcours de scan
    creer_scan_avec_dim(idim, jdim, i_scan, j_scan);
    
    // Remplir vec_illu avec des valeurs de test (0 à 255)
    for (int k = 0; k < lenscantotal; k++) {
        vec_illu[k] = (k * 255) / lenscantotal;
    }
    
    // Test sans offset
    printf("Test reconstruction_scan sans offset (offset=0):\n");
    reconstruction_scan(vec_illu, i_scan, j_scan, lenscantotal, reconst, 0, idim, jdim);
    
    printf("\nImage reconstruite (premiers pixels):\n");
    for (int i = 0; i < 5; i++) {
        printf("Ligne %d: ", i);
        for (int j = 0; j < jdim; j++) {
            printf("%3d ", reconst[i][j]);
        }
        printf("\n");
    }
    
    // Test avec offset
    printf("\n\nTest reconstruction_scan avec offset=10:\n");
    reconstruction_scan(vec_illu, i_scan, j_scan, lenscantotal, reconst, 10, idim, jdim);
    
    printf("\nImage reconstruite (premiers pixels):\n");
    for (int i = 0; i < 5; i++) {
        printf("Ligne %d: ", i);
        for (int j = 0; j < jdim; j++) {
            printf("%3d ", reconst[i][j]);
        }
        printf("\n");
    }
    
    // Libération
    free(i_scan);
    free(j_scan);
    free(vec_illu);
    free_2D_char(reconst);
    
    return 0;
}
