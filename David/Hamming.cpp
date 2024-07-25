// Universidad del Valle de Guatemala
// Redes - Sección: 10
// David Jonthan Aragon Vasquez - Carné: 21053
// Laboratorio 2 - Hamming

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

std::vector<int> generateHammingCode(std::vector<int> msg) {
    int m = msg.size();
    int r = 0;

    // Calculate the number of parity bits needed
    while ((m + r + 1) > std::pow(2, r)) {
        r++;
    }

    std::vector<int> hammingCode(m + r);
    int j = 0, k = 0;

    // Place the data bits and leave spaces for parity bits
    for (int i = 1; i <= m + r; i++) {
        if ((i & (i - 1)) == 0) {
            hammingCode[i - 1] = 0; // Placeholder for parity bit
        } else {
            hammingCode[i - 1] = msg[j++];
        }
    }

    // Calculate parity bits
    for (int i = 0; i < r; i++) {
        int parityPos = std::pow(2, i);
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

int main() {
    std::string input;
    std::cout << "Enter a binary message: ";
    std::cin >> input;

    std::vector<int> msg(input.length());
    for (size_t i = 0; i < input.length(); i++) {
        msg[i] = input[i] - '0';
    }

    std::vector<int> hammingCode = generateHammingCode(msg);

    std::cout << "Hamming code: ";
    for (int bit : hammingCode) {
        std::cout << bit;
    }
    std::cout << std::endl;

    return 0;
}
