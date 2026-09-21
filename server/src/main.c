#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "modelo/servidor.h"
#include "controlador/cliente_handler.h"
 
#define PUERTO 5000
#define TAMANO_COLA_ESPERA 10 /* backlog de listen(): conexiones ya
                                  llegadas que esperan su accept() */
 
int main(void) {
    Servidor *servidor = servidor_crear();
    if (servidor == NULL) {
        fprintf(stderr, "No se pudo crear el servidor\n");
        return 1;
    }
 
    int socket_escucha = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_escucha < 0) {
        perror("socket");
        return 1;
    }
 
    int reuse = 1;
    setsockopt(socket_escucha, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
 
    struct sockaddr_in direccion;
    memset(&direccion, 0, sizeof(direccion));
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(PUERTO);
 
    if (bind(socket_escucha, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) {
        perror("bind");
        return 1;
    }
 
    if (listen(socket_escucha, TAMANO_COLA_ESPERA) < 0) {
        perror("listen");
        return 1;
    }
 
    printf("Servidor escuchando en el puerto %d...\n", PUERTO);
 
    /*
     * Ciclo principal: SOLO acepta conexiones, nunca lee/escribe
     * datos de un cliente directamente. Cada conexion aceptada se
     * delega de inmediato a un hilo nuevo y dedicado, para que un
     * cliente lento o inactivo nunca bloquee la llegada de otros.
     */
    while (1) {
        int socket_cliente = accept(socket_escucha, NULL, NULL);
        if (socket_cliente < 0) {
            perror("accept");
            continue; /* un accept() fallido no debe tumbar el servidor */
        }
 
        printf("Cliente conectado (socket %d).\n", socket_cliente);
 
        /*
         * Se reserva en el heap (no en la pila) porque esta variable
         * debe seguir viva despues de que esta iteracion del ciclo
         * termine y 'socket_cliente' cambie de valor en la siguiente
         * vuelta. El propio hilo la libera al empezar
         * (ver atender_cliente en cliente_handler.c).
         */
        ArgumentoCliente *arg = malloc(sizeof(ArgumentoCliente));
        if (arg == NULL) {
            fprintf(stderr, "No se pudo reservar memoria para el nuevo cliente\n");
            close(socket_cliente);
            continue;
        }
        arg->servidor = servidor;
        arg->socket_cliente = socket_cliente;
 
        pthread_t hilo;
        if (pthread_create(&hilo, NULL, atender_cliente, arg) != 0) {
            perror("pthread_create");
            free(arg);
            close(socket_cliente);
            continue;
        }
 
        /*
         * pthread_detach: le decimos al sistema que nadie va a hacer
         * pthread_join sobre este hilo. Sin esto, cada hilo que
         * termina quedaria como un "hilo zombie" ocupando recursos
         * hasta que alguien lo uniera explicitamente — y aqui no
         * tiene sentido unirlo, porque no sabemos cuando terminara
         * (depende de cuando se desconecte ESE cliente en particular).
         */
        pthread_detach(hilo);
    }
 
    /* No se llega aqui en esta fase: el servidor corre
       indefinidamente hasta que se mate el proceso (Ctrl+C). Un
       apagado ordenado (capturar la señal, cerrar el socket de
       escucha, esperar a los hilos) queda pendiente para una fase
       posterior. */
    close(socket_escucha);
    servidor_destruir(servidor);
    return 0;
}