#include <iostream>  
#include <string>       
#include <fstream>     
#include <vector>        
#include <filesystem>    
using namespace std;
namespace fs = std::filesystem; 

// Ruta al archivo desde donde se leera
const string ASSETS_PATH = "assets/fbruta.txt";

// Alfabeto valido para la fuerza bruta
const string ALFABETO = "abcdefghijklmnopqrstuvwxyz";

// Maxima longitud segura (+Caracteres = Kaboom)
const size_t MAX_BRUTE_LEN = 6; // Longitud maxima segura para fuerza bruta

// Clase que implementa busqueda por fuerza bruta
class f_b {

private:
    string objetivo;        // Cadena objetivo
    string alfabeto;        // Conjunto de caracteres permitidos
    long long intentos = 0; // Contador de intentos realizados

public:
    // Inicializa objetivo y alfabeto con referencias para evitar
    f_b(const string& a, const string& b) : objetivo(a), alfabeto(b) {}

    // Genera cadenas y compara con el objetivo actual
    // Si la cadena construida devuelve true es porque se encontro la contraseña
    bool generar(const string& actual) {
        if (objetivo.empty()) return false; // Sin objetivos no ejecuta nada
        // Si la longitud de la cadena actual iguala la del objetivo, comprobamos si coincide.
        if (actual.size() == objetivo.size()) {
            intentos++; // Aumentamos el contador de intentos cada vez que probamos una cadena completa
            if (actual == objetivo) {
                // Si coincide, mostramos resultado y retornamos true para detener busqueda.
                cout << "Contraseña encontrada: " << actual << endl;
                cout << "Intentos realizados: " << intentos << endl;
                return true;
            }
            // Si no coincide, devolvemos false y se prueba la siguiente combinacion.
            return false;
        }
        // Para cada caracter del alfabeto, concatenamos y continuamos generando recursivamente.
        for (char c : alfabeto) {
            // Si en cualquier rama la llamada devuelve true, propagamos true hacia arriba para detener la busqueda completa.
            if (generar(actual + c)) return true;
        }
        // Si se probaron todas las opciones en esta rama y no se encontro, devolvemos false.
        return false;
    }
};

// Lee el contenido del archivo
string leerContenidoCompletoDesdeAssets() {
    // Verifica si el archivo existe antes de intentar abrirlo
    if (!fs::exists(ASSETS_PATH)) {
        cout << "Archivo no encontrado: " << ASSETS_PATH << endl;
        return ""; // Indicamos si fallo
    }
    // Abrimos el archivo
    ifstream f(ASSETS_PATH);
    if (!f) {
        // Si no se pudo abrir lo informamos
        cout << "No se pudo abrir: " << ASSETS_PATH << endl;
        return "";
    }
    // Lee todo el contenido, concatenamos cada linea (para preservar espacios y texto)
    string linea;
    string contenido;
    // Usamos getline en bucle para leer todas las lineas del archivo
    while (getline(f, linea)) {
        contenido += linea; // Concatenamos sin añadir '\n'
                            // Si lo queremos incluir cambiar a contenido += linea + '\n';
    }
    return contenido; // Devolvemos el contenido completo
}

// Comprueba que el objetivo sea adecuado para fuerza bruta
bool objetivoValidoParaBrute(const string& objetivo, const string& alfabeto) {
    if (objetivo.empty()) return false; // Objetivo vacío no es valido
    // Comprueba la longitud (evita que se nos muera la PC)
    if (objetivo.size() > MAX_BRUTE_LEN) {
        cout << "Objetivo demasiado largo para fuerza bruta segura (len=" << objetivo.size()
            << ", max=" << MAX_BRUTE_LEN << ")." << endl;
        return false;
    }
    // Comprueba que cada caracter del objetivo este en el alfabeto permitido
    for (char ch : objetivo) {
        if (alfabeto.find(tolower(static_cast<unsigned char>(ch))) == string::npos) { // tolower para permitir mayúsculas en el archivo (se normaliza a minuscula)
            cout << "Caracter no permitido en objetivo para fuerza bruta: '" << ch << "'" << endl;
            return false;
        }
    }
    // Si paso todas las comprobaciones, es valido
    return true;
}

// Busca una cadena (en texto) dentro de un archivo completo y muestra offsets
void buscarCadenaEnArchivo(const string& rutaArchivo, const string& cadenaBuscada) {
    // Verifica la existencia del archivo
    if (!fs::exists(rutaArchivo)) {
        cout << "No se encontro el archivo para buscar: " << rutaArchivo << endl;
        return;
    }

    // Abre el archivo en modo binario para leer bytes tal cual (seguro para todo tipo de contenido)
    ifstream archivo(rutaArchivo, ios::binary);
    if (!archivo) {
        cout << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return;
    }

    // Lee todo el contenido en una sola string usando iteradores de flujo
    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());

    // Vector para acumular las posiciones donde aparece la cadena buscada
    vector<size_t> posiciones;
    // Buscamos la primera ocurrencia
    size_t pos = contenido.find(cadenaBuscada);
    // Mientras se encuentren ocurrencias, las guardamos y buscamos desde la siguiente posicion
    while (pos != string::npos) {
        posiciones.push_back(pos);
        pos = contenido.find(cadenaBuscada, pos + 1);
    }

    // Mostramos resultados segun si se encontraron o no ocurrencias
    if (posiciones.empty()) {
        cout << "Cadena no encontrada en el archivo." << endl;
    } else {
        cout << "Cadena encontrada " << posiciones.size() << " veces en " << rutaArchivo << "." << endl;
        cout << "Posiciones (offset desde inicio del archivo, base 0): ";
        for (size_t p : posiciones) cout << p << " ";
        cout << endl;
    }
}

// Flujo principal del programa
int main() {
    // Leemos el contenido del archivo
    string objetivo = leerContenidoCompletoDesdeAssets();
    if (objetivo.empty()) {
        // Si no hay objetivo (archivo vacio o no encontrado), informamos y no intentamos fuerza bruta
        cout << "No hay objetivo valido" << endl;
    } else {
        // Si se leyo algo, mostramos el objetivo tal cual
        cout << "Objetivo leido: '" << objetivo << "'" << endl;
        // Valida el objetivo antes de iniciar la fuerza bruta 
        if (objetivoValidoParaBrute(objetivo, ALFABETO)) {
            cout << "Iniciando fuerza bruta" << endl;
            // Crea una instancia de la clase de fuerza bruta con objetivo y alfabeto definidos
            f_b brute(objetivo, ALFABETO);
            // Llamada inicial con cadena vacia para comenzar la generacion recursiva
            brute.generar("");
        } else {
            // Si no pasa validacion, evitamos ejecutar la busqueda
            cout << "Omitiendo fuerza bruta." << endl;
        }
    }

    cout << "Ingrese la cadena a buscar en " << ASSETS_PATH << ": ";
    string cadena;
    getline(cin, cadena); // Lee entrada completa
    if (cadena.empty()) {
        cout << "Cadena vacia. Terminando." << endl;
        return 0;
    }

    // Realizamos la busqueda en el archivo (muestra posiciones y cantidad de ocurrencias)
    buscarCadenaEnArchivo(ASSETS_PATH, cadena);
    return 0; // Fin del programa 
}