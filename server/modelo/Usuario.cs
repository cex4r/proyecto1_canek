using System.Net.Sockets;
using System.Text;
using chatComun;

namespace server.modelo;

/// <summary>
/// Representa a un usuario identificado en el chat. Guarda la conexión de
/// socket subyacente y expone un método seguro para escribirle mensajes,
/// ya que distintos hilos (el propio hilo del cliente y los hilos de otros
/// clientes que quieran notificarle algo) pueden escribirle al mismo tiempo.
/// </summary>
public class Usuario
{
    private readonly NetworkStream _stream;
    private readonly object _escrituraLock = new();

    public string Nombre { get; }
    public EstadoUsuario Estado { get; set; } = EstadoUsuario.ACTIVE;

    /// <summary>Nombres de las salas a las que el usuario se ha unido.</summary>
    public HashSet<string> SalasUnidas { get; } = new();

    public Usuario(string nombre, NetworkStream stream)
    {
        Nombre = nombre;
        _stream = stream;
    }

    /// <summary>
    /// Envía una línea (mensaje JSON ya serializado) al cliente de este
    /// usuario, agregando el salto de línea delimitador. Es seguro para
    /// llamarse concurrentemente desde varios hilos.
    /// </summary>
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
                // El socket ya se cerró; el hilo dueño de esta conexión se
                // encargará de limpiar el estado del usuario.
            }
            catch (ObjectDisposedException)
            {
            }
        }
    }
}
