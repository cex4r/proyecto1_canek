using System.Net.Sockets; 
using System.Text; 
using Chat.comun; 

namespace server.modelo;

/*
    Representa a un usuario identificado en el chat, guarda conexion de socket y expone un metodo para
    escribirle mensajes, ya que distintos hilos pueden hacer cosas mañosas.
*/

public class Usuario
{
    //variables para que sea solo de lectura y podamos mandar y recibir cosas del servidor
    private readonly NerworkStream _stream; 
    private readonly object _escrituraLock = new(); 

    private string Nombre { get; }

    //donde vamos a almacenar donde se ha unido el usuario
    public HashSet<string> SalasUnidas { get; } = new(); 

    //constructor de la clase usuario 
    public Usuario(string nombre, NetworkStream stream)
    {
        Nombre = nombre; 
        _stream = stream;
    }

    /*
        Envia una linea, (mensaje Json ya serializado) al cliente de ese usuario
        agregando el salto de linea delimitador, es seguro para llamare concurrentemente desde 
        varios ghilos.
    */

    public void Enviar(string mensajeJson)
    {
        byte[] bytes = Encoding.UTF8.GetBytes(mensajeJson + "\n");
        lock (_escrituraLock)
        {
            try
            {
                _stream.Write(bytes, 0, bytes.Length);
                _stream.Flush();
                
            }
            catch (IOException)
            {
                //en este punto el socket ya se cerro,el hilo dueño de esa conexion se
                //encargara de limpiar el estado del usuario
            }
            catch (ObjectDisposedException)
            {
                
            }
        }
    }
}