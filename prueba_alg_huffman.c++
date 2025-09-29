#include <bits/stdc++.h>
#include <fstream>
using namespace std;

string calamardo();

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
        return a->freq > b->freq; // menor frecuencia = prioridad
    }
};

// Construir el árbol de Huffman
Nodo* construirHuffman(map<char,int>& freq) {
    priority_queue<Nodo*, vector<Nodo*>, Comparar> pq;

    // Crear nodos para cada letra
    for(auto par : freq) {
        pq.push(new Nodo(par.first, par.second));
    }

    // Combinar nodos hasta tener un solo árbol
    while(pq.size() > 1) {
        Nodo* izq = pq.top(); pq.pop();
        Nodo* der = pq.top(); pq.pop();

        Nodo* padre = new Nodo('\0', izq->freq + der->freq);
        padre->izq = izq;
        padre->der = der;

        pq.push(padre);
    }

    return pq.top(); // raíz del árbol
}

// Generar los códigos binarios de cada letra
void generarCodigos(Nodo* raiz, string codigo, map<char,string>& codigos) {
    if(!raiz) return;

    // Si es hoja, guardar código
    if(!raiz->izq && !raiz->der) {
        codigos[raiz->letra] = codigo;
    }

    generarCodigos(raiz->izq, codigo + "0", codigos);
    generarCodigos(raiz->der, codigo + "1", codigos);
}

// Codificar un mensaje
string codificar(string mensaje, map<char,string>& codigos) {
    string resultado = "";
    for(char c : mensaje) {
        resultado += codigos[c];
    }
    return resultado;
}

// Decodificar un mensaje
string decodificar(string codigoBinario, Nodo* raiz) {
    string mensaje = "";
    Nodo* temp = raiz;
    for(char bit : codigoBinario) {
        if(bit == '0') temp = temp->izq;
        else temp = temp->der;

        // Si llegamos a una hoja
        if(!temp->izq && !temp->der) {
            mensaje += temp->letra;
            temp = raiz; // volver a la raíz
        }
    }
    return mensaje;
}

// Función principal
int main() {
    string mensaje = calamardo();   
    
    // Contar frecuencia de cada letra
    map<char,int> freq;
    for(char c : mensaje) freq[c]++;

    // Construir árbol de Huffman
    Nodo* raiz = construirHuffman(freq);

    // Generar códigos binarios
    map<char,string> codigos;
    generarCodigos(raiz, "", codigos);

    // Mostrar los códigos
    cout << "Codigos de Huffman:\n";
    for(auto par : codigos) {
        cout << par.first << ": " << par.second << endl;
    }

    // Codificar mensaje
    string codificado = codificar(mensaje, codigos);
    cout << "\nMensaje codificado: " << codificado << endl;

    // Decodificar mensaje
    string decodificado = decodificar(codificado, raiz);
    cout << "Mensaje decodificado: " << decodificado << endl;

    return 0;
}

string calamardo(){
    string palabra;
    ifstream texto("textito.txt");
    getline(texto,palabra);
    if (!texto) {
    cerr << "Error: no se pudo abrir el archivo\n";
    return "ERROR";
}
    return palabra;
}