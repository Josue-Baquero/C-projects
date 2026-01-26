#include <stdio.h>
#include <sys/time.h>


/* tic */
void tic(struct timeval* temps, char* texte)
{
  gettimeofday(temps, NULL);

  if (texte != NULL)
    printf("%s\n", texte) ;
}

/* toc */
double toc(struct timeval start, char* texte)
{
  struct timeval end ;
  double elapsed ;

  gettimeofday(&end, NULL);

  elapsed = (double)(end.tv_sec-start.tv_sec) 
    + 0.000001*(double)(end.tv_usec-start.tv_usec);
  if (texte != NULL)
    printf("%s : %f\n", texte, elapsed) ;

  return elapsed ;
}
