#include <stdarg.h> // para el manejo de argumentos variables en assertions
#include <stddef.h> // para size_t y null 
#include <setjmp.h> // para saltos no-locales
#include <cmocka.h> // para hacer unit tests mas facil
#include "modelo/usuario.h"

static void test_usuario_nuevo_esta_activo(void **state) {

}

static void test_usuario_username_demasiado_largo_falla(void **state) {

}

static void test_cambiar_status_valido(void **state) {

}

static void test_cambiar_status_invalido_es_rechazado(void **state) {

}

static void test_unirse_a_sala(void **state) {

}

static void test_unirse_dos_veces_a_la_misma_sala_falla(void **state) {

}

static void test_invitaciones(void **state) {
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_usuario_nuevo_esta_activo),
        cmocka_unit_test(test_usuario_username_demasiado_largo_falla),
        cmocka_unit_test(test_cambiar_status_valido),
        cmocka_unit_test(test_cambiar_status_invalido_es_rechazado),
        cmocka_unit_test(test_unirse_a_sala),
        cmocka_unit_test(test_unirse_dos_veces_a_la_misma_sala_falla),
        cmocka_unit_test(test_invitaciones),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
