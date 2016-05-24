#ifndef MESSAGE_INCLUDE_H
#define MESSAGE_INCLUDE_H

/*
 * dans les messages, l'entete est composee des 2 champs entiers du
 * message : type et taille. Ils sont codes tous les 2 sur 3 caracteres
 * (valeurs entieres de 000 a 999), et sont separes par une virgule.
 * La taille de cet entete est donc 2*3+1+1.
 * exemple : "001,160"
 */
#define HEADER_SIZE  8

typedef struct message_t {
  int type;                     /* type (voir protocole.h) */
  int taille;			/* taille du message (contenu) */
  char * contenu;		/* le contenu du message (du texte) */
  struct message_t * next;	/* message suivant */
} Message;

/* cree un nouveau message (malloc) et retourne son adresse,
 * ou NULL en cas d'erreur. Le type est initialise avec la
 * valeur du parametre ; de plus, le champ contenu est alloue
 * (malloc) avec le parametre taille. */
Message * newMessage(int type,  int taille ) ;

/* cree un nouveau message a partir de la fonction precedente, avec
 * la taille fixee a la longueur de la chaine plus le '\0' terminal.
 * Elle retourne NULL en cas d'erreur */
Message * newTexteMessage(int type, char *texte ) ;

/* lit un message a partir du canal ouvert en lecture. Elle retourne le
 * message correctement valorise, sinon NULL en cas d'erreur. Ce message
 * est alloue (malloc). */
Message * lireMessage(int canal) ;

/* ecrit le message msg sur le canal ouvert en ecriture. Elle retourne
 * 0 en cas de succes, -1 sinon. Le message n'est pas libere (free). */
int       ecrireMessage(int canal, Message *msg);

/* affiche le message (debug) */
void      displayMessage ( char *texte, Message *msg ) ;

/* libere les ressources associées au message : le contenu ainsi que le
 * message lui-même */
void      freeMessage ( Message **msg ) ;

#endif
