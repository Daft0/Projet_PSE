#include "pse.h"
#include "ph.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef SIMULATION
#define Simulation
double const COEFF_CST_GRAVITATION = 6.67384;
int const EXP_CST_GRAVITATION = -11;
#define PAS_TEMPS 3600*24 //definit le pas de temps de la simulation
#endif

int minimum(int x, int y)
{
	if (x<y)
		return x;
	else 
		return y;
}

int maximum(int x, int y)
{
	if (x>y)
		return x;
	else 
		return y;
}

int signe(int x)
{
	if (x<0)
		return -1;
	else
		return 1;
}

double calculTheta(double coeffX, int exposantX, double coeffY, int exposantY)//calcule le theta reliant les coordonnees cartesiennes et polaires, entre le soleil et l'element stellaire
{
	double theta = 0, coeffTemp = 0;
	int exposantTempA = 0, exposantTempB = 0;
	
	exposantTempA = maximum(exposantX, exposantY);
	if (exposantX == exposantTempA)
	{
		exposantTempB = 2*exposantY - 2*exposantTempA;
		coeffTemp = pow(coeffX, 2) + pow(coeffY, 2) * pow(10, exposantTempB);
	}
	else 
	{
		exposantTempB = 2*exposantX - 2*exposantTempA;
		coeffTemp = pow(coeffX, 2) * pow(10, -2*exposantTempA) + pow(coeffY, 2);
	}
	coeffTemp = sqrt(coeffTemp);
	printf("Calcul du theta : %lf, %lf, %lf\n", coeffX, coeffTemp, (coeffX / coeffTemp) * pow(10, exposantX - exposantTempA));
	theta = fabs(acos( (coeffX / coeffTemp) * pow(10, exposantX - exposantTempA)));
	if (coeffY < 0)
		theta = -theta;
	//theta = theta * signe( asin ( (coeffY / coeffTemp) * pow(10, exposantY - exposantTempA) ));
	
	return theta;
}

void projectionXY(double* vitesseX, double* vitesseY, double theta, double valeur, double coeffX, double coeffY)//Calcule la projection de la valeur sur le corps
{
	
	*vitesseX = valeur * cos( M_PI/2 + theta );
	*vitesseY = valeur * sin( M_PI/2 + theta );
	printf("Valeur du cos : %lf\nValeur du sin : %lf\n", cos( M_PI/2 + theta ), sin( M_PI/2 + theta ));
	printf("Valeur rentree : %f, %f\n", M_PI/2, theta);
	
	/* Test a la main
	if ( coeffX >= 0 && coeffY >= 0 )
	{
		*vitesseX = vitesse * cos( M_PI/2 + theta );
		*vitesseY = vitesse * sin( M_PI/2 + theta );
	}
	if ( coeffX <= 0 && coeffY >= 0)
	{
		*vitesseX = vitesse * cos( M_PI/2 + theta );
		*vitesseY = vitesse * sin( M_PI/2 + theta );
	}
	if ( coeffX <= 0 && coeffY <= 0)
	{
		*vitesseX = vitesse * cos( M_PI/2 + theta );
		*vitesseY = vitesse * sin( M_PI/2 + theta );
	}
	if ( coeffX >= 0 && coeffY <= 0)
	{
		*vitesseX = vitesse * cos( M_PI/2 + theta );
		*vitesseY = vitesse * sin( M_PI/2 + theta );
	}
	*/
}



void calculDistance(double * coeffResult, int * exposantResult, double coeffX, int exposantX, double coeffY, int exposantY)
{
	int exposantTempA = 0, exposantTempB = 0;
	exposantTempA = minimum(exposantX, exposantY);
	if (exposantX == exposantTempA)
	{
		exposantTempB = exposantY - exposantTempA;
		*coeffResult = pow(coeffY, 2) + pow(coeffX, 2) * pow(10, -2*exposantTempA);
	}
	else 
	{
		exposantTempB = exposantX - exposantTempA;
		*coeffResult = pow(coeffX, 2) + pow(coeffY, 2) * pow(10, -2*exposantTempA);
	}
	*exposantResult = exposantTempA + exposantTempB;
	*coeffResult = sqrt(*coeffResult);
}

void calculVecteur(double * coeffResult, int * exposantResult, double coeffDepart, int exposantDepart, double coeffArrivee, int exposantArrivee)//calcule le vecteur sur une direction (pex X)
{
	int exposantTempA = 0, exposantTempB = 0;
	exposantTempA = maximum(exposantDepart, exposantArrivee);
	if (exposantDepart == exposantTempA)
	{
		exposantTempB = exposantArrivee - exposantTempA;
		*coeffResult = coeffArrivee * pow(10, exposantTempB) - exposantDepart;
	}
	else 
	{
		exposantTempB = exposantDepart - exposantTempA;
		*coeffResult = coeffArrivee - exposantDepart * pow(10, exposantTempB);
	}
	*exposantResult = exposantTempA + exposantTempB;
}

