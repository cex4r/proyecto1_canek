#ifndef CLIENTE_HANDLER_H
#define CLIENTE_HANDLER_H

#include "modelo/servidor.h"

/*
    Argumento que le pasa a cada hilo, se reserva con malloc en el hilo principal, 
    antes de crear el ciclo, lo que hace que cada hilo tenga su propia copia y me 
    evito el problema de que pasen casos chistosos 
*/

typedef struct {
    Servidor *servidor;
    int socket_cliente; 
} ArgumentoCliente; 

/*
    esta funcion es muy importante, es una funcion que corre en un hilo, dedicado por 
    cada cliente conectado.
    se encarga de:
        -leer bytes del socket y usar BufferLineas para separarlos en mensajes completos
        -Parsear cada mensaje con MensajeJson,
        -Aplicar regla IDENTIFY del protocolo, por ahora responde solo con ese
        cualquier otra cosa responde invalid 
        -Limpia todo, es decir, cierra el socket, quita al usuario del servidor y asi
*/

void *atender_cliente(void *arg_void);

#endif