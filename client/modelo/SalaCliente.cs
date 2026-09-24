using chatComun;

namespace client.modelo;

/// <summary>
/// Vista local a la sala que se ha unido el cliente
/// </summary>
public class SalaCliente
{
    public string Nombre {get;}
    public Dictionary<string, EstadoUsuario> Usuarios{get; } = new();
    public SalaCliente(string nombre)
    {
        Nombre = nombre; 
    }
}