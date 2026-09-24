using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;
using chatComun;

namespace client.controlador;

/// <summary>
/// Clase dedicada exclusivamente al networking: abre la conexión TCP al
/// servidor, expone un método thread-safe para enviarle una línea, y
/// arranca un hilo en segundo plano que lee continuamente mensajes
/// entrantes (uno por línea, vía <see cref="BufferLineas"/>) y los entrega
/// ya parseados como <see cref="JsonObject"/> a través del evento
/// <see cref="MensajeRecibido"/>.
///
/// Esta clase no sabe nada del protocolo del chat (no conoce "IDENTIFY" ni
/// "TEXT"); eso es responsabilidad de <see cref="MensajeProtocolo"/> para
/// enviar, y del controlador principal para interpretar lo recibido.
/// </summary>
public class Networking
{
    private TcpClient? _tcpClient;
    private NetworkStream? _stream;
    private readonly object _escrituraLock = new();

    public bool Conectado { get; private set; }

    /// <summary>Se dispara en el hilo lector cada vez que llega un mensaje completo del servidor.</summary>
    public event Action<JsonObject>? MensajeRecibido;

    /// <summary>Se dispara cuando la conexión se cierra (ya sea porque el servidor la cerró o por error).</summary>
    public event Action? Desconectado;

    public void Conectar(string host, int puerto)
    {
        _tcpClient = new TcpClient();
        _tcpClient.Connect(host, puerto);
        _stream = _tcpClient.GetStream();
        Conectado = true;

        var hiloLector = new Thread(CicloLectura) { IsBackground = true };
        hiloLector.Start();
    }

    /// <summary>Envía una línea (mensaje JSON ya serializado) al servidor. Seguro para llamarse desde cualquier hilo.</summary>
    public void EnviarLinea(string json)
    {
        if (_stream is null)
            throw new InvalidOperationException("No hay conexión activa.");

        byte[] bytes = Encoding.UTF8.GetBytes(json + "\n");
        lock (_escrituraLock)
        {
            _stream.Write(bytes, 0, bytes.Length);
            _stream.Flush();
        }
    }

    public void Cerrar()
    {
        Conectado = false;
        _tcpClient?.Close();
    }

    private void CicloLectura()
    {
        var buffer = new BufferLineas(_stream!);
        string? linea;
        while ((linea = buffer.LeerLinea()) != null)
        {
            JsonObject? mensaje;
            try
            {
                mensaje = JsonNode.Parse(linea) as JsonObject;
            }
            catch (System.Text.Json.JsonException)
            {
                continue; // mensaje mal formado del servidor: se ignora
            }

            if (mensaje is not null)
                MensajeRecibido?.Invoke(mensaje);
        }

        Conectado = false;
        Desconectado?.Invoke();
    }
}
