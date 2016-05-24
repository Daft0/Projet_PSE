#include "pse.h"
#include "ph.h"

#define CMD   "client"

int main(int argc, char *argv[]) {
  	int sock, arret = FAUX, ret, nbecr;
  	struct sockaddr_in *sa;
  	char texte[LIGNE_MAX];

  	corps planete1;
  	corps planete2;
  	corps tab[2];

  	tab[0] = planete1;
  	tab[1] = planete2;
  
  	if (argc != 3) {
   		erreur("usage: %s machine port\n", argv[0]);
  	}

  	printf("%s: creating a socket\n", CMD);
  	sock = socket (AF_INET, SOCK_STREAM, 0);
  	if (sock < 0) {
    		erreur_IO("socket");
  	}

  	printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
  	sa = resolv(argv[1], argv[2]);
  	if (sa == NULL) {
    		erreur("adresse %s et port %s inconnus\n", argv[1], argv[2]);
  	}
  	printf("%s: adr %s, port %hu\n", CMD,stringIP(ntohl(sa->sin_addr.s_addr)),ntohs(sa->sin_port));

  	/* connexion sur site distant */
  	printf("%s: connecting the socket\n", CMD);
  	ret = connect(sock, (struct sockaddr *) sa, sizeof(struct sockaddr_in));
  	if (ret < 0) {
    		erreur_IO("Connect");
  	}

  	freeResolv();

  	while (arret == FAUX) {
   		printf("ligne> ");
    		if (fgets(texte, LIGNE_MAX, stdin) == NULL) {
     			printf("Fin de fichier (ou erreur) : arret.\n");
      			arret = VRAI;
      		continue;
    		}
    		else {
      			nbecr = ecrireLigne(sock, texte);
      			if (nbecr == -1) {
				erreur_IO("ecrireLigne");
      			}
      			if (strcmp(texte, "fin\n") == 0) {
				printf("Client. arret demande.\n");
				arret = VRAI;
      			}
      			else if (strcmp(texte, "calc\n") == 0) {
				read(sock, tab, sizeof(tab));
				printf ("<%d>, <%d>, <%f>, <%d>\n<%d>, <%d>, <%f>, <%d>\n", tab[0].posx, tab[0].posy, tab[0].vitesse, tab[0].type_corps, tab[1].posx, tab[1].posy, tab[1].vitesse, tab[1].type_corps);
				/*
				read(sock, &fun, sizeof(corps));i
				printf ("<%d>, <%d>, <%d>\n", fun.posx, fun.posy, fun.vitesse);
				fun.posx++;
				fun.posy++;
				fun.vitesse++;
				write(sock, &fun, sizeof(corps));*/
      			}
      			else {
				printf("%s: ligne de %d octets envoyee au serveur.\n", CMD, nbecr);
      			}
    		}
  	}

  	exit(EXIT_SUCCESS);
}
