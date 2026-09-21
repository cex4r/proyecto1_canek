#ifndef BUFFER_LINEAS_H
#define BUFFER_LINEAS_H

/*
    aqui voy a manejar la parte de leer lienas, en este caso
    el protocolo tcp ers el mejor ya que nos asegura que va a llegar completo el mensaje
    tambien se saltan lineas vacias ya que asi lo dice el canek-protocolo
*/
#define BUFFER_LINEAS_CAPACIDAD 4096

typedef struct BufferLineas BufferLineas;

BufferLineas *buffer_lineas_crear(void);
void buffer_lineas_destruir(BufferLineas *buffer);

/*
    agrega longitudf bytes recien leidos del socket, si el mensaje s muy larog
    se desconecta al cliente o se ve maniaco tambien se desconecta
*/
int buffer_lineas_agregar_datos(BufferLineas *buffer, const char *datos, int longitud);

int buffer_lineas_obtener_siguiente(BufferLineas *buffer, char *linea, int tam_max);

#endif