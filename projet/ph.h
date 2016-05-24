#ifndef PROJECT_HEADER_H
#define PROJECT_HEADER_H

typedef struct {
	int posx;
	int posy;
	float vitesse;
	int type_corps;
}corps;

int journal;

/* ferme le fichier, le rouvre vide et retourne le nouveu descripteur */
int remiseAZeroLog(int fd, int mode);
void *traiterRequete(void *arg);




#endif /* PROJECT_HEADER_H */
