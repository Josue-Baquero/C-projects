#include "scan_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

ScanPattern creer_scan_avec_dim(int idim, int jdim) {
    ScanPattern pattern;
    
    // Arrondir aux dimensions paires
    idim = (idim / 2) * 2;
    jdim = (jdim / 2) * 2;
    
    int scan_len = 2 * (jdim - 1);
    pattern.length = (idim / 2) * scan_len;
    pattern.i_scan = (double*)malloc(pattern.length * sizeof(double));
    pattern.j_scan = (double*)malloc(pattern.length * sizeof(double));
    
    // Créer le motif j_scan (rampe triangle)
    int idx = 0;
    for (int n = 0; n < idim / 2; n++) {
        // Montée: 1 à jdim
        for (int j = 1; j <= jdim; j++) {
            pattern.j_scan[idx++] = (double)j;
        }
        // Descente: jdim-1 à 2
        for (int j = jdim - 1; j >= 2; j--) {
            pattern.j_scan[idx++] = (double)j;
        }
    }
    
    // Créer i_scan (linéaire de 1 à idim)
    double pas = 1.0 / (idim * (jdim - 1));
    for (int i = 0; i < pattern.length; i++) {
        pattern.i_scan[i] = i * pas * idim + 1.0;
    }
    
    return pattern;
}

Image reconstruction_scan(Vector vec_illu, int idim, int jdim) {
    Image img;
    img.idim = idim;
    img.jdim = jdim;
    img.data = (uint8_t*)calloc(idim * jdim, sizeof(uint8_t));
    
    ScanPattern pattern = creer_scan_avec_dim(idim, jdim);
    
    if (pattern.length != vec_illu.length) {
        printf("ERR: erreur de dimension (pattern=%d, vec=%d)\n", pattern.length, vec_illu.length);
        free_scan_pattern(&pattern);
        return img;
    }
    
    for (int n = 0; n < pattern.length; n++) {
        int i = (int)floor(pattern.i_scan[n]) - 1;  // -1 car indices C commencent à 0
        int j = (int)floor(pattern.j_scan[n]) - 1;
        
        if (i >= 0 && i < idim && j >= 0 && j < jdim) {
            double val = vec_illu.data[n];
            img.data[i * jdim + j] = (uint8_t)(val > 255 ? 255 : (val < 0 ? 0 : val));
        }
    }
    
    free_scan_pattern(&pattern);
    return img;
}

// Fonction auxiliaire pour appliquer un décalage circulaire
Vector circshift(Vector vec, int shift) {
    Vector result;
    result.length = vec.length;
    result.data = (double*)malloc(vec.length * sizeof(double));
    
    for (int i = 0; i < vec.length; i++) {
        int new_idx = (i - shift + vec.length) % vec.length;
        if (new_idx < 0) new_idx += vec.length;
        result.data[i] = vec.data[new_idx];
    }
    
    return result;
}

// Fonction auxiliaire pour appliquer un filtre moyenneur 3x3
void apply_filter(Image *img) {
    uint8_t *temp = (uint8_t*)malloc(img->idim * img->jdim * sizeof(uint8_t));
    memcpy(temp, img->data, img->idim * img->jdim);
    
    for (int i = 1; i < img->idim - 1; i++) {
        for (int j = 1; j < img->jdim - 1; j++) {
            int sum = 0;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    sum += temp[(i + di) * img->jdim + (j + dj)];
                }
            }
            img->data[i * img->jdim + j] = (uint8_t)(sum / 9);
        }
    }
    
    free(temp);
}

// Fonction auxiliaire pour calculer la variance
double calculate_variance(Image *img) {
    double mean = 0.0;
    int total = img->idim * img->jdim;
    
    for (int i = 0; i < total; i++) {
        mean += img->data[i];
    }
    mean /= total;
    
    double variance = 0.0;
    for (int i = 0; i < total; i++) {
        double diff = img->data[i] - mean;
        variance += diff * diff;
    }
    variance /= total;
    
    return variance;
}

SearchResult cherche_modif(const char *filename, int idim, int jdim, int modifmin, int modifmax) {
    SearchResult result;
    result.tabcrit = NULL;
    result.recons.data = NULL;
    
    int idim_read, jdim_read;
    Vector vec_illu = loadpgm(filename, &idim_read, &jdim_read);
    
    if (vec_illu.data == NULL) {
        printf("ERR: échec du chargement de %s\n", filename);
        return result;
    }
    
    ScanPattern pattern = creer_scan_avec_dim(idim, jdim);
    
    if (pattern.length != vec_illu.length) {
        printf("ERR: erreur de dimension\n");
        free_vector(&vec_illu);
        free_scan_pattern(&pattern);
        return result;
    }
    
    // Boucle de recherche
    int num_tests = modifmax - modifmin + 1;
    result.tabcrit_len = num_tests;
    result.tabcrit = (double*)malloc(num_tests * sizeof(double));
    
    printf("Recherche du décalage optimal...\n");
    
    for (int n = 0; n < num_tests; n++) {
        int shift = modifmin + n;
        
        Vector vec_mod = circshift(vec_illu, -shift);
        Image out = reconstruction_scan(vec_mod, idim, jdim);
        
        apply_filter(&out);
        result.tabcrit[n] = calculate_variance(&out);
        
        free_image(&out);
        free_vector(&vec_mod);
        
        if (n % 100 == 0) {
            printf("  Progression: %d/%d\n", n, num_tests);
        }
    }
    
    // Trouver le maximum
    double max_val = result.tabcrit[0];
    int max_idx = 0;
    for (int i = 1; i < num_tests; i++) {
        if (result.tabcrit[i] > max_val) {
            max_val = result.tabcrit[i];
            max_idx = i;
        }
    }
    
    result.modif = -(modifmin + max_idx);
    
    // Reconstruire avec le meilleur décalage
    Vector vec_final = circshift(vec_illu, result.modif);
    result.recons = reconstruction_scan(vec_final, idim, jdim);
    
    free_vector(&vec_final);
    free_vector(&vec_illu);
    free_scan_pattern(&pattern);
    
    return result;
}

void free_scan_pattern(ScanPattern *pattern) {
    if (pattern->i_scan) {
        free(pattern->i_scan);
        pattern->i_scan = NULL;
    }
    if (pattern->j_scan) {
        free(pattern->j_scan);
        pattern->j_scan = NULL;
    }
    pattern->length = 0;
}

void free_search_result(SearchResult *result) {
    if (result->tabcrit) {
        free(result->tabcrit);
        result->tabcrit = NULL;
    }
    free_image(&result->recons);
}
