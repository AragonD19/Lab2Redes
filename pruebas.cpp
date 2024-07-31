#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <ctime>
#include <cstdlib>
#include <winsock2.h>
#include <iomanip>
#include <sstream>
#include <random>
#include <unistd.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")// Para usleep()

using namespace std;

int numeroPruebas = 50; // Asigna el valor que prefieras aquí

int numPruebaActual = 0;


// Declaración de funciones (deberías implementar estas funciones según tu necesidad)
vector<int> codificarMensaje(const vector<int>& mensaje, int rate);
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

vector<int> aplicarRuido(const vector<int>& mensajeCodificado) {
    double probabilidadError = 0.4;
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

    

    

    // Verificar si se está enviando el último mensaje de prueba
    if (numPruebaActual == numeroPruebas - 1) {
        string finPruebas = "FIN_PRUEBAS";
        string message = algoritmo + "|" + mensajeCodificadoStr.str() + "|" + mensajeConRuidoStr.str() + "|" + finPruebas;
        send(sock, message.c_str(), message.length(), 0);
        cout << "Indicador de fin de pruebas enviado\n";
    }else{
        string finPruebas = "NO";
        string message = algoritmo + "|" + mensajeCodificadoStr.str() + "|" + mensajeConRuidoStr.str() + "|" + finPruebas;
        send(sock, message.c_str(), message.length(), 0);
        cout << "Mensaje enviado\n";
    }

    // Incrementar el contador de pruebas enviadas
    numPruebaActual++;

    closesocket(sock);
    WSACleanup();
}

// Función para ejecutar el receptor automatizado
void ejecutarReceptor() {
    // Aquí puedes agregar el código para iniciar el receptor, si es necesario
    cout << "Ejecutando el receptor automatizado..." << endl;
    // Por ejemplo, podrías usar system("receptor.exe") para ejecutar el receptor
    // system("receptor.exe");
}

// Función para generar una palabra aleatoria
string generarPalabraAleatoria(int longitudMin, int longitudMax) {
    static const char alfabeto[] = "abcdefghijklmnopqrstuvwxyz";
    static mt19937 gen(std::random_device{}());
    uniform_int_distribution<> longitudDist(longitudMin, longitudMax);
    uniform_int_distribution<> charDist(0, sizeof(alfabeto) - 2);

    int longitud = longitudDist(gen);
    std::string palabra;

    for (int i = 0; i < longitud; ++i) {
        palabra += alfabeto[charDist(gen)];
    }

    return palabra;
}

// Función que será ejecutada por el hilo
void generadorDePalabras(int longitudMin, int longitudMax, int numeroPruebas, vector<string>& palabras) {
    for (int i = 0; i < numeroPruebas; ++i) {
        palabras.push_back(generarPalabraAleatoria(longitudMin, longitudMax));
        usleep(500000); // Esperar 0.5 segundos entre cada generación (ajustar según sea necesario)
    }
}

// Función principal
int main() {
    srand(static_cast<unsigned int>(time(0)));
    
    int Nalgoritmo;
    
    cout << "Selecciona el algoritmo a utilizar:\n";
    cout << "1. Hamming\n";
    cout << "2. Viterbi\n";
    cout << "3. CRC-32\n";
    cin >> Nalgoritmo;


    string algoritmo;

    // Iniciar el receptor en un proceso separado (puedes ajustar esto según cómo ejecutas el receptor)
    ejecutarReceptor();

    // Esperar un momento para asegurar que el receptor esté listo
    usleep(2000000); // Esperar 2 segundos (ajustar según sea necesario)

    vector<string> palabras;
    thread hiloGenerador(generadorDePalabras, 5, 10, numeroPruebas, ref(palabras));

    for (int i = 0; i < numeroPruebas; ++i) {
        // Esperar a que la palabra sea generada
        while (palabras.size() <= i) {
            usleep(100000); // Esperar 0.1 segundos
        }
        string input = palabras[i];
        vector<int> mensaje;
        for (char c : input) {
            bitset<8> bin(c);
            for (int j = 7; j >= 0; --j) {
                mensaje.push_back(bin[j]);
            }
        }

        vector<int> mensajeCodificado;
        switch (Nalgoritmo) {
            case 1:
                cout << "Algoritmo Hamming no implementado." << endl;
                return 1;
            case 2:
                //mensajeCodificado = codificarMensaje(mensaje, 2);
                break;
            case 3:
                mensajeCodificado = crc32(mensaje);
                algoritmo = "CRC-32";
                break;
            default:
                cout << "Opción no válida." << endl;
                return 1;
        }

        vector<int> mensajeConRuido = aplicarRuido(mensajeCodificado);
        enviarMensaje(algoritmo, mensajeCodificado, mensajeConRuido);



        // Esperar un momento antes de iniciar la siguiente prueba
        usleep(2000000); // Esperar 2 segundos (ajustar según sea necesario)
    }

    // Esperar a que el hilo de generación de palabras termine
    hiloGenerador.join();

    return 0;
}