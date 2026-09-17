#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "modelo/servidor.h"

#define PUERTO 5000
#define TAM_BUFFER 1024


int main(void) {
    /*Creamos un servidor para un solo cliente*/
    Servidor *servidor = servidor_crear(); 

    if(servidor == NULL){
        fprintf(stderr,"No se pudo crear el servidor \n"); 
        return 1; 
    }

    /*
    Forma estandar de crear un servidor, practicamente lo hago con el
    formato de tcp, ya que necesito que los mensajes lleguen bien
    */

    int socket_escucha = socket(AF_INET, SOCK_STREAM,0); 
    if(socket_escucha < 0){
        perror("socket"); 
        return 1; 
    }

    int reuse = 1; 
    setsockopt(socket_escucha, SOL_SOCKET , SO_REUSEADDR, &reuse, sizeof(reuse)); 

    struct sockaddr_in direccion;
    memset(&direccion, 0, sizeof(direccion));
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(PUERTO);

    if (bind(socket_escucha, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(socket_escucha, 1) < 0) {
        perror("listen");
        return 1;
    }

    printf("Servidor escuchando en el puerto %d...\n", PUERTO); 

    //aqui solo vamos a aceptar un solo cliente, por ahora 
    int socket_cliente = accept(socket_escucha, NULL, NULL); 
    if(socket_cliente < 0){
        perror("accept"); 
        return 1; 
    }
    printf("Cliente concetado \n"); 

    Usuario *usuario = servidor_registrar_usuario(servidor, "daira", socket_cliente); 
    if(usuario == NULL){
        fprintf(stderr,"No se creo el usuario correctamente"); 
        close(socket_cliente); 
        return 1; 
    }

    printf("Usuario '%s' registrado con estado '%s'.\n",
           usuario_obtener_username(usuario),
           usuario_obtener_status(usuario));


    char buffer[TAM_BUFFER]; 
    ssize_t bytes_leidos; 

    //aqui leemos todo lo que el cliente mande, si deja de leer que manda algo, se desconecta
    while((bytes_leidos = read(socket_cliente, buffer, sizeof(buffer)-1)) > 0) {
        buffer[bytes_leidos] = '\0';
        printf("[%s] %s", usuario_obtener_username(usuario), buffer);
        write(socket_cliente, buffer, bytes_leidos);
    }

    printf("Cliente '%s' desconectado\n", usuario_obtener_username(usuario));

    servidor_eliminar_usuario(servidor, "Kimberly");
    close(socket_cliente);
    close(socket_escucha);
    servidor_destruir(servidor);

    return 0;





}