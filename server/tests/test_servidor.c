#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h> // para el manejo de argumentos variables en assertions
#include <stddef.h> // para size_t y null 
#include <setjmp.h> // para saltos no-locales
#include <stdint.h>
#include <cmocka.h> // para hacer unit tests mas facil
#include <pthread.h>
#include "modelo/servidor.h"
 
 

static void test_servidor_nuevo_esta_vacio(void **state) {
 
    Servidor *s = servidor_crear();
 
    assert_true(s != NULL); 
    assert_true(servidor_num_usuarios(s) == 0);
    assert_true(servidor_num_salas(s) == 0);
 
    servidor_destruir(s);
}

/* La parte del registro de ususario  */

 
static void test_registrar_usuario_nuevo(void **state) {
    printf("test_registrar_usuario_nuevo:\n");
 
    Servidor *s = servidor_crear();
 
    Usuario *u = servidor_registrar_usuario(s, "Kimberly", 10);
 
    assert_true(u != NULL);
    assert_true(strcmp(usuario_obtener_username(u), "Kimberly") == 0);
    assert_true(servidor_num_usuarios(s) == 1);
 
    servidor_destruir(s);
}
 
static void test_registrar_usuario_duplicado_falla(void **state) {

    Servidor *s = servidor_crear();
    servidor_registrar_usuario(s, "Kimberly", 10);
 
    Usuario *segundo = servidor_registrar_usuario(s, "Kimberly", 20);
 
    assert_true(segundo == NULL);
    assert_true(servidor_num_usuarios(s) == 1);
 
    servidor_destruir(s);
}
 
static void test_buscar_usuario(void **state) {
    
 
    Servidor *s = servidor_crear();
    Usuario *registrado = servidor_registrar_usuario(s, "Luis", 5);
 
    Usuario *encontrado = servidor_buscar_usuario(s, "Luis");
    Usuario *no_encontrado = servidor_buscar_usuario(s, "Fernando");
 
    assert_true(encontrado == registrado);
    assert_true(no_encontrado == NULL);
 
    servidor_destruir(s);
}
 
static void test_eliminar_usuario_existente(void **state) {
    
 
    Servidor *s = servidor_crear();
    servidor_registrar_usuario(s, "Antonio", 1);
 
    servidor_eliminar_usuario(s, "Antonio");
 
    assert_true(servidor_num_usuarios(s) == 0);
    assert_true(servidor_buscar_usuario(s, "Antonio") == NULL);
 
    servidor_destruir(s);
}
 
static void test_eliminar_usuario_inexistente_es_seguro(void **state) {
    
 
    Servidor *s = servidor_crear();
    servidor_registrar_usuario(s, "Antonio", 1);
 
    
    servidor_eliminar_usuario(s, "NoExiste");
 
    assert_true(servidor_num_usuarios(s) == 1);
    assert_true(servidor_buscar_usuario(s, "Antonio") != NULL);
 
    servidor_destruir(s);
}
 
static void test_obtener_usuario_por_indice(void **state) {
    
 
    Servidor *s = servidor_crear();
    servidor_registrar_usuario(s, "Kimberly", 1);
 
    Usuario *valido = servidor_obtener_usuario_por_indice(s, 0);
    Usuario *fuera_de_rango = servidor_obtener_usuario_por_indice(s, 5);
    Usuario *indice_negativo = servidor_obtener_usuario_por_indice(s, -1);
 
    assert_true(valido != NULL);
    assert_true(fuera_de_rango == NULL);
    assert_true(indice_negativo == NULL);
 
    servidor_destruir(s);
}
 
static void test_capacidad_maxima_usuarios(void **state) {
    
 
    Servidor *s = servidor_crear();
    char username[MAX_USERNAME_LEN + 1];
    int todos_exitosos = 1;
 
    for (int i = 0; i < MAX_USUARIOS_SERVIDOR; i++) {
        snprintf(username, sizeof(username), "U%d", i);
        if (servidor_registrar_usuario(s, username, i) == NULL) {
            todos_exitosos = 0;
        }
    }
    assert_true(todos_exitosos);
    assert_true(servidor_num_usuarios(s) == MAX_USUARIOS_SERVIDOR);
 
    Usuario *uno_de_mas = servidor_registrar_usuario(s, "Extra", 999);
    assert_true(uno_de_mas == NULL);
    assert_true(servidor_num_usuarios(s) == MAX_USUARIOS_SERVIDOR);
 
    servidor_destruir(s);
}
 

/* registro de sala                                                       */

static void test_registrar_sala_nueva(void **state) {
    
 
    Servidor *s = servidor_crear();
 
    Sala *sala = servidor_registrar_sala(s, "Sala1");
 
    assert_true(sala != NULL);
    assert_true(servidor_num_salas(s) == 1);
 
    servidor_destruir(s);
}
 
