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

    for (int i = 0; i < 6; i++)
    {
        fgets(caracteres,100,archivo);
        fputs(caracteres, archivo1);
    }

    while(fgets(caracteres,100,archivo) != NULL){
        strcpy(temp,caracteres);
        ptr = strtok(temp,"\t");
        ptr = strtok(NULL,"\t");
        ptr = strtok(NULL,"\t");
        ptr = strtok(NULL,"\t");
        if(atoi(ptr)==link){
            continue;
        }
        else
            //printf("%s\n", caracteres);
            fputs(caracteres, archivo1);
        
    }

    fclose(archivo);
    fclose(archivo1);
}
