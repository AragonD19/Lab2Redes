def crc32_receptor(trama_codificada):
    # Polinomio en formato lista
    polinomio = [1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1]
    SIZE = 32

    # Convertir la trama en una lista de enteros
    data = [int(bit) for bit in trama_codificada]

    # Valores a utilizar
    data_calc = data[:len(polinomio)]
    data = data[len(polinomio):]

    # polinomio | data
    while True:
        # Se eliminan los bits no significativos
        eliminados = 0

        while data_calc and data_calc[0] == 0:
            data_calc.pop(0)
            eliminados += 1

        # Se agregan los bits a data_calc para que sea del mismo tamaño que el polinomio
        for _ in range(eliminados):
            if not data:
                break
            data_calc.append(data.pop(0))

        # Si data_calc es menor que el polinomio, se termina el proceso
        if len(data_calc) < len(polinomio):
            if not data:
                # Se agregan 0 al inicio de data_calc para que sea del mismo tamaño que el polinomio
                data_calc = [0] * (SIZE - len(data_calc)) + data_calc
                break

        # Si data_calc es igual al polinomio, se hace XOR entre ambos
        if len(data_calc) == len(polinomio):
            for i in range(len(polinomio)):
                data_calc[i] ^= polinomio[i]

    # Comprobar si el CRC es correcto (todos los bits deben ser 0)
    if sum(data_calc) == 0:
        # Eliminar el CRC de la trama original
        trama_original = trama_codificada[:-SIZE]
        return "No se detectaron errores. Mensaje original: " + trama_original
    else:
        return "Se detectaron errores. El mensaje se descarta."



def main():
    trama_codificada = input("Introduce la trama codificada en binario: ")
    resultado = crc32_receptor(trama_codificada)
    print(resultado)

if __name__ == "__main__":
    main()
