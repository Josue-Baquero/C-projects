/*
 * Test de la fonction var_vec_bord
 * 
 * Compilation:
 *   gcc test_var_vec_bord.c var_vec_bord.c -o test_var_vec_bord -Wall -lm
 * 
 * Exécution:
 *   ./test_var_vec_bord
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef unsigned char uint8;

double var_vec_bord(uint8** in, int bord, int idim, int jdim);

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
    int idim = 6;
    int jdim = 6;
    
    // Allocation
    uint8** img = alloue_2D_char(idim, jdim);
    
    // Test 1: Image uniforme (variance = 0)
    printf("Test 1: Image uniforme (valeur 100)\n");
    for (int i = 0; i < idim; i++) {
        for (int j = 0; j < jdim; j++) {
            img[i][j] = 100;
        }
    }
    
    double var = var_vec_bord(img, 1, idim, jdim);
    printf("Variance (bord=1): %.2f (attendu: 0.00)\n\n", var);
    
    // Test 2: Image avec variation
    printf("Test 2: Image en damier\n");
    for (int i = 0; i < idim; i++) {
        for (int j = 0; j < jdim; j++) {
            img[i][j] = ((i + j) % 2) * 255;
            printf("%3d ", img[i][j]);
        }
        printf("\n");
    }
    
    var = var_vec_bord(img, 1, idim, jdim);
    printf("Variance (bord=1): %.2f\n\n", var);
    
    // Test 3: Image avec gradient
    printf("Test 3: Image avec gradient\n");
    for (int i = 0; i < idim; i++) {
        for (int j = 0; j < jdim; j++) {
            img[i][j] = (i * jdim + j) * 255 / (idim * jdim);
            printf("%3d ", img[i][j]);
        }
        printf("\n");
    }
    
    var = var_vec_bord(img, 0, idim, jdim);
    printf("Variance (bord=0): %.2f\n", var);
    var = var_vec_bord(img, 1, idim, jdim);
    printf("Variance (bord=1): %.2f\n", var);
    var = var_vec_bord(img, 2, idim, jdim);
    printf("Variance (bord=2): %.2f\n\n", var);
    
    // Libération
    free_2D_char(img);
    
    return 0;
}
