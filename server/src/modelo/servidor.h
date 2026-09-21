#ifndef SERVIDOR_H
#define SERVIDOR_H
 
#include "usuario.h"
#include "sala.h"
 
#define MAX_USUARIOS_SERVIDOR 128
#define MAX_SALAS_SERVIDOR 64
 
typedef struct Servidor Servidor;
 
Servidor *servidor_crear(void);
void servidor_destruir(Servidor *servidor);
 
/*
 * Every public method below locks/unlocks the server's internal
 * mutex itself, so calling code (the Controller, later) never has
 * to remember to lock manually — it just calls these and trusts
 * the object to be thread-safe.
 */
 
/* Users */
Usuario *servidor_buscar_usuario(Servidor *servidor, const char *username);
Usuario *servidor_registrar_usuario(Servidor *servidor, const char *username, int socket_fd);
void servidor_eliminar_usuario(Servidor *servidor, const char *username);
int servidor_num_usuarios(Servidor *servidor);
Usuario *servidor_obtener_usuario_por_indice(Servidor *servidor, int indice);
 
/* Rooms */
Sala *servidor_buscar_sala(Servidor *servidor, const char *roomname);
Sala *servidor_registrar_sala(Servidor *servidor, const char *roomname);
void servidor_eliminar_sala(Servidor *servidor, const char *roomname);
int servidor_num_salas(Servidor *servidor);
 
/*
 * Copia hasta 'max_sockets' descriptores de socket de todos los
 * usuarios registrados EXCEPTO el que tiene 'socket_excluido', en
 * 'sockets_salida'. Devuelve cuantos se copiaron realmente.
 *
 * Esto existe especificamente para difusion (broadcast): se toma el
 * lock, se copia la lista de sockets, se libera el lock, y RECIEN
 * DESPUES el llamador hace write() a cada uno — nunca se debe hacer
 * I/O de red mientras el lock esta tomado, porque un cliente lento
 * bloquearia a todos los demas hilos que necesiten tocar el
 * registro de usuarios/salas mientras tanto.
 */
int servidor_obtener_sockets_excepto(Servidor *servidor, int socket_excluido,
                                      int *sockets_salida, int max_sockets);
 
#endif /* SERVIDOR_H */