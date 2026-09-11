#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "servidor.h"

struct Servidor {
    Usuario *usuarios[MAX_USUARIOS_SERVIDOR];
    int num_usuarios; 
    
    Sala *salas[MAX_SALAS_SERVIDOR]; 
    int num_salas; 

    pthread_mutex_t lock; //Con esto protejo que los hilos de ejecucion no modifiquen al azar

};

Servidor *servidor_crear(void){
    Servidor *servidor = malloc(sizeof(Servidor)); 

    servidor->num_usuarios = 0; 
    servidor->num_salas = 0; 
    /*

    lo quue hace esta linea es proteger al servidor cuando lo vaya a modificar 
    es decir, que ningun hilo me vaya a modificar la memoria
    */ 
    pthread_mutex_init(&servidor->lock, NULL); 
    return servidor; 
}


void *servidor_destruir(Servidor *servidor){
    pthread_mutex_destroy(&servidor->lock);
    free(servidor); 
}




//metodos que usan al usuario 

static Usuario *buscar_usuario_interno(Servidor *servidor, const char *username){
    
    for (int i = 0; i < servidor->num_usuarios; i++) {
        if (strcmp(usuario_obtener_username(servidor->usuarios[i]), username) == 0) {
            return servidor->usuarios[i];
        }
    }
    return NULL;

}



Usuario *servidor_buscar_usuario(Servidor *servidor, const char *username){
    pthread_mutex_lock(&servidor->lock);
    Usuario *encontrado = buscar_usuario_interno(servidor, username);
    pthread_mutex_unlock(&servidor->lock);
    return encontrado;
}

Usuario *servidor_registrar_usuario(Servidor *servidor, const char *username, int socket_fd){
    pthread_mutex_lock(&servidor->lock);
    Usuario *nuevo = usuario_crear(username, socket_fd);
    servidor->usuarios[servidor->num_usuarios] = nuevo;
    servidor->num_usuarios++;
    pthread_mutex_unlock(&servidor->lock);
    return nuevo; 
}

void servidor_eliminar_usuario(Servidor *servidor, const char *username){
    pthread_mutex_lock(&servidor->lock);
    for (int i = 0; i < servidor->num_usuarios; i++) {
        if (strcmp(usuario_obtener_username(servidor->usuarios[i]), username) == 0) {
            usuario_destruir(servidor->usuarios[i]);
            servidor->usuarios[i] = servidor->usuarios[servidor->num_usuarios - 1];
            servidor->num_usuarios--;
            break;
        }
    }
 
    pthread_mutex_unlock(&servidor->lock);
}

int servidor_num_usuarios(Servidor *servidor){
    pthread_mutex_lock(&servidor->lock);
    int n = servidor->num_usuarios;
    pthread_mutex_unlock(&servidor->lock);
    return n;
}

Usuario *servidor_obtener_usuario_por_indice(Servidor *servidor, int indice) {
    pthread_mutex_lock(&servidor->lock);
    Usuario *u = servidor->usuarios[indice]; 
    pthread_mutex_unlock(&servidor->lock);
    return u;
}

//Metodos que usan a las salas

static Sala *buscar_sala_interno(Servidor *servidor, const char *roomname) {
    for (int i = 0; i < servidor->num_salas; i++) {
        if (strcmp(sala_obtener_nombre(servidor->salas[i]), roomname) == 0) {
            return servidor->salas[i];
        }
    }
    return NULL;
}


Sala *servidor_buscar_sala(Servidor *servidor, const char *roomname) {
    pthread_mutex_lock(&servidor->lock);
    Sala *encontrada = buscar_sala_interno(servidor, roomname);
    pthread_mutex_unlock(&servidor->lock);
    return encontrada;
}

Sala *servidor_registrar_sala(Servidor *servidor, const char *roomname) {
    pthread_mutex_lock(&servidor->lock);

    Sala *nueva = sala_crear(roomname);
    servidor->salas[servidor->num_salas] = nueva;
    servidor->num_salas++;
 
    pthread_mutex_unlock(&servidor->lock);
    return nueva;
}

void servidor_eliminar_sala(Servidor *servidor, const char *roomname) {
    pthread_mutex_lock(&servidor->lock);
    for (int i = 0; i < servidor->num_salas; i++) {
        if (strcmp(sala_obtener_nombre(servidor->salas[i]), roomname) == 0) {
            servidor->salas[i] = servidor->salas[servidor->num_salas - 1];
            servidor->num_salas--;
            break;
        }
    } 
    pthread_mutex_unlock(&servidor->lock);
}

