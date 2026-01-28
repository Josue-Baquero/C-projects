/*

 ligne de compilation
 gcc cherche_modif.c -pthread -Wall -O2 -o cherche_modif

 Performances avec optimisations (var_vec_bord en 1 passe):
 - Speedup avec 4 threads : x4.0
 - Barbara 512x512, modmax=600 : 0.16s (4 threads) vs 0.65s (séquentiel)
 - Babar 516x420, modmax=600 : 0.14s (4 threads) vs 0.54s (séquentiel)
 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>    /* clock */
#include <unistd.h>  /* usleep */
#include <pthread.h>


/* copier coller des fonctions tic et toc */
#include "tictoc.c"

#define SIZE_LINE_TEXT 1024

typedef unsigned char uint8 ;


#define HELP \
"Programme pour determiner la modif (shift) de la donne scan\n" \
"pour que l'image reconstruire soit correcte : \n" \
"avec le critere: reconst -> filter2_local (3x3) -> calcul variance\n" \
"\n" \
"Le programme execute TOUJOURS les 2 versions pour comparaison:\n" \
"  1) Version sequentielle (cherche_modif)\n" \
"  2) Version multithread (cherche_modif_th avec nbth threads)\n" \
"\n" \
"usage : cherche_modif <filename_scan> <idim> <jdim> <modifMax> <nbth>\n\n" \
"filename_scan : fichier pgm en ligne : (idimxjdim)x1 \n" \
"idim          : dimension de l'image\n" \
"jdim          : dimension de l'image\n" \
"modifMax      : recherche entre [0, modifMax]\n" \
"nbth          : nombre de threads pour la version parallele\n"



int read_param(int argc, char** argv, char **filename, int* idim, int* jdim, int* modifmax, int* nbth) ;
uint8 ** alloue_2D_char(int ligne, int colonne) ;
void free_2D_char(uint8** tab) ;
int loadPgmEntete(char *filename, int *idim, int* jdim, int* dyn) ;
int loadpgm(char *filename, uint8* data, int idim, int jdim) ;
int uint82pgm(char *namefile, uint8* data, int idim, int jdim) ;


int creer_scan_avec_dim(int idim, int jdim, int *i_scan, int *j_scan) ;
int reconstruction_scan(uint8* vec_illu, int* i_scan, int* j_scan, int len, uint8** reconst, int offset) ;
void filter2D_local(uint8** in, uint8** out, int idim, int jdim) ;
double var_vec_bord(uint8** in, int bord, int idim, int jdim) ;

int cherche_modif(uint8* vec_illu, int* i_scan, int* j_scan, int lenscan, 
		  double* vec_crit, int modmax,
		  uint8** reconst, uint8** reconst_pb, int idim, int jdim) ;

int cherche_modif_th(uint8* vec_illu, int* i_scan, int* j_scan, int lenscan, 
		     double* vec_crit, int modmax,
		     uint8** reconst, uint8** reconst_pb, int idim, int jdim, int nbth) ;



