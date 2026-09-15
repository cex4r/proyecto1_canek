#ifndef SALA_H
#define SALA_H
 
#include "usuario.h" /* rehusa variables de usuario */
 
#define MAX_USUARIOS_POR_SALA 64
 
typedef struct Sala Sala;
 
Sala *sala_crear(const char *roomname);
void sala_destruir(Sala *sala);
 
const char *sala_obtener_nombre(const Sala *sala);
 
/* miembros unidos */
int sala_tiene_miembro(const Sala *sala, const char *username);
int sala_agregar_miembro(Sala *sala, const char *username);
int sala_quitar_miembro(Sala *sala, const char *username);
int sala_num_miembros(const Sala *sala);
const char *sala_obtener_miembro(const Sala *sala, int indice);
int sala_esta_vacia(const Sala *sala);
 
/* Invitados pero no unidos */
int sala_tiene_invitado(const Sala *sala, const char *username);
int sala_agregar_invitado(Sala *sala, const char *username);
int sala_quitar_invitado(Sala *sala, const char *username);
 
#endif 