#include <iostream>
#include <random>
#include <string>

// Función para generar una palabra aleatoria
std::string generarPalabraAleatoria(int longitudMin, int longitudMax) {
    static const char alfabeto[] = "abcdefghijklmnopqrstuvwxyz";
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> longitudDist(longitudMin, longitudMax);
    std::uniform_int_distribution<> charDist(0, sizeof(alfabeto) - 2);

    int longitud = longitudDist(gen);
    std::string palabra;

    for (int i = 0; i < longitud; ++i) {
        palabra += alfabeto[charDist(gen)];
    }

    return palabra;
}

int main() {
    std::string palabra = generarPalabraAleatoria(5, 10);
    std::cout << "Palabra aleatoria: " << palabra << std::endl;
    return 0;
}