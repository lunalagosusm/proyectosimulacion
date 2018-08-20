#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ospf.h"

#define MAX_NUMERO 4

void generaRuta(char *rutaTop, char *salidaRut){
    int i, j;
    FILE *archivo, *archivo_rut, *archivo_top;
    short int construyendo_numero = 0;
    int tipo_red = 0, nodos_line = 0, cont_num_line = 0, enlace = 0, enlaces = 0, fuente, destino, saltos;
    char caracteres[MAX_NUMERO];

    int nodos = buscanodos(rutaTop);
    int cost_matrix[nodos][nodos];
    int links_matrix[nodos][nodos];
    archivo = fopen(rutaTop,"r");
    archivo_rut = fopen(salidaRut,"w");
    archivo_top = fopen("../proyectosimulacion/Red_new.top","w");

    //Se inicia la matriz de pesos
    for(i = 0; i < nodos; i++){
        for(j = 0; j < nodos; j++){
            cost_matrix[i][j] = 100000;
        }
    }

    //Se inicia la matriz de enlaces
    for(i = 0; i < nodos; i++){
        for(j = 0; j < nodos; j++){
            links_matrix[i][j] = -1;
        }
    }

    if (archivo == NULL){
        printf("\nARCHIVO 1 VACIO\n\n");
        exit (1);
    }
    while (feof(archivo) == 0)
    {
        char caracter;
        caracter = fgetc(archivo);

        if (isdigit(caracter))
        {
            caracteres[i] = caracter;
            construyendo_numero = 1;
            i++;
        }

        else if (construyendo_numero)
        {
            caracteres[i] = '\0';
            i = 0;
            construyendo_numero = 0;

            if(nodos_line == 0){
                tipo_red = atoi(caracteres);
                printf("\nTipo de Red: %d\n", tipo_red);
                fprintf(archivo_top, "Tipo\tde\tRed:\t0\r\n");
            }
            else if(nodos_line == 1){
                nodos = atoi(caracteres);
                printf("Nodos: %d\n", nodos);
                fprintf(archivo_top, "Number\tof\tNodes:\t%d\r\n", nodos);
            }
            else if(nodos_line == 2){
                enlaces = atoi(caracteres);
                printf("Enlaces: %d\n\n", enlaces);
                fprintf(archivo_top, "Number\tof\tlinks:\t%d\r\n", enlaces/2);
                fprintf(archivo_top, "=========================================================================\r\n");
                fprintf(archivo_top, "source\t    dest.      hops       path       peso\t\r\n");
                fprintf(archivo_top, "=========================================================================");
            }
            else if(nodos_line > 5){
                cont_num_line++;
                if(cont_num_line == 1){         //Guardar Fuente
                    fuente = atoi(caracteres);
                }
                else if(cont_num_line == 2){    //Guardar Destino
                    destino = atoi(caracteres);
                }
                else if(cont_num_line == 3){    //Guardar Saltos
                    saltos = atoi(caracteres);
                }
                else if(cont_num_line == 4){    //Guardar Enlace
                    enlace = atoi(caracteres);
                }
            }
        }

        else if(caracter == '\n'){              //Se almacena el pero por enlace e ID's
            nodos_line++;
            cont_num_line = 0;
            if(nodos_line > 6){
                int random = rand() % (10-1+1) + 1;
                cost_matrix[fuente][destino] = random;
                cost_matrix[destino][fuente] = random;
                links_matrix[fuente][destino] = enlace/2;
                //links_matrix[destino][fuente] = enlace;
                printf("ID enlace %d a %d: %d (%d)\n", fuente, destino, links_matrix[fuente][destino],cost_matrix[fuente][destino]);
                fprintf(archivo_top, "\r\n%d\t%d\t1\t%d\t%d",fuente, destino, links_matrix[fuente][destino],cost_matrix[fuente][destino]);
            }
        }
    }
    fprintf(archivo_top, "\n");
    fclose(archivo_top);
    fclose(archivo);

    printf("\n Enter the no of routers: %d", nodos);
    printf("\n Enter the cost matrix values: ");
    for(i = 0; i < nodos; i++){
        for(j = 0; j < nodos; j++){
            printf("\n%d --> %d: %d", i, j, cost_matrix[i][j]);
        }
    }
/*
    printf("\n");
    printf("Rutas\tpor\tcnx.\t1\n");
    printf("Number\tof\tnodes:\t%d\n", nodos);
    printf("Number\tof\tlinks:\t%d\n", enlaces);
    printf("==================================================\n");
    printf("source\tdest.\thops\tpath\t(link\tids)\n");
    printf("==================================================");*/

    fprintf(archivo_rut, "Rutas\tpor\tcnx.\t1\n");
    fprintf(archivo_rut, "Number\tof\tnodes:\t%d\n", nodos);
    fprintf(archivo_rut, "Number\tof\tlinks:\t%d\n", enlaces/2);
    fprintf(archivo_rut, "==================================================\n");
    fprintf(archivo_rut, "source\tdest.\thops\tpath\t(link\tids)\n");
    fprintf(archivo_rut, "==================================================");

    for (int i = 0; i < nodos; i++){
        ospf(i, nodos, cost_matrix, links_matrix, archivo_rut);
    }
    printf("\n");
    fclose(archivo_rut);

}

