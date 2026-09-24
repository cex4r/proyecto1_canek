using chatComun;

namespace client.modelo;

/// <summary>
/// Modelo del cliente: guarda todo lo que este cliente sabe sobre sí mismo
/// y sobre el resto del chat (usuarios conocidos y salas a las que se ha
/// unido). El controlador de red actualiza este estado según van llegando
/// mensajes del servidor; la vista lo consulta para dibujar listas de
/// usuarios y salas cuando el usuario lo pide.
///
/// No conoce nada de sockets ni de JSON: solo mantiene datos.
/// </summary>
public class EstadoAplicacion
{
    public string? NombreUsuario { get; set; }
    public EstadoUsuario EstadoPropio { get; set; } = EstadoUsuario.ACTIVE;

    public bool Identificado => NombreUsuario is not null;

    /// <summary>Todos los usuarios conocidos del chat y su estado, según USER_LIST/NEW_USER/NEW_STATUS.</summary>
    public Dictionary<string, EstadoUsuario> Usuarios { get; } = new();

    /// <summary>Salas a las que este cliente se ha unido.</summary>
    public Dictionary<string, SalaCliente> Salas { get; } = new();

    public void RegistrarUsuario(string nombre, EstadoUsuario estado) => Usuarios[nombre] = estado;

    public void QuitarUsuario(string nombre) => Usuarios.Remove(nombre);

    public SalaCliente ObtenerOCrearSala(string nombre)
    {
        if (!Salas.TryGetValue(nombre, out var sala))
        {
            sala = new SalaCliente(nombre);
            Salas[nombre] = sala;
        }
        return sala;
    }

    public void QuitarSala(string nombre) => Salas.Remove(nombre);
}