int main(int argc, char **argv)
{
  /* variables pour tic toc */
  struct timeval chrono ;

  int idim, jdim ; // prevue pour des dimensions paires
  int modifmax ;
  char *filename ;
  int idimillu ;
  int jdimillu ;
  int dyn ;
  int nbth ;

  read_param(argc, argv, &filename, &idim, &jdim, &modifmax, &nbth) ;
 
  printf("fichier: %s\n", filename) ;
  printf("idim x jdim : %d x %d\n", idim, jdim) ;
  printf("critere entre [0, %d] sur %d thread\n" , modifmax, nbth) ;

  /* lecture de la data (pertube) */
  loadPgmEntete(filename, &idimillu , &jdimillu , &dyn) ;
  uint8* vec_illu = (uint8*)malloc(idimillu*jdimillu*sizeof(uint8)) ;
  loadpgm(filename, vec_illu, idimillu, jdimillu) ;
 
  const int lenscantotal = (jdim-1)*idim  ;

  if (jdimillu != lenscantotal)
    {
      fprintf(stderr, "ERR: erreur de dimension\n") ;
      exit(0) ;
    }

  const int lencritere = modifmax + 1 ;
  double* vec_crit = (double*)malloc(lencritere*sizeof(double)) ;

  int* i_scan = (int*)malloc(lenscantotal*sizeof(int)) ;
  int* j_scan = (int*)malloc(lenscantotal*sizeof(int)) ;

  uint8** reconst = alloue_2D_char(idim, jdim) ;
  uint8** reconst_pb = alloue_2D_char(idim, jdim) ; // filter2

  creer_scan_avec_dim(idim, jdim, i_scan, j_scan) ;
  
  // Test SANS threads
  tic(&chrono, "\ncherche_modif SANS threads ...") ;
  int modif_seq = cherche_modif(vec_illu, i_scan, j_scan, lenscantotal, 
				vec_crit, modifmax,
				reconst, reconst_pb, idim, jdim) ;
  toc(chrono, "temps ecoule SANS threads") ;
  
  // Test AVEC threads
  tic(&chrono, "\ncherche_modif AVEC threads ...") ;
  int modif = cherche_modif_th(vec_illu, i_scan, j_scan, lenscantotal, 
			       vec_crit, modifmax,
			       reconst, reconst_pb, idim, jdim, nbth) ;
  toc(chrono, "temps ecoule AVEC threads") ;
  
  printf("\nComparaison: modif_seq=%d, modif_th=%d\n", modif_seq, modif) ;

  /* affichage: image pgm */
  reconstruction_scan(vec_illu, i_scan, j_scan, lenscantotal, reconst, 0) ;
  uint82pgm("reconst_init.pgm", reconst[0], idim, jdim) ;
  reconstruction_scan(vec_illu, i_scan, j_scan, lenscantotal, reconst, modif) ;
  uint82pgm("reconst_find.pgm", reconst[0], idim, jdim) ;

  
  /* aff critere par image uint8: recast en uint8, necessite au nouveau malloc */
  /*
  double critmax = 0 ;
  for (int m=0; m<=modifmax; m++)
    if (critmax <  vec_crit[m]) critmax = vec_crit[m] ;
  
  uint8* crit = (uint8*)malloc(lencritere*sizeof(uint8)) ;

  for (int m=0; m<=modifmax; m++)
    crit[m] = 255*vec_crit[m]/critmax ;

  uint82pgm("critere_find.pgm", crit, 1, lencritere) ;
  free(crit) ;
  */
  /* fin aff critere */
  

  /* dealloc */
  free_2D_char(reconst) ;
  free_2D_char(reconst_pb) ;
  free(vec_illu) ;
  free(i_scan) ;
  free(j_scan) ;
  free(vec_crit) ;
 
  return 0 ;
}



// sous fonction pour scan_modif
// sous fonction pour scan_modif
// sous fonction pour scan_modif


/** ij_scan allouer pour  lenscantotal=(jdim-1)*idim   valeurs
   idim et jdim puissance de 2
*/
int creer_scan_avec_dim(int idim, int jdim, int *i_scan, int *j_scan)
{
  // scan = [ (1:1:jdim) ,  ((jdim-1) : -1 : 2)]  ;
  int lenscan = 2*jdim - 2 ;
  int *scan = (int*)malloc(lenscan*sizeof(int)) ;
  
  // Première partie : 1 à jdim
  for (int i = 0; i < jdim; i++) {
    scan[i] = i + 1 ;
  }
  
  // Deuxième partie : (jdim-1) à 2 (décroissant)
  int idx = jdim ;
  for (int i = jdim-1; i >= 2; i--) {
    scan[idx] = i ;
    idx++ ;
  }

  // for n = 1 : 1 : floor(idim/2)
  //   j_scan = [ j_scan , scan] ;
  // end
  int nb_repeat = idim/2 ;
  int pos = 0 ; // position courante dans j_scan
  for (int n = 0; n < nb_repeat; n++) { //pour chaque répétition on copie le scan dans j_scan
    for (int k = 0; k < lenscan; k++) { 
      j_scan[pos] = scan[k] ;
      pos++ ;
    }
  }
  
  // pas = 1/(idim*(jdim-1)) ;
  // i_scan = 0 : pas : 1-pas ;
  // i_scan = i_scan*idim + 1 ;
  int lenscantotal = (jdim-1)*idim ; // lenscantotal = lenscan × nb_repeat
  double pas = 1.0 / (idim*(jdim-1)) ;
  for (int k = 0; k < lenscantotal; k++) {
    i_scan[k] = (int)(k * pas * idim + 1) ;
  }

  free(scan) ;
  return 0 ;
}

