#include "lectura_AFD.h"
#include "mensajes.h"

/*
    Muestra archivos JSON del directorio actual,
    regresa 0 si existe AL MENOS un archivo con esa extensión.
*/
int listar_json()
{
    DIR *directorioActual = opendir(".");
    struct dirent *archivos;
    int existencia = 0;

    if (directorioActual == NULL)
    {
        perror("\t\t«. Error al abrir el directorio.");
        exit(1);
    }

    printf("Sus opciones son:\n");
    separadores_cortos();
    // Lee cada archivo del directorio y unicamente muestra los que tengan extensión .json
    while ((archivos = readdir(directorioActual)) != NULL)
    {
        if (strstr(archivos->d_name, ".json") != NULL)
        {
            printf("%d.- %s\n", existencia + 1, archivos->d_name);
            existencia++;
        }
    }
    closedir(directorioActual);
    free(archivos);
    if (existencia != 0)
    {
        return 0;
    }
    else
    {
        perror("\t\t«. No se encontraron archivos JSON.\n");
        exit(1);
    }
}

/*
    Verifica que el archivo JSON en el argumento exista en el directorio actual,
    regresa 0 si el archivo tiene esa extensión y además se puede acceder a él.
*/
int verificar_json(char NombreArchivo[256])
{
    if (strstr(NombreArchivo, ".json") != NULL && access(NombreArchivo, F_OK) == 0)
    {
        separadores_cortos();
        printf("\t\t». Su archivo se abrio.\n");
        separadores_cortos();
        return 0;
    }
    else
    {
        separadores_cortos();
        printf("\t\t«. Intente de nuevo.\n");
        return 1;
    }
}

/*
    Pide el nombre del archivo JSON que se quiere cargar y verifica su existencia, 
    regresa 0 únicamente cuando se compruebe que el usuario lo ingresó correctamente.
*/
int elegir_json(char *NombreArchivo)
{
    int coincide;
    if (listar_json() == 0)
    {
        do
        {
            separadores_cortos();
            printf("Escriba el AFD a cargar: ");
            scanf("%255s", NombreArchivo);
            coincide = verificar_json(NombreArchivo);
            if (coincide == 1)
            {
                listar_json();
            }
        } while (coincide == 1);
        return 0;
    }
}

/*
    Extrae exlusivamente el "estadoInicial" del archivo JSON,
    regresa la cadena correspondiente.
*/
char *obtener_estado_inicial(char *Objeto, char *NombreArchivo)
{
    json_t *repositorio;
    json_error_t error;

    json_t *datoCrudo;
    const char *datoManejable;
    char *estadoInicial;

    repositorio = json_load_file(NombreArchivo, 0, &error);
    datoCrudo = json_object_get(repositorio, Objeto);
    datoManejable = json_string_value(datoCrudo);

    estadoInicial = (char*)malloc(sizeof(char) * json_string_length(datoCrudo));

    estadoInicial = strdup(datoManejable);

    json_decref(datoCrudo);
    json_decref(repositorio);

    return estadoInicial;
}

/*
    Extrae un determinado arreglo unidimensional del archivo JSON,
    regresa la tupla correspondiente.
*/
tupla obtener_conjuntos(char *Objeto, char *NombreArchivo)
{
    json_t *repositorio;
    json_error_t error;

    json_t *arregloCrudo;
    tupla arregloManejable;

    repositorio = json_load_file(NombreArchivo, 0, &error);
    arregloCrudo = json_object_get(repositorio, Objeto);

    arregloManejable.cardinalidad = json_array_size(arregloCrudo);
    arregloManejable.conjunto = (char**)malloc(sizeof(char*) * arregloManejable.cardinalidad);

    for (size_t i = 0; i < arregloManejable.cardinalidad; i++)
    {
        json_t *datoCrudo;
        const char *datoManejable;

        datoCrudo = json_array_get(arregloCrudo, i);
        datoManejable = json_string_value(datoCrudo);
        arregloManejable.conjunto[i] = strdup(datoManejable);

        json_decref(datoCrudo);
    }

    json_decref(arregloCrudo);
    json_decref(repositorio);

    return arregloManejable;
}

