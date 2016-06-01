#ifndef PROJECT_HEADER_H
#define PROJECT_HEADER_H

typedef struct {
	int posx;
	int posy;
	float vitesse;
	int type_corps;
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

/* Initialise la simulation */
void init_Simulation();




#endif /* PROJECT_HEADER_H */
