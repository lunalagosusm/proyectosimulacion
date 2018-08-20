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
    int wea = link%2;

    if (link%2==0){ //si la linea es par o es 0
        linea1=link;
        linea2=link-1;
    }
    else{
        linea1=link+1;
        linea2=link;
    }

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
        if(atoi(ptr)==linea1 || atoi(ptr)==linea2){
            continue;
        }
        else
            printf("%s\n", caracteres);
            fputs(caracteres, archivo1);
        
    }

    fclose(archivo);
    fclose(archivo1);
}
