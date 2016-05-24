#include "pse.h"

void initBal (Bal *bal) {
  bal->nombre = 0;
  bal->debut = NULL;
  bal->fin = NULL;
}

int ajoutMessageBal(Message *msg, Bal *bal) {
  if (bal == NULL || msg == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (bal->nombre == 0) {
    /* la bal est vide */
    bal->debut = bal->fin = msg;
    msg->next = NULL;
  }
  else {
    /* insertion a la fin */
    bal->fin->next = msg;
    bal->fin = msg;
    msg->next = NULL;
  }
  bal->nombre++;
  return 0;
}

Message * retirerMessageBal(Bal * bal) {
  Message *temp;
  if (bal == NULL || bal->nombre == 0) return NULL;
  /* on retire le premier de la file */
  temp = bal->debut;
  bal->debut = temp->next;
  bal->nombre--;
  /* peut etre raccourci en :
     if (bal->nombre <= 1) bal->fin = bal->debut;
     car si bal->nombre vaut 0, alors bal->debut vaut NULL */
  if (bal->nombre == 0) bal->fin = NULL;
  else if (bal->nombre == 1) bal->fin = bal->debut;
  return temp;
}

