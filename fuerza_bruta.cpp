#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem> // requiere C++17
using namespace std;
namespace fs = std::filesystem;

const string ASSETS_PATH = "assets/textito.txt";
const string ALFABETO = "abcdefghijklmnopqrstuvwxyz";
const size_t MAX_BRUTE_LEN = 6; // longitud máxima segura para fuerza bruta

// Clase fuerza bruta
class f_b {
private:
    string objetivo;
    string alfabeto;
    long long intentos = 0;
public:
    f_b(const string& a, const string& b) : objetivo(a), alfabeto(b) {}
    bool generar(const string& actual) {
        if (objetivo.empty()) return false;
        if (actual.size() == objetivo.size()) {
            intentos++;
            if (actual == objetivo) {
                cout << "Contraseña encontrada: " << actual << endl;
                cout << "Intentos realizados: " << intentos << endl;
                return true;
            }
            return false;
        }
        for (char c : alfabeto) {
            if (generar(actual + c)) return true;
        }
        return false;
    }
};

// Lee primera línea
string leerPrimeraLineaDesdeAssets() {
    if (!fs::exists(ASSETS_PATH)) {
        cout << "Archivo no encontrado: " << ASSETS_PATH << endl;
        return "";
    }
    ifstream f(ASSETS_PATH);
    if (!f) {
        cout << "No se pudo abrir: " << ASSETS_PATH << endl;
        return "";
    }
    string linea;
    getline(f, linea);
    return linea;
}

// Validar que objetivo use solo caracteres del alfabeto
bool objetivoValidoParaBrute(const string& objetivo, const string& alfabeto) {
    if (objetivo.empty()) return false;
    // comprobar longitud razonable
    if (objetivo.size() > MAX_BRUTE_LEN) {
        cout << "Objetivo demasiado largo para fuerza bruta segura (len=" << objetivo.size()
            << ", max=" << MAX_BRUTE_LEN << ")." << endl;
        return false;
    }
    // comprobar caracteres permitidos
    for (char ch : objetivo) {
        if (alfabeto.find(tolower(static_cast<unsigned char>(ch))) == string::npos) {
            cout << "Caracter no permitido en objetivo para fuerza bruta: '" << ch << "'" << endl;
            return false;
        }
    }
    return true;
}

// Buscar cadena en archivo (bloque completo)
void buscarCadenaEnArchivo(const string& rutaArchivo, const string& cadenaBuscada) {
    if (!fs::exists(rutaArchivo)) {
        cout << "No se encontró el archivo para buscar: " << rutaArchivo << endl;
        return;
    }
    ifstream archivo(rutaArchivo, ios::binary);
    if (!archivo) {
        cout << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return;
    }
    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    vector<size_t> posiciones;
    size_t pos = contenido.find(cadenaBuscada);
    while (pos != string::npos) {
        posiciones.push_back(pos);
        pos = contenido.find(cadenaBuscada, pos + 1);
    }
    if (posiciones.empty()) {
        cout << "Cadena no encontrada en el archivo." << endl;
    } else {
        cout << "Cadena encontrada " << posiciones.size() << " veces en " << rutaArchivo << "." << endl;
        cout << "Posiciones (offset desde inicio del archivo, base 0): ";
        for (size_t p : posiciones) cout << p << " ";
        cout << endl;
    }
}

int main() {
    cout << "Current working directory: " << fs::current_path() << endl;

    // leer objetivo
    string objetivo = leerPrimeraLineaDesdeAssets();
    if (objetivo.empty()) {
        cout << "No hay objetivo válido (archivo no encontrado o primera línea vacía)." << endl;
    } else {
        cout << "Objetivo leído (primera línea): '" << objetivo << "'" << endl;
        // Validar antes de fuerza bruta
        if (objetivoValidoParaBrute(objetivo, ALFABETO)) {
            cout << "Iniciando fuerza bruta (objetivo válido y longitud segura)..." << endl;
            f_b brute(objetivo, ALFABETO);
            brute.generar("");
        } else {
            cout << "Omitiendo fuerza bruta por motivos de seguridad/validación." << endl;
        }
    }

    // buscar cadena solicitada
    cout << "Ingrese la cadena a buscar en " << ASSETS_PATH << ": ";
    string cadena;
    getline(cin, cadena);
    if (cadena.empty()) {
        cout << "Cadena vacía. Terminando." << endl;
        return 0;
    }
    buscarCadenaEnArchivo(ASSETS_PATH, cadena);
    return 0;
}