#ifndef MENSAJE_JSON_H
#define MENSAJE_JSON_H

#include <cjson/cJSON.h>

/*
    Ojo esta es la unica clase que el servidor sabe que existe, asi que un dia 
    hay que mejorar la parte de los mensajes de json, esta es la unica clase que hay que 
    cambiar.

    el codig solo debe llamar a las funciones de este codigo, no a las de cjson directamente
    porque si no se complica un póco mas todso xd

*/


    /*
        interpreta linea por linea si es un objeto JSON, devuelve 0 si es valido
        NULL en otro caso
    */

cJSON *mensaje_parsear(const char *linea);



    /*
        devuelve el valor del campo "type" como cadena o NULL si no existe o no es una 
        cadena
    
    */
const char *mensaje_obtener_tipo(const cJSON *mensaje, const char *campo);



    /*
        Getter normalon
    */
const char *mensaje_obtener_campo_string(const cJSON *mensaje, const char *campo);


    /*
        libera un mensaje obtenido de mensaje_parsear
    */
void mensaje_liberar(cJSON *mensaje);

/* --- Construccion de mensajes salientes --- */
/*
  cada builder devuelve una cadena reservada con malloc, ya
  terminada en \n (como exige el protocolo), lista para pasarse
  directamente a write()
  
  el llamador es responsable de hacer
  free() de la cadena devuelta. Devuelven NULL si cJSON falla al
  construir el objeto
 
/* construye un RESPONSE generico: {"type":"RESPONSE","operation":X,
   "result":Y[,"extra":Z]}. 'extra' puede ser NULL para omitir ese
   campo (como en los casos NOT_IDENTIFIED / INVALID del protocolo) */
char *mensaje_construir_respuesta(const char *operacion, const char *resultado, const char *extra);


/* atajos para los casos que ya usamos en esta fase, construidos
   internamente sobre mensaje_construir_respuesta. */
char *mensaje_construir_identify_exito(const char *username);
char *mensaje_construir_identify_ya_existe(const char *username);
char *mensaje_construir_no_identificado(void);
char *mensaje_construir_invalido(void);
 
/* {"type":"NEW_USER","username":X} — no es un RESPONSE, se manda a
   los demas clientes cuando alguien se identifica exitosamente */
char *mensaje_construir_new_user(const char *username);
 
#endif



