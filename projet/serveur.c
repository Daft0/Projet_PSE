#include "pse.h"
#include "ph.h"
#include <SDL2/SDL.h>

#define CMD            "serveur"
#define NTHREADS       3
#define MILLISECONDES  1000
#define ATTENTE        2000*MILLISECONDES
#define WIDTH	       720
#define HEIGHT	       720
#define TAILLE_GLOBALE 8
#define CLIENTS_MIN    3
#define NBRE_TOUR_MAX	300


/* 
* Déclaration des variables globales
* Elles sont utilisées dans les différents threads
*/
int nbClient = 0;
int simulationStart = 0;
int affichageStart = 0;
int nbClientSeuil = -1;
corps planete[TAILLE_GLOBALE];
FILE *journal;

// Déclaration d'un mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Variables globales de la SDL
SDL_Window* pWindow = NULL;
SDL_Renderer *pRenderer;
SDL_Surface *pTitle;
SDL_Surface *pLoad;
SDL_Surface *pDone;
SDL_Texture *pTexture;
SDL_Texture *pTexture2;
SDL_Texture *pTexture3;


int main(int argc, char *argv[]) {

	// Déclaration des variables du main
	int ecoute, canal, ret, ilibre, i;
	struct sockaddr_in adrEcoute, reception;
  	socklen_t receptionlen = sizeof(reception);
  	short port;
	int tours = 0;

	SDL_Event event; // Déclaration d'une variable "évenement"
	int boucle = 0;	
  
  	DataSpec cohorte[NTHREADS]; // Cohorte

	srand (time (NULL)); // Initialisation du temps (pour la génération de nombres aléatoire)


  	if (argc != 2) { // SI le nombre d'arguments n'est pas correct
    		erreur("usage: %s port\n", argv[0]);
 	}

	port = (short) atoi(argv[1]);

	/* Message d'intro sur console */
	printf ("Simulation spatiale par calculs distribués\n");
	printf ("Par LASSERRE Antoine & MAESTRE Gael\n");
	printf ("SERVEUR\n");

	printf ("Preparation du journal...\n"); // Préparation du journal
  	journal = fopen("journal.log", "w"); // Réécriture d'un nouveau journal
  	if (journal == NULL) {
    		erreur_IO("open journal");
  	}
	
	printf ("Veuillez consulter le journal pour avoir des informations\n");

	fprintf (journal, "Simulation spatiale par calculs distribués\n");
	fprintf (journal, "Par LASSERRE Antoine et MAESTRE Gaël\n\n");

	fprintf (journal, "Chargement :\n");
		

	fprintf (journal, "Initialisation de la SDL...\n");
	/* Initialisation simple */
    	if (SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        	fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        	exit(EXIT_FAILURE);
    	}
	fprintf (journal, "Creation de la fenetre...\n");
	/* Création de la fenêtre */
        pWindow = SDL_CreateWindow("Simulation spatiale par calculs distribués",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);

	if (pWindow == NULL) {
		fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	fprintf (journal, "Creation du renderer...\n");
	/* Affichage d'une image de fond en utilisant le GPU de l'ordinateur */
	pRenderer = SDL_CreateRenderer(pWindow, -1, 0); // Création d'un SDL_Renderer utilisant l'accélération matérielle
	/* SI échec lors de la création du Renderer */
	if (pRenderer == NULL) {
		fprintf (stdout, "Echec de creation du renderer (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);		
	}
	
	fprintf (journal, "Chargement des textures...\n");
	pTitle = SDL_LoadBMP("img/title.bmp"); // Chargement de l'écran titre
	pLoad = SDL_LoadBMP("img/chargement.bmp"); // Chargement du logo chargement
	pDone = SDL_LoadBMP("img/done.bmp"); // Chargement du logo chargement

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

	fprintf (journal, "Traitement des textures...\n");
	pTexture = SDL_CreateTextureFromSurface(pRenderer, pTitle); // Préparation du sprite 1
	if (pTexture == NULL) {
		fprintf (stdout, "Echec de creation de la texture (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	pTexture2 = SDL_CreateTextureFromSurface(pRenderer, pLoad); // Sprite Chargement
	if (pTexture == NULL) {
		fprintf (stdout, "Echec de creation de la texture (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	pTexture3 = SDL_CreateTextureFromSurface(pRenderer, pDone); // Sprite Done
	if (pTexture == NULL) {
		fprintf (stdout, "Echec de creation de la texture (%s)\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}


	fprintf (journal, "Preparation de l'affichage...\n");
	SDL_Rect dest = {WIDTH/2 - pTitle->w/2, HEIGHT/2 - pTitle->h/2, pTitle->w, pTitle->h}; // Destination 1 (Fond de base chargement)
	SDL_Rect dest2 = {WIDTH/2 - pLoad->w/2, HEIGHT/2 - pLoad->h/2, pLoad->w, pLoad->h}; // Logo chargement
	SDL_Rect dest3 = {WIDTH/2 - pDone->w/2, HEIGHT/2 - pDone->h/2+70, pDone->w, pDone->h}; // Logo fin de chargement

	SDL_RenderCopy(pRenderer, pTexture, NULL, &dest); // Copie du titre grâce à SDL_Renderer
	SDL_RenderCopy(pRenderer, pTexture2, NULL, &dest2); // Copie du chargement grâce à SDL_Renderer

	fprintf (journal, "Affichage...\n");
	SDL_RenderPresent(pRenderer); // Affichage

	fprintf (journal, "Initialisation cohorte...\n");
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
  
  	fprintf(journal, "Creation d'un socket...\n"); // Création d'un coket
  	ecoute = socket (AF_INET, SOCK_STREAM, 0);
  	if (ecoute < 0) {
    		erreur_IO("socket");
  	}
  
  	adrEcoute.sin_family = AF_INET; // Préparation de l'écoute
 	adrEcoute.sin_addr.s_addr = INADDR_ANY;
  	adrEcoute.sin_port = htons(port);
  	printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
	fprintf(journal, "%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  	ret = bind (ecoute,  (struct sockaddr *) &adrEcoute, sizeof(adrEcoute));
  	if (ret < 0) {
    		erreur_IO("bind");
  	}
  
  	fprintf (journal, "Ecoute...\n");
  	ret = listen (ecoute, 20); // Lancement de l'écoute
  	if (ret < 0) {
    		erreur_IO("listen");
  	}


	SDL_FreeSurface(pLoad);	// Libération surface Load

	SDL_RenderCopy(pRenderer, pTexture3, NULL, &dest3); // Copie de fin de chargement grâce à SDL_Renderer
	SDL_RenderPresent(pRenderer); // Affichage


	fprintf (journal, "Chargement termine !\n");
	printf ("En attente d'un appui sur la touche <s>...\n");
	/* Attente de l'appui sur s */
	while (boucle == 0) {
		while (SDL_PollEvent(&event) && boucle == 0) // Récupération des actions de l'utilisateur
		{
    			switch(event.type) {
        			case SDL_KEYUP: // Relâchement d'une touche
            			if ( event.key.keysym.sym == SDLK_s ) { // Touche s
					boucle++; // On sort de la boucle
					printf ("OK\n\n");
				}
            			break;
    			}
		}
	}

	initSimulation(); // Initialisation des différentes planètes
	affichage(); // Affichage

	printf ("Le serveur est pret et attend 2 clients pour demarrer...\n");

  	while (VRAI) {
		while (SDL_PollEvent(&event)) // Récupération des actions de l'utilisateur
		{
    			switch(event.type) {
        			case SDL_KEYUP: // Relâchement d'une touche
            			if ( event.key.keysym.sym == SDLK_q ) { // Touche s
					fprintf (journal, "L'utilisateur souhaite quitter...");
					printf ("L'utilisateur souhaite quitter...\n\n");
					SDL_FreeSurface(pTitle); // Libération des ressource pour le sprite du titre
					SDL_FreeSurface(pDone);	// Idem chargement
					SDL_DestroyRenderer(pRenderer); // Libération de la mémoire du Renderer
        				SDL_DestroyWindow(pWindow); // Destruction de la fenêtre
					SDL_Quit(); // Fermeture de la SDL
        				exit(EXIT_SUCCESS); // Fermeture du programme	
				}
            			break;
    			}	
		}

		if (affichageStart == nbClientSeuil && tours < NBRE_TOUR_MAX) { // Si la simulation est terminée	
			fprintf (journal, "Simulation terminee\n");
			tours++;
			printf ("Tours de simulation effectues : %d\n", tours);
			fprintf (journal, "Tours de simulation effectués : %d\n", tours);
			simulationStart = 0; // Arrêt de la simulation
			nbClient = 0; // Réinitialisation du nombre de clients
			affichageStart = 0; // On ne veut plus afficher
			affichage(); // Actualisation de l'affichage de la simulation
			/* réinitialisation cohorte */
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
		}
		if (simulationStart == 0 && tours < NBRE_TOUR_MAX) { // Si la simulation n'est pas en cours
	    		fprintf(journal, "%s: waiting to a connection\n", CMD); // Attente de connexions
	    		canal = accept(ecoute, (struct sockaddr *) &reception, &receptionlen); // Dès qu'une connexion est acceptée
	    		if (canal < 0) {
	      			erreur_IO("accept");
	    		}
	    		fprintf(journal, "%s: adr %s, port %hu\n", CMD,
	      		stringIP(ntohl(reception.sin_addr.s_addr)), ntohs(reception.sin_port));

	    		ilibre = NTHREADS;
	    		while (ilibre == NTHREADS) { // Création des threads
	      			for (ilibre=0; ilibre<NTHREADS; ilibre++)
					if (cohorte[ilibre].libre) break;
	      				fprintf(journal, "serveur: %d\n", ilibre);
	      				if (ilibre == NTHREADS) usleep(ATTENTE);
	   			}

	    			cohorte[ilibre].canal = canal;
	    			sem_post(&cohorte[ilibre].sem);
	    			fprintf(journal, "%s: worker %d choisi\n", CMD, ilibre);
				nbClient++; // Un nouveau client est disponible
				fprintf (journal, "Nombre de clients : %d\n", nbClient);
				sem_wait(&cohorte[ilibre].sem);
				cohorte[ilibre].libre = FAUX;
				if (nbClient >= CLIENTS_MIN) { // Si le nombre de clients est suffisant, alors on commence la simulation
					fprintf (journal, "La simulation demarre !\n\n");
					nbClientSeuil = nbClient; // Définition du seuil
					simulationStart = 1; // Flag de démarrage de la simulation
				}
				else {
					fprintf (journal, "Le nombre de clients n'est pas suffisant pour commencer la simulation\n");
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
	int tailleTot = TAILLE_GLOBALE;
	int acq = 0;
	int ecart = 0;
	int octetEnvoyer = 0;
	corps *tab; // Tableau partiel
	corps *tabTemp; // Tableau temporaire
	int i = 0;
	int etape = 0;
	int boucle = 0;
	int tempsPrecedent = 0, tempsActuel = 0;

		while(simulationStart == 0);
		fprintf (journal, "Worker %d : La simulation demarre !\n", data->tid);
		boucle = 0;
		etape = 0;
		tempsPrecedent = SDL_GetTicks();
		/*
		* Etapes :
		* 1) Le serveur envoie la taille du tableau de structure à allouer
		* 2) Le client acq
		* 3) Le serveur envoie le tableau global
		* 4) Le client acq
		* 5) Le serveur envoie la taille du tableau de structure à allouer
		* 6) Le client acq
		* 7) Le serveur envoie le tableau fractionné
		* 8) Le client calcul et acq
		* 9) Le client envoie les données et le serveur rassemble
		* 10) Le serveur affiche les infos
		* 11) Retour en 1)
		*/

		while (boucle == 0) {
			tempsActuel = SDL_GetTicks();
			if (tempsActuel - tempsPrecedent > 5000) {/* Si 5 s se sont écoulées lors de la simulation, c'est qu'il y a eu une erreur */
            			tempsPrecedent = tempsActuel; /* Le temps "actuel" devient le temps "precedent" pour nos futurs calculs */
				fprintf (journal, "Worker %d : TIMEOUT!\n", data->tid);
				etape = 9;
        		}
			switch(etape) {
				case 0:
					// 1) Le serveur envoie la taille du tableau de structure à allouer
					if (pthread_mutex_lock(&mutex) != 0) { // Utilisation d'un mutex
			    			erreur_IO("pthread_mutex_lock");
			  		}

					fprintf (journal, "Worker %d : Transmission de la taille de la structure...\n", data->tid);
					write(data->canal, &tailleTot, sizeof(int)); // Ecriture de données
			    
			  		if (pthread_mutex_unlock(&mutex) != 0) { // Fin du mutex
			    			erreur_IO("pthread_mutex_unlock");
			 		}
					etape++;
					break;

				case 1:
					// 2) Le client acq	
					acq = 0;
					fprintf (journal, "Worker %d : Attente de l'acq...\n", data->tid);

					while (acq == 0) {
						read(data->canal, &acq, sizeof(int)); // Lecture de l'acquittement
					}
					acq = 0;
					fprintf (journal, "Worker %d : Acq OK\n", data->tid);
					etape++;
					break;
				case 2:
					// 3) Le serveur envoie le tableau de structure
					if (pthread_mutex_lock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_lock");
			  		}

					fprintf (journal, "Worker %d : Transmission du tableau global...\n", data->tid);
					write(data->canal, planete, TAILLE_GLOBALE*sizeof(corps)); // Transmission du tableau de structure
			    
			  		if (pthread_mutex_unlock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_unlock");
			 		}
					etape++;
					break;
				case 3:
					// 4) Le client acq	
					acq = 0;
					fprintf (journal, "Worker %d : Attente de l'acq2...\n", data->tid);

					while (acq == 0) {
						read(data->canal, &acq, sizeof(int)); // ACQ
					}
					acq = 0;
					fprintf (journal, "Worker %d : Acq2 OK\n", data->tid);
					etape++;
					break;				
				case 4:
					// 5) Le serveur envoie la taille du tableau de structure à allouer
					/*
					* On utilise data->tid qui est le numéro du worker
					* On utilise nbClient qui contient le nombre de clients
					* On calcul le nombre de cases pour chaque client
					*/
					if (pthread_mutex_lock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_lock");
			  		}
					fprintf (journal, "Worker %d : Preparation des valeurs...\n", data->tid);
					ecart = TAILLE_GLOBALE/nbClient;
					if (TAILLE_GLOBALE%nbClient != 0 && data->tid == nbClient-1) { // Si nombre de client impair, le dernier client prend un élément de plus
						octetEnvoyer = TAILLE_GLOBALE - ecart * data->tid;
						fprintf (journal, "Worker %d : est le dernier\n", data->tid);
						fprintf (journal, "Worker %d Il a %d a faire\n", data->tid, octetEnvoyer);
			
						
						//for (i=0; i<TAILLE_GLOBALE; i++)
						//	printf("%d : %lf *10^%i : %lf *10^%i\n", i, planete[i].coeffX, planete[i].exposantX, planete[i].coeffY, planete[i].exposantY);
						fprintf (journal, "Il a %d donnees a calculer\n", data->tid);
						fprintf (journal, "Worker %d : %d\n", data->tid, ecart);
						fprintf (journal, "Worker %d : Transmission de la taille de la structure...\n", data->tid);
						write(data->canal, &octetEnvoyer, sizeof(int));
					}
					else {
						octetEnvoyer = ecart;
						fprintf (journal, "Worker %d : %d\n", data->tid, ecart);
						fprintf (journal, "Worker %d : Transmission de la taille de la structure...\n", data->tid);
						write(data->canal, &octetEnvoyer, sizeof(int));
					}
					
					
					if (pthread_mutex_unlock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_unlock");
			 		}
					etape++;
					break;
				case 5:
					// 6) Le client acq
					acq = 0;
					fprintf (journal, "Worker %d : Attente de l'acq3...\n", data->tid);

					while (acq == 0) {
						read(data->canal, &acq, sizeof(int));
					}
					acq = 0;
					fprintf (journal, "Worker %d : Acq3 OK\n", data->tid);
					etape++;
					break;
				case 6:
					// 7) Le serveur envoie le tableau fractionné
		
					if (pthread_mutex_lock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_lock");
			  		}
					fprintf (journal, "Worker %d : Transmission du tableau partiel...\n", data->tid);
					if (TAILLE_GLOBALE%nbClient != 0 && data->tid == nbClient-1) {
						tab = (corps*) calloc(octetEnvoyer, sizeof(corps)); // Attribution de l'espace
						for (i = 0 ; i < octetEnvoyer ; i++) {
						//printf("%d : %lf *10^%i : %lf *10^%i\n", i, planete[i].coeffX, planete[i].exposantX, planete[i].coeffY, planete[i].exposantY);
						//printf("affichage de %d + %d * %d\n", i, ecart, data->tid);
							tab[i] = planete[i + TAILLE_GLOBALE - octetEnvoyer]; // Sauvegarde des valeurs
						}
						write(data->canal, tab, (octetEnvoyer)*sizeof(corps)); // Transfert
					}
					else
					{
						tab = (corps*) calloc(octetEnvoyer, sizeof(corps)); // Attribution de l'espace
						for (i = 0 ; i < octetEnvoyer ; i++) {
						//printf("%d : %lf *10^%i : %lf *10^%i\n", i, planete[i].coeffX, planete[i].exposantX, planete[i].coeffY, planete[i].exposantY);
						//printf("affichage de %d + %d * %d\n", i, ecart, data->tid);
						tab[i] = planete[i+ecart*data->tid]; // Sauvegarde des valeurs
						}
						write(data->canal, tab, octetEnvoyer*sizeof(corps)); // Transfert
					}

					if (pthread_mutex_unlock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_unlock");
			 		}
					etape++;
					break;
				case 7:
					// 8) Le client calcul et acq
					acq = 0;
					fprintf (journal, "Worker %d : Attente de l'acq4...\n", data->tid);

					while (acq == 0) {
						read(data->canal, &acq, sizeof(int));
					}
					acq = 0;
					fprintf (journal, "Worker %d : Acq4 OK\n", data->tid);
					etape++;
					break;
				case 8:
					//  9) Le client envoie les données et le serveur rassemble
		
					if (pthread_mutex_lock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_lock");
			  		}

					fprintf (journal, "Worker %d : Reception des donnees...\n", data->tid);
					if (nbClient%2 == 1 && data->tid == nbClient-1) {
						tabTemp = (corps*) calloc(octetEnvoyer, sizeof(corps)); // Attribution de l'espace
						read(data->canal, tabTemp, (octetEnvoyer)*sizeof(corps));
						for (i = 0 ; i < octetEnvoyer ; i++) {
						planete[i+TAILLE_GLOBALE - octetEnvoyer] = tabTemp[i]; // Sauvegarde des valeurs
						}
					}
					else
					{
						tabTemp = (corps*) calloc(octetEnvoyer+1, sizeof(corps)); // Attribution de l'espace
						read(data->canal, tabTemp, octetEnvoyer*sizeof(corps));
						for (i = 0 ; i < octetEnvoyer ; i++) {
						planete[i+ecart*data->tid] = tabTemp[i]; // Sauvegarde des valeurs
						}
					}

		
					fprintf (journal, "Worker %d : Reception terminee !\n", data->tid);

					if (pthread_mutex_unlock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_unlock");
			 		}
					etape++;
					break;

				case 9:
					fprintf (journal, "Worker %d : La simulation est terminee !\n", data->tid);
			    		if (close(data->canal) == -1) {
			      			erreur_IO("close");
			   		}
		
					if (pthread_mutex_lock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_lock");
			  		}
					affichageStart++;

			    		if (pthread_mutex_unlock(&mutex) != 0) {
			    			erreur_IO("pthread_mutex_unlock");
			 		}
					boucle++;
					break;			
			}
		}
		if(tab != NULL)
			free(tab);
		if (tabTemp != NULL)
			free(tabTemp);
		fprintf (journal, "Worker %d : En attente des autres clients...\n", data->tid);
		while (affichageStart < nbClientSeuil);
		nbClient--;
		
	//}
	data->canal = -1;
	data->libre = VRAI;
	pthread_exit(NULL);
	
}

void affichage() {
	/*
	* Cette fonction permet de lire les données puis de les afficher
	*/

	double valueX = 0;
	double valueY = 0;

	fprintf (journal, "Affichage...\n");
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255); // Affichage d'un fond noir
	SDL_RenderClear(pRenderer); // Actualisation du renderer

	// Déclaration des sprites
	SDL_Surface *pSpriteSoleil;
	SDL_Surface *pSpritePlanete;
	SDL_Surface *pFond;
	SDL_Texture *pTexture;

	pSpriteSoleil = SDL_LoadBMP("img/sun.bmp"); // Chargement du sprite soleil
	if (pSpriteSoleil == NULL) {
			fprintf (stdout, "Echec de chargement d'un sprite (%s)\n", SDL_GetError());
			SDL_Quit();
			exit(EXIT_FAILURE);
	}
	pSpritePlanete = SDL_LoadBMP("img/planete.bmp"); // Chargement sprite planete
	if (pSpritePlanete == NULL) {
			fprintf (stdout, "Echec de chargement d'un sprite (%s)\n", SDL_GetError());
			SDL_Quit();
			exit(EXIT_FAILURE);
	}
	pFond = SDL_LoadBMP("img/fond.bmp"); // Chargement sprite planete
	if (pFond == NULL) {
			fprintf (stdout, "Echec de chargement d'un sprite (%s)\n", SDL_GetError());
			SDL_Quit();
			exit(EXIT_FAILURE);
	}

	
	
	pTexture = SDL_CreateTextureFromSurface(pRenderer, pFond); // Préparation du sprite
	if (pTexture == NULL) {
			fprintf (stdout, "Echec de creation de la texture (%s)\n", SDL_GetError());
			SDL_Quit();
			exit(EXIT_FAILURE);
	}
	SDL_Rect destSprite = {0, 0, pFond->w, pFond->h}; // Destination
	
	SDL_RenderCopy(pRenderer, pTexture, NULL, &destSprite); // Copie de la texture


	
	int i = 0;
	for (i = 0 ; i < TAILLE_GLOBALE ; i++) {
		valueX = planete[i].coeffX*pow(10,planete[i].exposantX-13); // Calcul de la position en adaptant à la fenêtre
		valueY =  planete[i].coeffY*pow(10,planete[i].exposantY-13); // Idem
		if (planete[i].typeCorps == 0) { // Si c'est un soleil
			pTexture = SDL_CreateTextureFromSurface(pRenderer, pSpriteSoleil); // Préparation du sprite
			SDL_Rect destSprite = {(int)(valueX*WIDTH/2 + WIDTH/2), (int)(valueY*HEIGHT/2 + HEIGHT/2), pSpriteSoleil->w, pSpriteSoleil->h}; // Destination
			SDL_RenderCopy(pRenderer, pTexture, NULL, &destSprite); // Copie de la texture
		}
		else {
			pTexture = SDL_CreateTextureFromSurface(pRenderer, pSpritePlanete); // Préparation du sprite
			SDL_Rect destSprite = {(int)(valueX*WIDTH/2 + WIDTH/2), (int)(valueY*HEIGHT/2 + HEIGHT/2), pSpritePlanete->w, pSpritePlanete->h}; // Destination
			SDL_RenderCopy(pRenderer, pTexture, NULL, &destSprite); // Copie de la texture
		}
		if (pTexture == NULL) {
			fprintf (stdout, "Echec de creation de la texture (%s)\n", SDL_GetError());
			SDL_Quit();
			exit(EXIT_FAILURE);
		}
			
	}
	SDL_RenderPresent(pRenderer); // Affichage
	fprintf (journal, "OK\n");
}

void initSimulation() {
	/* Cette fonction permet d'initialiser les corps de la simulation */
	int i = 1;
	planete[0].coeffX = 0; // Initialisation du Soleil
	planete[0].exposantX = 13;
	planete[0].coeffY = 0;
	planete[0].exposantY = 13;
	planete[0].vitesseX = 0;
	planete[0].vitesseY = 0;
	planete[0].typeCorps = 0;
	planete[0].coeffMasse = 1.9891; // Masse du soleil
	planete[0].exposantMasse = 30;
	planete[0].idAstre = 0;

	for (i = 1; i < TAILLE_GLOBALE ; i++) { // Initialisation des autres planetes
		planete[i].coeffX = ((double)((rand() % 1999)+1)/100)-9; // Génération d'un nombre aléatoire entre 0 et 1 (double)
		planete[i].exposantX = ((rand() % 2) + 11);
		planete[i].coeffY = ((double)((rand() % 1999)+1)/100)-9; // Génération d'un nombre aléatoire entre 0 et 1 (double)
		planete[i].exposantY = ((rand() % 2) + 11);
		miseOrbite(&planete[i], &planete[0]);
		planete[i].typeCorps = 1;
		planete[i].coeffMasse = (double)((rand() % 999)+1)/100;
		planete[i].exposantMasse = ((rand() % 5) + 23);
		planete[i].idAstre = i;
	}
}
