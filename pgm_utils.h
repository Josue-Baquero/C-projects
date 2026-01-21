#ifndef PGM_UTILS_H
#define PGM_UTILS_H

#include <stdint.h>

// Structure pour stocker une image
typedef struct {
    int idim;
    int jdim;
    uint8_t *data;
} Image;

// Structure pour stocker un vecteur
typedef struct {
    int length;
    double *data;
} Vector;

// Fonctions de lecture/écriture PGM
Vector loadpgm(const char *filename, int *idim, int *jdim);
void pgm8write(const char *filename, Image *img);

// Libération de mémoire
void free_vector(Vector *vec);
void free_image(Image *img);

#endif // PGM_UTILS_H
