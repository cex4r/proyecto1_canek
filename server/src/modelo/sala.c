/*

    Propiedades que conforman una sala:

    -roomname ---- (maximo 16 caracteres, una vez que una sala desaparece, su nombre se vuelve a habilitar)
    -set de miembros que se han unido 
    -set de miembros que han sido invitados pero que no se han unido


*/

//Constructor y atributos de la clase sala

#include <stdlib.h>
#include <string.h>
#include "sala.h"

struct Sala {
    char roomname[MAX_ROOMNAME_LEN];
    char miembros[MAX_USUARIOS_POR_SALA][MAX_USERNAME_LEN]; 
    int num_miembros; 
    
    char invitados[MAX_USUARIOS_POR_SALA][MAX_USERNAME_LEN]; 
    int num_invitados; 
};

Sala *sala_crear(const char *roomname){

    if(roomname == NULL || strlen(roomname) == 0 || strlen(roomname) > MAX_ROOMNAME_LEN ){
        return NULL; 
    }

    Sala *sala = malloc(sizeof(Sala)); 

    if(sala == NULL){
        return NULL; // si pasa eso nos quedamos sin memoria pa 
    }

    strcpy(sala->roomname, roomname); 
    sala->num_miembros = 0;
    sala->num_invitados = 0; 
    
    return sala;
}

void sala_destruir(Sala *sala){
    free(sala); 
}

const char *sala_obtener_nombre(const Sala *sala){
    return sala->roomname; 
}

//Funciones con la lista de miembros de la sala

int sala_tiene_miembro(const Sala *sala, const char *username){

    for(int i = 0; i < sala->num_miembros; i++){
        if(strcmp(sala->miembros[i], username) == 0){
            return 1; 
        }
    }
    return 0; 
}

int sala_agregar_miembro(Sala *sala, const char*username){
    
    if(sala_tiene_miembro(sala, username)){
        return 0; 
    }
    strcpy(sala->miembros[sala->num_miembros], username); 
    sala->num_miembros++; 
    return 1; 
}

int sala_quitar_miembro(Sala *sala, const char *username){
    
    for(int i = 0; i < sala->num_miembros; i++){
        if(strcmp(sala->miembros[i], username) == 0){
            strcpy(sala->miembros[i], sala->miembros[sala->num_miembros - 1]); 
            sala->num_miembros--; 
            return 1; 
        }
    }
    return 0;

}

int sala_num_miembros(const Sala *sala){
    return sala->num_miembros; 
}

const char *sala_obtener_miembro(const Sala *sala, int indice){

    if (indice < 0 || indice >= sala->num_miembros) {
        return NULL;
    }

    return sala->miembros[indice]; 
}


int sala_esta_vacia(const Sala *sala){
    return sala->num_miembros==0; 
}

//Usuarios invitados pero que no se han unido

int sala_tiene_invitado(const Sala *sala, const char *username){
    for (int i = 0; i < sala->num_invitados; i++) {
        if (strcmp(sala->invitados[i], username) == 0) {
            return 1;
        }
    }
    return 0;
}

int sala_agregar_invitado(Sala *sala, const char *username){
    if (sala_tiene_invitado(sala, username)) {
        return 0;
    }
    if (sala->num_invitados >= MAX_USUARIOS_POR_SALA) {
        return 0;
    }
    strcpy(sala->invitados[sala->num_invitados], username);
    sala->num_invitados++;
    return 1;
}

int sala_quitar_invitado(Sala *sala, const char *username){
     for (int i = 0; i < sala->num_invitados; i++) {
        if (strcmp(sala->invitados[i], username) == 0) {
            strcpy(sala->invitados[i], sala->invitados[sala->num_invitados - 1]);
            sala->num_invitados--;
            return 1;
        }
    }
    return 0;
}


