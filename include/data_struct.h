#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

typedef struct planete{
	//long poids;//il faut trouver une representation utilisable du poids (du 10^24 ca rentre pas dans les variables)
	int vitesse_x, vitesse_y;
	long position_x, position_y;
}Planete;