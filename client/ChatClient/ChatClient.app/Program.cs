using System;
using System.Threading;
using ChatClient.App.Networking;
using ChatClient.App.Protocolo;
 
namespace ChatClient.App
{
    internal class Program
    {
        private const string Host = "127.0.0.1";
        private const int Puerto = 5000;
 
        private static void Main(string[] args)
        {
            var conexion = new ServerConnection();
 
            try
            {
                Console.Write("Nombre de usuario (max 8 caracteres): ");
                string username = Console.ReadLine()?.Trim() ?? string.Empty;
 
                Console.WriteLine($"Conectando a {Host}:{Puerto}...");
                conexion.Conectar(Host, Puerto);
                Console.WriteLine("Conectado.");
 
                /* --- IDENTIFY real, en JSON --- */
                string identify = MensajeProtocolo.ConstruirIdentify(username);
                Console.WriteLine($"Enviando: {identify}");
                conexion.EnviarMensaje(identify);
 
                string? respuesta = conexion.LeerLinea();
                if (respuesta == null)
                {
                    Console.WriteLine("El servidor cerró la conexión antes de responder.");
                    return;
                }
 
                bool identificado = ImprimirMensajeEntrante(respuesta);
                if (!identificado)
                {
                    /* Si el servidor respondió NOT_IDENTIFIED o
                       INVALID, ya cerró la conexión de su lado
                       (segun el protocolo) — no tiene caso seguir. */
                    return;
                }
 
                /* --- Hilo en segundo plano: escucha lo que el
                   servidor mande sin que se lo pidamos (por ejemplo,
                   NEW_USER cuando otro cliente se identifica). Esto
                   es minimo a propósito — el Controller real que
                   coordina esto con una View y un Model llega en la
                   siguiente fase; por ahora solo demuestra que el
                   cliente puede leer mensajes asincronos. */
                var hiloEscucha = new Thread(() => EscucharEnSegundoPlano(conexion))
                {
                    IsBackground = true,
                };
                hiloEscucha.Start();
 
                Console.WriteLine("Escuchando mensajes del servidor. Presiona Enter para salir...");
                Console.ReadLine();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error: {ex.Message}");
            }
            finally
            {
                conexion.Desconectar();
                Console.WriteLine("Desconectado.");
            }
        }
 
        /// <summary>
        /// Corre en su propio hilo, leyendo líneas hasta que el
        /// servidor cierre la conexión (LeerLinea devuelve null).
        /// </summary>
        private static void EscucharEnSegundoPlano(ServerConnection conexion)
        {
            string? linea;
            while ((linea = conexion.LeerLinea()) != null)
            {
                ImprimirMensajeEntrante(linea);
            }
            Console.WriteLine("(el servidor cerró la conexión)");
        }
 
        /// <summary>
        /// Interpreta una línea entrante y la imprime de forma
        /// legible según su 'type'. Devuelve true si fue un
        /// IDENTIFY exitoso (para que Main sepa si debe continuar).
        /// </summary>
        private static bool ImprimirMensajeEntrante(string linea)
        {
            var mensaje = MensajeProtocolo.Parsear(linea);
            string? tipo = MensajeProtocolo.ObtenerTipo(mensaje);
 
            if (tipo == null)
            {
                Console.WriteLine($"[?] mensaje no reconocido: {linea}");
                return false;
            }
 
            switch (tipo)
            {
                case "RESPONSE":
                    string? operacion = MensajeProtocolo.ObtenerCampoString(mensaje, "operation");
                    string? resultado = MensajeProtocolo.ObtenerCampoString(mensaje, "result");
                    string? extra = MensajeProtocolo.ObtenerCampoString(mensaje, "extra");
 
                    Console.WriteLine(extra != null
                        ? $"[Servidor] {operacion} -> {resultado} ({extra})"
                        : $"[Servidor] {operacion} -> {resultado}");
 
                    /* Solo un RESPONSE de IDENTIFY con SUCCESS cuenta
                       como "ya podemos seguir usando la conexión". */
                    return operacion == "IDENTIFY" && resultado == "SUCCESS";
 
                case "NEW_USER":
                    string? nuevoUsuario = MensajeProtocolo.ObtenerCampoString(mensaje, "username");
                    Console.WriteLine($"* {nuevoUsuario} se unió al chat");
                    return true;
 
                default:
                    /* Tipos que el cliente todavia no sabe interpretar
                       (llegaran en fases posteriores del protocolo). */
                    Console.WriteLine($"[{tipo}] (todavía no implementado) {linea}");
                    return true;
            }
        }
    }
}
 