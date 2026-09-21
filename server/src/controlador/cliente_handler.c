#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cliente_handler.h"
#include "buffer_lineas.h"
#include "json/mensaje_json.h"
 
#define TAM_LECTURA 512
#define TAM_LINEA_MAX 1024

/*
    Envia una cadena ya terminada en \n por un socket, pequeño helper
    para no repetir strlen()+write() en cada punto de salida
*/
static void enviar(int socket_fd, const char *mensaje) {
    if (mensaje != NULL) {
        write(socket_fd, mensaje, strlen(mensaje));
    }
}

/*
    Construye 'mensaje', lo manda a todos los usuarios registrados EXCEPTO 'socket_excluido'
    y libera la cadena
*/
static void difundir_a_todos_excepto(Servidor *servidor, int socket_excluido, char *mensaje) {
    if (mensaje == NULL) {
        return; 
    }

    int sockets[MAX_USUARIOS_SERVIDOR];
    int cantidad = servidor_obtener_sockets_excepto(servidor, socket_excluido, sockets, MAX_USUARIOS_SERVIDOR);

    for (int i = 0; i < cantidad; i++) {
        enviar(sockets[i], mensaje); 
    }
    free(mensaje); 
}

/*
    procesa exactamente una linea/mensaje ya extraida por BufferLineas.
    Devuelve 1 si la conexion debe seguir abierta, 0 si debe cerrarse.
*/
static int procesar_linea(Servidor *servidor, int socket_cliente, const char *linea, Usuario **usuario_actual) {
    cJSON *mensaje = mensaje_parsear(linea);
    const char *tipo = mensaje_obtener_tipo(mensaje);
 
    if (tipo == NULL) {
        /* JSON invalido, o valido pero sin "type" como cadena */
        char *respuesta = mensaje_construir_invalido();
        enviar(socket_cliente, respuesta);
        free(respuesta);
        mensaje_liberar(mensaje);
        return 0;
    }
 
    if (*usuario_actual == NULL) {
        /* Todavia no identificado: SOLO se acepta IDENTIFY */
        if (strcmp(tipo, "IDENTIFY") != 0) {
            char *respuesta = mensaje_construir_no_identificado();
            enviar(socket_cliente, respuesta);
            free(respuesta);
            mensaje_liberar(mensaje);
            return 0;
        }
 
        const char *username = mensaje_obtener_campo_string(mensaje, "username");
 
        if (username == NULL || strlen(username) == 0 || strlen(username) > MAX_USERNAME_LEN) {
            /* Campo faltante o username fuera del limite del
               protocolo: esto es un mensaje mal formado (INVALID),
               NO un USER_ALREADY_EXISTS. Se valida ANTES de tocar
               el Servidor para poder distinguir ambos casos. */
            char *respuesta = mensaje_construir_invalido();
            enviar(socket_cliente, respuesta);
            free(respuesta);
            mensaje_liberar(mensaje);
            return 0;
        }
 
        Usuario *nuevo = servidor_registrar_usuario(servidor, username, socket_cliente);
 
        if (nuevo == NULL) {
            /* Como ya validamos la longitud arriba, la unica razon
               realista para fallar aqui es que el username ya
               existe (USER_ALREADY_EXISTS). El protocolo no
               especifica que hacer si el servidor esta lleno; lo
               tratamos igual por simplicidad. */
            char *respuesta = mensaje_construir_identify_ya_existe(username);
            enviar(socket_cliente, respuesta);
            free(respuesta);
            mensaje_liberar(mensaje);
            return 1; /* NO se desconecta: puede reintentar con otro nombre */
        }
 
        *usuario_actual = nuevo;
 
        char *respuesta = mensaje_construir_identify_exito(username);
        enviar(socket_cliente, respuesta);
        free(respuesta);
 
        char *aviso = mensaje_construir_new_user(username);
        difundir_a_todos_excepto(servidor, socket_cliente, aviso);
 
        mensaje_liberar(mensaje);
        return 1;
    }

    mensaje_liberar(mensaje);
    return 1;
}

void *atender_cliente(void *arg_void) {
    ArgumentoCliente *arg = arg_void;
    Servidor *servidor = arg->servidor;
    int socket_cliente = arg->socket_cliente;
    free(arg); /* ya copiamos lo que necesitabamos a variables locales */
 
    BufferLineas *buffer = buffer_lineas_crear();
    Usuario *usuario_actual = NULL;
    char lectura[TAM_LECTURA];
    char linea[TAM_LINEA_MAX];
    ssize_t bytes_leidos;
    int seguir_conectado = 1;
 
    while (seguir_conectado && (bytes_leidos = read(socket_cliente, lectura, sizeof(lectura))) > 0) {
        if (!buffer_lineas_agregar_datos(buffer, lectura, (int)bytes_leidos)) {
            /* mensaje demasiado largo para el buffer: cortamos la
               conexion en vez de desbordar memoria. */
            break;
        }
 
        while (buffer_lineas_obtener_siguiente(buffer, linea, sizeof(linea))) {
            seguir_conectado = procesar_linea(servidor, socket_cliente, linea, &usuario_actual);
            if (!seguir_conectado) {
                break;
            }
        }
    }
 
    /* si llego a identificarse, se quita del registro y
       se avisa a los demas. */
    if (usuario_actual != NULL) {
        printf("Cliente '%s' desconectado.\n", usuario_obtener_username(usuario_actual));
        servidor_eliminar_usuario(servidor, usuario_obtener_username(usuario_actual));
    } else {
        printf("Cliente desconectado antes de identificarse.\n");
    }
 
    buffer_lineas_destruir(buffer);
    close(socket_cliente);
    return NULL;
}