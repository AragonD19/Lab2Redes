// Universidad del Valle de Guatemala
// Redes - Sección: 10
// Mario Antonio Guerra Morales - Carné: 21008
// David Jonathan Aragón Vásquez - Carné: 21053
// Kristopher Javier Alvarado López - Carné: 21188
// Laboratorio 2 - Emisor

#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <ctime>
#include <cstdlib>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

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

vector<int> aplicarRuido(const vector<int>& mensajeCodificado) {
    double probabilidadError = 0.01;
    vector<int> mensajeConRuido = mensajeCodificado;
    for (int& bit : mensajeConRuido) {
        if (rand() % 100 < (probabilidadError * 100)) {
            bit = 1 - bit; //Voltea un bit.
        }
    }
    return mensajeConRuido;
}

void enviarMensaje(const vector<int>& mensajeConRuido) {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    string message;

    for (int bit : mensajeConRuido) {
        message += to_string(bit);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Fallo en iniciar WinSock" << endl;
        return;
    }

    // Crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cout << "Error al crear el socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9090);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        cout << "Conexión fallida: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Enviar el mensaje
    send(sock, message.c_str(), message.length(), 0);
    cout << "Mensaje enviado\n";

    closesocket(sock);
    WSACleanup();
}

int main() {
    srand(time(0));
    
    string input;
    cout << "Introduce el mensaje en texto: ";
    cin >> input;

    // Convertir la cadena binaria a un vector de enteros.
    vector<int> mensaje;
    for (char c : input) {
        bitset<8> bin(c);
        for (int i = 7; i >= 0; --i) {
            mensaje.push_back(bin[i]);
        }
    }

    int rate = 2; // Tasa de código 2:1
    vector<int> mensajeCodificado = codificarMensaje(mensaje, rate);

    vector<int> mensajeConRuido = aplicarRuido(mensajeCodificado);

    enviarMensaje(mensajeConRuido);

    return 0;
}