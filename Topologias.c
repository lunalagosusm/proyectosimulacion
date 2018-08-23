#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Topologias.h"

void crear_top(int link,const char *topologia,const char *texto){
    FILE *archivo;
    FILE *archivo1;

    archivo= fopen(topologia,"r");
    archivo1 = fopen(texto,"w");

    char *ptr1,*ptr2,*ptr3,*ptr4;
    char caracteres[100];
    char caracteres2[100];
    char temp[100];

    for (int i = 0; i < 6; i++)
    {
        fgets(caracteres,100,archivo);
        fputs(caracteres, archivo1);
    }

    while(fgets(caracteres,100,archivo) != NULL){
        strcpy(temp,caracteres);
        ptr1 = strtok(temp,"\t");
        ptr2 = strtok(NULL,"\t");
        ptr3 = strtok(NULL,"\t");
        ptr4 = strtok(NULL,"\t");
        if(atoi(ptr4)==link){
            fprintf(archivo1, "%s\t%s\t%s\t%s\t10000\r\n",ptr1,ptr2,ptr3,ptr4 );
        }
        else
            fputs(caracteres, archivo1);
        
    }

    fclose(archivo);
    fclose(archivo1);
}
