#include <stdlib.h>
#include "lcgrand.h"
#include "scheduler.h"
#include <stdio.h> 
#include <math.h>

int main(){
	int i;
	int seed[100];
	float unif;
	long zrng;
	int contador=0;
	int contador2=0;
	float numero_2=0;

	/*for (int i = 0; i < 100; i++)
	{
		float numero = lcgrand(i);
		for (int j = 0; j < 100; j++)
		{
			float numero_2 = lcgrand(j);
			if (numero_2==numero)
			{
				contador++;
			}
			else
				contador2++;
		}
		
	}*/

	float numero = lcgrand(0);
	printf("%f\n",numero );
	for (int i = 0; i < 5; ++i)
	{
		float numero_2 = lcgrand(0);
		printf("%f\n",numero_2 );
	}
	printf("%f\n",numero );
	for (int i = 0; i < 7; ++i)
	{
		float numero_3 = lcgrand(0);
		printf("%f\n",numero_3 );
	}
	for (int i = 0; i < 10; ++i)
	{
		float numero_4 = lcgrand(0);
		printf("%f\n",numero_4 );
	}

	/*float numero=lcgrand(91);
	//printf("numero:%f\n",numero);
	for (int i = 1; i >= 1; ++i)
	{
		float numero_2 = lcgrand(1);
		//printf("numero:%f\n",numero);
		if (numero_2==numero)
		{
			//printf("stop:\n");
			break;
		}
		else{
			contador++;
		}

		
	}*/
	//printf("%i\n",contador);
	printf("numeros iguales:%i\n",contador );
	printf("numeros no iguales:%i\n",contador2 );
	

}