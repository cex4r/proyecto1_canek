#include <stdlib.h>
#include <string.h>
#include "mensaje_json.h"

cJSON *mensaje_parsear(const char *linea) {
    cJSON *raiz = cJSON_Parse(linea);
    if (raiz == NULL) {
        return NULL; // json invalido 
    }

    if (!cJSON_IsObject(raiz)) {
        cJSON_Delete(raiz);
        return NULL; 
    }
    return raiz;
}

const char *mensaje_obtener_tipo(const cJSON *mensaje) {
    return mensaje_obtener_campo_string(mensaje, "type");
}

const char *mensaje_obtener_campo_string(const cJSON *mensaje, const char *campo) {
    if (mensaje == NULL) {
        return NULL; 
    }

    cJSON *item = cJSON_GetObjectItemCaseSensitive(mensaje, campo);
    if (item == NULL || !cJSON_IsString(item)) {
        return NULL; 
    }
    return item->valuestring;
}

void mensaje_liberar(cJSON *mensaje) {
    cJSON_Delete(mensaje); // acepta null sin pedos 
}

/*
    nos ayuda a imprimir un json en una sola linea, sin espacios extra, y le agrega 
    un \n delimitador, libera tantp e cjson como el buffer intermedio de cjson_print 
    antes de devolver el resultado final
*/

static char *serializar_y_liberar(cJSON *objeto) {
    if (objeto == NULL) {
        return NULL; 
    }

    char *sin_salto = cJSON_PrintUnformatted(objeto); 
    cJSON_Delete(objeto);

    if (sin_salto == NULL) {
        return NULL; 
    }

    size_t longitud = strlen(sin_salto); 
    char *resultado = malloc(longitud + 2); 
    if (resultado == NULL) {
        cJSON_free(sin_salto);
        return NULL; 
    }
    memcpy(resultado, sin_salto, longitud);
    resultado[longitud] = '\n';
    resultado[longitud + 1] = '\0';

    cJSON_free(sin_salto);
    return resultado; 
}

char *mensaje_construir_respuesta(const char *operacion, const char *resultado, const char *extra) {
    cJSON *objeto = cJSON_CreateObject(); 
    if (objeto == NULL) {
        return NULL; 
    }

    cJSON_AddStringToObject(objeto, "type", "RESPONSE");
    cJSON_AddStringToObject(objeto, "operation", operacion);
    cJSON_AddStringToObject(objeto, "result", resultado);
    if (extra != NULL) {
        cJSON_AddStringToObject(objeto, "extra", extra);
    }
 
    return serializar_y_liberar(objeto);
}

char *mensaje_construir_identify_exito(const char *username) {
    return mensaje_construir_respuesta("IDENTIFY", "SUCCESS", username);
}
 
char *mensaje_construir_identify_ya_existe(const char *username) {
    return mensaje_construir_respuesta("IDENTIFY", "USER_ALREADY_EXISTS", username);
}
 
char *mensaje_construir_no_identificado(void) {
    return mensaje_construir_respuesta("INVALID", "NOT_IDENTIFIED", NULL);
}
 
char *mensaje_construir_invalido(void) {
    return mensaje_construir_respuesta("INVALID", "INVALID", NULL);
}

char *mensaje_construir_new_user(const char *username) {
    cJSON *objeto = cJSON_CreateObject();
    if (objeto == NULL) {
        return NULL;
    }
 
    cJSON_AddStringToObject(objeto, "type", "NEW_USER");
    cJSON_AddStringToObject(objeto, "username", username);
 
    return serializar_y_liberar(objeto);
}