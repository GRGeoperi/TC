/*  Dependencias:
    Es necesario instalar la librería Jansson-2.14

    La guía de instalación:
    https://www.linuxfromscratch.org/blfs/view/svn/general/jansson.html
*/
/*  Compilación:
    gcc -o <nombreArchivo> <nombreArchivo>.c -ljansson
*/
/*  Testeo:
    El siguiente programa solo ha sido probado en Linux.
*/

#include "lectura_AFD.c"
#include "mensajes.c"

int main(void)
{
    char nombreArchivoJson[256];
    char cadena[256];
    char *estadoInicial;
    tupla estados;
    tupla alfabeto;
    tupla estadosAceptacion;
    tablaTransicion datosTransiciones;
    transicion *transiciones;
    quintupla afd;

    system("clear");
    mensaje_inicial();

    if (elegir_json(nombreArchivoJson) == 0)
    {
        estadoInicial = obtener_estado_inicial("estadoInicial", nombreArchivoJson);
        estados = obtener_conjuntos("estados", nombreArchivoJson);
        alfabeto = obtener_conjuntos("alfabeto", nombreArchivoJson);
        estadosAceptacion = obtener_conjuntos("estadosAceptacion", nombreArchivoJson);
        datosTransiciones = obtener_tabla_transiciones("transiciones", nombreArchivoJson);
        transiciones = obtener_transiciones(datosTransiciones);

        afd = crear_afd(estados, alfabeto, estadosAceptacion, estadoInicial, transiciones, datosTransiciones.numeroFilas);

        printf("Escriba su cadena: ");
        scanf("%255s", cadena);

        if(comprobar_cadena(afd.sigma, cadena) == strlen(cadena))
        {
            obtener_resultado(afd, cadena);
        }
        else
        {
            separadores_cortos();
            printf("\t\t». Caracteres invalidos.\n");
        }

        eliminar_todo(afd, datosTransiciones);
    }    
    return 0;
}
