using System.Collections.Concurrent; 
using chatComun; 

namespace server.modelo; 

/*
    resultado generico de una operacion del protocolo: el codigo que se debe 
    colocar en el "result" y opcionalmente, el "extra" asociado
*/

public readonly record struct ResultadoOperacion(string Resultado, string? Extra = null)
{
    public static ResultadoOperacion Exito(string? extra = null) => new ("SUCCESS", extra); 
}


/*
    raiz de agregado del modelo del servidor, contiene el estado compartido de todos losusuarios
    conectados y todas las salas, y expone la logica del negocio descrita e nel protocolo, cada
    metodo es seguro llamarse concurrentemente desde los distintos hilos que atienden a cada cliente 

    la sincronizacion se ehace por un lock global ya que algunas operaciones mañosas del protoclo complican 
    las cosas, entonoces un lock por coleccion seria dificil y toma mas tiempo cosa que no tengo en este
    momento, fuck canek XD

*/

public class Servidor
{
    private readonly object _lock = new(); 
    private readonly Dictionary<string, Usuario> _usuarios = new(); 
    private readonly Dictionary<string, Sala> _salas = new(); 
    public const int LimiteNombreUsuario = 8; 
    public const int LimiteNombreSala = 16; 


    // ---------------------------------------------------------------
    // IDENTIFY
    // ---------------------------------------------------------------

public ResultadoOperacion Identificar(Usuario usuario)
    {
        lock (_lock)
        {
            if(_usuarios.ContainsKey(usuario.Nombre))
                return new ResultadoOperacion("USER_ALREADY_EXISTS", usuario.Nombre);

            _usuarios[usuario.Nombre] = usuario; 
            NotificarATodosMenos(usuario.Nombre, new 
            {
            type = "NEW_USER", 
            username = usuario.Nombre
            }); 
            return ResultadoOperacion.Exito(usuario.Nombre);
        }
    }

    // ---------------------------------------------------------------
    // STATUS
    // ---------------------------------------------------------------

    public void CambiarEstado(Usuario usuario, EstadoUsuario nuevoEstado)
    {
        lock (_lock)
        {
            if (usuario.Estado == nuevoEstado)
                return;

            usuario.Estado = nuevoEstado;
            NotificarATodosMenos(usuario.Nombre, new
            {
                type = "NEW_STATUS",
                username = usuario.Nombre,
                status = nuevoEstado.ToString()
            });
        }
    }


    // ---------------------------------------------------------------
    // USERS
    // ---------------------------------------------------------------

    public Dictionary<string, string> ObtenerUsuarios()
    {
        lock (_lock)
        {
            return _usuarios.Values.ToDictionary(u => u.Nombre, u => u.Estado.ToString());
        }
    }

    // ---------------------------------------------------------------
    // TEXT
    // ---------------------------------------------------------------

    public ResultadoOperacion EnviarTextoPrivado(Usuario emisor, string destinatario, string texto)
    {
        lock (_lock)
        {
            if (!_usuarios.TryGetValue(destinatario, out var receptor))
                return new ResultadoOperacion("NO_SUCH_USER", destinatario);

            receptor.Enviar(SerializadorJson.Serializar(new
            {
                type = "TEXT_FROM",
                username = emisor.Nombre,
                text = texto
            }));
            return ResultadoOperacion.Exito();
        }
    }

    // ---------------------------------------------------------------
    // PUBLIC_TEXT
    // ---------------------------------------------------------------

    public void EnviarTextoPublico(Usuario emisor , string texto)
    {
        lock (_lock)
        {
            NotificarATodosMenos(emisor.Nombre, new
            {
                type = "PUBLIC_TEXT_FROM",
                username = emisor.Nombre,
                text = texto
            });
        }
    }


    // ---------------------------------------------------------------
    // NEW_ROOM
    // ---------------------------------------------------------------

    public ResultadoOperacion CrearSala(Usuario creador, string nombreSala)
    {
        lock (_lock)
        {
            if (_salas.ContainsKey(nombreSala))
                return new ResultadoOperacion("ROOM_ALREADY_EXISTS", nombreSala);

            var sala = new Sala(nombreSala);
            sala.Usuarios.Add(creador.Nombre);
            _salas[nombreSala] = sala;
            creador.SalasUnidas.Add(nombreSala);
            return ResultadoOperacion.Exito(nombreSala);
        }
    }

    // ---------------------------------------------------------------
    // INVITE
    // ---------------------------------------------------------------

    public ResultadoOperacion Invitar(Usuario invitador, string nombreSala, IEnumerable<string> nombresUsuarios)
    {
        lock (_lock)
        {
            if (!_salas.TryGetValue(nombreSala, out var sala))
                return new ResultadoOperacion("NO_SUCH_ROOM", nombreSala);

            // Primera pasada: validar que todos los usuarios existan.
            foreach (var nombre in nombresUsuarios)
            {
                if (!_usuarios.ContainsKey(nombre))
                    return new ResultadoOperacion("NO_SUCH_USER", nombre);
            }

            foreach (var nombre in nombresUsuarios)
            {
                if (sala.Usuarios.Contains(nombre) || sala.Invitados.Contains(nombre))
                    continue; // ya está en la sala o ya fue invitado: se ignora

                sala.Invitados.Add(nombre);
                _usuarios[nombre].Enviar(SerializadorJson.Serializar(new
                {
                    type = "INVITATION",
                    username = invitador.Nombre,
                    roomname = nombreSala
                }));
            }

            return ResultadoOperacion.Exito(nombreSala);
        }
    }

