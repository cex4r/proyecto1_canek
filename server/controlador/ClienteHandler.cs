using System.Net.Sockets;
using System.Text.Json;
using System.Text.Json.Nodes;
using chatComun;
using server.modelo;

namespace server.controlador;

/*
    Controlador de una conexión de cliente, cada instancia corre su propio hilo, lee líneas JSON 
    del socket con ayuda de la clase BufferLineas, las traduce en llamadas al modelo y escribe de vuelta
    las respuestas que el protocolo indica.
*/

public class ClienteHandler
{
    private readonly TcpClient _tcpClient; 
    private readonly Servidor _servidor; 
    private readonly Action<string> _log; 
    private NetworkStream _stream = null!; 
    private BufferLineas _buffer = null!; 
    private Usuario? _usuario; 

    public ClienteHandler(TcpClient tcpClient, Servidor servidor, Action<string> log)
    {
        _tcpClient = tcpClient;
        _servidor = servidor;
        _log = log;
    }

    /// <summary>Punto de entrada del hilo dedicado a este cliente.</summary>
    public void Atender()
    {
        try
        {
            _stream = _tcpClient.GetStream();
            _buffer = new BufferLineas(_stream);

            string? linea; 
            while ((linea = _buffer.LeerLinea()) != null)
            {
                if (!ProcesarLinea(linea))
                    break; 
            }
        }
        finally
        {
            // CORREGIDO: Se reemplazó el ';' por ',' en los argumentos
            _servidor.Desconectar(_usuario ?? new Usuario("", _stream));
            _tcpClient.Close(); 
            string nombreParaLog = _usuario?.Nombre ?? "sin identificar";
            _log($"Conexión cerrada ({nombreParaLog})");
        }
    }

    /// <summary>Regresa false si, según el protocolo, hay que desconectar al cliente.</summary>
    private bool ProcesarLinea(string linea)
    {
        JsonNode? node;
        try
        {
            // CORREGIDO: Se usó 'node' en lugar de 'nodo'
            node = JsonNode.Parse(linea);
        }
        catch (JsonException)
        {
            EnviarInvalido();
            return false; 
        }

        if (node is not JsonObject obj || obj["type"] is not JsonValue tipoValor
            || !tipoValor.TryGetValue(out string? tipo) || tipo is null)
        {
            EnviarInvalido();
            return false; 
        } 

        if (_usuario is null && tipo != "IDENTIFY")
        {
            EnviarLinea(new { type = "RESPONSE", operation = "INVALID", result = "NOT_IDENTIFIED" });
            return false;
        }

        try
        {
            return tipo switch
            {
                "IDENTIFY" => ManejarIdentify(obj),
                "STATUS" => ManejarStatus(obj),
                "USERS" => ManejarUsers(),
                "TEXT" => ManejarText(obj),
                "PUBLIC_TEXT" => ManejarPublicText(obj),
                "NEW_ROOM" => ManejarNewRoom(obj),
                "INVITE" => ManejarInvite(obj),
                "JOIN_ROOM" => ManejarJoinRoom(obj),
                "ROOM_USERS" => ManejarRoomUsers(obj),
                "ROOM_TEXT" => ManejarRoomText(obj),
                "LEAVE_ROOM" => ManejarLeaveRoom(obj),
                "DISCONNECT" => false,
                _ => Invalido()
            };
        }
        catch (Exception)
        {
            return Invalido();
        }
    }

    // ---------------------------------------------------------------
    // Manejadores por tipo de mensaje
    // ---------------------------------------------------------------

    private bool ManejarIdentify(JsonObject obj)
    {
        string nombre = LeerCadena(obj, "username");

        if (nombre.Length == 0 || nombre.Length > Servidor.LimiteNombreUsuario)
            return Invalido();

        var usuarioCandidato = new Usuario(nombre, _stream);
        var resultado = _servidor.Identificar(usuarioCandidato);

        EnviarLinea(new
        {
            type = "RESPONSE",
            operation = "IDENTIFY",
            result = resultado.Resultado,
            extra = resultado.Extra
        });

        if (resultado.Resultado == "SUCCESS")
        {
            _usuario = usuarioCandidato; 
            _log($"Usuario identificado: {nombre}");
        }

        return true; 
    }

    private bool ManejarStatus(JsonObject obj)
    {
        string texto = LeerCadena(obj, "status");
        if (!Enum.TryParse<EstadoUsuario>(texto, out var estado))
            return Invalido();

        _servidor.CambiarEstado(_usuario!, estado);
        return true; 
    }

    private bool ManejarUsers()
    {
        EnviarLinea(new { type = "USER_LIST", users = _servidor.ObtenerUsuarios() });
        return true;
    }

