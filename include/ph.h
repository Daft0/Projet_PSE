#ifndef PROJECT_HEADER_H
#define PROJECT_HEADER_H

typedef struct {
	int posx;
	int posy;
	float vitessex;
	float vitessey;
	int typeCorps;
	float coefMasse;
	int expMasse;
	float coefDistance;
	int expDistance;
}corps;

int journal;

/* ferme le fichier, le rouvre vide et retourne le nouveu descripteur */
int remiseAZeroLog(int fd, int mode);

/* Traite des requêtes */
void *traiterRequete(void *arg);

/* Affichage de la simulation */
void affichage();

/* Initialise les valeurs de la simulation */
void initSimulation();




#endif /* PROJECT_HEADER_H */
