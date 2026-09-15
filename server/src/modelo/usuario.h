#ifndef USUARIO_H
#define USUARIO_H

//Definiendo los limites del usuario 
#define MAX_USERNAME_LEN 8 
#define MAX_ROOMNAME_LEN 16
#define MAX_SALAS_POR_USUARIO 16 

//Definiendo mi constructor
typedef struct Usuario Usuario; 

Usuario *usuario_crear(const char *username, int socket_fd); 
void *usuario_destruir(Usuario *usuario); 

//Mis setters y getters basicos

const char *usuario_obtener_username(const Usuario *usuario); 
const char *usuario_obtener_status(const Usuario *usuario); 
int usuario_poner_status(Usuario *usuario, const char *status); 
int usuario_obtener_socket(const Usuario *usuario); 


//Salas en las que se ha unido el usuario

int usuario_esta_unido_a_sala(const Usuario *usuario, const char *roomname); 
int usuario_unir_a_sala(Usuario *usuario, const char *roomname); 
int usuario_salir_de_sala(Usuario *usuario, const char *roomname);
int usuario_num_salas_unidas(const Usuario *usuario);
const char *usuario_obtener_sala_unida(const Usuario *usuario, int indice);


//Invitaciones pendientes 

int usuario_tiene_invitacion(const Usuario *usuario, const char *roomname); 
int usuario_agregar_invitacion(Usuario *usuario, const char *roomname); 
int usuario_quitar_invitacion(Usuario *usuario, const char *roomname);


#endif 