#include <iostream>      
#include <fstream>       
#include <vector>       
#include <string>        
#include <queue>        
#include <map>          
#include <algorithm>    
using namespace std;

// Lee todo el contenido del archivo y lo devuelve como string
string leerArchivo(const string& ruta) {
    ifstream archivo(ruta, ios::binary);  // Abre el archivo en modo binario
    if (!archivo) {
        cerr << "Error: no se pudo abrir el archivo " << ruta << endl;
        return "";
    }

    // Usa iteradores para copiar todo el contenido al string
    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    return contenido;
}

// Representa un nodo en el árbol de Huffman. Cada nodo puede ser una hoja (una letra) o un nodo interno (sin letra) 
struct Nodo {
    char letra;         // Caracter representado (solo en hojas)
    int freq;           // Frecuencia de apariciOn
    Nodo *izq, *der;    // Punteros a hijos izquierdo y derecho

    Nodo(char l, int f) : letra(l), freq(f), izq(nullptr), der(nullptr) {}
};

// Se usa en la cola de prioridad para ordenar nodos según su frecuencia (menor frecuencia = mayor prioridad)
struct Comparar {
    bool operator()(Nodo* a, Nodo* b) {
        return a->freq > b->freq; // prioridad más alta para menor frecuencia
    }
};

// Construye el arbol de Huffman a partir de un mapa de frecuencias. Devuelve la raiz del arbol 
Nodo* construirHuffman(map<char,int>& freq) {
    // Cola de prioridad
    priority_queue<Nodo*, vector<Nodo*>, Comparar> pq;

    // Crea una hoja para cada caracter y lo asigna en la cola
    for (auto& par : freq)
        pq.push(new Nodo(par.first, par.second));

    // Combina los dos nodos de menor frecuencia hasta que quede uno solo
    while (pq.size() > 1) {
        Nodo* izq = pq.top(); pq.pop();  // Nodo con menor frecuencia
        Nodo* der = pq.top(); pq.pop();  // Siguiente menor frecuencia

        // Crea un nuevo nodo padre con la suma de ambas frecuencias
        Nodo* padre = new Nodo('\0', izq->freq + der->freq);
        padre->izq = izq;
        padre->der = der;

        // Inserta el nuevo nodo en la cola
        pq.push(padre);
    }

    // El ultimo nodo es la raiz del arbol de Huffman
    return pq.top();
}

// Genera los codigos binarios (prefijos Huffman) para cada letra.
// Recorre el arbol de manera recursiva:
//  - Añade '0' al moverse a la izquierda
//  - Añade '1' al moverse a la derecha
void generarCodigos(Nodo* raiz, string codigo, map<char,string>& codigos) {
    if (!raiz) return;

    // Si es hoja (no tiene hijos), asigna el codigo generado
    if (!raiz->izq && !raiz->der) {
        codigos[raiz->letra] = codigo;
    }

    // Continua por la izquierda y derecha
    generarCodigos(raiz->izq, codigo + "0", codigos);
    generarCodigos(raiz->der, codigo + "1", codigos);
}

// Reemplaza cada caracter del mensaje por su codigo Huffman
string codificar(const string& mensaje, map<char,string>& codigos) {
    string resultado;
    for (char c : mensaje)
        resultado += codigos[c];  // Concatena el codigo de cada letra
    return resultado;
}

// Decodifica el mensaje binario recorriendo el arbol Huffman
// Muestra en consola los pasos realizados (paso a paso)
string decodificarConPrefijos(const string& codigoBinario, Nodo* raiz, map<char,string>& codigos) {
    string mensaje;
    Nodo* temp = raiz;
    int paso = 0;

    cout << "Decodificacion paso a paso:";

    // Lee bit por bit del mensaje codificado
    for (char bit : codigoBinario) {
        paso++;

        // Navega por el arbol
        if (bit == '0') temp = temp->izq;
        else temp = temp->der;

        // Si llegamos a una hoja, se encontro una letra completa
        if (!temp->izq && !temp->der) {
            mensaje += temp->letra;
            cout << "Paso " << paso << ": se encontro '" << temp->letra
                << "' con prefijo " << codigos[temp->letra] << endl;
            temp = raiz; // Vuelve a la raiz para seguir decodificando
        }
    }
    return mensaje;
}

// Libera la memoria usada por el arbol de Huffman
void liberarArbol(Nodo* raiz) {
    if (!raiz) return;
    liberarArbol(raiz->izq);
    liberarArbol(raiz->der);
    delete raiz;
}

// Flujo principal del programa
int main() {
    string rutaArchivo = "assets/huffman.txt";

    // Lee el mensaje desde archivo
    string mensaje = leerArchivo(rutaArchivo);
    if (mensaje.empty()) return 1;  // Si falla la lectura, terminar

    // Calcula peso original (8 bits por caracter)
    size_t pesoInicial = mensaje.size() * 8;
    cout << "Peso inicial: " << pesoInicial << " bits (" 
        << mensaje.size() << " bytes)";

    // Cuenta la frecuencia de cada caracter en el texto
    map<char,int> freq;
    for (char c : mensaje)
        freq[c]++;

    cout << "Frecuencia de caracteres:";
    for (auto& par : freq) {
        if (par.first == ' ') cout << "'espacio'";
        else if (par.first == ' ') cout << "''";
        else cout << "'" << par.first << "'";
        cout << ": " << par.second << endl;
    }

    // Construye el arbol y genera los codigos Huffman
    Nodo* raiz = construirHuffman(freq);
    map<char,string> codigos;
    generarCodigos(raiz, "", codigos);

    // Muestra un prefijo por letra unica
    cout << "Codigos Huffman:";
    for (auto& par : codigos) {
        if (par.first == ' ') cout << "'espacio'";
        else if (par.first == ' ') cout << "' '";
        else cout << "'" << par.first << "'";
        cout << " → " << par.second << endl;
    }

    // Codifica el mensaje
    string codificado = codificar(mensaje, codigos);
    cout << "Mensaje codificado (primeros 200 bits): "
        << codificado.substr(0, min((size_t)200, codificado.size())) << "...";
    cout << "Longitud total: " << codificado.size() << " bits";

    // Calcula el porcentaje de compresion
    double porcentajeCompresion = 100.0 * (1.0 - (double)codificado.size() / pesoInicial);
    cout << "Compresion lograda: " << porcentajeCompresion << "%";

    // Decodifica paso a paso
    string decodificado = decodificarConPrefijos(codificado, raiz, codigos);
    cout << "Mensaje decodificado (primeros 200 caracteres): "
        << decodificado.substr(0, min((size_t)200, decodificado.size())) << endl;

    // Libera memoria
    liberarArbol(raiz);
    return 0;
}