namespace server.modelo;

/// <summary>
/// Representa una sala de chat. Mantiene el conjunto de usuarios que ya se
/// unieron y el conjunto de usuarios invitados que aún no se han unido.
/// El acceso concurrente a estos conjuntos se protege con un lock propio de
/// la sala, tomado siempre a través de <see cref="Servidor"/>.
/// </summary>
public class Sala
{
    public string Nombre { get; }

    /// <summary>Usuarios que ya están dentro de la sala.</summary>
    public HashSet<string> Usuarios { get; } = new();

    /// <summary>Usuarios invitados que todavía no se han unido.</summary>
    public HashSet<string> Invitados { get; } = new();

    public Sala(string nombre)
    {
        Nombre = nombre;
    }
}