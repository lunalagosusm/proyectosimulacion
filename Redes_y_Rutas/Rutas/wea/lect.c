#include <stdlib.h>
#include <stdio.h> 
#include <math.h>


int main(){
	FILE *fp;

	char file1 [] = "ArpaNet.rut";
	char file2 [] = "EON.rut";
	char file3 [] = "EuroCore.rut";
	char file4 [] = "EuroLarge.rut";
	char file5 [] = "NSFNet.rut";
	char file6 [] = "UKNet.rut"; 
	
	fp = fopen(file5, "r");
	char linea[100];
	char num[3];
	int datosRed[3];
	int i, j,k;
	for(i = 0; i<3; i++){
		fgets(linea, 100,fp);
		for(j=0;j<100; j++){
			if( linea[j] >= 48 && linea[j] <= 57){
				num[0] = linea[j];
				num[1] = linea[j+1];
				num[2] = linea[j+2];
				j = j+2;
				datosRed[i] =atoi(num);
				break;
			}	
		}
		printf("%i\n", datosRed[i]);
	}

	fgets(linea, 100,fp);
	fgets(linea, 100,fp);
	fgets(linea, 100,fp);
	printf("%s\n",linea);

	int N;
	N = datosRed[1];
	int datosRutas[N*(N)][3];
	j = 0;
	k = 0;
	while( feof(fp) == 0 ){
		fgets(linea, 100,fp);

		for(i=0;i<100; i++){
			if((linea[i]>=48)&&(linea[i]<=57)){
				num[0] = linea[i];
				//++i;
				if ((linea[i]>=48)&&(linea[i]<=57))
				{
					num[1] = linea[i+1];
					++i;
					if ((linea[i]>=48)&&(linea[i]<=57))
					{
						num[2] = linea[i+1];
						++i;
						//printf("hola estoy aki\n");
					}
					//else printf("%s\n", "entro 3");
				}
				//else printf("%s\n", "entro 2");
			}
			//else printf("entro 1\n");

			if(k == 2) {
				datosRutas[j][k] = atoi(num);
				++j;
				k=0;
				break;
			}
			//else if(k>=3 &&)
			else{
				datosRutas[j][k] = atoi(num);
				++k;
			}
		}
	}
	printf("while 1\n");
	rewind(fp);
	int ** mat;
	int f,c;

	for (i = 0; i < 6; ++i)
	{
		fgets(linea, 100,fp);
	}
	printf("%s\n", linea);
	j = 0;
	k =0;

	f = N*N;//sera igual N*N
	mat = (int **) malloc(f*sizeof(int*));

	for (i = 0; i < f; ++i)
	{
		c = datosRutas[f][2];
		if(c == 0){
			c = 1;
			mat[i] = (int *)malloc(c*sizeof(int));	
		}
		else mat[i] = (int *)malloc(c*sizeof(int));
	}		
	j=0;
	k=0;
	int countab;
	countab = 0;
	printf("crea matriz\n");

	while( feof(fp) == 0 ){
		fgets(linea, 100,fp);
		printf("entra while 2\n");
		for(i=0;i<100; i++){
			if (countab < 3)
			{
				if (linea[i] == 9)
				{
					++countab;
				}
				else countab = countab;
			}
			else
			{
				if(linea[i] == 10){
					break;
				}
				else{
					printf("entro else\n");
					printf("j,k : %i,%i\n", j,k);
					printf("%s\n", linea);
					if((linea[i]>=48)&&(linea[i]<=57)){
						num[0] = linea[i];
						if((linea[i+1]>=48)&&(linea[i+1]<=57))
						{
							++i;
							num[1] = linea[i];
							if ((linea[i+1]>=48)&&(linea[i+1]<=57))
							{
								++i;
								num[2] = linea[i];
							}
							else{num[2]=10;}
						}
						else{num[1]=10;num[2]=10;}
						printf("datosRutas[%i][2]: %i\n",j,datosRutas[j][2]);
						if (datosRutas[j][2] == 0)
						{
							printf("j,k : %i,%i\n", j,k);
							printf("num[0]: %c\n",num[0]);
							printf("num[1]: %c\n",num[1]);
							printf("num[2]: %c\n",num[2]);
							mat[j][k] = 0;	
						}
						else{
							printf("j,k : %i,%i\n", j,k);
							printf("num[0]: %c\n",num[0]);
							printf("num[1]: %c\n",num[1]);
							printf("num[2]: %c\n",num[2]);
							printf("atoi(num): %i\n", atoi(num));
							mat[j][k] = atoi(num);
						}
						++k;
					}
					else{num[0]=10;num[1]=10;num[2]=10;}
				}
			}		
		}
		printf("sale for\n");
		++j;
		countab=0;
		k=0;
	}
	for ( j = 0; j < (N*N); ++j)
	{
		printf("%i ",datosRutas[j][2]);

	}
	printf("Ahora viene el path\n");
	for ( j = 0; j < (N*N); ++j)
	{
		for (k = 0; k < (datosRutas[j][2]); ++k)
		{
			printf("%i\t", mat[j][k]);
		}
		printf("\n");
	}

	fclose(fp);
} 