    // ---------------------------------------------------------------
    // JOIN_ROOM
    // ---------------------------------------------------------------


    public ResultadoOperacion UnirseASala(Usuario usuario, string nombreSala)
    {
        lock (_lock)
        {
            if (!_salas.TryGetValue(nombreSala, out var sala))
                return new ResultadoOperacion("NO_SUCH_ROOM", nombreSala);

            if (!sala.Invitados.Remove(usuario.Nombre) && !sala.Usuarios.Contains(usuario.Nombre))
                return new ResultadoOperacion("NOT_INVITED", nombreSala);

            sala.Usuarios.Add(usuario.Nombre);
            usuario.SalasUnidas.Add(nombreSala);

            NotificarASalaMenos(sala, usuario.Nombre, new
            {
                type = "JOINED_ROOM",
                roomname = nombreSala,
                username = usuario.Nombre
            });

            return ResultadoOperacion.Exito(nombreSala);
        }
    }


    // ---------------------------------------------------------------
    // ROOM_USERS
    // ---------------------------------------------------------------

    public (ResultadoOperacion resultado, Dictionary<string, string>? usuarios) ObtenerUsuariosDeSala(
        Usuario usuario, string nombreSala)
    {
        lock (_lock)
        {
            if (!_salas.TryGetValue(nombreSala, out var sala))
                return (new ResultadoOperacion("NO_SUCH_ROOM", nombreSala), null);

            if (!sala.Usuarios.Contains(usuario.Nombre))
                return (new ResultadoOperacion("NOT_JOINED", nombreSala), null);

            var dict = sala.Usuarios.ToDictionary(n => n, n => _usuarios[n].Estado.ToString());
            return (ResultadoOperacion.Exito(), dict);
        }
        
    }



    // ---------------------------------------------------------------
    // ROOM_TEXT
    // ---------------------------------------------------------------
    public ResultadoOperacion EnviarTextoSala(Usuario emisor, string nombreSala, string texto)
    {
        lock (_lock)
        {
            if (!_salas.TryGetValue(nombreSala, out var sala))
                return new ResultadoOperacion("NO_SUCH_ROOM", nombreSala);

            if (!sala.Usuarios.Contains(emisor.Nombre))
                return new ResultadoOperacion("NOT_JOINED", nombreSala);

            NotificarASalaMenos(sala, emisor.Nombre, new
            {
                type = "ROOM_TEXT_FROM",
                roomname = nombreSala,
                username = emisor.Nombre,
                text = texto
            });

            return ResultadoOperacion.Exito();
        }
    }

    // ---------------------------------------------------------------
    // LEAVE_ROOM
    // ---------------------------------------------------------------
    public ResultadoOperacion AbandonarSala(Usuario usuario , string nombreSala)
    {
        lock (_lock)
        {
            if (!_salas.TryGetValue(nombreSala, out var sala))
                return new ResultadoOperacion("NO_SUCH_ROOM", nombreSala);

            if (!sala.Usuarios.Contains(usuario.Nombre))
                return new ResultadoOperacion("NOT_JOINED", nombreSala);

            QuitarDeSala(usuario, sala);
            return ResultadoOperacion.Exito();
        }
    }


    // ---------------------------------------------------------------
    // DISCONNECT / cierre de conexión
    // ---------------------------------------------------------------
    public void Desconectar(Usuario usuario)
    {
        lock (_lock)
        {
            if (!_usuarios.Remove(usuario.Nombre))
                return; // nunca llegó a identificarse del todo

            foreach (var nombreSala in usuario.SalasUnidas.ToList())
            {
                if (_salas.TryGetValue(nombreSala, out var sala))
                    QuitarDeSala(usuario, sala);
            }

            NotificarATodosMenos(usuario.Nombre, new
            {
                type = "DISCONNECTED",
                username = usuario.Nombre
            });
        }
    }


    // ---------------------------------------------------------------
    // Helpers internos (siempre invocados ya con _lock tomado)
    // ---------------------------------------------------------------

    private void QuitarDeSala(Usuario usuario, Sala sala)
    {
        sala.Usuarios.Remove(usuario.Nombre);
        sala.Invitados.Remove(usuario.Nombre);
        usuario.SalasUnidas.Remove(sala.Nombre);

        NotificarASalaMenos(sala, usuario.Nombre, new
        {
            type = "LEFT_ROOM",
            roomname = sala.Nombre,
            username = usuario.Nombre
        });

        if (sala.Usuarios.Count == 0)
            _salas.Remove(sala.Nombre);
    }

    private void NotificarATodosMenos(string nombreExcluido, object mensaje)
    {
        string json = SerializadorJson.Serializar(mensaje);
        foreach (var usuario in _usuarios.Values)
        {
            if (usuario.Nombre != nombreExcluido)
                usuario.Enviar(json);
        }
    }

    private void NotificarASalaMenos(Sala sala, string nombreExcluido, object mensaje)
    {
        string json = SerializadorJson.Serializar(mensaje);
        foreach (var nombre in sala.Usuarios)
        {
            if (nombre != nombreExcluido && _usuarios.TryGetValue(nombre, out var usuario))
                usuario.Enviar(json);
        }
    }

}

