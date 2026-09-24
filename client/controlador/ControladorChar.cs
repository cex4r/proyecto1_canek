using System.Text.Json.Nodes;
using chatComun;
using client.modelo;
using client.vista;

namespace client.controlador;

/// <summary>
/// Controlador principal del cliente (patrón MVC): recibe los mensajes ya
/// parseados de <see cref="Networking"/>, actualiza el <see cref="EstadoAplicacion"/>
/// (modelo) según corresponda y le pide a <see cref="VistaConsola"/> que
/// muestre lo que haya que mostrar. También recibe los comandos que el
/// usuario escribe en la vista y los traduce en llamadas a
/// <see cref="MensajeProtocolo"/>.
/// </summary>
public class ControladorChat
{
    private readonly Networking _red;
    private readonly MensajeProtocolo _protocolo;
    private readonly EstadoAplicacion _estado;
    private readonly VistaConsola _vista;

    public ControladorChat(Networking red, MensajeProtocolo protocolo, EstadoAplicacion estado, VistaConsola vista)
    {
        _red = red;
        _protocolo = protocolo;
        _estado = estado;
        _vista = vista;

        _red.MensajeRecibido += ManejarMensaje;
        _red.Desconectado += () => _vista.MostrarSistema("Se perdió la conexión con el servidor.");
    }

    public void Conectar(string host, int puerto) => _red.Conectar(host, puerto);

    public void Identificarse(string nombre)
    {
        _estado.NombreUsuario = nombre;
        _protocolo.Identify(nombre);
    }

    // ---------------------------------------------------------------
    // Comandos que llegan desde la vista (entrada del usuario)
    // ---------------------------------------------------------------

    public void CambiarEstado(EstadoUsuario estado) => _protocolo.Status(estado);
    public void PedirUsuarios() => _protocolo.Users();
    public void EnviarTexto(string usuario, string texto) => _protocolo.Text(usuario, texto);
    public void EnviarTextoPublico(string texto) => _protocolo.PublicText(texto);
    public void CrearSala(string sala) => _protocolo.NewRoom(sala);
    public void Invitar(string sala, IEnumerable<string> usuarios) => _protocolo.Invite(sala, usuarios);
    public void UnirseASala(string sala) => _protocolo.JoinRoom(sala);
    public void PedirUsuariosDeSala(string sala) => _protocolo.RoomUsers(sala);
    public void EnviarTextoSala(string sala, string texto) => _protocolo.RoomText(sala, texto);
    public void AbandonarSala(string sala) => _protocolo.LeaveRoom(sala);

    public void Desconectarse()
    {
        _protocolo.Disconnect();
        _red.Cerrar();
    }

    // ---------------------------------------------------------------
    // Mensajes que llegan del servidor
    // ---------------------------------------------------------------

