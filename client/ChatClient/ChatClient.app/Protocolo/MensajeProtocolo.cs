
using System.Text.Json;
using System.Text.Json.Nodes;
 
namespace ChatClient.App.Protocolo
{
    /*
        Esta clase es el equivalente en C# de la clase mensaje_json en C, 
        el demas codigo trabaja con un mensaje abstracto
    */
    public static class MensajeProtocolo
    {
        /*
            Intenta interpretar correctamente que es un json valido
        */
        public static JsonObject? Parsear(string linea)
        {
            try
            {
                JsonNode? nodo = JsonNode.Parse(linea);
                return nodo as JsonObject; // null si no es un objeto
            }
            catch (JsonException)
            {
                return null; // JSON sintacticamente invalido
            }
        }
 
        
        public static string? ObtenerTipo(JsonObject? mensaje) =>
            ObtenerCampoString(mensaje, "type");
 
        public static string? ObtenerCampoString(JsonObject? mensaje, string campo)
        {
            if (mensaje == null)
            {
                return null;
            }
            if (!mensaje.TryGetPropertyValue(campo, out JsonNode? valor) || valor == null)
            {
                return null;
            }
            if (valor is JsonValue jsonValue && jsonValue.TryGetValue(out string? resultado))
            {
                return resultado;
            }
            return null; // existe pero no es una cadena (ej. un numero)
        }
 
        /*
            Sencillamente construye el identify..        
        */
        public static string ConstruirIdentify(string username)
        {
            var objeto = new JsonObject
            {
                ["type"] = "IDENTIFY",
                ["username"] = username,
            };
            return objeto.ToJsonString(); // compacto, una sola linea
        }
    }
}