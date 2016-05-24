/* Module de gestion de lignes de texte
   Voir documentation dans "ligne.h"
   (c) P Lalevée, 2012 */

#include "pse.h"

int lireLigne(int fd, char *buffer) {
  ssize_t nbLus;
  int i;
  char car;

  if (buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  i = 0;
  while (i < LIGNE_MAX - 1) {
    nbLus = read(fd, &car, 1);   
    if (nbLus == -1) {
      return -1;
    }      
    else if (nbLus == 0) {
      buffer[i] = '\0';
      return LIGNE_EOF;
    }
    else if (car == '\n') {
      buffer[i] = '\0';
      i++;
      return i;
    }
    buffer[i] = car;
    i++;      
  }
  
  buffer[i] = '\0';
  return LIGNE_MAX;
}

int ecrireLigne(int fd, char *buffer) {
  char *position;
  int taille, ecr, nbecr;

  position = strchr(buffer, '\n');
  if (position == NULL) {
    taille = strlen(buffer);
    if (taille >= LIGNE_MAX - 1) {
      errno = EINVAL;
      return -1;
    }
    buffer[taille++] = '\n';
    buffer[taille] = '\0';
  }
  else {
    taille = position - buffer + 1;
  }

  nbecr = taille;

  while (taille > 0) {
    ecr = write(fd, buffer, taille);
    if (ecr == -1) {
      return -1;
    }
    taille -= ecr;
  }
  return nbecr;
}
