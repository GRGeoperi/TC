#include "mensajes.h"

void separadores_cortos()
{
    printf("════════════════════════════════════════════\n");
}

void separadores_largos()
{
    printf("════════════════════════════════════════════════════════════════════════════════════════\n");
}

void mensaje_inicial()
{
    printf("Descripcion:\n");
    separadores_largos();
    printf("El siguiente programa le permite visualizar archivos JSON que contengan un AFD,\n" 
            "escribir cual de ellos cargar, escribir la cadena que quiere evaluar y finalmente,\n"
            "comprobar si su cadena es valida o no.\n");
    separadores_largos();
    printf("Reglas:\n");
    separadores_largos();
    printf("1.- Debe tener en el directorio al menos un archivo que contenga un AFD.\n"
            "2.- El AFD debe tener el mismo esquema del ejemplo 'x0_AFD.json'.\n"
            "3.- Por un bug, no se permite introducir una cadena que contenga algun simbolo\n"
            "    que no este especificado en el alfabeto de su archivo.\n");
    separadores_largos();
    printf("Presione una tecla para continuar...");
    getchar();
    system("clear");
}
