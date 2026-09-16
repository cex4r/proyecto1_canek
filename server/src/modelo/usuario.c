/*
    propiedades que conforman un usuario:

    -username  ------ (maximo 8 caracteres, estado de saber si el username esta en uso)
    -status    ------ (al conectrse por default esta en ACTIVE, necesario para reportar a los demas usuarios del su propio estado de conexion)
    -indentificacion del state ------ (es lo primero que se tiene que verificar)
    -set de habitaciones en las que se ha unido 
    -set de habitaciones pendientes en las que ha estado
    -socket del usuario para el controlador

*/


#include <stdlib.h>
#include <string.h>
#include "usuario.h"


/*
    Aqui estoy definiendo las propiedades del usuario
    almaceno el nombre del usuario, el estado del usario, el puerto/socket 
    las salas en las que puede estar y el numero de invitaciones que ha recibido
    por el momento no voy a implementar todo ya que quiero algo minimamente funcional

*/

struct Usuario {
    char username[MAX_USERNAME_LEN + 1]; 
    char status[8]; 
    int socket; 

    char salas_unidas[MAX_SALAS_POR_USUARIO][MAX_ROOMNAME_LEN];
    int num_salas_unidas;
    
    char invitaciones[MAX_SALAS_POR_USUARIO][MAX_ROOMNAME_LEN + 1];
    int num_invitaciones;  
};



//Creando al usuario con sus propiedades/constructor de la clase

Usuario *usuario_crear(const char *username, int socket_fd){

    if(username == NULL || strlen(username) == 0 || strlen(username) > MAX_USERNAME_LEN){
        return NULL; 
    }

    Usuario *usuario = malloc(sizeof(Usuario)); 

    if(usuario == NULL){
        return NULL; //nos quedamos sin memoria pa 
    }

    strcpy(usuario->username, username);
    strcpy(usuario->status,"ACTIVE"); 
    
    usuario->socket = socket_fd;  
    usuario->num_invitaciones = 0; 
    usuario->num_salas_unidas = 0; 

    return usuario; 

    }

void *usuario_destruir(Usuario *usuario){
    free(usuario); 
}

//Metodos basicos del usuario

const char *usuario_obtener_username(const Usuario *usuario){
    return usuario->username; 
}

const char *usuario_obtener_status(const Usuario *usuario){
    return usuario->status; 
}

int usuario_poner_status(Usuario *usuario, const char *status){

    if(strcmp(status, "AWAY") != 0 
    && strcmp(status, "BUSY") != 0
    && strcmp(status, "ACTIVE") != 0){
            return 0; 
        }

    strcpy(usuario->status, status); 
    return 1; 
}

int usuario_obtener_socket(const Usuario *usuario){
    return usuario->socket; 
}


//Salas en las que ha estado el usuario
//Mejorando estas cosas con un hashMap xd
int usuario_esta_unido_a_sala(const Usuario *usuario, const char *roomname){
    for(int i = 0; i < usuario->num_salas_unidas; i++){
        if(strcmp(usuario->salas_unidas[i], roomname) == 0) {
            return 1; 
        }
    }
    return 0; 
}

int usuario_unir_a_sala(Usuario *usuario, const char *roomname){

    if(usuario_esta_unido_a_sala(usuario, roomname) == 1){
        return 0; 
    }
         
    if(usuario->num_salas_unidas >= MAX_SALAS_POR_USUARIO){
        return 0; 
    }

    strcpy(usuario->salas_unidas[usuario->num_salas_unidas], roomname); 
    usuario->num_salas_unidas++;
    return 1;

}

int usuario_salir_de_sala(Usuario *usuario, const char *roomname){
    for(int i = 0; i < usuario->num_salas_unidas; i++){
        if(strcmp(usuario->salas_unidas[i], roomname) == 0){
            strcpy(usuario->salas_unidas[i], usuario->salas_unidas[usuario->num_salas_unidas -1]); 
            usuario->num_salas_unidas--; 
            return 1; 
        }
    }
    return 0; 
    
}

int usuario_num_salas_unidas(const Usuario *usuario){
    return usuario->num_salas_unidas; 
}

const char *usuario_obtener_sala_unida(const Usuario *usuario, int indice){
    return usuario->salas_unidas[indice];
}

//initaciones pendientes 

int usuario_tiene_invitacion(const Usuario *usuario, const char *roomname){
    for (int i = 0; i < usuario->num_invitaciones; i++) {
        if (strcmp(usuario->invitaciones[i], roomname) == 0) {
            return 1;
        }
    }
    return 0;
}

int usuario_agregar_invitacion(Usuario *usuario, const char *roomname){
    strcpy(usuario->invitaciones[usuario->num_invitaciones], roomname);
    usuario->num_invitaciones++;
    return 1;
}

int usuario_quitar_invitacion(Usuario *usuario, const char *roomname) {
    for (int i = 0; i < usuario->num_invitaciones; i++) {
        if (strcmp(usuario->invitaciones[i], roomname) == 0) {
            strcpy(usuario->invitaciones[i], usuario->invitaciones[usuario->num_invitaciones - 1]);
            usuario->num_invitaciones--;
            return 1;
        }
    }
    return 0;
}