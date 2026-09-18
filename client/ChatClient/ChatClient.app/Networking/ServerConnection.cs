/*
    esta clase matiene limpio el Program.cs.
    da metodos como Conectar, EnviarMensaje, LeerRespuesta y Desconectar
*/

using System;
using System.Net.Sockets;
using System.Text;

namespace ChatClient.App.Networking;

public class ServerConnection
{
    private TcpClient? _cliente; 
    private NetworkStream? _flujo; 

    // crea un cliente y establece la conexión con el servidor
    public void Conectar(string host, int puerto)
    {
        _cliente = new TcpClient(); 
        _cliente.Connect(host, puerto);
        _flujo = _cliente.GetStream();  
    }

    // envia un mensaje convirtiendolo a bytes con codificación utf-8
    public void EnviarMensaje(string mensaje)
    {
        if (_flujo == null)
        {
            throw new InvalidOperationException("no hay conexión  con el servidor");
        }

        
        byte[] bytes = Encoding.UTF8.GetBytes(mensaje);
        _flujo.Write(bytes, 0, bytes.Length); 
    }

    
    public string LeerRespuesta()
    {
        if (_flujo == null)
        {
            throw new InvalidOperationException("no hay conexión con el servidor");
        }

        byte[] buffer = new byte[1024]; 
        int bytesLeidos = _flujo.Read(buffer, 0, buffer.Length); 

        if (bytesLeidos == 0)
        {
            return string.Empty; 
        }

        return Encoding.UTF8.GetString(buffer, 0, bytesLeidos);
    }

    public void Desconectar()
    {
        _flujo?.Close(); 
        _cliente?.Close(); 
    }
}