/**
   reconstruction avec un parcours scan (triangulaire avec creer_scan_avec_dim())
   
   vec_illu [in]
   i_scan [in]
   j_scan [in]
   len [in]
   reconst [out]
   offset [in] : décalage appliqué au vecteur vec_illu
   
 */
int reconstruction_scan(uint8* vec_illu, int* i_scan, int* j_scan, int len, 
			uint8** reconst, int offset)
{
  // out = zeros(idim, jdim) ; déjà alloué par le main avec alloue_2D_char

  // if (nargin < 4)
  // [i_scan, j_scan] = creer_scan_avec_dim(idim, jdim) ;
  // end ; paramètres obligatoires dans la signature de la fonction

  //len = length(i_scan) ; déjà dans les paramètres de la fonction

  //  len = length(i_scan) ;
  // if (len ~= length(vec_illu))
  //   disp('ERR: erreur de dimension') ;
  //   return
  // end ; vérification des dimensions déjà faite dans le main

  // for n = 1:len
  //   out(floor(i_scan(n)) , floor(j_scan(n))) = vec_illu(n) ;
  // end
  
  for (int n = 0; n < len; n++) {
    int idx = (n + offset) % len;  // Position dans vec_illu avec décalage
    int i = i_scan[n] - 1;  // -1 car MATLAB indexe à partir de 1
    int j = j_scan[n] - 1;  // -1 car MATLAB indexe à partir de 1
    
    reconst[i][j] = vec_illu[idx];
  }
 
  return 0 ;
}



/**
   filtrer localement par au kernel 3x3 : ones(3,3)/9
   filtre pas les bords
 */
void filter2D_local(uint8** in, uint8** out, int idim, int jdim)
{
  // Filtre moyenneur 3x3
  // Ne traite pas les bords (i=0, i=idim-1, j=0, j=jdim-1)
  
  for (int i = 1; i < idim-1; i++) {
    for (int j = 1; j < jdim-1; j++) {
      // Somme des 9 pixels voisins
      int sum = 0;
      for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
          sum += in[i+di][j+dj];
        }
      }
      // Moyenne : division par 9
      out[i][j] = sum / 9;
    }
  }
  
  // Copier les bords sans modification
  for (int j = 0; j < jdim; j++) {
    out[0][j] = in[0][j];           // première ligne
    out[idim-1][j] = in[idim-1][j]; // dernière ligne
  }
  for (int i = 0; i < idim; i++) {
    out[i][0] = in[i][0];           // première colonne
    out[i][jdim-1] = in[i][jdim-1]; // dernière colonne
  }
}



/**
   calcul la variance en enlevant les bords
  */
double var_vec_bord(uint8** in, int bord, int idim, int jdim)
{
  double sum = 0;      // Somme des valeurs
  double sum_sq = 0;   // Somme des carrés
  int count = 0;
  
  // Calcul de somme et somme des carrés
  for (int i = bord; i < idim - bord; i++) {
    for (int j = bord; j < jdim - bord; j++) {
      double val = in[i][j];
      sum += val;
      sum_sq += val * val;
      count++;
    }
  }
  
  if (count == 0) return 0;
  
  // Variance = E[X²] - E[X]²
  double mean = sum / count;
  double var = sum_sq / count - mean * mean;
  
  return var;
} 




/**
   calcul le critere pour [modmin, modmax]
 */
