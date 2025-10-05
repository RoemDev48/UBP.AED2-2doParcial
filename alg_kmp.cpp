#include <iostream>     
#include <fstream>     
#include <vector>       
#include <string>       
#include <filesystem>  
#include <chrono>     

using namespace std;
namespace fs = std::filesystem; 

class KMPCounter {
private:
    string patron;      // Patron a buscar
    vector<int> lps;    // Arreglo LPS

    // Construye el arreglo LPS para el patron 
    void construirLPS() {
        int m = patron.size();
        lps.assign(m, 0); // Inicializa el vector LPS con ceros (tamaño m)

        int len = 0;      // Longitud del prefijo mas largo encontrado hasta ahora
        int i = 1;        // Empezamos en i=1 (LPS[0] siempre es 0)
        while (i < m) {
            if (patron[i] == patron[len]) {
                // Si los caracteres coinciden, incrementamos len y guardamos en lps[i]
                len++;
                lps[i] = len;
                i++;
            } else {
                // Si no coinciden, tenemos dos posibilidades:
                if (len != 0) {
                    // Retrocedemos len usando lps del indice anterior (no avanzamos i)
                    // esto permite reutilizar informacion previa (clave en KMP)
                    len = lps[len - 1];
                } else {
                    // len == 0: no hay prefijo valido, LPS para i es 0 y avanzamos i
                    lps[i] = 0;
                    i++;
                }
            }
        }
    }

public:   
    // Constructor recibe el patron y construye LPS automaticamente
    KMPCounter(const string& p) : patron(p) {
        construirLPS();
    }

    // Busca un patron en texto y devuelve un vector de pares:
    // (posicion_inicio_de_ocurrencia, comparaciones_acumuladas_al_encontrar)
    // Ademas modifica comparacionesTotales por referencia con el total de comparaciones.
    vector<pair<size_t, long long>> buscarEnTextoConContador(const string& texto, long long &comparacionesTotales) {
        vector<pair<size_t, long long>> resultados; // Acumulador de resultados
        comparacionesTotales = 0;                   // Iniciamos el contador de comparaciones

        int n = (int)texto.size();
        int m = (int)patron.size();
        // Si hay patron vacio, texto vacio o el patron es mas largo que el texto -> nada que buscar
        if (m == 0 || n == 0 || m > n) return resultados;

        int i = 0; // Indice en texto
        int j = 0; // Indice en patron

        // Avanzamos por el texto y usamos LPS para saltos en el patron
        while (i < n) {
            // Cada comparacion entre texto[i] y patron[j] se considera un intento
            comparacionesTotales++;
            if (texto[i] == patron[j]) {
                // Avanzamos ambos indices
                i++;
                j++;
            } else {
                // Si j!=0 usamos LPS para evitar retroceder i
                if (j != 0) {
                    j = lps[j - 1]; // Reutilizamos prefijos ya comparados
                } else {
                    // Si j==0 no hay prefijo que reutilizar; avanzamos solo i
                    i++;
                }
            }

            // Si j alcanza m, encontramos una ocurrencia completa
            if (j == m) {
                // La posicion de inicio es i - j (porque i esta despues del ultimo char del patron)
                resultados.emplace_back(static_cast<size_t>(i - j), comparacionesTotales);
                // Prepara a j para buscar posibles solapamientos posteriores
                j = lps[j - 1];
            }
        }

        return resultados;
    }
};

// Lee el archivo completo y devuelve su contenido.
// Si falla, devuelve una cadena vacia
string leerArchivo(const string& ruta) {
    // Verifica la existencia del archivo antes de intentar abrirlo
    if (!fs::exists(ruta)) {
        cerr << "Archivo no encontrado: " << ruta << ' ';
        return "";
    }
    // Abre el archivo en modo binario para leer bytes tal cual (seguro para todo tipo de contenido)
    ifstream f(ruta, ios::binary);
    if (!f) {
        cerr << "No se pudo abrir: " << ruta << ' ';
        return "";
    }
    // Lee todo el contenido usando iteradores de flujo
    string contenido((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    return contenido;
}

// Flujo principal del programa
int main() {
    const string rutaArchivo = "assets/kmp.txt"; // Ruta del archivo a analizar

    // Lee el archivo completo
    string texto = leerArchivo(rutaArchivo);
    if (texto.empty()) {
        // Si el archivo no existe o esta vacio, informamos
        cerr << "El archivo esta vacio o no se pudo leer. Asegurate de que " << rutaArchivo << " exista." << endl;
        return 1;
    }

    cout << "Ingrese la cadena a buscar: ";
    string patron;
    getline(cin, patron);
    if (patron.empty()) {
        // Patron vacio -> nada que buscar
        cout << "Cadena vacia. Saliendo." << endl;
        return 0;
    }

    // Ejecutamos KMP con contador de comparaciones y medimos el tiempo de ejecucion
    KMPCounter buscador(patron);     // Construimos LPS dentro del constructor
    long long comparacionesTotales = 0;

    // Toma de tiempo (inicio)
    auto t0 = chrono::steady_clock::now();

    // Llama al metodo que realiza la busqueda y devuelve las ocurrencias
    vector<pair<size_t, long long>> ocurrencias = buscador.buscarEnTextoConContador(texto, comparacionesTotales);

    // Toma de tiempo (fin)
    auto t1 = chrono::steady_clock::now();
    // Convierte la duracion a milisegundos en formato double
    double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();

    // Mostramos el resultado
    if (ocurrencias.empty()) {
        // Si no se encontraron ocurrencias, mostramos comparaciones totales y tiempo
        cout << "La cadena \"" << patron << "\" NO se encontro en el archivo." << endl;
        cout << "Comparaciones realizadas: " << comparacionesTotales << endl;
        cout << "Tiempo de busqueda: " << ms << " ms" << endl;
    } else {
        // Mostramos cuantas veces se encontro y los detalles de cada ocurrencia
        cout << "La cadena \"" << patron << "\" se encontro " << ocurrencias.size() << " veces." << endl;
        cout << "Resultados (posicion_inicio, comparaciones_acumuladas_al_encontrar):" << endl;
        for (size_t k = 0; k < ocurrencias.size(); ++k) {
            cout << "  #" << (k + 1) << ": pos = " << ocurrencias[k].first
                << ", comparaciones = " << ocurrencias[k].second << ' ';
        }
        // Mostramos los totales acumulados y el tiempo tomado
        cout << "Comparaciones totales realizadas durante toda la busqueda: " << comparacionesTotales << endl;
        cout << "Tiempo de busqueda: " << ms << " ms" << endl;
    }

    return 0;
}