    private bool ManejarText(JsonObject obj)
    {
        string destinatario = LeerCadena(obj, "username");
        string texto = LeerCadena(obj, "text");
        var resultado = _servidor.EnviarTextoPrivado(_usuario!, destinatario, texto);

        if (resultado.Resultado != "SUCCESS")
        {
            EnviarLinea(new
            {
                type = "RESPONSE",
                operation = "TEXT",
                result = resultado.Resultado,
                extra = resultado.Extra
            });
        }
        return true; 
    }

    private bool ManejarPublicText(JsonObject obj)
    {
        string texto = LeerCadena(obj, "text");
        _servidor.EnviarTextoPublico(_usuario!, texto);
        return true; 
    }

    private bool ManejarNewRoom(JsonObject obj)
    {
        string nombreSala = LeerCadena(obj, "roomname");
        if (nombreSala.Length == 0 || nombreSala.Length > Servidor.LimiteNombreSala)
            return Invalido();

        var resultado = _servidor.CrearSala(_usuario!, nombreSala);
        EnviarLinea(new
        {
            type = "RESPONSE",
            operation = "NEW_ROOM",
            result = resultado.Resultado,
            extra = resultado.Extra
        });
        return true; 
    }

    private bool ManejarInvite(JsonObject obj)
    {
        // CORREGIDO: Se corrigió 'LeerCandena' a 'LeerCadena'
        string nombreSala = LeerCadena(obj, "roomname");
        var arreglo = obj["usernames"] as JsonArray
            ?? throw new FormatException("usernames faltante");

        // CORREGIDO: Se igualó la variable del lambda (n => n!)
        var nombres = arreglo.Select(n => n!.GetValue<string>()).ToList();

        var resultado = _servidor.Invitar(_usuario!, nombreSala, nombres);
        if (resultado.Resultado != "SUCCESS")
        {
            EnviarLinea(new
            {
                type = "RESPONSE",
                operation = "INVITE",
                result = resultado.Resultado,
                extra = resultado.Extra
            });
        }
        
        // CORREGIDO: Se agregó retorno true para cuando la invitación es exitosa
        return true; 
    }

    private bool ManejarJoinRoom(JsonObject obj)
    {
        string nombreSala = LeerCadena(obj, "roomname");
        var resultado = _servidor.UnirseASala(_usuario!, nombreSala);
        EnviarLinea(new
        {
            type = "RESPONSE",
            operation = "JOIN_ROOM",
            result = resultado.Resultado,
            extra = resultado.Extra
        });
        return true; 
    }

    private bool ManejarRoomUsers(JsonObject obj)
    {
        string nombreSala = LeerCadena(obj, "roomname");
        var (resultado, usuarios) = _servidor.ObtenerUsuariosDeSala(_usuario!, nombreSala);
        if (resultado.Resultado == "SUCCESS")
        {
            EnviarLinea(new { type = "ROOM_USER_LIST", roomname = nombreSala, users = usuarios });
        }
        else
        {
            EnviarLinea(new
            {
                type = "RESPONSE",
                operation = "ROOM_USERS",
                result = resultado.Resultado,
                extra = resultado.Extra
            });
        }
        return true; 
    }

    private bool ManejarRoomText(JsonObject obj)
    {
        string nombreSala = LeerCadena(obj, "roomname");
        string texto = LeerCadena(obj, "text");
        var resultado = _servidor.EnviarTextoSala(_usuario!, nombreSala, texto);

        if (resultado.Resultado != "SUCCESS")
        {
            EnviarLinea(new
            {
                type = "RESPONSE",
                operation = "ROOM_TEXT",
                result = resultado.Resultado,
                extra = resultado.Extra
            });
        }
        return true;
    }

    private bool ManejarLeaveRoom(JsonObject obj)
    {
        string nombreSala = LeerCadena(obj, "roomname");
        var resultado = _servidor.AbandonarSala(_usuario!, nombreSala);
        if (resultado.Resultado != "SUCCESS")
        {
            EnviarLinea(new
            {
                type = "RESPONSE",
                operation = "LEAVE_ROOM",
                result = resultado.Resultado,
                extra = resultado.Extra
            });
        }
        return true;
    }

    // ---------------------------------------------------------------
    // Utilidades
    // ---------------------------------------------------------------

    private static string LeerCadena(JsonObject obj, string llave)
    {
        if (obj[llave] is JsonValue valor && valor.TryGetValue(out string? texto) && texto is not null)
            return texto;
        throw new FormatException($"Falta o es inválida la llave '{llave}'");
    }

    private bool Invalido()
    {
        EnviarInvalido();
        return false;
    }

    private void EnviarInvalido()
    {
        EnviarLinea(new { type = "RESPONSE", operation = "INVALID", result = "INVALID" });
    }

    private void EnviarLinea(object mensaje)
    {
        string json = SerializadorJson.Serializar(mensaje);
        if (_usuario is not null)
        {
            _usuario.Enviar(json);
        }
        else
        {
            byte[] bytes = System.Text.Encoding.UTF8.GetBytes(json + "\n");
            _stream.Write(bytes, 0, bytes.Length);
            _stream.Flush();
        }
    }
}