void calcul_crit(uint8* vec_illu, int* i_scan, int* j_scan, int lenscan, 
		 double* vec_crit, int modmin, int modmax,
		 uint8** reconst, uint8** reconst_pb, int idim, int jdim)
{
  for (int m=modmin; m<=modmax; m++)
    {
      reconstruction_scan(vec_illu, i_scan, j_scan, lenscan, reconst, m) ;
      filter2D_local(reconst, reconst_pb, idim, jdim) ;
      vec_crit[m] = var_vec_bord(reconst_pb, 1, idim, jdim) ;
    }
}



int cherche_modif(uint8* vec_illu, int* i_scan, int* j_scan, int lenscan, 
		  double* vec_crit, int modmax,
		  uint8** reconst, uint8** reconst_pb, int idim, int jdim)
{
  int modif=0 ;

  calcul_crit(vec_illu,  i_scan, j_scan, lenscan, 
	      vec_crit, 0, modmax,
	      reconst, reconst_pb, idim, jdim) ;

  // recherche d'un max du critere
  double critmax = 0 ;
  for (int m=0; m<=modmax; m++)
    if (critmax <  vec_crit[m])
      {
	critmax = vec_crit[m] ;
	modif = m ;
      }
  
  return modif ;
}



/**
   lancement recherche_modif en thread
 */

struct arg_fnc_calcul_crit
{
  uint8* vec_illu;
  int* i_scan;
  int* j_scan;
  int lenscan;
  double* vec_crit;
  int modmin;
  int modmax;
  uint8** reconst;
  uint8** reconst_pb;
  int idim;
  int jdim;
} ;


void* calcul_crit_th(void* argin)
{
  struct arg_fnc_calcul_crit* arg = (struct arg_fnc_calcul_crit*)argin ;

  calcul_crit(arg->vec_illu, arg->i_scan, arg->j_scan, arg->lenscan,
              arg->vec_crit, arg->modmin, arg->modmax,
              arg->reconst, arg->reconst_pb, arg->idim, arg->jdim);
 
  pthread_exit(0) ;
}


int cherche_modif_th(uint8* vec_illu, int* i_scan, int* j_scan, int lenscan, 
		     double* vec_crit, int modmax,
		     uint8** reconst, uint8** reconst_pb, int idim, int jdim, const int nbth)
{
  int modif=0 ;

  pthread_t tab_thread[nbth] ;
  struct arg_fnc_calcul_crit arg[nbth] ;

  
  for (int i=1; i<nbth; i++)
  {
    // conf arg
    arg[i].vec_illu = vec_illu;
    arg[i].i_scan = i_scan;
    arg[i].j_scan = j_scan;
    arg[i].lenscan = lenscan;
    arg[i].vec_crit = vec_crit;
    arg[i].modmin = i * (modmax / nbth);
    arg[i].modmax = (i + 1) * (modmax / nbth) - 1;
    arg[i].idim = idim;
    arg[i].jdim = jdim;
    
    // Allouer des buffers séparés pour chaque thread
    arg[i].reconst = alloue_2D_char(idim, jdim);
    arg[i].reconst_pb = alloue_2D_char(idim, jdim);
    
    if (i == nbth-1) arg[i].modmax = modmax ;
  
    // on lance le thread
    pthread_create(&tab_thread[i], NULL, calcul_crit_th, &arg[i]);
  }
  
  calcul_crit(vec_illu,  i_scan, j_scan, lenscan, 
	      vec_crit, 0, modmax/nbth -1,
	      reconst, reconst_pb, idim, jdim) ;
  
  
  for (int i=1; i<nbth; i++)
  {
    // Attend la fin des threads
    pthread_join(tab_thread[i], NULL);
    
    // Libérer les buffers alloués pour ce thread
    free_2D_char(arg[i].reconst);
    free_2D_char(arg[i].reconst_pb);
  }
  
  
  // recherche d'un max du critere
  double critmax = 0 ;
  for (int m=0; m<=modmax; m++)
    if (critmax <  vec_crit[m])
      {
	critmax = vec_crit[m] ;
	modif = m ;
      }
  
  return modif ;
}





// sous fonctions pour que le programme fonctionne
// sous fonctions pour que le programme fonctionne
// sous fonctions pour que le programme fonctionne

/**
   \brief lecture des parametres d'entree 
   dans un ordre precis : <filename> <idim> <jdim> 
 */
