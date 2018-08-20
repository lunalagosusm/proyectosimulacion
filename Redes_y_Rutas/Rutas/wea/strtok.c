

#include <stdio.h>
#include <string.h>

int main()
{
   char s1[49] = "Esto es un ejemplo para usar la funcion strtok()";
   char s2[4] = " \n\t";
   char *ptr;

   printf( "s1=%s\n", s1 );

   ptr = strtok( s1, s2 );    // Primera llamada => Primer token
   printf( "primera:%s\n", ptr );
   while( (ptr = strtok( NULL, s2 )) != NULL )    // Posteriores llamadas
     printf( "%s\n", ptr );

   return 0;
}
