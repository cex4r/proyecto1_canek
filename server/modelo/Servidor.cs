using System.Collections.Concurrent; 
using Chat.comun; 

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


public ResultadoOperacion Identificar(Usuario usuario)
    {
        lock (_lock)
        {
            if(_usuarios.ContainsKey(usuario.nombre))
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

    public void CambiarEstado(Usuario usuario, EstadoUsuario nuevoEstado)
    {

    }

    public Dictionary<string, string> ObtenerUsuarios()
    {
        
    }

    public ResultadoOperacion EnviarTextoPrivado(Usuario emisor, string destinatario, string texto)
    {
        
    }

    public void EnviarTextoPublico(Usuario emisor , string texto)
    {
        
    }

    public ResultadoOperacion CrearSala(Usuario creador, string nombreSala)
    {
        
    }

    public ResultadoOperacion Invitar(Usuario invitador, string nombreSala, IEnumerable<string> nombresUsuarios)
    {
        
    }

    public ResultadoOperacion UnirseSala(Usuario usuario, string nombreSala)
    {
        
    }

    public (ResultadoOperacion resultado, Dictionary<string, string>? usuarios) ObtenerUsuariosDeSala(
        Usuario usuario, string nombreSala)
    {
        
    }

    public ResultadoOperacion EnviarTextoSala(Usuario emisor, string nombreSala, string texto)
    {
        
    }

    public ResultadoOperacion AbandonarSala(Usuario usuario , string nombreSala)
    {
        
    }

    public void Desconectar(Usuario usuario)
    {
        
    }

    private void QuitarDeSala(Usuario usuario, Sala sala)
    {
        
    }

    private void NotificarATodosMenos(string nombreExcluido, object mensaje)
    {
        
    }

    private void NotificarASalaMenos(Sala sala, string nombreExcluido, object mensaje)
    {
        
    }

}

