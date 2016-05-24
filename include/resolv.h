#ifndef RESOLV_INCLUDE_H
#define RESOLV_INCLUDE_H

/* module resolv: resolution DNS */

/*
  resolv retourne l'adresse IP correspondant au nom de domaine 'host'
  ou a l'adresse specifie au format "aaa.bbb.ccc.ddd", et le numero
  de port associe au 'service' ou au numero "999".
  en cas d'erreur, elle affiche le message d'erreur sur stderr, et
  retourne NULL
*/
struct sockaddr_in *resolv (const char *host, const char *service) ;

/* stringIP convertit en chaine de caracteres, avec le format
   aaa.bbb.ccc.ddd, l'adresse IP dont la valeur entiere au format
   Host Order de 'entierIP'. La chaine de caractere est statique.
   elle retourne NULL en cas d'erreur.
*/
char *stringIP(unsigned int entierIP);

/*
  liberation des ressources allouees pour la fonction resolv.
*/
void freeResolv(void);

#endif

