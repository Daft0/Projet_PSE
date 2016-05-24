/* Module de gestion des donnees statiques des threads
   Voir documentation dans "datathread.h"
   (c) P Lalevée, 2012 */


#include "pse.h"

DataThread *listeDataThread;

static DataThread *lastDataThread;

void initDataThread(void) {
  if (listeDataThread != NULL) libererDataThread();
  listeDataThread = lastDataThread = NULL;
}

DataThread *ajouterDataThread(void) {
  DataThread *tmp;
  tmp = (DataThread *) malloc(sizeof(DataThread));
  if (tmp == NULL) return NULL;
  tmp->next = NULL;
  tmp->spec.libre = FAUX;
  if (listeDataThread == NULL) {
    listeDataThread = tmp;
  }
  else {
    lastDataThread->next = tmp;
  }
  lastDataThread = tmp;
  return tmp;
}

int joinDataThread(void) {
  DataThread *cur = listeDataThread;
  int nThread = 0;
  while (cur != NULL) {
    if (cur->spec.id != 0 && cur->spec.libre == VRAI) {
      pthread_join(cur->spec.id, NULL);
      cur->spec.id = 0;
      nThread++;
    }
    cur = cur->next;
  }
  return nThread;
}

void libererDataThread(void) {
  DataThread *cur, *tmp;
  cur = listeDataThread;
  while (cur != NULL) {
    tmp = cur;
    cur = cur->next;
    free(tmp);
  }
  listeDataThread = lastDataThread = NULL;
}
