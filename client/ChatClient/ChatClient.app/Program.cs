using System; 
using ChatClient.App.Networking;

namespace ChatClient.App;

internal class Program
{
    private const string Host = "127.0.0.1";
    private const int Puerto = 5000; 

    private static void Main(string[] args)
    {
        var conexion = new ServerConnection();

        try
        {
            Console.WriteLine($"Conectando a {Host}:{Puerto}..."); 
            conexion.Conectar(Host, Puerto); 
            Console.WriteLine("Conectado con éxito.");

            string mensaje = "hola mundo";
            Console.WriteLine($"Enviando: {mensaje.TrimEnd()}");
            
            
            conexion.EnviarMensaje(mensaje);
            
            string respuesta = conexion.LeerRespuesta(); 
            Console.WriteLine($"Se recibió: {respuesta.TrimEnd()}");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
        }
        finally
        {
            conexion.Desconectar();
            Console.WriteLine("Conexión cerrada.");
        }
    }
}