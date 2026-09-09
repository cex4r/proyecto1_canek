/*
    propiedades que conforman un usuario:

    -username  ------ (maximo 8 caracteres, estado de saber si el username esta en uso)
    -status    ------ (al conectrse por default esta en ACTIVE, necesario para reportar a los demas usuarios del su propio estado de conexion)
    -indentificacion del state ------ (es lo primero que se tiene que verificar)
    -set de habitaciones en las que se ha unido 
    -set de habitaciones pendientes en las que ha estado
    -socket del usuario para el controlador

*/


int usuario() {


    //getStatusUsuario regresa el estado de el usuario si esta en Busy, Away o Active .
    int getStatusUsuario() {
        return 0; 
    } 


    //Regresa una lista en donde el usuario se ha unido
    int getSalasVisitadas() {
        return 0; 
    }


    //Regresa si el usuario ya ha sido indentificado (se ha puesto un username correctamente)
    int getEstadoUsuario() {
        return 0; 
    }

    //Regresa una lista de invitaciones donde el usuario aun no se ha unido 
    int getInvitaciones() {
        return 0; 
    }





}