/*
    Extrae exlusivamente el arreglo bidimensional de "transiciones" del archivo JSON,
    regresa la tabla correspondiente.
*/
tablaTransicion obtener_tabla_transiciones(char *Objeto, char *NombreArchivo)
{
    json_t *repositorio;
    json_error_t error;

    json_t *tablaCruda;
    tablaTransicion tablaManejable;

    repositorio = json_load_file(NombreArchivo, 0, &error);
    tablaCruda = json_object_get(repositorio, Objeto);

    tablaManejable.numeroFilas = json_array_size(tablaCruda);
    tablaManejable.tabla = (char***)malloc(sizeof(char**) * tablaManejable.numeroFilas);

    for (size_t i = 0; i < tablaManejable.numeroFilas; i++)
    {
        json_t *arregloCrudo = json_array_get(tablaCruda, i);
        char **arregloManejable = (char**)malloc(sizeof(char *) * 3);
        for (size_t j = 0; j < 3; j++)
        {
            json_t *datoCrudo;
            const char *datoManejable;

            datoCrudo = json_array_get(arregloCrudo, j);
            datoManejable = json_string_value(datoCrudo);
            arregloManejable[j] = strdup(datoManejable);
            
            json_decref(datoCrudo);
        }
        tablaManejable.tabla[i] = arregloManejable;
        json_decref(arregloCrudo);
    }

    json_decref(tablaCruda);
    json_decref(repositorio);
    
    return tablaManejable;
}

/*
    Extrae cada transición de la tabla y las define por su evento y su acción enlazada,
    regresa el conjunto de transiciones.
*/
transicion *obtener_transiciones(tablaTransicion TablaManejable)
{
    transicion *transiciones;
    transiciones = malloc(sizeof(transicion) * TablaManejable.numeroFilas);

    for(size_t i = 0; i < TablaManejable.numeroFilas; i++)
    {
        transiciones[i].estadoActual = TablaManejable.tabla[i][0];
        transiciones[i].simbolo = TablaManejable.tabla[i][1];
        transiciones[i].estadoSiguiente = TablaManejable.tabla[i][2];
    }

    return transiciones;
}

/*
    Busca la acción enlazada apartir de su evento dentro de las transiciones definidas,
    regresa esa acción si el evento existe.
*/
char *obtener_estado_siguiente(funcionTransicion Delta, char *EstadoActual, char *Simbolo)
{
    for (size_t i = 0; i < Delta.cardinalidad; i++)
    {
        if (strcmp(Delta.conjunto[i].estadoActual, EstadoActual) == 0 && strcmp(Delta.conjunto[i].simbolo, Simbolo) == 0)
        {
            return Delta.conjunto[i].estadoSiguiente;
        }
    }
    return NULL;
}

/*
    Crea el autómata finito determinista apartir de los datos cargados del archivo JSON,
    regresa la quintupla correspondiente.
*/
quintupla crear_afd(tupla Estados, tupla Alfabeto, tupla EstadosAceptacion, char *EstadoInicial, transicion *Transiciones, size_t CantidadTransiciones)
{
    quintupla afd;
    afd.qoppa = Estados;
    afd.sigma = Alfabeto;
    afd.qoppaSubZero = EstadoInicial;
    afd.digamma = EstadosAceptacion;
    afd.delta.conjunto = Transiciones;
    afd.delta.cardinalidad = CantidadTransiciones;
    return afd;
}

/*
    Recorre todos los simbolos de la cadena de acuerdo a su acción enlazada,
    regresa la última acción conseguida.
*/
char *obtener_estado_final(quintupla Afd, char *Cadena)
{
    char *estadoActual = Afd.qoppaSubZero;
    size_t longitudCadena = strlen(Cadena);
    char *estadoFinal;

    for (size_t i = 0; i < longitudCadena; i++)
    {
        char simboloActual[2] = {Cadena[i], '\0'};
        estadoFinal = obtener_estado_siguiente(Afd.delta, estadoActual, simboloActual);
        if (estadoFinal != NULL)
        {
            estadoActual = estadoFinal;
        }
        else
        {
            return NULL;
        }
    }
    return estadoActual;
}

