#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Función para solicitar y validar la entrada de la trama
std::string menu() {
    std::string trama;
    while (true) {
        std::cout << "Ingrese una trama en binario: ";
        std::getline(std::cin, trama);

        // Eliminar espacios
        trama.erase(std::remove(trama.begin(), trama.end(), ' '), trama.end());

        // Revisar que la trama sea binaria (solo tenga 0s y 1s)
        bool isBinary = std::all_of(trama.begin(), trama.end(), [](char c) { return c == '0' || c == '1'; });

        if (!isBinary) {
            std::cout << "La trama debe ser binaria (solo 0s y 1s)" << std::endl;
        } else {
            break;
        }
    }
    return trama;
}

// Función para calcular el CRC32
std::string crc32(const std::string& trama) {
    // Polinomio en formato vector
    std::vector<int> polinomio = {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1};
    const int SIZE = 32;

    

    // Convertir la trama en un vector de enteros
    std::vector<int> data(trama.begin(), trama.end());
    for (auto& bit : data) bit -= '0'; // Convertir char a int

    // Agregar ceros al final de la trama
    data.insert(data.end(), SIZE, 0);

    // Valores a utilizar
    std::vector<int> data_calc(data.begin(), data.begin() + polinomio.size());
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

    // Convertir el resultado de vuelta a una representación en cadena
    std::string crc;
    for (const auto& bit : data_calc) {
        crc += std::to_string(bit);
    }
    return trama + crc;
}

int main() {

    // Ejemplo: 111011011011100010000011001000001

    std::string trama = menu();
    std::string encoded = crc32(trama);
    std::cout << "\nTrama codificada: " << encoded << std::endl;
    return 0;
}