static void test_registrar_sala_duplicada_falla(void **state) {
 
    Servidor *s = servidor_crear();
    servidor_registrar_sala(s, "Sala1");
 
    Sala *segunda = servidor_registrar_sala(s, "Sala1");
 
    assert_true(segunda == NULL);
    assert_true(servidor_num_salas(s) == 1);
 
    servidor_destruir(s);
}
 
static void test_eliminar_sala_libera_el_nombre(void **state) {
    
 
    Servidor *s = servidor_crear();
    servidor_registrar_sala(s, "Sala1");
 
    servidor_eliminar_sala(s, "Sala1");
    assert_true(servidor_num_salas(s) == 0);
    assert_true(servidor_buscar_sala(s, "Sala1") == NULL);
 

    Sala *recreada = servidor_registrar_sala(s, "Sala1");
    assert_true(recreada != NULL);
 
    servidor_destruir(s);
}
 

/* Concurrencia                                                        */

#define NUM_HILOS 20


//agrupamos el sevidor y el indice
typedef struct {
    Servidor *servidor;
    int indice;
} ArgHilo;
 
/* esta funcion lo que hace es usar el hilo para generar un unico 
    username, esto para probar la concurrencia
 */
static void *hilo_registrar_distinto(void *arg_void) {
    ArgHilo *arg = arg_void;
    char username[MAX_USERNAME_LEN + 1];
    snprintf(username, sizeof(username), "H%d", arg->indice);
    servidor_registrar_usuario(arg->servidor, username, arg->indice);
    return NULL;
}
 

static void test_concurrencia_usernames_distintos(void **state) {
 
    Servidor *s = servidor_crear();
    pthread_t hilos[NUM_HILOS];
    ArgHilo args[NUM_HILOS];
    /*
    creo el servidor y preparo los 20 usuarios con 20 hilos distintos
    en el servidor
    */
    for (int i = 0; i < NUM_HILOS; i++) {
        args[i].servidor = s;
        args[i].indice = i;
        pthread_create(&hilos[i], NULL, hilo_registrar_distinto, &args[i]);
    }
    //me aseguro que todos los hilos hayan terminado su proceso 
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }
 
    /* si el mutex protege el registro correctamente, NINGUN registro
       se pierd, el contador debe ser exactamente NUM_HILOS, sin
       importar el orden en que los hilos se ejecutaron */
    assert_true(servidor_num_usuarios(s) == NUM_HILOS);
 
    char username[MAX_USERNAME_LEN + 1];
    int todos_encontrados = 1;
    for (int i = 0; i < NUM_HILOS; i++) {
        snprintf(username, sizeof(username), "H%d", i);
        if (servidor_buscar_usuario(s, username) == NULL) {
            todos_encontrados = 0;
        }
    }
    assert_true(todos_encontrados);
 
    servidor_destruir(s);
}
 
/* todos los hilos intentan registrar el MISMO username a la vez */
static void *hilo_registrar_mismo(void *arg_void) {
    Servidor *s = arg_void;
    /* el valor de retorno del hilo es el propio puntero Usuario*,
       o NULL si fallo. pthread_join nos lo entrega de vuelta */
    return servidor_registrar_usuario(s, "Disputa", 0); 
}
 
static void test_concurrencia_mismo_username_solo_uno_gana(void **state) {
    
 
    Servidor *s = servidor_crear();
    pthread_t hilos[NUM_HILOS];
    void *resultados[NUM_HILOS];
 
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_create(&hilos[i], NULL, hilo_registrar_mismo, s);
    }
 
    int exitosos = 0;
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], &resultados[i]);
        if (resultados[i] != NULL) {
            exitosos++;
        }
    }
 
    /* si el lock realmente hAce la secuencia
       "verificar si existe, luego insertar", exactamente UN hilo
       debe ganar la carrera, mas de uno indicaria una condicion de
       carrera */
    assert_true(exitosos == 1);
    assert_true(servidor_num_usuarios(s) == 1);
 
    servidor_destruir(s);
}
 
 
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_servidor_nuevo_esta_vacio),
        cmocka_unit_test(test_registrar_usuario_nuevo),
        cmocka_unit_test(test_registrar_usuario_duplicado_falla),
        cmocka_unit_test(test_buscar_usuario),
        cmocka_unit_test( test_eliminar_usuario_existente),
        cmocka_unit_test( test_eliminar_usuario_inexistente_es_seguro),
        cmocka_unit_test(test_obtener_usuario_por_indice),
        cmocka_unit_test(test_capacidad_maxima_usuarios),
        cmocka_unit_test(test_registrar_sala_nueva),
        cmocka_unit_test(test_registrar_sala_duplicada_falla),
        cmocka_unit_test(test_eliminar_sala_libera_el_nombre),
        cmocka_unit_test(test_concurrencia_usernames_distintos),
        cmocka_unit_test(test_concurrencia_mismo_username_solo_uno_gana),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
 