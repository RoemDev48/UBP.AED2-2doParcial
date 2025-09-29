#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class f_b{
    private:
        string objetivo; // contraseña a adivinar
        string alfabeto;
        long long intento2 = 0;
        public:
        f_b(string a,string b){
            objetivo=a;
            alfabeto=b;
        }
        bool generar(string actual) {
    // Si ya armamos un string del mismo tamaño que la contraseña
    if (actual.size() == objetivo.size()) {
        intento2++;
        if (actual == objetivo) {
            cout << "Contraseña encontrada: " << actual << endl;
            cout << "Intentos realizados: " << intento2 << endl;
            return true; // encontrado, parar
        }
        return false; // no coincide
    }

    // Todavía faltan letras: seguir agregando
    for (char c : alfabeto) {
        if (generar(actual + c)) {
            return true; // encontrado, propagar true para cortar
        }
    }

    return false; // no encontrado en esta rama
}
};    // para contar los intentos
// Función recursiva para generar combinaciones

string texto();
int main() {
    f_b p(texto(),"abcdefghijklmnopqrstuvwxyz");
    p.generar("");
    // Llamamos la función recursiva empezando con string vacío
    return 0;
}

string texto(){
    ifstream clave("textito.txt");
    string palabra;
    if(!clave){
        cout<<"archivo no encontrado"<<endl;
        return "";
    }
    getline(clave,palabra);

    return palabra;
}