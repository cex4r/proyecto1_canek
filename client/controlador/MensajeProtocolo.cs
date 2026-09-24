using chatComun;

namespace client.controlador;

/// <summary>
/// Traduce cada operación del protocolo (IDENTIFY, STATUS, TEXT, etc.) a su
/// mensaje JSON correspondiente y lo manda a través de <see cref="Networking"/>.
/// Es la única clase del cliente que conoce la forma exacta de los mensajes
/// que se envían al servidor; el resto del cliente llama a estos métodos
/// por su nombre y no arma JSON directamente.
/// </summary>
public class MensajeProtocolo
{
    private readonly Networking _red;

    public MensajeProtocolo(Networking red)
    {
        _red = red;
    }

    public void Identify(string username) =>
        Enviar(new { type = "IDENTIFY", username });

    public void Status(EstadoUsuario status) =>
        Enviar(new { type = "STATUS", status = status.ToString() });

    public void Users() =>
        Enviar(new { type = "USERS" });

    public void Text(string username, string text) =>
        Enviar(new { type = "TEXT", username, text });

    public void PublicText(string text) =>
        Enviar(new { type = "PUBLIC_TEXT", text });

    public void NewRoom(string roomname) =>
        Enviar(new { type = "NEW_ROOM", roomname });

    public void Invite(string roomname, IEnumerable<string> usernames) =>
        Enviar(new { type = "INVITE", roomname, usernames = usernames.ToArray() });

    public void JoinRoom(string roomname) =>
        Enviar(new { type = "JOIN_ROOM", roomname });

    public void RoomUsers(string roomname) =>
        Enviar(new { type = "ROOM_USERS", roomname });

    public void RoomText(string roomname, string text) =>
        Enviar(new { type = "ROOM_TEXT", roomname, text });

    public void LeaveRoom(string roomname) =>
        Enviar(new { type = "LEAVE_ROOM", roomname });

    public void Disconnect() =>
        Enviar(new { type = "DISCONNECT" });

    private void Enviar(object mensaje) => _red.EnviarLinea(SerializadorJson.Serializar(mensaje));
}
