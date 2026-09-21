#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cmocka.h>

#include "json/mensaje_json.h"

/* ------------------------------------------------------------------ */
/* Parseo de mensajes entrantes                                        */
/* ------------------------------------------------------------------ */

static void test_parsear_json_valido(void **state) {
    (void) state;

    cJSON *m = mensaje_parsear("{\"type\":\"IDENTIFY\",\"username\":\"Kimberly\"}");

    assert_non_null(m);
    assert_string_equal(mensaje_obtener_tipo(m), "IDENTIFY");
    assert_string_equal(mensaje_obtener_campo_string(m, "username"), "Kimberly");

    mensaje_liberar(m);
}

static void test_parsear_json_sintacticamente_invalido(void **state) {
    (void) state;

    cJSON *m = mensaje_parsear("esto no es json valido {{{");

    assert_null(m);
}

static void test_parsear_json_que_no_es_objeto(void **state) {
    (void) state;

    cJSON *arreglo = mensaje_parsear("[1, 2, 3]");
    cJSON *numero = mensaje_parsear("42");
    cJSON *cadena = mensaje_parsear("\"hola\"");

    assert_null(arreglo);
    assert_null(numero);
    assert_null(cadena);
}

static void test_tipo_ausente_devuelve_null(void **state) {
    (void) state;

    cJSON *m = mensaje_parsear("{\"username\":\"Kimberly\"}");

    assert_non_null(m);
    assert_null(mensaje_obtener_tipo(m));

    mensaje_liberar(m);
}

static void test_tipo_no_es_cadena_devuelve_null(void **state) {
    (void) state;

    cJSON *m = mensaje_parsear("{\"type\":123}");

    assert_null(mensaje_obtener_tipo(m));

    mensaje_liberar(m);
}

static void test_campo_generico_ausente(void **state) {
    (void) state;

    cJSON *m = mensaje_parsear("{\"type\":\"IDENTIFY\"}");

    assert_null(mensaje_obtener_campo_string(m, "username"));

    mensaje_liberar(m);
}

static void test_liberar_null_es_seguro(void **state) {
    (void) state;

    /* No debe tronar al liberar un mensaje NULL */
    mensaje_liberar(NULL);

    assert_true(1);
}

/* ------------------------------------------------------------------ */
/* Construccion de mensajes salientes                                   */
/* ------------------------------------------------------------------ */

static void test_construir_identify_exito(void **state) {
    (void) state;

    char *resultado = mensaje_construir_identify_exito("Kimberly");

    assert_non_null(resultado);
    assert_int_equal(resultado[strlen(resultado) - 1], '\n');

    cJSON *reparseado = mensaje_parsear(resultado);
    assert_non_null(reparseado);
    assert_string_equal(mensaje_obtener_tipo(reparseado), "RESPONSE");
    assert_string_equal(mensaje_obtener_campo_string(reparseado, "operation"), "IDENTIFY");
    assert_string_equal(mensaje_obtener_campo_string(reparseado, "result"), "SUCCESS");
    assert_string_equal(mensaje_obtener_campo_string(reparseado, "extra"), "Kimberly");

    mensaje_liberar(reparseado);
    free(resultado);
}

static void test_construir_identify_ya_existe(void **state) {
    (void) state;

    char *resultado = mensaje_construir_identify_ya_existe("Kimberly");
    assert_non_null(resultado);

    cJSON *reparseado = mensaje_parsear(resultado);
    assert_non_null(reparseado);
    assert_string_equal(mensaje_obtener_campo_string(reparseado, "result"), "USER_ALREADY_EXISTS");

    mensaje_liberar(reparseado);
    free(resultado);
}

static void test_construir_no_identificado_no_tiene_extra(void **state) {
    (void) state;

    char *resultado = mensaje_construir_no_identificado();
    assert_non_null(resultado);

    cJSON *reparseado = mensaje_parsear(resultado);
    assert_non_null(reparseado);
    assert_string_equal(mensaje_obtener_campo_string(reparseado, "operation"), "INVALID");
    assert_string_equal(mensaje_obtener_campo_string(reparseado, "result"), "NOT_IDENTIFIED");
    assert_null(mensaje_obtener_campo_string(reparseado, "extra"));

    mensaje_liberar(reparseado);
    free(resultado);
}

static void test_construir_invalido(void **state) {
    (void) state;

    char *resultado = mensaje_construir_invalido();
    assert_non_null(resultado);

    cJSON *reparseado = mensaje_parsear(resultado);
    assert_non_null(reparseado);
    assert_string_equal(mensaje_obtener_campo_string(reparseado, "operation"), "INVALID");
    assert_string_equal(mensaje_obtener_campo_string(reparseado, "result"), "INVALID");

    mensaje_liberar(reparseado);
    free(resultado);
}

static void test_construir_new_user(void **state) {
    (void) state;

    char *resultado = mensaje_construir_new_user("Luis");
    assert_non_null(resultado);

    cJSON *reparseado = mensaje_parsear(resultado);
    assert_non_null(reparseado);
    assert_string_equal(mensaje_obtener_tipo(reparseado), "NEW_USER");
    assert_string_equal(mensaje_obtener_campo_string(reparseado, "username"), "Luis");

    mensaje_liberar(reparseado);
    free(resultado);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parsear_json_valido),
        cmocka_unit_test(test_parsear_json_sintacticamente_invalido),
        cmocka_unit_test(test_parsear_json_que_no_es_objeto),
        cmocka_unit_test(test_tipo_ausente_devuelve_null),
        cmocka_unit_test(test_tipo_no_es_cadena_devuelve_null),
        cmocka_unit_test(test_campo_generico_ausente),
        cmocka_unit_test(test_liberar_null_es_seguro),
        cmocka_unit_test(test_construir_identify_exito),
        cmocka_unit_test(test_construir_identify_ya_existe),
        cmocka_unit_test(test_construir_no_identificado_no_tiene_extra),
        cmocka_unit_test(test_construir_invalido),
        cmocka_unit_test(test_construir_new_user),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}