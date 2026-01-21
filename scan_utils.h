#ifndef SCAN_UTILS_H
#define SCAN_UTILS_H

#include "pgm_utils.h"

// Structure pour le motif de scan
typedef struct {
    int length;
    double *i_scan;
    double *j_scan;
} ScanPattern;

// Créer un motif de scan
ScanPattern creer_scan_avec_dim(int idim, int jdim);

// Reconstruction d'image depuis un vecteur
Image reconstruction_scan(Vector vec_illu, int idim, int jdim);

// Recherche de modification
typedef struct {
    Image recons;
    int modif;
    double *tabcrit;
    int tabcrit_len;
} SearchResult;

SearchResult cherche_modif(const char *filename, int idim, int jdim, int modifmin, int modifmax);

// Libération de mémoire
void free_scan_pattern(ScanPattern *pattern);
void free_search_result(SearchResult *result);

#endif // SCAN_UTILS_H
