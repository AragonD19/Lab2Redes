// Universidad del Valle de Guatemala
// Redes - Sección: 10
// Mario Antonio Guerra Morales - Carné: 21008
// Laboratorio 2 - Algoritmo de Viterbi (Emisor)

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Función para codificar un binario por medio de código convolucional
vector<int> codificarMensaje(const vector<int>& mensaje, int rate) {
    vector<int> mensajeCodificado;
    int state = 0;
    for (int bit : mensaje) {
        // Calcular salidas para el bit actual.
        int salida1 = (state & 1) ^ bit;
        int salida2 = ((state >> 1) & 1) ^ bit;
        // Añadir salidas al mensaje codificado.
        mensajeCodificado.push_back(salida1);
        mensajeCodificado.push_back(salida2);
        // Actualizar el estado.
        state = (((state << 1) | bit ) & 1);
    }
    return mensajeCodificado;
}

int main() {
    string input;
    cout << "Introduce el mensaje en binario: ";
    cin >> input;

    // Convertir la cadena binaria a un vector de enteros.
    vector<int> mensaje;
    for (char c : input) {
        mensaje.push_back(c - '0');
    }

    int rate = 2; // Tasa de código 2:1
    vector<int> mensajeCodificado = codificarMensaje(mensaje, rate);

    // Imprimir el mensaje codificado.

    cout << "Mensaje codificado: ";
    for (int bit : mensajeCodificado) {
        cout << bit;
    }
    cout << endl;

    return 0;
}