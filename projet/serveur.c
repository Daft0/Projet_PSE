#include "pse.h"
#include "ph.h"
#include <SDL2/SDL.h>

#define CMD            "serveur"
#define NTHREADS       10
#define MILLISECONDES  1000
#define ATTENTE        2000*MILLISECONDES
#define WIDTH	       1024
#define HEIGHT	       768
#define TAILLE_GLOBALE 5

int nbClient = 0;
int simulationStart = 0;
corps planete[TAILLE_GLOBALE];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
	printf ("SERVEUR\n");

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

  	while (VRAI) {

	// Simulation affichage
 
	if (simulationStart == 0) {
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
			nbClient++;
			printf ("Nombre de clients : %d\n", nbClient);
			if (nbClient >= 3) {
				printf ("Demarrage de la simulation\n\n");
				simulationStart++;
				/*
				* Etapes :
				* 1) Le serveur envoie la taille du tableau de structure à allouer
				* 2) Le client acq
				* 3) Le serveur envoie le tableau global
				* 4) Le client acq
				* 5) Le serveur envoie la taille du tableau de structure à allouer
				* 6) Le client acq
				* 7) Le serveur envoie le tableau fractionné
				* 8) Le client calcul, le serveur attend
				* 9) Dès que les réponses sont arrivées, le serveur rassemble
				* 10) Le serveur affiche les infos
				* 11) Retour en 1)
				*/
			}
			else {
				printf ("Le nombre de clients n'est pas suffisant pour commencer la simulation\n");
			}
  		}
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
  	//int arret = FAUX, nblus;
	//int mode;
  	//char texte[LIGNE_MAX];
	int tailleTot = 5;
	int acq = 0;
	int ecart = 0;

	
  
  	//mode = O_WRONLY|O_APPEND|O_CREAT|O_TRUNC;

  	while (VRAI) {
		sem_wait(&data->sem);
		data->libre = FAUX;
    		//printf("worker %d: lecture canal %d.\n", data->tid, data->canal);
		while(simulationStart == 0);
		printf ("Worker %d : La simulation demarre !\n", data->tid);
		/*
		* Etapes :
		* 1) Le serveur envoie la taille du tableau de structure à allouer
		* 2) Le client acq
		* 3) Le serveur envoie le tableau global
		* 4) Le client acq
		* 5) Le serveur envoie la taille du tableau de structure à allouer
		* 6) Le client acq
		* 7) Le serveur envoie le tableau fractionné
		* 8) Le client calcul, le serveur attend
		* 9) Dès que les réponses sont arrivées, le serveur rassemble
		* 10) Le serveur affiche les infos
		* 11) Retour en 1)
		*/


		// 1) Le serveur envoie la taille du tableau de structure à allouer
		if (pthread_mutex_lock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_lock");
  		}

		printf ("Worker %d : Transmission de la taille de la structure...\n", data->tid);
		write(data->canal, &tailleTot, sizeof(int));
    
  		if (pthread_mutex_unlock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_unlock");
 		}

		// 2) Le client acq	
		if (pthread_mutex_lock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_lock");
  		}
		acq = 0;
		printf ("Worker %d : Attente de l'acq...\n", data->tid);

		while (acq == 0) {
		read(data->canal, &acq, sizeof(int));
		}
		acq = 0;
  		if (pthread_mutex_unlock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_unlock");
 		}
		printf ("Worker %d : Acq OK\n", data->tid);

		// 3) Le serveur envoie le tableau de structure
		if (pthread_mutex_lock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_lock");
  		}

		printf ("Worker %d : Transmission du tableau global...\n", data->tid);
		planete[0].posx = 57;
		write(data->canal, &planete, TAILLE_GLOBALE*sizeof(corps));
    
  		if (pthread_mutex_unlock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_unlock");
 		}

		// 4) Le client acq	
		if (pthread_mutex_lock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_lock");
  		}
		acq = 0;
		printf ("Worker %d : Attente de l'acq2...\n", data->tid);

		while (acq == 0) {
		read(data->canal, &acq, sizeof(int));
		}
		acq = 0;
  		if (pthread_mutex_unlock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_unlock");
 		}
		printf ("Worker %d : Acq2 OK\n", data->tid);

		// 5) Le serveur envoie la taille du tableau de structure à allouer
		/*
		* On utilise data->tid qui est le numéro du worker
		* On utilise nbClient qui contient le nombre de clients
		* On calcul le nombre de cases pour chaque client
		* LE NOMBRE DE PLANETES DOIT ETRE PAIR !
		*/
		printf ("Preparation des valeurs...\n");
		if (pthread_mutex_lock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_lock");
  		}
		ecart = TAILLE_GLOBALE/nbClient;
		if (nbClient%2 == 1 && data->tid == nbClient-1) { // Si nombre de client impair, le dernier client prend un élément de plus
			printf ("Worker %d est le dernier\n", data->tid);
			ecart += TAILLE_GLOBALE-ecart*nbClient;
			printf ("Il a %d a faire\n", ecart);
			
		}
		printf ("%d\n", ecart);
		
		printf ("Transmission de la taille de la structure...\n");
		write(data->canal, &ecart, sizeof(int));
		
		if (pthread_mutex_unlock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_unlock");
 		}

		// 6) Le client acq
		if (pthread_mutex_lock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_lock");
  		}
		acq = 0;
		printf ("Worker %d : Attente de l'acq3...\n", data->tid);

		while (acq == 0) {
		read(data->canal, &acq, sizeof(int));
		}
		acq = 0;
  		if (pthread_mutex_unlock(&mutex) != 0) {
    			erreur_IO("pthread_mutex_unlock");
 		}
		printf ("Worker %d : Acq3 OK\n", data->tid);
		



		sleep(5);

		exit(EXIT_SUCCESS);





		
		/*
    		printf("worker %d: attente canal.\n", data->tid);
    		//attente canal
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
				nbClient--;
	      			arret = VRAI;
      			}
      			else if (strcmp(texte, "calc") == 0) {
	     		
	     		//write(data->canal, &fun, sizeof(corps));
	      		//read(data->canal, &fun, sizeof(corps));
	      		//printf ("<%d>, <%d>, %d>\n", fun.posx, fun.posy, fun.vitesse);
			write(data->canal, tab, sizeof(tab));
      			}
			else if (strcmp(texte, "nbClient") == 0) {
				printf ("Il y a actuellement %d clients\n", nbClient);
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
		*/

		while(1);

		printf ("La simulation est terminee !\n");
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

void init_Simulation() {
	/*
	* 1) Déclaration de 10 corps (10 structures)
	*/

}
