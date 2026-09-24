using chatComun;
using client.controlador;

namespace client.vista;

/// <summary>
/// Vista del cliente: todo lo que tiene que ver con leer comandos del
/// usuario por consola e imprimir lo que ocurre en el chat. No contiene
/// lógica de protocolo ni de red: cuando el usuario escribe algo, la vista
/// simplemente reconoce el comando y llama al método correspondiente del
/// <see cref="ControladorChat"/>.
/// </summary>
public class VistaConsola
{
    private readonly object _consolaLock = new();
    private ControladorChat _controlador = null!;

    /// <summary>Se asigna después de construir el controlador, ya que ambos se necesitan mutuamente.</summary>
    public void Enlazar(ControladorChat controlador) => _controlador = controlador;

    public void MostrarSistema(string texto)
    {
        lock (_consolaLock)
            Console.WriteLine($"* {texto}");
    }

    public void MostrarError(string texto)
    {
        lock (_consolaLock)
            Console.WriteLine($"! {texto}");
    }

    public void MostrarTextoPrivado(string de, string texto)
    {
        lock (_consolaLock)
            Console.WriteLine($"[privado de {de}] {texto}");
    }

    public void MostrarTextoPublico(string de, string texto)
    {
        lock (_consolaLock)
            Console.WriteLine($"[{de}] {texto}");
    }

    public void MostrarTextoSala(string sala, string de, string texto)
    {
        lock (_consolaLock)
            Console.WriteLine($"[{sala}] {de}: {texto}");
    }

    public void MostrarUsuarios(Dictionary<string, EstadoUsuario> usuarios)
    {
        lock (_consolaLock)
        {
            Console.WriteLine("Usuarios conectados:");
            foreach (var (nombre, estado) in usuarios)
                Console.WriteLine($"  {nombre} ({estado})");
        }
    }

    public void MostrarUsuariosDeSala(string sala, Dictionary<string, EstadoUsuario> usuarios)
    {
        lock (_consolaLock)
        {
            Console.WriteLine($"Usuarios en '{sala}':");
            foreach (var (nombre, estado) in usuarios)
                Console.WriteLine($"  {nombre} ({estado})");
        }
    }

    /// <summary>
    /// Ciclo principal de entrada: lee líneas de la consola e interpreta
    /// comandos con el prefijo '/'. Cualquier línea sin prefijo se manda
    /// como texto público. Corre en el hilo principal mientras el hilo de
    /// <see cref="Networking"/> recibe en segundo plano.
    /// </summary>
    public void CicloDeComandos()
    {
        ImprimirAyuda();
        string? linea;
        while ((linea = Console.ReadLine()) != null)
        {
            linea = linea.Trim();
            if (linea.Length == 0)
                continue;

            if (linea == "/quit")
            {
                _controlador.Desconectarse();
                return;
            }

            try
            {
                InterpretarComando(linea);
            }
            catch (Exception ex)
            {
                MostrarError($"Comando inválido: {ex.Message}");
            }
        }
    }

    private void InterpretarComando(string linea)
    {
        if (!linea.StartsWith('/'))
        {
            _controlador.EnviarTextoPublico(linea);
            return;
        }

        string[] partes = linea.Split(' ', 3, StringSplitOptions.RemoveEmptyEntries);
        string comando = partes[0];

        switch (comando)
        {
            case "/help":
                ImprimirAyuda();
                break;
            case "/status":
                _controlador.CambiarEstado(Enum.Parse<EstadoUsuario>(partes[1].ToUpperInvariant()));
                break;
            case "/users":
                _controlador.PedirUsuarios();
                break;
            case "/msg":
                _controlador.EnviarTexto(partes[1], partes[2]);
                break;
            case "/newroom":
                _controlador.CrearSala(partes[1]);
                break;
            case "/invite":
                {
                    string sala = partes[1];
                    var usuarios = partes[2].Split(' ', StringSplitOptions.RemoveEmptyEntries);
                    _controlador.Invitar(sala, usuarios);
                }
                break;
            case "/join":
                _controlador.UnirseASala(partes[1]);
                break;
            case "/roomusers":
                _controlador.PedirUsuariosDeSala(partes[1]);
                break;
            case "/roomtext":
                _controlador.EnviarTextoSala(partes[1], partes[2]);
                break;
            case "/leave":
                _controlador.AbandonarSala(partes[1]);
                break;
            default:
                MostrarError($"Comando desconocido: {comando}. Escribe /help para ver la lista.");
                break;
        }
    }

    private void ImprimirAyuda()
    {
        Console.WriteLine("""
            Comandos disponibles:
              <texto>                      manda un texto público
              /status ACTIVE|AWAY|BUSY     cambia tu estado
              /users                       lista los usuarios conectados
              /msg <usuario> <texto>       manda un texto privado
              /newroom <sala>              crea una sala
              /invite <sala> <u1> <u2>...  invita usuarios a una sala
              /join <sala>                 te unes a una sala a la que fuiste invitado
              /roomusers <sala>            lista los usuarios de una sala
              /roomtext <sala> <texto>     manda un texto a una sala
              /leave <sala>                abandonas una sala
              /quit                        te desconectas y cierras el cliente
            """);
    }
}
