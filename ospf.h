#ifndef OSPF_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void generaRuta(char * rutaTop, char * salidaRut);
void generaRuta2(char *rutaTop, char *salidaRut);
void ospf(int src_router, int nodos, int cost_matrix[nodos][nodos],
		  int links_matrix[nodos][nodos], FILE *archivo_rut);
int buscanodos(char rutaarchivo[100]);
#endif


