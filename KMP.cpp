#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;

// Clase KMP que además cuenta comparaciones (intentos)
class KMPCounter {
private:
    string patron;
    vector<int> lps;

    void construirLPS() {
        int m = patron.size();
        lps.assign(m, 0);
        int len = 0;
        int i = 1;
        while (i < m) {
            if (patron[i] == patron[len]) {
                len++;
                lps[i] = len;
                i++;
            } else {
                if (len != 0) {
                    len = lps[len - 1];
                } else {
                    lps[i] = 0;
                    i++;
                }
            }
        }
    }

public:
    KMPCounter(const string& p) : patron(p) {
        construirLPS();
    }

    // Buscar patrón en texto y devolver pares (posicion_inicio, comparaciones_al_encontrar)
    // También devuelve, por referencia, el total de comparaciones realizadas.
    vector<pair<size_t, long long>> buscarEnTextoConContador(const string& texto, long long &comparacionesTotales) {
        vector<pair<size_t, long long>> resultados;
        comparacionesTotales = 0;

        int n = (int)texto.size();
        int m = (int)patron.size();
        if (m == 0 || n == 0 || m > n) return resultados;

        int i = 0; // índice en texto
        int j = 0; // índice en patrón

        // Bucle principal de KMP
        while (i < n) {
            // Cada vez que comparamos texto[i] y patron[j] contamos un intento.
            comparacionesTotales++;
            if (texto[i] == patron[j]) {
                i++;
                j++;
            } else {
                if (j != 0) {
                    j = lps[j - 1];
                } else {
                    i++;
                }
            }

            // Si encontramos una ocurrencia completa
            if (j == m) {
                // La posición de inicio es i - j
                resultados.emplace_back(static_cast<size_t>(i - j), comparacionesTotales);
                // Preparar j para seguir buscando (posibles solapamientos)
                j = lps[j - 1];
            }
        }

        return resultados;
    }
};

// Leer archivo completo y devolver su contenido, string vacío si falla
string leerArchivo(const string& ruta) {
    if (!fs::exists(ruta)) {
        cerr << "Archivo no encontrado: " << ruta << '\n';
        return "";
    }
    ifstream f(ruta, ios::binary);
    if (!f) {
        cerr << "No se pudo abrir: " << ruta << '\n';
        return "";
    }
    string contenido((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    return contenido;
}

int main() {
    const string rutaArchivo = "assets/kmp.txt";

    // 1) Leer archivo
    string texto = leerArchivo(rutaArchivo);
    if (texto.empty()) {
        cerr << "El archivo esta vacio o no se pudo leer. Asegurate de que " << rutaArchivo << " exista." << endl;
        return 1;
    }

    // 2) Pedir patrón al usuario
    cout << "Ingrese la cadena a buscar: ";
    string patron;
    getline(cin, patron);
    if (patron.empty()) {
        cout << "Cadena vacia. Saliendo." << endl;
        return 0;
    }

    // 3) Ejecutar KMP con contador y medir tiempo
    KMPCounter buscador(patron);
    long long comparacionesTotales = 0;
    auto t0 = chrono::steady_clock::now();
    vector<pair<size_t, long long>> ocurrencias = buscador.buscarEnTextoConContador(texto, comparacionesTotales);
    auto t1 = chrono::steady_clock::now();
    double ms = chrono::duration_cast<chrono::duration<double, milli>>(t1 - t0).count();

    // 4) Mostrar resultados
    if (ocurrencias.empty()) {
        cout << "La cadena \"" << patron << "\" NO se encontro en el archivo." << endl;
        cout << "Comparaciones realizadas: " << comparacionesTotales << endl;
        cout << "Tiempo de busqueda: " << ms << " ms" << endl;
    } else {
        cout << "La cadena \"" << patron << "\" se encontro " << ocurrencias.size() << " veces." << endl;
        cout << "Resultados (posicion_inicio, comparaciones_acumuladas_al_encontrar):" << endl;
        for (size_t k = 0; k < ocurrencias.size(); ++k) {
            cout << "  #" << (k + 1) << ": pos = " << ocurrencias[k].first
                << ", comparaciones = " << ocurrencias[k].second << '\n';
        }
        cout << "Comparaciones totales realizadas durante toda la busqueda: " << comparacionesTotales << endl;
        cout << "Tiempo de busqueda: " << ms << " ms" << endl;
    }

    return 0;
}