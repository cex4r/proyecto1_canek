using System.Net;
using System.Net.Sockets;
using server.controlador;
using server.modelo;
using server.vista;

int puerto = args.Length > 0 && int.TryParse(args[0], out int p) ? p : 5000;

var servidor = new Servidor();
var listener = new TcpListener(IPAddress.Any, puerto);
listener.Start();

VistaConsola.Log($"Servidor de chat escuchando en el puerto {puerto}.");
VistaConsola.Log("Presiona Ctrl+C para detener.");

while (true)
{
    // Accept() bloquea el hilo principal hasta que llega una nueva conexión;
    // cada conexión aceptada se despacha de inmediato a su propio hilo
    // (programación concurrente: un hilo por cliente) para que un cliente
    // lento o inactivo nunca bloquee a los demás.
    TcpClient tcpClient = listener.AcceptTcpClient();
    VistaConsola.Log($"Nueva conexión desde {tcpClient.Client.RemoteEndPoint}.");

    var handler = new ClienteHandler(tcpClient, servidor, VistaConsola.Log);
    var hilo = new Thread(handler.Atender)
    {
        IsBackground = true
    };
    hilo.Start();
}