#include <stdlib.h>
#include <stdio.h>
#include <jansson.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

// Almacena "estados", "alfabeto" y "estadosAceptacion" del JSON
typedef struct Tupla
{
    char **conjunto;
    size_t cardinalidad;
}tupla;

// Almacena "transiciones" del JSON
typedef struct TablaTransicion
{
    char ***tabla;
    size_t numeroFilas;
    /* numeroColumnas?
        No se establece una variable para el número de columnas porque
        el mismo esquema JSON ya establece que siempre será de tres:
        ["estadoActual", "simbolo", "estadoSiguiente"]
    */
}tablaTransicion;

// Define cada transición almacenada
typedef struct Transicion
{
    char *estadoActual;
    char *simbolo;
    char *estadoSiguiente;
}transicion;

// Reune las transiciones definidas
typedef struct FuncionTransicion
{
    transicion *conjunto;
    size_t cardinalidad;
}funcionTransicion;

// Define un autómata finito determinista
typedef struct Quintupla
{
    tupla qoppa;
    tupla sigma;
    char *qoppaSubZero;
    funcionTransicion delta;
    tupla digamma;
}quintupla;

void separadores_cortos();
void separadores_largos();
void mensaje_inicial();

int listar_json();
int verificar_json(char NombreArchivo[256]);
int elegir_json(char *NombreArchivo);

char *obtener_estado_inicial(char *Objeto, char *NombreArchivo);
tupla obtener_conjuntos(char *Objeto, char *NombreArchivo);
tablaTransicion obtener_tabla_transiciones(char *Objeto, char *NombreArchivo);

transicion *obtener_transiciones(tablaTransicion TablaManejable);
char *obtener_estado_siguiente(funcionTransicion Delta, char *EstadoActual, char *Simbolo);
quintupla crear_afd(tupla Estados, tupla Alfabeto, tupla EstadosAceptacion, char *EstadoInicial, transicion *Transiciones, size_t CantidadTransiciones);

char *obtener_estado_final(quintupla Afd, char *Cadena);
size_t busqueda_bin_cadena(char** Conjunto, size_t Inicio, size_t Cardinalidad, char* Cadena);
void validar_estado_final(tupla Digamma, char *EstadoResultante);
void obtener_resultado(quintupla Afd, char *Cadena);

void eliminar_conjunto_transiciones(funcionTransicion Delta);
void eliminar_tabla_transiciones(tablaTransicion TablaManejable);
void eliminar_conjunto(tupla ArregloManejable);
void eliminar_todo(quintupla Afd, tablaTransicion TransicionesCrudas);

void imprimir_transiciones(tablaTransicion TablaManejable);
void imprimir_conjunto(tupla ArregloManejable);
