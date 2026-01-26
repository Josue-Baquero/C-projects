/*
 * Test de la fonction filter2D_local
 * 
 * Compilation:
 *   gcc test_filter2D_local.c filter2D_local.c -o test_filter2D_local -Wall
 * 
 * Exécution:
 *   ./test_filter2D_local
 */

#include <stdlib.h>
#include <stdio.h>

typedef unsigned char uint8;

void filter2D_local(uint8** in, uint8** out, int idim, int jdim);

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
    int idim = 5;
    int jdim = 5;
    
    // Allocation
    uint8** in = alloue_2D_char(idim, jdim);
    uint8** out = alloue_2D_char(idim, jdim);
    
    // Créer une image de test avec des valeurs
    printf("Image d'entrée:\n");
    for (int i = 0; i < idim; i++) {
        for (int j = 0; j < jdim; j++) {
            in[i][j] = (i * jdim + j) * 10;
            printf("%3d ", in[i][j]);
        }
        printf("\n");
    }
    
    // Appliquer le filtre
    filter2D_local(in, out, idim, jdim);
    
    // Afficher le résultat
    printf("\nImage filtrée (moyenneur 3x3):\n");
    for (int i = 0; i < idim; i++) {
        for (int j = 0; j < jdim; j++) {
            printf("%3d ", out[i][j]);
        }
        printf("\n");
    }
    
    // Test avec une image uniforme
    printf("\n\nTest avec image uniforme (valeur 100):\n");
    for (int i = 0; i < idim; i++) {
        for (int j = 0; j < jdim; j++) {
            in[i][j] = 100;
        }
    }
    
    filter2D_local(in, out, idim, jdim);
    
    printf("Résultat (devrait être partout 100):\n");
    for (int i = 0; i < idim; i++) {
        for (int j = 0; j < jdim; j++) {
            printf("%3d ", out[i][j]);
        }
        printf("\n");
    }
    
    // Libération
    free_2D_char(in);
    free_2D_char(out);
    
    return 0;
}
