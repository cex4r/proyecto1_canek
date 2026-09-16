#include <stdarg.h> // para el manejo de argumentos variables en assertions
#include <stddef.h> // para size_t y null 
#include <setjmp.h> // para saltos no-locales
#include <stdint.h>
#include <cmocka.h> // para hacer unit tests mas facil
#include "modelo/sala.h"


//tests para saber que los setters funcionan correctamente
static void test_obtener_nombre(void **state){
    Sala *s = sala_crear("Sala1"); 
    assert_non_null(s); 
    assert_string_equal(sala_obtener_nombre(s), "Sala1");

    sala_destruir(s); 
}

static void test_sala_esta_vacia(void **state){
    Sala *s = sala_crear("Sala"); 
    int n1 = sala_num_miembros(s); 
    assert_true(n1 == 0); 
    assert_true(sala_esta_vacia(s) == 1);

    sala_destruir(s); 

}

static void test_sala_nombre_valido(void **state){
    Sala *s1 = sala_crear("Sala con nombre largo"); 
    assert_null(s1); 
    sala_destruir(s1); 

    Sala *s2 = sala_crear(""); 
    assert_null(s2); 
    sala_destruir(s2); 

    
}


//tests con miembros de la sala 

static void test_sala_tiene_miembro(void **state){
    Sala *s = sala_crear("Sala"); 
    assert_null( sala_obtener_miembro(s, 10)); 

    sala_destruir(s); 

}

static void test_sala_agrega_miembro(void **state){

    Sala *s = sala_crear("Sala"); 

    sala_agregar_miembro(s, "Ana"); 
    int x1 = sala_num_miembros(s); 
    assert_true(x1 == 1); 
    sala_destruir(s); 
}

static void test_sala_agrega_miembro_dos_veces(void **state){

    Sala *s = sala_crear("Sala"); 

    sala_agregar_miembro(s, "Ana");
    assert_true(sala_agregar_miembro(s, "Ana") == 0); 
    int x1 = sala_num_miembros(s); 
    assert_true(x1 == 1); 

    sala_destruir(s); 
}

static void test_sala_quita_miembro(void **state){

    Sala *s = sala_crear("Sala"); 

    sala_agregar_miembro(s, "Ana"); 
    assert_true(sala_quitar_miembro(s, "Ana") == 1);
    assert_null(sala_obtener_miembro(s, 0)); 
    assert_true(sala_num_miembros(s) == 0);

    sala_destruir(s); 
    
}

static void test_sala_quita_un_no_miembro(void **state){
    Sala *s = sala_crear("Sala"); 
    sala_agregar_miembro(s, "Benito"); 
    sala_agregar_miembro(s, "Cesar");
    sala_agregar_miembro(s, "Daira");  
    assert_true(sala_quitar_miembro(s, "Fer") == 0);
    assert_true(sala_num_miembros(s) == 3); 

    sala_destruir(s); 

}

static void test_sala_vacia(void **state){
    Sala *s = sala_crear("Sala"); 

    sala_agregar_miembro(s, "daira"); 
    assert_true(sala_esta_vacia(s)== 0); 
    sala_quitar_miembro(s,"daira"); 
    assert_true(sala_esta_vacia(s) == 1); 

    sala_destruir(s); 
}

static void test_sala_obten_miembro(void **state){
    Sala *s = sala_crear("Sala"); 
    sala_agregar_miembro(s, "Aleyda"); 
    sala_agregar_miembro(s, "Cesar");
    sala_agregar_miembro(s, "Daira"); 
    
    assert_string_equal(sala_obtener_miembro(s, 0), "Aleyda");
    assert_string_equal(sala_obtener_miembro(s, 1), "Cesar");
    assert_string_equal(sala_obtener_miembro(s, 2), "Daira");
    
    assert_null(sala_obtener_miembro(s, 10)); 
    assert_null(sala_obtener_miembro(s, -10)); 

    sala_destruir(s);

}

//tests de miembros que aun no se han unido 


/*
    Aqui hay un caso chistoso, que pasa si 
    un cliente se intenta unir,  pero quitas la invitacion
    y justo en ese momento se une? que sucede? creo que 
    eso es a nivel de hilos de ejecucion, vere como testeo eso xd

*/

static void test_sala_prueba_invitado(void **state){
    Sala *s = sala_crear("Sala"); 

    assert_true(sala_agregar_invitado(s, "daira") == 1);
    assert_true(sala_tiene_invitado(s, "daira") == 1);
    assert_true(sala_agregar_invitado(s, "daira") == 0);
    assert_true(sala_quitar_invitado(s, "cesar") ==0 ); 

    sala_destruir(s); 

}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_obtener_nombre),
        cmocka_unit_test(test_sala_esta_vacia),
        cmocka_unit_test(test_sala_nombre_valido),
        cmocka_unit_test(test_sala_tiene_miembro),
        cmocka_unit_test(test_sala_agrega_miembro),
        cmocka_unit_test(test_sala_agrega_miembro_dos_veces),
        cmocka_unit_test(test_sala_quita_miembro),
        cmocka_unit_test(test_sala_quita_un_no_miembro),
        cmocka_unit_test(test_sala_vacia),
        cmocka_unit_test(test_sala_obten_miembro),
        cmocka_unit_test(test_sala_prueba_invitado),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
