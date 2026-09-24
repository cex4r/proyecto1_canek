using client.controlador;
using client.modelo;
using client.vista;

Console.Write("Servidor (host, Enter para 'localhost'): ");
string host = Console.ReadLine() is { Length: > 0 } h ? h.Trim() : "localhost";

Console.Write("Puerto (Enter para 5000): ");
string puertoTexto = Console.ReadLine() ?? "";
int puerto = int.TryParse(puertoTexto, out int p) ? p : 5000;

Console.Write("Nombre de usuario (máx 8 caracteres): ");
string nombreUsuario = (Console.ReadLine() ?? "").Trim();

var red = new Networking();
var protocolo = new MensajeProtocolo(red);
var estado = new EstadoAplicacion();
var vista = new VistaConsola();
var controlador = new ControladorChat(red, protocolo, estado, vista);
vista.Enlazar(controlador);

try
{
    controlador.Conectar(host, puerto);
}
catch (Exception ex)
{
    Console.WriteLine($"No se pudo conectar a {host}:{puerto}: {ex.Message}");
    return;
}

controlador.Identificarse(nombreUsuario);

// El hilo de Networking sigue leyendo del servidor en segundo plano;
// el hilo principal queda dedicado a leer los comandos del usuario.
vista.CicloDeComandos();
