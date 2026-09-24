using chatComun;

///<summary>
/// Modelo del cliente: guarda practicamente todos los datos de si mismo 
/// el controlador se encarga de manejar las conexiones, tanto recibir como 
/// mandar cosas al servidor. 
/// La vista lo consulta solo para que el usuario no vea cosas raras y pueda dibujar
/// facilmente.
/// </summary>



public class EstadoAplicacion
{
    public string? nombreUsuario{ get; set; }
    public EstadoUsuario EstadoPropio{get; set; } = EstadoUsuario.ACTIVE; 
    public bool Identificado => NombreUsuario is not null;  
    /// <summary>
    /// El estado de los usuarios en el servidor 
    /// </summary>
    public Dictionary<string, EstadoUsuario>Usuarios {get; } = new();
    /// <summary>
    /// Las salas a las que se ha unido el cliente
    /// </summary>
    public Dictionary<string, SalaCliente> Salas {get;} = new();
    public void RegistrarUsuario(string nombre, EstadoUsuario estado) => Usuarios[nombre] = estado; 
    public void QuitarUsuario(string nombre) => Usuarios.Remove(nombre);
    public SalaCliente ObtenerOCrearSala(string nombre)
    {
        if(!Salas.TryGetValue(nombre, out var sala))
        {
            sala = new SalaCliente(nombre);
            Salas[nombre] = sala; 
        }
        return sala; 
    }

    public void QuitarSala(string nombre) => Salas.Remove(nombre);  



}