using System.Text.Json;

namespace chatComun;

/*
    punto unico donde se configura ystem.Text.Json:
*/
public static class SerializadorJson
{
    private static readonly JsonSerializerOptions Opciones = new()
    {
        WriteIndented = false
    };

    public static string Serializar(object mensaje) => JsonSerializer.Serialize(mensaje, Opciones);
}
