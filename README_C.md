# Reconstruction de Scan d'Images - Version C

Conversion du code MATLAB vers C pour la détection et correction automatique de désalignement dans les images scannées.

## Fichiers du projet

- `pgm_utils.h/c` : Fonctions de lecture/écriture de fichiers PGM
- `scan_utils.h/c` : Fonctions de création de scan et reconstruction d'images
- `main.c` : Programme principal
- `Makefile` : Fichier de compilation

## Compilation

### Avec Make:
```bash
make
```

### Sans Make:
```bash
gcc -Wall -O2 -std=c99 main.c pgm_utils.c scan_utils.c -o scan_reconstruction.exe -lm
```

## Utilisation

```bash
./scan_reconstruction.exe
```

Le programme :
1. Charge l'image `cochon_scan_modif_512x512.pgm`
2. Reconstruit l'image brute → `output_original.pgm`
3. Recherche le décalage optimal (1-500)
4. Sauvegarde l'image corrigée → `output_reconstructed.pgm`
5. Exporte les critères de variance → `criteres.txt`

## Modification des paramètres

Éditez `main.c` pour changer :
- Le fichier d'entrée (`filename`)
- Les dimensions (`idim`, `jdim`)
- La plage de recherche (`modifmin`, `modifmax`)
