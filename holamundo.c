#include <stdio.h>
//probando como traer clases xd; 
#include "resta.c"

void sumar(int primero, int segundo){
        printf("%d\n", primero + segundo); 
    }

int multiplicar(int p, int s){
    return p * s;
}

int main() {

    // Aprendiendo a hacer comentarios en C 
    printf("Hola mundo\n");

    //Poniendo que tenemos muchos caracteres
    char libro[] = "EL programador pragmatico";

    //Imprimiendo un string o chars
    printf("%s\n", libro); 
    printf("imprimi un char\n");

    //imprimiendo numeros

    int entero = 0;
    printf("%d\n",entero);
    

    float deci = 0.01;
    printf("%f\n", deci);


    //haciendo una lista de numeros

    int numeros[] = {1,2,3,4,5,6}; 
    printf("%d\n", numeros[0]);


    //lista de chars
    char *animales[] = {"perro", "gato"}; 
    printf("%s", animales[1]); 


    //aprendiendo sobre constantes
    //este no se puede cambiar
    const float pi = 3.14;
    printf("%f\n", pi); 


    //operadores
    //lo mismo que en Java xd 

    printf("%d\n", 4 == 4); //1 verdadero 


    //condicionales

    int autorizado = 1; 

    if(autorizado) {
        printf("Puede ingresar\n"); 
    } else if (autorizado == 1){
        printf("funciona igual que Java\n"); 
    } else {
        printf("puto el que lo lea\n"); 
    }



    //viendo como el switch

    char color = 'A'; 

    switch (color)
    {
    case 'A':
        printf("Exito\n"); 
        break;
    
    default:
        break;
    }



    //Bucles 

    //Igual que java xd 
    for(int i = 0; i < 2; i++){
        printf("%s\n", animales[i]); 
    }

    for(int i = 0; i < 5; i++){
        printf("%d\n", numeros[i]); 
    }

    int ent = 1; 

    int emergencia = 5; 
    while(entero <= emergencia) {
        printf("%d\n", entero);
        entero++; 
    }

    //viendo como funcionan las fuciones xd 

    sumar(1,2);
    sumar(5,10);
    
    printf("%d\n",multiplicar(0, 1)); 

    //proband la funcion resta

    printf("%d\n",restar(4,2)); 



    return 0;
}


