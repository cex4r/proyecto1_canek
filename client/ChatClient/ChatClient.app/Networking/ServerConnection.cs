
using System;
using System.Net.Sockets;
using System.Text;
 
namespace ChatClient.App.Networking
{
    /// <summary>
    /// Envuelve la conexión TCP con el servidor. Ahora que hablamos
    /// el protocolo real, lee y escribe por LÍNEAS completas
    /// (delimitadas por '\n'), en vez de bytes crudos como en el
    /// milestone bare-bones — StreamReader/StreamWriter hacen del
    /// lado del cliente el mismo trabajo que BufferLineas hace a
    /// mano del lado del servidor en C.
    /// </summary>
    public class ServerConnection
    {
        private TcpClient? _cliente;
        private NetworkStream? _flujo;
        private StreamReader? _lector;
        private StreamWriter? _escritor;
 
        /// <summary>
        /// Abre la conexión TCP contra el servidor y prepara los
        /// lectores/escritores de línea sobre el mismo stream.
        /// </summary>
        public void Conectar(string host, int puerto)
        {
            _cliente = new TcpClient();
            _cliente.Connect(host, puerto);
            _flujo = _cliente.GetStream();
 
            /* UTF8Encoding(false) = sin BOM (Byte Order Mark): el
               protocolo no espera esos bytes extra al inicio.
 
               leaveOpen: true en AMBOS es importante — _lector y
               _escritor envuelven el MISMO stream. Sin leaveOpen,
               el primero que se cierre (Dispose) cerraría también
               el NetworkStream de abajo, y el segundo Dispose
               fallaría o sería inútil. Cerramos el stream nosotros
               mismos, explícitamente, en Desconectar(). */
            var utf8SinBom = new UTF8Encoding(false);
 
            _lector = new StreamReader(
                _flujo, utf8SinBom, detectEncodingFromByteOrderMarks: false,
                bufferSize: 1024, leaveOpen: true);
 
            _escritor = new StreamWriter(_flujo, utf8SinBom, bufferSize: 1024, leaveOpen: true)
            {
                AutoFlush = true, // cada WriteLine se manda de inmediato, sin esperar a llenar el buffer
                NewLine = "\n",   // el protocolo exige '\n' — el default de .NET en Windows sería "\r\n"
            };
        }
 
        /// <summary>
        /// Manda 'mensajeJson' seguido de '\n'. El texto NO debe
        /// traer el '\n' incluido — WriteLine ya lo agrega (usando
        /// el NewLine="\n" configurado arriba).
        /// </summary>
        public void EnviarMensaje(string mensajeJson)
        {
            if (_escritor == null)
            {
                throw new InvalidOperationException(
                    "No hay conexión activa. Llama a Conectar primero.");
            }
 
            _escritor.WriteLine(mensajeJson);
        }
 
        /// <summary>
        /// Bloquea hasta recibir una línea completa (sin el '\n').
        /// Devuelve null si el servidor cerró la conexión — esa es
        /// la convención de StreamReader.ReadLine(), distinta de
        /// como lo manejábamos con NetworkStream.Read() crudo (que
        /// devolvía 0 bytes en vez de null).
        /// </summary>
        public string? LeerLinea()
        {
            if (_lector == null)
            {
                throw new InvalidOperationException(
                    "No hay conexión activa. Llama a Conectar primero.");
            }
 
            return _lector.ReadLine();
        }
 
        /// <summary>
        /// Cierra lector, escritor, stream y socket, en ese orden.
        /// </summary>
        public void Desconectar()
        {
            _lector?.Dispose();
            _escritor?.Dispose();
            _flujo?.Close();
            _cliente?.Close();
        }
    }
}