int read_param(int argc, char** argv,
	       char **filename, int* idim, int* jdim, int* modifmax, int* nbth)
{
  if (argc != 6)
    {
      printf(HELP) ;
      exit(1) ;
    }
  
  *filename = argv[1] ;
  *idim     = atoi(argv[2]) ;
  *jdim     = atoi(argv[3]) ;
  *modifmax   = atoi(argv[4]) ;
  *nbth     = atoi(argv[5]) ;
 
  return 0 ;
}



/**
   \brief alloue en memoire (tas) un tableau (2D) de char en ligne
 */
uint8 ** alloue_2D_char(int ligne, int colonne)
{
  int i ;
  uint8 **tmp = malloc(ligne*sizeof(char*)) ;
  uint8 *data = calloc(ligne*colonne, sizeof(char)) ;
  for (i=0; i<ligne; i++)
    tmp[i] = &(data[i*colonne]) ;

  return tmp ;
}


/**
   \brief desalloue en memoire (tas) un tableau (2D) de char
 */
void free_2D_char(uint8** tab)
{
  free(tab[0]) ;
  free(tab) ;
}



/** actuellement ouvre que le format P5 8 bits (uint8)
 */
int loadPgmEntete(char *namefile, int *idim, int* jdim, int* dyn)
{
  char buffer[SIZE_LINE_TEXT] ;
  FILE *file = fopen(namefile, "rb") ;
  char* ret ; /* pour supprimer le warning du compilation */
  
  // lecture de la premiere ligne
  ret = fgets(buffer, SIZE_LINE_TEXT, file);
 
  /* pgm */
  if (!((buffer[0] == 'P') && (buffer[1] == '5')))
    return -1 ;
  
  /* saut des lignes de commentaires */
  ret = fgets(buffer, SIZE_LINE_TEXT, file);
  while ((buffer[0] == '#')|(buffer[0] == '\n')) 
    ret = fgets(buffer, SIZE_LINE_TEXT, file);
  
  /* lecture des dimensions de l'image */
  sscanf(buffer, "%d %d", jdim, idim) ;
  
  ret = fgets(buffer, SIZE_LINE_TEXT, file);
  sscanf(buffer, "%d", dyn) ;

  /* pour supprimer le warning du compilation */
  if (ret) ;
  fclose(file);
  
  return 0 ;
}


/** actuellement ouvre que le format P5 8 bits (uint8)
 */
int loadpgm(char *namefile, uint8* data, int idim, int jdim)
{
  char buffer[SIZE_LINE_TEXT] ;
  FILE *file = fopen(namefile, "rb") ;
  char* ret ; /* pour supprimer le warning du compilation */
  size_t retfread ;

  /* lecture de la premiere ligne */
  ret = fgets(buffer, SIZE_LINE_TEXT, file);

  /* saut des lignes de commentaires et dimension */
  ret = fgets(buffer, SIZE_LINE_TEXT, file);
  while ((buffer[0] == '#')|(buffer[0] == '\n')) 
    ret = fgets(buffer, SIZE_LINE_TEXT, file);

  /* saut de la dynamique */
  ret = fgets(buffer, SIZE_LINE_TEXT, file);

  /* lecture de la data */
  retfread = fread(data, sizeof(uint8), idim*jdim, file) ;
 
  /* pour supprimer le warning du compilation */
  if (ret) ;
  fclose(file);
 
  return retfread ; /* le nombre octect lue */
}



/** ecrit une image en uint8 au format pgm P5 */
int uint82pgm(char *namefile, uint8* data, int idim, int jdim)
{
  FILE *file = fopen(namefile, "wb") ;
 
  fprintf(file, "P5\n") ;
  fprintf(file, "# Cours C/GPU Systemes Embarques\n") ;
  fprintf(file, "# ECM - Marseille - France\n") ;
  // taille
  fprintf(file, "%d %d\n", jdim, idim) ;
  // dynamique
  fprintf(file, "%d\n" , 255 );

  fwrite(data, sizeof(uint8), idim*jdim, file);
  
  fclose(file);
 
  return 0 ;
}



