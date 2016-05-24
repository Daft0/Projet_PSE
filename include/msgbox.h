#ifndef BAL_INCLUDE_H
#define BAL_INCLUDE_H

#include "msg.h"

typedef struct bal_t {
  int nombre;		/* nombre de messages dans la boite */
  Message * debut;	/* le premier message */
  Message * fin; 	/* le dernier message */
} Bal;


void initBal (Bal *bal) ;
int ajoutMessageBal(Message *msg, Bal *bal) ;
Message * retirerMessageBal(Bal *bal) ;
#endif
