#ifndef LIGNE_INCLUDE_H
#define LIGNE_INCLUDE_H

/* module ligne: lecture et ecriture de lignes de texte, definie
   comme une chaine de caracteres terminee par '\n' et '\0'.*/

/*
  Marqueur de fin de fichier
*/
#define LIGNE_EOF 0

/*
  Taille maximale d'une ligne de texte
*/
#define LIGNE_MAX 160

/*
  lit une ligne de texte depuis le fichier ouvert 'fd'.
  elle retourne le nombre de caracteres lus (<= LIGNE_MAX - 1),
  la valeur LIGNE_EOF en cas de fin de fichier,
  -1 en cas d'erreur (errno est positionne), ou
  la valeur LIGNE_MAX si la taille maximale a ete lue sans
  rencontrer '\n'.
*/
int lireLigne (int fd, char *buffer) ;

/*
  ecrit une ligne de texte dans le fichier ouvert 'fd'.
  s'il n'y a pas de '\n' dans le buffer, elle en ajoute un si la taille
  de buffer < LIGNE_MAX - 1, sinon une erreur est retournee et errno
  est positionne a EBADMSG.
  elle retourne le nombre de caracteres ecrits (le '\0' de fin de chaine
  n'est pas ecrit),
  -1 en cas d'erreur
*/
int ecrireLigne (int fd, char *buffer) ;

#endif

