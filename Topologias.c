#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Topologias.h"

void crear_top(int link,const char *topologia,const char *texto){

	FILE *archivo;
	FILE *archivo1;

	archivo= fopen(topologia,"r");
	archivo1 = fopen(texto,"w");

	char *ptr;
	char caracteres[100];
	char temp[100];

	int linea1;
	int linea2;

	if (link%2==0){ //si la linea es par o es 0
		linea1=link;
		linea2=link-1;
	}
	else{
		linea1=link+1;
		linea2=link;
	}

	while(fgets(caracteres,100,archivo) != NULL){
		strcpy(temp,caracteres);
		ptr = strtok(temp,"\t");
		if (strlen(ptr)>40){//============
			fputs(caracteres, archivo1);
		}
		else{
			ptr = strtok(NULL,"\t");
			ptr = strtok(NULL,"\t");
			ptr = strtok(NULL,"\t");
	 		if(atoi(ptr+2)==linea1 || atoi(ptr+2)==linea2){
	 			continue;
	 		}
	 		else
	 			fputs(caracteres, archivo1);
		}
	}

	fclose(archivo);
	fclose(archivo1);
}

