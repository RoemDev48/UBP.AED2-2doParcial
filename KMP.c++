#include <iostream>
#include <vector>
#include <string>
using namespace std;

class KMP {
private:
    string texto;            // texto donde buscar
    string patron;           // patron a buscar
    vector<int> lps;         // tabla LPS

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
    // Constructor recibe texto y patron
    KMP(const string &t, const string &p) {
        texto = t;
        patron = p;
        construirLPS();
    }

    // Buscar el patron en el texto y mostrar resultado sin tildes
    void buscar() {
        bool encontrado = false;
        int n = texto.size();
        int m = patron.size();
        int i = 0;
        int j = 0;

        while (i < n) {
            if (texto[i] == patron[j]) {
                i++;
                j++;
            }

            if (j == m) {
                encontrado = true; // coincidencia completa
                break;
            } else if (i < n && texto[i] != patron[j]) {
                if (j != 0) {
                    j = lps[j - 1];
                } else {
                    i++;
                }
            }
        }

        if (encontrado) {
            cout << "El patron \"" << patron << "\" se encontro en el texto." << endl;
        } else {
            cout << "El patron \"" << patron << "\" no se encontro en el texto." << endl;
        }
    }
};

int main() {
    string texto = "PALRANABABACA";
    string patron = "ABABACA";

    KMP buscador(texto, patron);
    buscador.buscar();

    return 0;
}
