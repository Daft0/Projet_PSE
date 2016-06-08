#ifndef PROJECT_HEADER_H
#define PROJECT_HEADER_H

typedef struct {
	float coeffX;
	int exposantX;
	float coeffY;
	int exposantY;
	double vitesseX;
	double vitesseY;
	int typeCorps;
	float coeffMasse;
	int exposantMasse;
}corps;

/* Traite des requêtes */
void *traiterRequete(void *arg);

/* Affichage de la simulation */
void affichage();

/* Initialise les valeurs de la simulation */
void initSimulation();

/* Fonctions utilsées dans la simulation */
int minimum(int x, int y);
int signe(int x);
double calculTheta(double coeffX, int exposantX, double coeffY, int exposantY);
void projectionXY(double* vitesseX, double* vitesseY, double theta, double valeur, float coeffX, float coeffY);
void calculDistance(float * coeffResult, int * exposantResult, float coeffX, int exposantX, float coeffY, int exposantY);
void calculVecteur(float * coeffResult, int * exposantResult, float coeffDepart, int exposantDepart, float coeffArrivee, int exposantArrivee);
void deplacementCorps(corps * corpsCible, corps * listeCorps, int nbreCorps);
void miseOrbite(corps * corpsCible, corps * soleilSysteme);

#include "simulation.c"




#endif /* PROJECT_HEADER_H */
