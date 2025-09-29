#include <bits/stdc++.h>
#include <fstream>
using namespace std;

// Función para leer todo el archivo
string leerArchivo(const string& ruta) {
    ifstream archivo(ruta, ios::binary);
    if (!archivo) {
        cerr << "Error: no se pudo abrir el archivo " << ruta << endl;
        return "";
    }
    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    return contenido;
}

// Estructura del nodo del árbol de Huffman
struct Nodo {
    char letra;
    int freq;
    Nodo *izq, *der;

    Nodo(char l, int f) {
        letra = l;
        freq = f;
        izq = der = nullptr;
    }
};

// Comparador para la cola de prioridad
struct Comparar {
    bool operator()(Nodo* a, Nodo* b) {
        return a->freq > b->freq; // menor frecuencia = mayor prioridad
    }
};

// Construir el árbol de Huffman
Nodo* construirHuffman(map<char,int>& freq) {
    priority_queue<Nodo*, vector<Nodo*>, Comparar> pq;

    for(auto par : freq) {
        pq.push(new Nodo(par.first, par.second));
    }

    while(pq.size() > 1) {
        Nodo* izq = pq.top(); pq.pop();
        Nodo* der = pq.top(); pq.pop();

        Nodo* padre = new Nodo('\0', izq->freq + der->freq);
        padre->izq = izq;
        padre->der = der;

        pq.push(padre);
    }

    return pq.top();
}

// Generar códigos binarios de cada letra
void generarCodigos(Nodo* raiz, string codigo, map<char,string>& codigos) {
    if(!raiz) return;

    if(!raiz->izq && !raiz->der) {
        codigos[raiz->letra] = codigo;
    }

    generarCodigos(raiz->izq, codigo + "0", codigos);
    generarCodigos(raiz->der, codigo + "1", codigos);
}

// Codificar mensaje
string codificar(const string& mensaje, map<char,string>& codigos) {
    string resultado = "";
    for(char c : mensaje) {
        resultado += codigos[c];
    }
    return resultado;
}

// Decodificar mensaje
string decodificar(const string& codigoBinario, Nodo* raiz) {
    string mensaje = "";
    Nodo* temp = raiz;
    for(char bit : codigoBinario) {
        if(bit == '0') temp = temp->izq;
        else temp = temp->der;

        if(!temp->izq && !temp->der) {
            mensaje += temp->letra;
            temp = raiz;
        }
    }
    return mensaje;
}

// Función recursiva para liberar memoria de árbol
void liberarArbol(Nodo* raiz) {
    if(!raiz) return;
    liberarArbol(raiz->izq);
    liberarArbol(raiz->der);
    delete raiz;
}

int main() {
    string rutaArchivo = "assets/huffman.txt";

    // Leer todo el archivo
    string mensaje = leerArchivo(rutaArchivo);
    if(mensaje.empty()) {
        cerr << "Archivo vacío o no se pudo leer." << endl;
        return 1;
    }

    // Peso inicial en bits
    size_t pesoInicial = mensaje.size() * 8;
    cout << "Peso inicial del archivo: " << pesoInicial << " bits (" << mensaje.size() << " bytes)\n";

    // Contar frecuencia de cada carácter
    map<char,int> freq;
    for(char c : mensaje) freq[c]++;

    cout << "\nFrecuencia de caracteres:\n";
    for(auto par : freq) {
        cout << "'" << par.first << "': " << par.second << endl;
    }

    // Construir árbol Huffman
    Nodo* raiz = construirHuffman(freq);

    // Generar códigos
    map<char,string> codigos;
    generarCodigos(raiz, "", codigos);

    cout << "\nCodigos de Huffman:\n";
    for(auto par : codigos) {
        cout << "'" << par.first << "': " << par.second << endl;
    }

    // Codificar y mostrar
    string codificado = codificar(mensaje, codigos);
    cout << "\nMensaje codificado (primeros 200 bits): " 
        << codificado.substr(0, min((size_t)200, codificado.size())) << "...\n";
    cout << "Longitud total del mensaje codificado: " << codificado.size() << " bits\n";

    // Mostrar peso final y porcentaje de compresión
    double porcentajeCompresion = 100.0 * (1.0 - (double)codificado.size() / pesoInicial);
    cout << "Compresion lograda: " << porcentajeCompresion << "%\n";

    // Decodificar y mostrar
    string decodificado = decodificar(codificado, raiz);
    cout << "\nMensaje decodificado (primeros 200 caracteres): "
        << decodificado.substr(0, min((size_t)200, decodificado.size())) << "...\n";

    // Liberar memoria
    liberarArbol(raiz);

    return 0;
}