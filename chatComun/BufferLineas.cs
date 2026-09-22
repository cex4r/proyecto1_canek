using System.Net.Sockets;
using System.Text;

namespace chatComun;

/*
    esta clase se encarga de acumular btes que van llegando y entrega una por una 
    delimitada por una \n, ignorando las lineas vacias, esta clase la usa tanto el servidor
    como el client

*/

public class BufferLineas
{
    private readonly NetworkStream _stream; 
    private readonly byte[] _bufferLectura = new byte[4096];
    private readonly StringBuilder _acumulado = new(); 
    private readonly Queue<string> _lineasListas = new(); 
    
    public BufferLineas(NetworkStream stream)
    {
        _stream = stream; 
    }

    /*
        Se bloquea hasta obtener la sifuiente linea no vacia, o hasta que el otro lado 
        cierre la conexion, en ese caso regresa null 
    */

    public string? LeerLinea()
    {
        while(_lineasListas.Count == 0)
        {
            if(!LlenarBuffer())
                return null; 
        }
        return _lineasListas.Dequeue(); 
    }

    private bool LlenarBuffer()
    {
        int leidos;
        try
        {
            leidos = _stream.Read(_bufferLectura, 0, _bufferLectura.Length);
        }
        catch (IOException)
        {
            return false;
        }
        catch (ObjectDisposedException)
        {
            return false;
        }
        if(leidos == 0)
            return false; 

        _acumulado.Append(Encoding.UTF8.GetString(_bufferLectura, 0, leidos));
        ExtraerLineasCompletas();
        return true;
    }


    private void ExtraerLineasCompletas()
    {
        while (true)
        {
            string texto = _acumulado.ToString(); 
            int indice = texto.IndexOf('\n');
            if(indice <0)
                break;

            string linea = texto[..indice];
            _acumulado.Remove(0, indice + 1);

            if(linea.Length > 0)
                _lineasListas.Enqueue(linea);
        }
    }
}