void generaRuta2(char *rutaTop, char *salidaRut){
    int i, j, peso;
    FILE *archivo, *archivo_rut;
    short int construyendo_numero = 0;
    int tipo_red = 0, nodos_line = 0, cont_num_line = 0, enlace = 0, enlaces = 0, fuente, destino, saltos;
    char caracteres[MAX_NUMERO];

    int nodos = buscanodos(rutaTop);
    int cost_matrix[nodos][nodos];
    int links_matrix[nodos][nodos];
    archivo = fopen(rutaTop,"r");
    archivo_rut = fopen(salidaRut,"w");

    //Se inicia la matriz de pesos
    for(i = 0; i < nodos; i++){
        for(j = 0; j < nodos; j++){
            cost_matrix[i][j] = 100000;
        }
    }

    //Se inicia la matriz de enlaces
    for(i = 0; i < nodos; i++){
        for(j = 0; j < nodos; j++){
            links_matrix[i][j] = -1;
        }
    }

    if (archivo == NULL){
        printf("\nARCHIVO 1 VACIO\n\n");
        exit (1);
    }
    while (feof(archivo) == 0)
    {
        char caracter;
        caracter = fgetc(archivo);

        if (isdigit(caracter))
        {
            caracteres[i] = caracter;
            construyendo_numero = 1;
            i++;
        }

        else if (construyendo_numero)
        {
            caracteres[i] = '\0';
            i = 0;
            construyendo_numero = 0;

            if(nodos_line == 0){
                tipo_red = atoi(caracteres);
                printf("\nTipo de Red: %d\n", tipo_red);
            }
            else if(nodos_line == 1){
                nodos = atoi(caracteres);
                printf("Nodos: %d\n", nodos);
            }
            else if(nodos_line == 2){
                enlaces = atoi(caracteres);
                printf("Enlaces: %d\n\n", enlaces);
            }
            else if(nodos_line > 5){
                cont_num_line++;
                if(cont_num_line == 1){         //Guardar Fuente
                    fuente = atoi(caracteres);
                }
                else if(cont_num_line == 2){    //Guardar Destino
                    destino = atoi(caracteres);
                }
                else if(cont_num_line == 3){    //Guardar Saltos
                    saltos = atoi(caracteres);
                }
                else if(cont_num_line == 4){    //Guardar Enlace
                    enlace = atoi(caracteres);
                }
                else if(cont_num_line == 5){    //Guardar Peso
                    peso = atoi(caracteres);
                }
            }
        }

        else if(caracter == '\n'){              //Se almacena el pero por enlace e ID's
            nodos_line++;
            cont_num_line = 0;
            if(nodos_line > 6){
                cost_matrix[fuente][destino] = peso;
                cost_matrix[destino][fuente] = peso;
                links_matrix[fuente][destino] = enlace;
                printf("ID enlace %d a %d: %d (%d)\n", fuente, destino, links_matrix[fuente][destino],cost_matrix[fuente][destino]);
            }
        }
    }
    fclose(archivo);

    printf("\n Enter the no of routers: %d", nodos);
    printf("\n Enter the cost matrix values: ");
    for(i = 0; i < nodos; i++){
        for(j = 0; j < nodos; j++){
            printf("\n%d --> %d: %d", i, j, cost_matrix[i][j]);
        }
    }

    printf("\n");
    printf("Rutas\tpor\tcnx.\t1\n");
    printf("Number\tof\tnodes:\t%d\n", nodos);
    printf("Number\tof\tlinks:\t%d\n", enlaces);
    printf("==================================================\n");
    printf("source\tdest.\thops\tpath\t(link\tids)\n");
    printf("==================================================");

    fprintf(archivo_rut, "Rutas\tpor\tcnx.\t1\n");
    fprintf(archivo_rut, "Number\tof\tnodes:\t%d\n", nodos);
    fprintf(archivo_rut, "Number\tof\tlinks:\t%d\n", enlaces);
    fprintf(archivo_rut, "==================================================\n");
    fprintf(archivo_rut, "source\tdest.\thops\tpath\t(link\tids)\n");
    fprintf(archivo_rut, "==================================================");

    for (int i = 0; i < nodos; i++){
        ospf(i, nodos, cost_matrix, links_matrix, archivo_rut);
    }
    printf("\n");
    fclose(archivo_rut);

}

