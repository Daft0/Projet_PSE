#include "pse.h"
#include "ph.h"
#include <SDL2/SDL.h>

#define CMD           "serveur"
#define NTHREADS      10
#define MILLISECONDES 1000
#define ATTENTE       2000*MILLISECONDES
#define WIDTH	      1024
#define HEIGHT	      768


int main(int argc, char *argv[]) {

	int ecoute, canal, ret, mode, ilibre, i;
	struct sockaddr_in adrEcoute, reception;
  	socklen_t receptionlen = sizeof(reception);
  	short port;

	SDL_Event event;
	int boucle = 0;
  
  	DataSpec cohorte[NTHREADS];

  	if (argc != 2) {
    		erreur("usage: %s port\n", argv[0]);
 	}

	port = (short) atoi(argv[1]);

	/* Message d'intro sur console */
	printf ("Simulation spatiale par calculs distribués\n");
	printf ("Par LASSERRE Antoine & MAESTRE Gael\n");

	printf ("Chargement :\n");
		

	printf ("Initialisation de la SDL...\n");
	/* Initialisation simple */
    	if (SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        	fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        	exit(EXIT_FAILURE);
    	}
	printf ("Creation de la fenetre...\n");
	/* Création de la fenêtre */
        SDL_Window* pWindow = NULL;
        pWindow = SDL_CreateWindow("Simulation spatiale par calculs distribués",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);

	if (pWindow == NULL) {
		fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	printf ("Creation du renderer...\n");
	/* Affichage d'une image de fond en utilisant le GPU de l'ordinateur */
	SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED); // Création d'un SDL_Renderer utilisant l'accélération matérielle
	/* SI échec lors de la création du Renderer */
	if (pRenderer == NULL) {
		fprintf (stdout, "Echec de creation du renderer (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);		
	}
	
	printf ("Chargement des textures...\n");
	SDL_Surface *pTitle = SDL_LoadBMP("img/title.bmp"); // Chargement de l'écran titre
	SDL_Surface *pLoad = SDL_LoadBMP("img/chargement.bmp"); // Chargement du logo chargement
	SDL_Surface *pDone = SDL_LoadBMP("img/done.bmp"); // Chargement du logo chargement

	/* Si échec lors du chargement du sprite */
	if (pTitle == NULL) {
		fprintf (stdout, "Echec de chargement du sprite de l'ecran titre (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
	/* Si échec lors du chargement du sprite */
	if (pLoad == NULL) {
		fprintf (stdout, "Echec de chargement du sprite de l'ecran titre (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
	/* Si échec lors du chargement du sprite */
	if (pDone == NULL) {
		fprintf (stdout, "Echec de chargement du sprite de l'ecran titre (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	printf ("Traitement des textures...\n");
	SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pTitle); // Préparation du sprite 1
	if (pTexture == NULL) {
		fprintf (stdout, "Echec de creation de la texture (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_Texture *pTexture2 = SDL_CreateTextureFromSurface(pRenderer, pLoad); // Sprite Chargement
	if (pTexture == NULL) {
		fprintf (stdout, "Echec de creation de la texture (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_Texture *pTexture3 = SDL_CreateTextureFromSurface(pRenderer, pDone); // Sprite Done
	if (pTexture == NULL) {
		fprintf (stdout, "Echec de creation de la texture (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}


	printf ("Preparation de l'affichage...\n");
	SDL_Rect dest = {WIDTH/2 - pTitle->w/2, HEIGHT/2 - pTitle->h/2, pTitle->w, pTitle->h}; // Destination 1 (Fond de base chargement)
	SDL_Rect dest2 = {WIDTH/2 - pLoad->w/2, HEIGHT/2 - pLoad->h/2, pLoad->w, pLoad->h}; // Logo chargement
	SDL_Rect dest3 = {WIDTH/2 - pDone->w/2, HEIGHT/2 - pDone->h/2, pDone->w, pDone->h}; // Logo fin de chargement

	SDL_RenderCopy(pRenderer, pTexture, NULL, &dest); // Copie du titre grâce à SDL_Renderer
	SDL_RenderCopy(pRenderer, pTexture2, NULL, &dest2); // Copie du chargement grâce à SDL_Renderer

	printf ("Affichage...\n");
	SDL_RenderPresent(pRenderer); // Affichage

	printf ("Preparation du journal...\n");
  	mode = O_WRONLY|O_APPEND|O_CREAT;
  	journal = open("journal.log", mode, 0660);
  	if (journal == -1) {
    		erreur_IO("open journal");
  	}

	printf ("Initialisation cohorte...\n");
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
  
  	printf("Creation d'un socket...\n");
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
  
  	printf ("Ecoute...\n");
  	ret = listen (ecoute, 20);
  	if (ret < 0) {
    		erreur_IO("listen");
  	}

	SDL_Delay(3000); // Attendre 3s

	SDL_FreeSurface(pLoad);	// Libération surface Load

	SDL_RenderCopy(pRenderer, pTexture3, NULL, &dest3); // Copie de fin de chargement grâce à SDL_Renderer
	SDL_RenderPresent(pRenderer); // Affichage


	printf ("Chargement termine !\n");
	printf ("En attente d'un appui sur la touche <s>...\n");
	/* Attente de l'appui sur s */
	while (boucle == 0) {
		while (SDL_PollEvent(&event) && boucle == 0) // Récupération des actions de l'utilisateur
		{
    			switch(event.type) {
        			case SDL_KEYUP: // Relâchement d'une touche
            			if ( event.key.keysym.sym == SDLK_s ) { // Touche s
					boucle++;
					printf ("OK\n\n");
				}
            			break;
    			}
		}
	}


	// Démarrage simulation

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
	SDL_FreeSurface(pTitle); // Libération des ressource pour le sprite du titre
	SDL_FreeSurface(pDone);	// Idem chargement
	SDL_DestroyRenderer(pRenderer); // Libération de la mémoire du Renderer
        SDL_DestroyWindow(pWindow); // Destruction de la fenêtre
	SDL_Quit();
        	
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
