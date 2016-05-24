/* module resolv: resolv.c */

#include "pse.h"

/*
  variable globale a visibilite restreinte a ce fichier.
*/
static struct addrinfo *static_infos = NULL;

struct sockaddr_in *resolv (const char *host, const char *service) {
  struct addrinfo hints;
  int code;

  memset ( &hints, 0, sizeof (struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  freeResolv();

  code = getaddrinfo (host, service, &hints, &static_infos);
  if ( code != 0) {
    fprintf(stderr, "ERREUR. %s\n", gai_strerror(code));
    return NULL;
  }
  return (struct sockaddr_in *) static_infos->ai_addr;
}

char *stringIP (unsigned int entierIP) {
  struct in_addr ia;
  ia.s_addr = htonl (entierIP); 
  return inet_ntoa(ia);
}

void freeResolv (void) {
  if (static_infos != NULL) {
    freeaddrinfo (static_infos);
  }
}

