#ifndef Servidor_H
#define Servidor_H

#include "usuario.h"
#include "sala.h"

#define MAX_USUARIOS_SERVIDOR 128
#define MAX_SALAS_SERVIDOR 64

typedef struct Servidor Servidor; 

Servidor *servidor_crear(void); 
void *servidor_destruir(Servidor *servidor); 

//Metodos que usan al usuario 
Usuario *servidor_buscar_usuario(Servidor *servidor, const char *username);
Usuario *servidor_registrar_usuario(Servidor *servidor, const char *username, int socket_fd);
void servidor_eliminar_usuario(Servidor *servidor, const char *username);
int servidor_num_usuarios(Servidor *servidor);
Usuario *servidor_obtener_usuario_por_indice(Servidor *servidor, int indice); 

//Metodos que usan a las salas 
Sala *servidor_buscar_sala(Servidor *servidor, const char *roomname);
Sala *servidor_registrar_sala(Servidor *servidor, const char *roomname);
void servidor_eliminar_sala(Servidor *servidor, const char *roomname);




#endif // Servidor_H

