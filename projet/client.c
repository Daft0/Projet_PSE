#include "pse.h"
#include "ph.h"

#define CMD   "client"

int main(int argc, char *argv[]) {
  	int sock, ret;
	//int arret = FAUX, nbecr;
  	struct sockaddr_in *sa;
  	//char texte[LIGNE_MAX];
	int tailleTot = 0;
	int taillePart = 0;
	int acq = 0;
	int i = 0;

	corps *tab; // Déclaration du tableau de structure total
	corps *tabPart; // Déclaration du tableau de structure partiel
  
  	if (argc != 3) {
   		erreur("usage: %s machine port\n", argv[0]);
  	}

	/* Message d'intro sur console */
	printf ("Simulation spatiale par calculs distribués\n");
	printf ("Par LASSERRE Antoine & MAESTRE Gael\n");
	printf ("CLIENT\n");

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

	printf ("Pret pour la simulation\n");
	// 1) Réception de la taille du tableau de structure à allouer
	printf ("Reception de la taille du tableau global...\n");
	while (tailleTot == 0) {
		read(sock, &tailleTot, sizeof(int));
	}
	printf ("Je dois allouer %d octets\n\n", tailleTot);

	// 2) Acq
	printf ("Allocation en cours...\n");
	tab = (corps*) calloc(tailleTot, sizeof(corps));
	if (tab == NULL) {
		exit(EXIT_FAILURE);
	}	
	printf ("Acq...\n\n");
	acq = 1; // Mise à 1 de la variable
	write(sock, &acq, sizeof(int)); // Transmission
	acq = 0; // Remise à 0 de la variable

	// 3) Réception du tableau global
	printf ("Reception du tableau global...\n");
	while (tab[0].posx == 0) {
		read(sock, tab, tailleTot*sizeof(corps));
	}
	printf ("C'est bon\n\n");

	// 4) Acq2
	printf ("Acq2...\n\n");
	acq = 1; // Mise à 1 de la variable
	write(sock, &acq, sizeof(int)); // Transmission
	acq = 0; // Remise à 0 de la variable

	// 5) Réception de la taille du tableau partiel de structure à allouer
	printf ("Reception de la taille du tableau partiel\n");
	while (taillePart == 0) {
		read(sock, &taillePart, sizeof(int));
	}
	printf ("Je dois allouer %d octets\n\n", taillePart);
	
	// 6) Acq3
	printf ("Allocation en cours...\n");
	tabPart = (corps*) calloc(taillePart, sizeof(corps));
	if (tabPart == NULL) {
		exit(EXIT_FAILURE);
	}	
	printf ("Acq3...\n\n");
	acq = 1; // Mise à 1 de la variable
	write(sock, &acq, sizeof(int)); // Transmission
	acq = 0; // Remise à 0 de la variable

	// 7) Réception du tableau partiel
	printf ("Reception du tableau partiel...\n");
	while (tabPart[0].posx == 0) {
		read(sock, tabPart, taillePart*sizeof(corps));
	}
	printf ("C'est bon\n\n");

	// 8) Le client calcul et acq
	printf ("Calculs en cours...\n");
	for (i = 0 ; i < taillePart ; i++) { // CALCULS
		tabPart[i].posx += 50;
		tabPart[i].posy += 50;
	}
	printf ("Calculs termines !\n");
	printf ("Acq...\n\n");
	acq = 1; // Mise à 1 de la variable
	write(sock, &acq, sizeof(int)); // Transmission
	acq = 0; // Remise à 0 de la variable

	//  9) Le client envoie les données et le serveur rassemble
	printf ("Transmission des donnees...\n");
	write(sock, tabPart, taillePart*sizeof(corps));
	printf ("C'est bon\n\n");
	
	
	


	/*
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
				
				//read(sock, &fun, sizeof(corps));i
				//printf ("<%d>, <%d>, <%d>\n", fun.posx, fun.posy, fun.vitesse);
				//fun.posx++;
				//fun.posy++;
				//fun.vitesse++;
				//write(sock, &fun, sizeof(corps));
      			}
      			else {
				printf("%s: ligne de %d octets envoyee au serveur.\n", CMD, nbecr);
      			}
    		}
  	}*/
	free(tab);
	free(tabPart);

  	exit(EXIT_SUCCESS);
}