    private void ManejarMensaje(JsonObject mensaje)
    {
        string tipo = mensaje["type"]?.GetValue<string>() ?? "";

        switch (tipo)
        {
            case "RESPONSE":
                ManejarRespuesta(mensaje);
                break;
            case "NEW_USER":
                {
                    string nombreNuevo = Cadena(mensaje, "username");
                    _estado.RegistrarUsuario(nombreNuevo, EstadoUsuario.ACTIVE);
                    _vista.MostrarSistema($"{nombreNuevo} se conectó.");
                }
                break;
            case "NEW_STATUS":
                {
                    string nombre = Cadena(mensaje, "username");
                    var nuevoEstado = Enum.Parse<EstadoUsuario>(Cadena(mensaje, "status"));
                    _estado.RegistrarUsuario(nombre, nuevoEstado);
                    _vista.MostrarSistema($"{nombre} ahora está {nuevoEstado}.");
                }
                break;
            case "USER_LIST":
                ActualizarListaUsuarios(mensaje);
                _vista.MostrarUsuarios(_estado.Usuarios);
                break;
            case "TEXT_FROM":
                _vista.MostrarTextoPrivado(Cadena(mensaje, "username"), Cadena(mensaje, "text"));
                break;
            case "PUBLIC_TEXT_FROM":
                _vista.MostrarTextoPublico(Cadena(mensaje, "username"), Cadena(mensaje, "text"));
                break;
            case "INVITATION":
                {
                    string invitador = Cadena(mensaje, "username");
                    string salaInvitada = Cadena(mensaje, "roomname");
                    _vista.MostrarSistema(
                        $"{invitador} te invitó a la sala '{salaInvitada}'. Usa /join {salaInvitada} para entrar.");
                }
                break;
            case "JOINED_ROOM":
                {
                    string sala = Cadena(mensaje, "roomname");
                    string usuario = Cadena(mensaje, "username");
                    _estado.ObtenerOCrearSala(sala).Usuarios[usuario] = EstadoUsuario.ACTIVE;
                    _vista.MostrarSistema($"{usuario} se unió a la sala '{sala}'.");
                }
                break;
            case "ROOM_USER_LIST":
                ActualizarUsuariosDeSala(mensaje);
                break;
            case "ROOM_TEXT_FROM":
                _vista.MostrarTextoSala(Cadena(mensaje, "roomname"), Cadena(mensaje, "username"), Cadena(mensaje, "text"));
                break;
            case "LEFT_ROOM":
                {
                    string sala = Cadena(mensaje, "roomname");
                    string usuario = Cadena(mensaje, "username");
                    if (_estado.Salas.TryGetValue(sala, out var salaCliente))
                        salaCliente.Usuarios.Remove(usuario);
                    _vista.MostrarSistema($"{usuario} salió de la sala '{sala}'.");
                }
                break;
            case "DISCONNECTED":
                {
                    string usuario = Cadena(mensaje, "username");
                    _estado.QuitarUsuario(usuario);
                    _vista.MostrarSistema($"{usuario} se desconectó.");
                }
                break;
            default:
                _vista.MostrarSistema($"Mensaje no reconocido del servidor: {mensaje}");
                break;
        }
    }

    private void ManejarRespuesta(JsonObject mensaje)
    {
        string operacion = Cadena(mensaje, "operation");
        string resultado = Cadena(mensaje, "result");
        string? extra = mensaje["extra"]?.GetValue<string>();

        if (operacion == "IDENTIFY" && resultado == "SUCCESS")
        {
            _vista.MostrarSistema($"Identificado como '{extra}'.");
            return;
        }

        if (resultado == "SUCCESS")
        {
            _vista.MostrarSistema($"{operacion}: éxito ({extra}).");
        }
        else
        {
            _vista.MostrarError($"{operacion} falló: {resultado}" + (extra is null ? "" : $" ({extra})"));

            if (operacion == "IDENTIFY")
                _estado.NombreUsuario = null; // no se pudo identificar; se puede reintentar
        }
    }

    private void ActualizarListaUsuarios(JsonObject mensaje)
    {
        var usuarios = mensaje["users"]!.AsObject();
        foreach (var (nombre, valor) in usuarios)
            _estado.RegistrarUsuario(nombre, Enum.Parse<EstadoUsuario>(valor!.GetValue<string>()));
    }

    private void ActualizarUsuariosDeSala(JsonObject mensaje)
    {
        string sala = Cadena(mensaje, "roomname");
        var salaCliente = _estado.ObtenerOCrearSala(sala);
        salaCliente.Usuarios.Clear();
        foreach (var (nombre, valor) in mensaje["users"]!.AsObject())
            salaCliente.Usuarios[nombre] = Enum.Parse<EstadoUsuario>(valor!.GetValue<string>());

        _vista.MostrarUsuariosDeSala(sala, salaCliente.Usuarios);
    }

    private static string Cadena(JsonObject mensaje, string llave) => mensaje[llave]?.GetValue<string>() ?? "";
}
