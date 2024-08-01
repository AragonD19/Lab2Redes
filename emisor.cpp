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
#include <iomanip>
#include <sstream>
#include <cmath>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

/*
* Código para Hamming
*/

// Función para generar el código de Hamming
vector<int> generateHammingCode(vector<int> msg) {
    int m = msg.size();
    int r = 0;

    // Calcular el número de bits de paridad necesarios
    while ((m + r + 1) > static_cast<int>(pow(2, r))) {
        r++;
    }

    vector<int> hammingCode(m + r);
    int j = 0, k = 0;

    // Colocar los bits de datos y dejar espacios para los bits de paridad
    for (int i = 1; i <= m + r; i++) {
        if ((i & (i - 1)) == 0) {
            hammingCode[i - 1] = 0; // Espacio para el bit de paridad
        } else {
            hammingCode[i - 1] = msg[j++];
        }
    }

    // Calcular los bits de paridad
    for (int i = 0; i < r; i++) {
        int parityPos = static_cast<int>(pow(2, i));
        int parity = 0;
        for (int j = 1; j <= m + r; j++) {
            if (j & parityPos) {
                parity ^= hammingCode[j - 1];
            }
        }
        hammingCode[parityPos - 1] = parity;
    }

    return hammingCode;
}


/*
* Código para Viterbi
*/

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

void enviarMensaje(const string& algoritmo, const vector<int>& mensajeCodificado, const vector<int>& mensajeConRuido) {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    stringstream mensajeCodificadoStr, mensajeConRuidoStr;

    for (int bit : mensajeCodificado) {
        mensajeCodificadoStr << bit;
    }
    for (int bit : mensajeConRuido) {
        mensajeConRuidoStr << bit;
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
        cout << "Conexion fallida: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    string message = algoritmo + "|" + mensajeCodificadoStr.str() + "|" + mensajeConRuidoStr.str();
    send(sock, message.c_str(), message.length(), 0);
    cout << "Mensaje enviado\n";
    

    closesocket(sock);
    WSACleanup();
}

/*
* Código para CRC-32
*/
vector<int> crc32(const vector<int>& trama) {
    // Polinomio en formato vector
    vector<int> polinomio = {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1};
    const int SIZE = 32;

    // Convertir la trama en un vector de enteros
    vector<int> data = trama;

    // Agregar ceros al final de la trama
    data.insert(data.end(), SIZE, 0);

    // Valores a utilizar
    vector<int> data_calc(data.begin(), data.begin() + polinomio.size());
    data.erase(data.begin(), data.begin() + polinomio.size());

    // polinomio | data
    while (true) {
        // Se eliminan los bits no significativos
        int eliminados = 0;

        while (!data_calc.empty() && data_calc[0] == 0) {
            data_calc.erase(data_calc.begin());
            eliminados++;
        }

        // Se agregan los bits a data_calc para que sea del mismo tamaño que el polinomio
        for (int i = 0; i < eliminados; ++i) {
            if (data.empty()) break;
            int bit = data[0];
            data.erase(data.begin());
            data_calc.push_back(bit);
        }

        // Si data_calc es menor que el polinomio, se termina el proceso
        if (data_calc.size() < polinomio.size()) {
            if (data.empty()) {
                // Se agregan 0 al inicio de data_calc para que sea del mismo tamaño que el polinomio
                data_calc.insert(data_calc.begin(), SIZE - data_calc.size(), 0);
                break;
            }
        }

        // Si data_calc es igual al polinomio, se hace XOR entre ambos
        if (data_calc.size() == polinomio.size()) {
            for (size_t i = 0; i < polinomio.size(); ++i) {
                data_calc[i] ^= polinomio[i];
            }
        }
    }

    // Combinar la trama original con el CRC calculado
    vector<int> resultado = trama;
    resultado.insert(resultado.end(), data_calc.begin(), data_calc.end());

    return resultado;
}



/*
* Función Main.
*/

int main() {
    srand(time(0));
    
    string input;
    cout << "Introduce el mensaje en texto: ";
    getline(cin, input); // Usar getline para aceptar espacios

    // Convertir la cadena binaria a un vector de enteros.
    vector<int> mensaje;
    for (char c : input) {
        bitset<8> bin(c);
        for (int i = 7; i >= 0; --i) {
            mensaje.push_back(bin[i]);
        }
    }

    int opcion;
    cout << "Selecciona el algoritmo a utilizar:\n";
    cout << "1. Hamming\n";
    cout << "2. Viterbi\n";
    cout << "3. CRC-32\n";
    cin >> opcion;

    vector<int> mensajeCodificado;
    string algoritmo;
    int rate = 2; // Tasa de código 2:1

    switch (opcion) {
        case 1:
            mensajeCodificado = generateHammingCode(mensaje);
            algoritmo = "Hamming";
            break;
        case 2:
            mensajeCodificado = codificarMensaje(mensaje, rate);
            algoritmo = "Viterbi";
            break;
        case 3:
            mensajeCodificado = crc32(mensaje);
            algoritmo = "CRC-32";
            break;
        default:
            cout << "Opcion no valida\n";
            return 1;
    }

    vector<int> mensajeConRuido = aplicarRuido(mensajeCodificado);
    enviarMensaje(algoritmo, mensajeCodificado, mensajeConRuido);

    return 0;
}