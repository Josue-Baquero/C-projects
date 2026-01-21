#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pgm_utils.h"
#include "scan_utils.h"

int main() {
    // Configuration (équivalent à test.m)
    const char *filename = "cochon_scan_modif_512x512.pgm";
    int idim = 512;
    int jdim = 512;
    int modifmin = 1;
    int modifmax = 500;
    
    printf("=== Test de reconstruction d'image ===\n");
    printf("Fichier: %s\n", filename);
    printf("Dimensions: %dx%d\n", idim, jdim);
    printf("Recherche de décalage entre %d et %d\n\n", modifmin, modifmax);
    
    // Charger et reconstruire l'image originale (sans correction)
    printf("Chargement de l'image...\n");
    int idim_read, jdim_read;
    Vector vec_illu = loadpgm(filename, &idim_read, &jdim_read);
    
    if (vec_illu.data == NULL) {
        printf("Erreur lors du chargement\n");
        return 1;
    }
    
    Image img_original = reconstruction_scan(vec_illu, idim, jdim);
    pgm8write("output_original.pgm", &img_original);
    printf("Image originale sauvegardée: output_original.pgm\n\n");
    
    free_vector(&vec_illu);
    free_image(&img_original);
    
    // Recherche du décalage optimal
    clock_t start = clock();
    
    SearchResult result = cherche_modif(filename, idim, jdim, modifmin, modifmax);
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    if (result.recons.data == NULL) {
        printf("Erreur lors de la recherche\n");
        return 1;
    }
    
    printf("\n=== Résultats ===\n");
    printf("Temps écoulé: %.3f secondes\n", elapsed);
    printf("Décalage détecté: %d\n", result.modif);
    
    // Sauvegarder l'image reconstruite
    pgm8write("output_reconstructed.pgm", &result.recons);
    printf("Image reconstruite sauvegardée: output_reconstructed.pgm\n");
    
    // Sauvegarder le graphe des critères
    FILE *f = fopen("criteres.txt", "w");
    if (f) {
        for (int i = 0; i < result.tabcrit_len; i++) {
            fprintf(f, "%d %f\n", modifmin + i, result.tabcrit[i]);
        }
        fclose(f);
        printf("Critères de variance sauvegardés: criteres.txt\n");
    }
    
    // Libérer la mémoire
    free_search_result(&result);
    
    printf("\nTerminé avec succès!\n");
    return 0;
}
