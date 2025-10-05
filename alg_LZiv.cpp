#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iterator>
#include <cstdint>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

namespace Compression {

// Funciona para texto o archivos binarios (imagenes, videos, etc.)
class LZW {
    static const int MAX_BITS = 16;
    static const int MAX_TABLE_SIZE = 1 << MAX_BITS;

public:
    // Recibe bytes (no solo texto)
    vector<uint16_t> compress(const vector<uint8_t> &input) {
        unordered_map<string, uint16_t> dictionary;
        vector<uint16_t> output;

        // Inicializamos el diccionario con los 256 posibles bytes
        for (int i = 0; i < 256; i++) {
            string ch(1, (char)i);
            dictionary[ch] = i;
        }

        string current;
        uint16_t nextCode = 256;

        for (uint8_t byte : input) {
            string currentPlusC = current + (char)byte;
            if (dictionary.find(currentPlusC) != dictionary.end()) {
                current = currentPlusC;
            } else {
                output.push_back(dictionary[current]);
                if (nextCode < MAX_TABLE_SIZE) {
                    dictionary[currentPlusC] = nextCode++;
                }
                current = string(1, (char)byte);
            }
        }

        if (!current.empty()) {
            output.push_back(dictionary[current]);
        }

        return output;
    }

    // Descompresion
    vector<uint8_t> decompress(const vector<uint16_t> &codes) {
        vector<string> dictionary(4096);
        for (int i = 0; i < 256; i++) {
            dictionary[i] = string(1, (char)i);
        }

        uint16_t nextCode = 256;
        string prev = dictionary[codes[0]];
        string result = prev;

        for (size_t i = 1; i < codes.size(); i++) {
            uint16_t code = codes[i];
            string entry;

            if (code < nextCode) {
                entry = dictionary[code];
            } else if (code == nextCode) {
                entry = prev + prev[0];
            } else {
                cerr << "Error en descompresion: codigo invalido";
                return {};
            }

            result += entry;
            if (nextCode < MAX_TABLE_SIZE) {
                dictionary[nextCode++] = prev + entry[0];
            }
            prev = entry;
        }

        // Convertimos string resultante a bytes
        vector<uint8_t> bytes(result.begin(), result.end());
        return bytes;
    }

    // Guardamos el comprimido
    void saveCompressed(const string &filename, const vector<uint16_t> &codes) {
        ofstream out(filename, ios::binary);
        uint32_t buffer = 0;
        int bitsInBuffer = 0;

        for (uint16_t code : codes) {
            buffer |= (code << bitsInBuffer);
            bitsInBuffer += 12;
            while (bitsInBuffer >= 8) {
                out.put(buffer & 0xFF);
                buffer >>= 8;
                bitsInBuffer -= 8;
            }
        }

        if (bitsInBuffer > 0) {
            out.put(buffer & 0xFF);
        }

        out.close();
    }

    // Cargamos el comprimido
    vector<uint16_t> loadCompressed(const string &filename) {
        ifstream in(filename, ios::binary);
        vector<uint16_t> codes;
        vector<uint8_t> bytes((istreambuf_iterator<char>(in)), {});
        uint32_t buffer = 0;
        int bitsInBuffer = 0;

        for (uint8_t byte : bytes) {
            buffer |= (byte << bitsInBuffer);
            bitsInBuffer += 8;
            while (bitsInBuffer >= 12) {
                uint16_t code = buffer & 0xFFF;
                codes.push_back(code);
                buffer >>= 12;
                bitsInBuffer -= 12;
            }
        }
        return codes;
    }
};

// Lee el archivo binario completo 
vector<uint8_t> readBinaryFile(const string &path) {
    ifstream file(path, ios::binary);
    if (!file) {
        cerr << "Error al abrir archivo: " << path << endl;
        return {};
    }
    return vector<uint8_t>((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

// Guarda el archivo binario
void saveBinaryFile(const string &path, const vector<uint8_t> &data) {
    ofstream file(path, ios::binary);
    file.write(reinterpret_cast<const char *>(data.data()), data.size());
}

} // namespace Compression


// Flujo principal del programa
int main() {
    string fileName;
    string defaultPath = "assets/lz/";   // Ruta por defecto
    string compressedFolder = "compressed/"; // Carpeta de salida comprimido
    string restoredFolder = "restored/";     // Carpeta de salida descomprimido

    // Crear carpetas si no existen
    fs::create_directories(compressedFolder);
    fs::create_directories(restoredFolder);

    cout << "Ingrese el nombre del archivo a comprimir (ej: imagen.jpg): ";
    getline(cin, fileName);

    string inputFile = defaultPath + fileName;
    string compressedFile = compressedFolder + fileName + ".rar";
    string decompressedFile = restoredFolder + fileName;

    // Lee el archivo
    auto originalData = Compression::readBinaryFile(inputFile);
    if (originalData.empty()) return 1;

    cout << "Tamaño original: " << originalData.size() << " bytes\n";

    // Comprime
    Compression::LZW lzw;
    auto comprimido = lzw.compress(originalData);
    lzw.saveCompressed(compressedFile, comprimido);

    cout << "Archivo comprimido guardado en: " << compressedFile
         << " (~" << (comprimido.size() * 12) / 8 << " bytes)\n";

    // Preguntar si desea descomprimir
    char respuesta;
    cout << "¿Desea descomprimir el archivo ahora? (s/n): ";
    cin >> respuesta;

    if (respuesta == 's' || respuesta == 'S') {
        auto cargado = lzw.loadCompressed(compressedFile);
        auto descomprimido = lzw.decompress(cargado);
        Compression::saveBinaryFile(decompressedFile, descomprimido);

        cout << "Archivo restaurado guardado en: " << decompressedFile << "\n";
        cout << "Tamaño descomprimido: " << descomprimido.size() << " bytes\n";

        if (descomprimido == originalData)
            cout << "Archivo restaurado correctamente\n";
        else
            cout << "Error: el archivo restaurado no coincide\n";
    } else {
        cout << "Se omitio la descompresion.\n";
    }

    return 0;
}