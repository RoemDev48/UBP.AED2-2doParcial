#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std; // 👈 ahora no hace falta std::

namespace Compression {

class LZ78 {
public:
    // Estructura para cada par (índice, carácter)
    struct Pair {
        int index;
        char nextChar;
    };

    // Método para comprimir
    vector<Pair> compress(const string &input) {
        unordered_map<string, int> dictionary;
        vector<Pair> output;

        string current;
        int dictIndex = 1; // índice empieza en 1

        for (char c : input) {
            current += c;
            // Si no está en el diccionario
            if (dictionary.find(current) == dictionary.end()) {
                // La parte sin el último carácter
                string prefix = current.substr(0, current.size() - 1);
                int index = 0;
                if (!prefix.empty() && dictionary.find(prefix) != dictionary.end()) {
                    index = dictionary[prefix];
                }

                // Agregamos el par (índice, último carácter)
                output.push_back({index, c});

                // Añadimos la cadena completa al diccionario
                dictionary[current] = dictIndex++;
                current.clear();
            }
        }

        return output;
    }

    // Método para descomprimir
    string decompress(const vector<Pair> &data) {
        vector<string> dictionary; // diccionario de cadenas
        dictionary.push_back(""); // índice 0 vacío

        string result;

        for (auto &p : data) {
            string entry;

            if (p.index == 0) {
                // Si el índice es 0, la entrada es solo el carácter
                entry = string(1, p.nextChar);
            } else {
                // Caso normal: cadena en diccionario + carácter
                entry = dictionary[p.index] + p.nextChar;
            }

            // Añadimos al resultado
            result += entry;

            // Guardamos en el diccionario
            dictionary.push_back(entry);
        }

        return result;
    }

    // Método para mostrar la salida comprimida
    void printCompressed(const vector<Pair> &data) {
        cout << "Salida comprimida: \n";
        for (auto &p : data) {
            cout << "(" << p.index << ", '" << p.nextChar << "') ";
        }
        cout << "\n";
    }
};

} // namespace Compression
string text();
int main() {
    string texto = text();
    Compression::LZ78 lz;

    // Comprimir
    auto comprimido = lz.compress(texto);
    lz.printCompressed(comprimido);

    // Descomprimir
    string descomprimido = lz.decompress(comprimido);
    cout << "Texto descomprimido: " << descomprimido << "\n";

    return 0;
}

string text(){
    string palabra;
    ifstream texto("textito.txt");
    getline(texto,palabra);
    if (!texto) {
    cerr << "Error: no se pudo abrir el archivo\n";
    return "ERROR";
}
    return palabra;
}