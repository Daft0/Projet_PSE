#ifndef ERREUR_INCLUDE_H
#define ERREUR_INCLUDE_H

#include <errno.h>

/*
  affichage du message d'erreur avec la fonction perror, 
  necessite donc que la variable errno soit positionnee.
  la fonction exit est ensuite appelee.
*/
void erreur_IO (const char *message) ;

/*
  affichage du message d'erreur avec la fonction perror, 
  necessite donc que la variable errno soit positionnee.
  la fonction pthread_exit est ensuite appelee.
*/
void erreur_pthread_IO ( const char *message );

/*
  cette fonction utilise le mecanisme de parametres en
  nombre variable. Le 'format' est une chaine de caracteres
  de type 'printf', avec utilisation de % pour indiquer le
  formatage a effectuer sur la valeurs transmises.
  Exemple:
     erreur ("usage: %s machine port\n", argv[0]);
  Le resultat du formatage est envoye sur stderr et
  la fonction exit est ensuite appelee.
*/
void erreur (const char *format, ...) ;

#endif