/*  
    Implementación de búsqueda binaria.
    Busca una cadena dentro de un conjunto de cadenas,
    si la cadena existe, regresa el índice donde se encuentra. 
*/
size_t busqueda_bin_cadena(char** Conjunto, size_t Inicio, size_t Cardinalidad, char* Cadena)
{
    if (Inicio <= Cardinalidad)
    {
        size_t medio = Inicio + (Cardinalidad - Inicio) / 2;
        // Compara la cadena con el elemento en el medio
        int comparacion = strcmp(Conjunto[medio], Cadena);
        // Si la cadena es igual al elemento en el medio, devolvemos el índice
        if (comparacion == 0)
        {
            return medio;
        }
        // Si la cadena es menor, buscamos en la mitad izquierda
        else if (comparacion > 0)
        {
            return busqueda_bin_cadena(Conjunto, Inicio, medio - 1, Cadena);
        }
        // Si la cadena es mayor, buscamos en la mitad derecha
        else
        {
            return busqueda_bin_cadena(Conjunto, medio + 1, Cardinalidad, Cadena);
        }
    }
    // Si la cadena no se encuentra en el arreglo, devolvemos -1
    return -1;
}

/*
    Busca la última acción conseguida en el conjunto de acciones aceptables
    para determinar si la cadena es aceptada o rechazada.
*/
void validar_estado_final(tupla Digamma, char *EstadoResultante)
{
    size_t indiceDigamma = -1;
    int comparacion;

    if (Digamma.cardinalidad > 1)
    {
        indiceDigamma = busqueda_bin_cadena(Digamma.conjunto, 0, Digamma.cardinalidad, EstadoResultante);
    }
    else
    {
        comparacion = strcmp(EstadoResultante, Digamma.conjunto[0]);
    }

    if (indiceDigamma != -1 || comparacion == 0)
    {
        separadores_cortos();
        printf("Su cadena es aceptada por el AFD.\n");
    }
    else
    {
        separadores_cortos();
        printf("Su cadena es rechazada por el AFD.\n");
    }
}

/*
    Procesa toda la cadena, obtiene su estado final
    y muestra el resultado asociado a esa cadena.
*/
void obtener_resultado(quintupla Afd, char *Cadena)
{
    char *estadoFinal;
    estadoFinal = obtener_estado_final(Afd, Cadena);
    validar_estado_final(Afd.digamma, estadoFinal);
    free(estadoFinal);
}

// Libera la memoria asociada al conjunto de transiciones.
void eliminar_conjunto_transiciones(funcionTransicion Delta)
{
    for (size_t i = 0; i < Delta.cardinalidad; i++)
    {
        free(Delta.conjunto[i].estadoActual);
        free(Delta.conjunto[i].simbolo);
        free(Delta.conjunto[i].estadoSiguiente);
    }
    free(Delta.conjunto);
}

// Libera la memoria asociada a la tabla de transiciones.
void eliminar_tabla_transiciones(tablaTransicion TablaManejable)
{
    for(size_t i = 0; i < TablaManejable.numeroFilas; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            free(TablaManejable.tabla[i][j]);
        }
        free(TablaManejable.tabla[i]);
    }
    free(TablaManejable.tabla);
}

// Libera la memoria asociada a las tuplas correspondientes.
void eliminar_conjunto(tupla ArregloManejable)
{
    for(size_t i = 0; i < ArregloManejable.cardinalidad; i++)
    {
        free(ArregloManejable.conjunto[i]);
    }
    free(ArregloManejable.conjunto);
}

// Libera la memoria usada por el autómata finito determinista y su tabla asociada.
void eliminar_todo(quintupla Afd, tablaTransicion TablaManejable)
{
    free(Afd.qoppaSubZero);
    eliminar_conjunto(Afd.qoppa);
    eliminar_conjunto(Afd.sigma);
    eliminar_conjunto(Afd.digamma);
    eliminar_tabla_transiciones(TablaManejable);
    eliminar_conjunto_transiciones(Afd.delta);
}

// Imprime la tabla de transiciones.
void imprimir_transiciones(tablaTransicion TablaManejable)
{
    printf("\t\tTabla de transiciones\n");
    for(size_t i = 0; i < TablaManejable.numeroFilas; i++)
    {
        printf("\tTransicion %ld\n\n", i);
        printf("Estado actual: %s\n", TablaManejable.tabla[i][0]);
        printf("Simbolo: %s\n", TablaManejable.tabla[i][1]);
        printf("Estado resultante: %s\n\n", TablaManejable.tabla[i][2]);
    }
}

// Imprime las tuplas correspondientes.
void imprimir_conjunto(tupla ArregloManejable)
{
    printf("Conjunto\n");
    for(size_t i = 0; i < ArregloManejable.cardinalidad; i++)
    {
        printf("%s\n", ArregloManejable.conjunto[i]);
    }
}
