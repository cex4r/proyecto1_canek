#include <stdarg.h> // para el manejo de argumentos variables en assertions
#include <stddef.h> // para size_t y null 
#include <setjmp.h> // para saltos no-locales
#include <stdint.h>
#include <cmocka.h> // para hacer unit tests mas facil
#include "modelo/usuario.h"

static void test_usuario_nuevo_esta_activo(void **state) {
    (void) state; 
    //vemos que el usuario se crea correctamente; 
    Usuario *u = usuario_crear("Kimberly",42); 
    assert_non_null(u);
    assert_string_equal(usuario_obtener_username(u),"Kimberly"); 
    assert_string_equal(usuario_obtener_status(u),"ACTIVE"); 
    
    usuario_destruir(u); //siempre tenemos que limpiar lo que creamos lo acabo de aprender :,v
    
}

static void test_usuario_username_demasiado_largo_falla(void **state) {
    Usuario *u = usuario_crear("usuariomuylargo", 42); 
    assert_null(u); 
    usuario_destruir(u);
}

static void test_cambiar_status_valido(void **state) {
    Usuario *u = usuario_crear("status", 42); 

    int res = usuario_poner_status(u,"AWAY");
    assert_true(res == 1); 
    assert_string_equal(usuario_obtener_status(u), "AWAY");
    usuario_destruir(u);
}

static void test_cambiar_status_invalido_es_rechazado(void **state) {
    Usuario *u = usuario_crear("usuario", 42);
    int res = usuario_poner_status(u, "putoelquelolea"); 
    assert_true(res == 0); 
    assert_string_equal(usuario_obtener_status(u),"ACTIVE"); //tiene que ser el mismo segun el protocolo xd
    usuario_destruir(u);
}

static void test_unirse_a_sala(void **state) {
    Usuario *u = usuario_crear("Ana", 69); 
    //ningun usuario esta unido a ninguna sala al inicio
    assert_true(usuario_esta_unido_a_sala(u, "Sala1") == 0); 
    int res = usuario_unir_a_sala(u, "Sala1"); 
    //esta parte esta medio xd, pero luego la mejoro
    assert_true(res == 1); 
    assert_true(usuario_esta_unido_a_sala(u, "Sala1") == 1); 
    usuario_destruir(u); 
}

static void test_unirse_dos_veces_a_la_misma_sala_falla(void **state) {
    Usuario *u = usuario_crear("Ana",42); 
    usuario_unir_a_sala(u, "Sala1"); 
    int intento2 = usuario_unir_a_sala(u, "Sala1"); 
    assert_true(intento2 == 0); 
    usuario_destruir(u); 
}

static void test_invitaciones(void **state) {
    //prueba que las invitaciones esten bien XD
    Usuario *u = usuario_crear("Ana", 10); 
    assert_true(usuario_tiene_invitacion(u, "Sala1") == 0); 
    usuario_agregar_invitacion(u, "Sala1"); 
    assert_true(usuario_tiene_invitacion(u, "Sala1") == 1 ); 
    usuario_quitar_invitacion(u, "Sala1");
    assert_true(usuario_tiene_invitacion(u, "Sala1") == 0); 

    usuario_destruir(u); 

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