void deplacementCorps(corps * corpsCible, corps * listeCorps, int nbreCorps)
{
	double accelerationX = 0, accelerationY = 0, theta = 0, accelerationTemp = 0, accelerationXTot = 0, accelerationYTot = 0;
	double coeffDistanceTemp = 0;
	int i =0, exposantXTemp = 0, exposantYTemp = 0, exposantDistanceTemp = 0;
	double coeffXTemp = 0, coeffYTemp = 0;
	for (i = 0; i < nbreCorps; i++)
	{	
		printf("Identifiants : %d VS %d\n", i, corpsCible->idAstre);
		if (i != corpsCible->idAstre)
		{
			accelerationTemp = 0;
			calculVecteur(&coeffXTemp, &exposantXTemp, corpsCible->coeffX, corpsCible->exposantX, listeCorps[i].coeffX, listeCorps[i].exposantX);
			printf("Calcul vecteurX : %lf : %d\n", coeffXTemp, exposantXTemp);
			calculVecteur(&coeffYTemp, &exposantYTemp, corpsCible->coeffY, corpsCible->exposantY, listeCorps[i].coeffY, listeCorps[i].exposantY);
			printf("Calcul vecteurX : %lf : %d\n", coeffYTemp, exposantYTemp);
			theta = calculTheta(coeffXTemp, exposantXTemp, coeffYTemp, exposantYTemp);
			calculDistance(&coeffDistanceTemp, &exposantDistanceTemp, coeffXTemp, exposantXTemp, coeffYTemp, exposantYTemp);
			printf("Acceleration totale : %lf : %lf\n", accelerationXTot, accelerationYTot);
			printf("Distance : %lf : %d\n", coeffDistanceTemp, exposantDistanceTemp);
			accelerationTemp = COEFF_CST_GRAVITATION * listeCorps[i].coeffMasse / pow(coeffDistanceTemp, 2);
			printf("Acceleration sans exposant : %lf\n", accelerationTemp);
			accelerationTemp = accelerationTemp * pow(10, EXP_CST_GRAVITATION + listeCorps[i].exposantMasse - 2 * exposantDistanceTemp);
			printf("Acceleration avec exposant : %lf apres coeff de %d\n",accelerationTemp, EXP_CST_GRAVITATION + listeCorps[i].exposantMasse - 2 * exposantDistanceTemp);
			projectionXY(&accelerationX, &accelerationY, theta, accelerationTemp, coeffXTemp, coeffYTemp);
			printf("Acceleration depart : %lf : %lf\n", accelerationX, accelerationY);
			accelerationXTot += accelerationX;
			accelerationYTot += accelerationY;
		}
	}
	printf("La vitesse change de %.20lf, %.20lf\n", accelerationXTot * PAS_TEMPS * pow(10, -corpsCible->exposantX), accelerationYTot * PAS_TEMPS * pow(10, -corpsCible->exposantY));
	corpsCible->coeffX = corpsCible->coeffX + accelerationXTot * PAS_TEMPS * pow(10, -corpsCible->exposantX + 5 );
	corpsCible->coeffY = corpsCible->coeffY + accelerationYTot * PAS_TEMPS * pow(10, -corpsCible->exposantY + 5 );
}

void miseOrbite(corps * corpsCible, corps * soleilSysteme)
{
	double vitesseIni = 0;
	double coeffDistanceTemp = 0, coeffXTemp = 0, coeffYTemp = 0;
	int exposantDistanceTemp = 0, exposantXTemp = 0, exposantYTemp = 0, theta = 0;
	calculVecteur(&coeffXTemp, &exposantXTemp, corpsCible->coeffX, corpsCible->exposantX, soleilSysteme->coeffX, soleilSysteme->exposantX);
	calculVecteur(&coeffYTemp, &exposantYTemp, corpsCible->coeffY, corpsCible->exposantY, soleilSysteme->coeffY, soleilSysteme->exposantY);
	calculDistance(&coeffDistanceTemp, &exposantDistanceTemp, coeffXTemp, exposantXTemp, coeffYTemp, exposantYTemp);
	vitesseIni = COEFF_CST_GRAVITATION * soleilSysteme->coeffMasse / coeffDistanceTemp;
	vitesseIni = vitesseIni * pow(10, EXP_CST_GRAVITATION + soleilSysteme->exposantMasse - exposantDistanceTemp);
	theta = calculTheta(coeffXTemp, exposantXTemp, coeffYTemp, exposantYTemp);
	projectionXY(&(corpsCible->vitesseX), &(corpsCible->vitesseY), theta, vitesseIni, coeffXTemp, coeffYTemp);
}

/*
int main(void)
{
	double vitesse = 256, vitesseX = 0, vitesseY = 0;
	double theta = 0;
	
	theta = calculTheta(1, 0, 1, 0);
	projectionXY(&vitesseX, &vitesseY, theta, vitesse, 1, 10);
	printf("Pos 1 10 %f %f %f\n", theta, vitesseX, vitesseY);
	
	theta = calculTheta(-1, 0, 1, 0);
	projectionXY(&vitesseX, &vitesseY, theta, vitesse, -1, 1);
	printf("Pos -1 10 %f %f %f\n", theta, vitesseX, vitesseY);
	
	theta = calculTheta(-1, 0, -1, 0);
	projectionXY(&vitesseX, &vitesseY, theta, vitesse, -1, -1);
	printf("Pos -10 -1 : %f %f %f\n", theta, vitesseX, vitesseY);
	
	theta = calculTheta(1, 0, -1, 0);
	projectionXY(&vitesseX, &vitesseY, theta, vitesse, 1, -1);
	printf("Pos 10 -1 %f %f %f\n", theta, vitesseX, vitesseY);
	
}
*/
