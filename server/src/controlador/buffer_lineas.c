#include <stdlib.h>
#include <string.h>
#include "buffer_lineas.h"

struct BuuferLineas {
    char datos[BUFFER_LINEAS_CAPACIDAD];
    int longitud; //los bytres que son vlaidos ahora mismo 
};

BufferLineas *bufferr_lineas_crear(void) {
    BufferLineas *buffer = malloc(sizeof(BufferLineas));
    if(buffer == NULL){
        return NULL; 
    }
    buffer->longitud = 0; 
    return buffer; 
}

void buffer_lineas_destruir(BufferLineas *buffer) {
    free(buffer);
}

int buffer_lineas_agregar_datos(BufferLineas *buffer, const char *datos, int longitud){
    if(buffer->longitud + longitud > BUFFER_LINEAS_CAPACIDAD){
        return 0; //mensaje muy larg
    }
    memcpy(buffer->datos + buffer->longitud, datos, longitud);
    buffer-> longitud += longitud; 
    return 1; 
}

static void consumir_bytes(BufferLineas *buffer, int cantidad) {
    memmove(buffer->datos , buffer->datos + cantidad, buffer->longitud - cantidad);
    buffer->longitud -= cantidad; 
}

int buffer_lineas_obtener_siguiente(BufferLineas *buffer, char *linea, int tam_max) {
    while (1) {
        /* Bbsca el primer \n dentro de lo que ya tenemos acumulado */
        int indice_salto = -1;
        for (int i = 0; i < buffer->longitud; i++) {
            if (buffer->datos[i] == '\n') {
                indice_salto = i;
                break;
            }
        }
 
        if (indice_salto == -1) {
            return 0; /* todavia no hay ninguna linea completa */
        }
 
        int longitud_linea = indice_salto; /* sin contar el '\n' */
 
        if (longitud_linea == 0) {
            /* linea vacia ('\n' pegado a otro '\n' o al inicio):
               el protocolo dice que se debe IGNORAR, asi que la
               descartamos y seguimos buscando la siguiente */
            consumir_bytes(buffer, 1); /* solo el '\n' */
            continue;
        }
 
        int copiar = longitud_linea;
        if (copiar > tam_max - 1) {
            copiar = tam_max - 1; /* evitar desbordar 'linea' */
        }
        memcpy(linea, buffer->datos, copiar);
        linea[copiar] = '\0';
 
        consumir_bytes(buffer, longitud_linea + 1); /* +1 por el '\n' */
        return 1;
    }
}