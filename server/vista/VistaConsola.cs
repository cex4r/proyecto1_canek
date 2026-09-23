namespace server.vista;
/*

    Simplemente muestra las conexiones y desconexiones de cada usuario en la consola
    el servidor no va a ser interactivo, creo que eso seria mala idea, para el 
    tiempo que me queda xd
*/

public static class VistaConsola
{
    private static readonly object ConsolaLock = new(); 

    public static void Log(string mensaje)
    {
        lock (ConsolaLock)
        {
            Console.WriteLine($"[{DateTime.Now:HH:mm:ss}] {mensaje}");
        }
    } 
}