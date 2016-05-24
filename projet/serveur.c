#include "pse.h"
#include "ph.h"
#include <SDL2/SDL.h>

#define CMD           "serveur"
#define NTHREADS      10
#define MILLISECONDES 1000
#define ATTENTE       2000*MILLISECONDES

int main(int argc, char *argv[]) {

	/* Initialisation simple */
    	if (SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        	fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        	return -1;
    	}
	else {
       		/* Création de la fenêtre */
        	SDL_Window* pWindow = NULL;
        	pWindow = SDL_CreateWindow("Simulation spatiale par calculs distribués",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,480,SDL_WINDOW_SHOWN);

	int ecoute, canal, ret, mode, ilibre, i;
	struct sockaddr_in adrEcoute, reception;
  	socklen_t receptionlen = sizeof(reception);
  	short port;
  
  	DataSpec cohorte[NTHREADS];

  	if (argc != 2) {
    		erreur("usage: %s port\n", argv[0]);
 	 }

  	mode = O_WRONLY|O_APPEND|O_CREAT;
  	journal = open("journal.log", mode, 0660);
  	if (journal == -1) {
    		erreur_IO("open journal");
  	}

  	/* initialisation cohorte */
  	for (i=0; i<NTHREADS; i++) {
    		cohorte[i].tid = i;
    		cohorte[i].libre = VRAI;
    		/* une valeur -1 indique pas de requete a traiter */
    		cohorte[i].canal = -1;
    		sem_init(&cohorte[i].sem,0,0); // Initialisation de chaque sémaphore
    		ret = pthread_create(&cohorte[i].id, NULL, traiterRequete, &cohorte[i]);
    		if (ret != 0) {
      			erreur_IO("pthread_create");
    		}
  	}

  	port = (short) atoi(argv[1]);
  
  	printf("%s: creating a socket\n", CMD);
  	ecoute = socket (AF_INET, SOCK_STREAM, 0);
  	if (ecoute < 0) {
    		erreur_IO("socket");
  	}
  
  	adrEcoute.sin_family = AF_INET;
 	 adrEcoute.sin_addr.s_addr = INADDR_ANY;
  	adrEcoute.sin_port = htons(port);
  	printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  	ret = bind (ecoute,  (struct sockaddr *) &adrEcoute, sizeof(adrEcoute));
  	if (ret < 0) {
    		erreur_IO("bind");
  	}
  
  	printf("%s: listening to socket\n", CMD);
  	ret = listen (ecoute, 20);
  	if (ret < 0) {
    		erreur_IO("listen");
  	}

  	while (VRAI) {
 
    	printf("%s: waiting to a connection\n", CMD);
    	canal = accept(ecoute, (struct sockaddr *) &reception, &receptionlen);
    	if (canal < 0) {
      		erreur_IO("accept");
    	}
    	printf("%s: adr %s, port %hu\n", CMD,
      	stringIP(ntohl(reception.sin_addr.s_addr)), ntohs(reception.sin_port));

    	ilibre = NTHREADS;
    	while (ilibre == NTHREADS) {
      		for (ilibre=0; ilibre<NTHREADS; ilibre++)
        		if (cohorte[ilibre].libre) break;
      			printf("serveur: %d\n", ilibre);
      			if (ilibre == NTHREADS) usleep(ATTENTE);
   		}

    		cohorte[ilibre].canal = canal;
    		sem_post(&cohorte[ilibre].sem);
    		printf("%s: worker %d choisi\n", CMD, ilibre);
  	}

	pthread_exit(NULL);
	if( pWindow )
        	{
            	SDL_Delay(3000); /* Attendre trois secondes, que l'utilisateur voit la fenêtre */

            	SDL_DestroyWindow(pWindow);
        	}
        	else
        	{
            	fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
       		}
	SDL_Quit();
	}

	exit(EXIT_SUCCESS);
}

void *traiterRequete(void *arg) {
  	DataSpec * data = (DataSpec *) arg;
  	int arret = FAUX, nblus, mode;
  	char texte[LIGNE_MAX];
  	corps planete1;
  	corps planete2;
  	corps tab[2];

  	tab[0] = planete1;
  	tab[1] = planete2;

  	tab[0].posx = 32;
  	tab[0].posy = 23;
  	tab[0].vitesse = 6;
	tab[0].type_corps = 1;

  	tab[1].posx = 78;
  	tab[1].posy = 87;
  	tab[1].vitesse = 3;
	tab[1].type_corps = 2;
  
  	mode = O_WRONLY|O_APPEND|O_CREAT|O_TRUNC;

  	while (VRAI) {
    		printf("worker %d: attente canal.\n", data->tid);
    		/* attente canal */
    		sem_wait(&data->sem);
    		data->libre = FAUX;
    		printf("worker %d: lecture canal %d.\n", data->tid, data->canal);

    		arret = FAUX;
    		while (arret == FAUX) {
     			 nblus = lireLigne (data->canal, texte);
      			if (nblus <= 0 || nblus == LIGNE_MAX) {
        			erreur("lireLigne\n");
      			}
      			if (strcmp(texte, "fin") == 0) {
	     			printf("worker %d: deconnexion demandee.\n", data->tid);
	      			arret = VRAI;
      			}
      			else if (strcmp(texte, "calc") == 0) {
	     		/*
	     		write(data->canal, &fun, sizeof(corps));
	      		read(data->canal, &fun, sizeof(corps));
	      		printf ("<%d>, <%d>, %d>\n", fun.posx, fun.posy, fun.vitesse);*/
			write(data->canal, tab, sizeof(tab));
      			}
      			else if (strcmp(texte, "init") == 0) {
	      			printf("worker %d: remise a zero du journal demandee.\n", data->tid);
        			journal = remiseAZeroLog(journal, mode);
	    		}
      			else if (ecrireLigne(journal, texte) != -1) {
	      			printf("worker %d: ligne de %d octets ecrite dans le journal.\n",data->tid, nblus);
      			}
      			else {
        			erreur_IO("ecrireLigne");
      			}
    		}

    		if (close(data->canal) == -1) {
      			erreur_IO("close");
   		 }
    		data->canal = -1;
   		data->libre = VRAI;
  	}
	
}

int remiseAZeroLog(int fd, int mode) {
  	int newFd;
  	if (close(fd) == -1) {
    		erreur_IO("close log");
  	}
  	newFd = open("journal.log", mode|O_TRUNC, 0660);
  	if (newFd == -1) {
   		 erreur_IO("open trunc log");
  	}
  	return newFd;
}
