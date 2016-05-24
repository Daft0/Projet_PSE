#include "pse.h"

static int full_read(int canal, char *buffer, int taille) {
  int code, nblus = 0;
  while (nblus < taille) {
    code = read(canal, buffer+nblus, taille-nblus);
    if (code < 0) {
      return -1;
    }
    nblus += code;
  }
  return nblus;
}

static int full_write(int canal, char *buffer, int taille) {
  int code, nbecr = 0;
  while (nbecr < taille) {
    code = write(canal, buffer+nbecr, taille-nbecr);
    if (code < 0) {
      return -1;
    }
    nbecr += code;
  }
  return nbecr;
}

Message *newMessage (int type, int taille) {
  Message * temp;
  temp = (Message *) malloc ( sizeof ( Message ) );
  if (temp == NULL) return NULL;
  temp->contenu = malloc ( taille );
  if (temp->contenu == NULL) {
    free ( temp );
    return NULL;
  }
  temp->type   = type;
  temp->taille = taille;
  return temp;
}

Message *newTexteMessage (int type, char *texte) {
  Message * temp;
  int taille;
  taille = strlen(texte)+1;
  temp = newMessage(type, taille);
  if (temp == NULL) {
    return NULL;
  }
  strcpy(temp->contenu, texte);
  return temp;
}

Message * lireMessage(int canal) {
  char entete[HEADER_SIZE];
  Message *msg;
  int code, type, taille;

  /* lecture entete : elle donnera la taille du contenu */
  code = full_read(canal, entete, HEADER_SIZE);
  if (code < 0) {
    erreur_IO("lireMessage::read_content");
  }

  /* recuperation des valeurs de l'entete */
  sscanf(entete, "%d,%d", &type, &taille);

  /* nouveau message avec les valeurs de l'entete lue */
  msg = newMessage(type, taille);
  if (msg == NULL) return NULL;

  /* lecture du contenu */
  code = full_read(canal, msg->contenu, taille);
  if (code < 0) {
    erreur_IO("lireMessage::read_content");
  }
  
  /* le message est lu */
  return msg;
}

int ecrireMessage(int canal, Message *msg) {
  char entete[HEADER_SIZE];
  int code;

  /* creation de l'entete avec les champs du message */
  sprintf(entete, "%3.3d,%3.3d", msg->type, msg->taille);

  /* ecriture entete */
  code = full_write(canal, entete, HEADER_SIZE);
  if (code < 0) {
    erreur_IO("ecrireMessage::write_header");
    }
  
  /* ecriture contenu */
  code = full_write(canal, msg->contenu, msg->taille);
  if (code < 0) {
    erreur_IO("ecrireMessage::write_content");
  }
  
  return 0;
}

void displayMessage ( char *texte,  Message *msg ) {
  printf("%s. type: %d taille: %d contenu: \"%s\"\n",
	 texte, msg->type, msg->taille, msg->contenu);
}

void freeMessage ( Message **msg ) {
  if ((*msg)->contenu != NULL) free((*msg)->contenu);
  free ( *msg );
  *msg = NULL;
}

