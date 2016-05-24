#ifndef DATATHREAD_INCLUDE_H
#define DATATHREAD_INCLUDE_H

/* module datathread : gestion des identites et des donnees specifiques
   des threads crees dynamiquement.
*/

/* donnees specifiques */
#include "dataspec.h"

/* donnees dynamiques */
typedef struct DataThread_t {
  DataSpec spec;              /* donnees specifiques : voir dataspec.h */
  struct DataThread_t *next;  /* si thread suivant, alors son adresse, 
                                 sinon NULL */
} DataThread;

/* variable globale : tete de la liste */
extern DataThread *listeDataThread;

/*
  initialise la liste.
  si elle n'est pas vide, elle appelle libererDataThread
*/
void initDataThread(void);

/*
  ajoute un element DataThread alloué avec malloc() dans la liste et
  le retourne (ou NULL si erreur).
*/
DataThread *ajouterDataThread();

/*
  parcourt la liste pour savoir si des threads sont termines, en testant
  le champ termine de chaque element.
  pour chaque thread termine, la fonction appelle pthread_join() et
  met la valeur 0 dans l'identifiant, pour le marquer 'a supprimer'.
  TO DO: suppression des elements 'a supprimer'
*/
int joinDataThread(void);

/*
  libere la memoire allouee
*/
void libererDataThread(void);

#endif