void ospf(int src_router, int nodos, int cost_matrix[nodos][nodos],int links_matrix[nodos][nodos], FILE *archivo_rut){
    int i, j, w, v, min, dist[100], last[100];
    int flag[100];
    int hops, id;
    int id_array[100];

    for(v = 0; v < nodos; v++){
        flag[v] = 0;
        last[v] = src_router;
        dist[v] = cost_matrix[src_router][v];
    }

    flag[src_router] = 1;

    for(i = 0; i < nodos; i++){
        min = 100000;
        for(w = 0; w < nodos; w++){
            if(!flag[w])
                if(dist[w] < min){
                    v = w;
                    min = dist[w];
                }
        }

        flag[v] = 1;

        for(w = 0; w < nodos; w++){
            if(!flag[w])
                if(min + cost_matrix[v][w] < dist[w]){
                    dist[w] = min + cost_matrix[v][w];
                    last[w] = v;
                }
        }
    }

    for(i = 0; i < nodos; i++){
        printf("\n%d\t%d\t", src_router, i);
        fprintf(archivo_rut, "\n%d\t%d\t", src_router, i);
        w = i;
        hops = 0;
        j = 0;
        while(w != src_router){
            id = links_matrix[last[w]][w];
            id_array[j] = id;
            j++;
            w = last[w];
            hops = hops + 1;
        }
        printf("%d\t", hops);
        fprintf(archivo_rut, "%d\t", hops);

        for(int k = 0; k < hops; k++){
            printf("%d\t", id_array[k]);
            fprintf(archivo_rut, "%d\t", id_array[k]);
        }
        printf("cost:%d", dist[i]);
    }
}

int buscanodos(char rutaarchivo[100]){
    FILE *archivotext1;
    int nodos_line = 0;
    int nodos;
    char caracteres[MAX_NUMERO];
    int i = 0;
    short int construyendo_numero = 0;
    archivotext1 = fopen(rutaarchivo, "r");

    if (archivotext1 == NULL){
        printf("\nARCHIVO VACIO\n\n");
        exit (1);
        }
    while (feof(archivotext1) == 0){
            char caracter;
            caracter = fgetc(archivotext1);
            if (isdigit(caracter))
            {
                caracteres[i] = caracter;
                construyendo_numero = 1;
                i++;
            }
            else if (construyendo_numero)
            {
                caracteres[i] = '\0';
                i = 0;
                construyendo_numero = 0;
                if(nodos_line == 1){
                    nodos = atoi(caracteres);
                    return nodos;
                }
            }
            else if(caracter == '\n'){
                nodos_line++;
            }
        }
    fclose(archivotext1);
    return 0;
}
