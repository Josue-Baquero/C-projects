#include "pgm_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vector loadpgm(const char *filename, int *idim, int *jdim) {
    Vector vec = {0, NULL};
    FILE *fid = fopen(filename, "rb");
    
    if (!fid) {
        printf("ERR: impossible d'ouvrir le fichier %s\n", filename);
        return vec;
    }
    
    char ligne[256];
    
    // Lire l'entête P5
    if (!fgets(ligne, sizeof(ligne), fid)) {
        printf("ERR: fichier vide\n");
        fclose(fid);
        return vec;
    }
    
    if (strncmp(ligne, "P5", 2) != 0) {
        printf("ERR: fichier incompatible (pas P5)\n");
        fclose(fid);
        return vec;
    }
    
    // Lire dimensions (en sautant les commentaires)
    do {
        if (!fgets(ligne, sizeof(ligne), fid)) {
            printf("ERR: format invalide\n");
            fclose(fid);
            return vec;
        }
    } while (ligne[0] == '#');
    
    sscanf(ligne, "%d %d", idim, jdim);
    
    // Lire dynamique
    if (!fgets(ligne, sizeof(ligne), fid)) {
        printf("ERR: format invalide\n");
        fclose(fid);
        return vec;
    }
    
    int dyn;
    sscanf(ligne, "%d", &dyn);
    
    if (dyn > 255) {
        printf("ERR: ne charge que du uint8\n");
        fclose(fid);
        return vec;
    }
    
    // Lire les données
    int total = (*idim) * (*jdim);
    uint8_t *temp = (uint8_t*)malloc(total * sizeof(uint8_t));
    fread(temp, sizeof(uint8_t), total, fid);
    fclose(fid);
    
    // Convertir en vecteur double (transpose implicite)
    vec.length = total;
    vec.data = (double*)malloc(total * sizeof(double));
    
    for (int j = 0; j < *jdim; j++) {
        for (int i = 0; i < *idim; i++) {
            vec.data[j * (*idim) + i] = (double)temp[i * (*jdim) + j];
        }
    }
    
    free(temp);
    return vec;
}

void pgm8write(const char *filename, Image *img) {
    FILE *fid = fopen(filename, "wb");
    if (!fid) {
        printf("ERR: impossible d'écrire le fichier %s\n", filename);
        return;
    }
    
    // Entête
    fprintf(fid, "P5\n");
    fprintf(fid, "%d %d\n", img->jdim, img->idim);
    fprintf(fid, "255\n");
    
    // Données (transpose)
    uint8_t *temp = (uint8_t*)malloc(img->idim * img->jdim * sizeof(uint8_t));
    for (int i = 0; i < img->idim; i++) {
        for (int j = 0; j < img->jdim; j++) {
            temp[j * img->idim + i] = img->data[i * img->jdim + j];
        }
    }
    
    fwrite(temp, sizeof(uint8_t), img->idim * img->jdim, fid);
    free(temp);
    fclose(fid);
}

void free_vector(Vector *vec) {
    if (vec->data) {
        free(vec->data);
        vec->data = NULL;
        vec->length = 0;
    }
}

void free_image(Image *img) {
    if (img->data) {
        free(img->data);
        img->data = NULL;
    }
}
