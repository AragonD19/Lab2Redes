# Universidad del Valle de Guatemala
# Redes - Sección: 10
# Mario Antonio Guerra Morales - Carné: 21008
# David Jonathan Aragón Vásquez - Carné: 21053
# Kristopher Javier Alvarado López - Carné: 21188
# Laboratorio 2 - Receptor

import socket

# Código para Hamming.

def calculate_parity_bits(hamming_code):
    n = len(hamming_code)
    r = 0

    # Calculate number of parity bits
    while (2**r < n + 1):
        r += 1

    parity_bits = []
    for i in range(r):
        parity_pos = 2 ** i
        parity = 0
        for j in range(1, n + 1):
            if j & parity_pos:
                parity ^= hamming_code[j - 1]
        parity_bits.append(parity)

    return parity_bits

def check_and_correct_hamming_code(hamming_code):
    n = len(hamming_code)
    r = 0

    # Calculate number of parity bits
    while (2**r < n + 1):
        r += 1

    # Calculate parity bits and find the error position
    error_pos = 0
    for i in range(r):
        parity_pos = 2 ** i
        parity = 0
        for j in range(1, n + 1):
            if j & parity_pos:
                parity ^= hamming_code[j - 1]
        if parity != 0:
            error_pos += parity_pos

    if error_pos:
        print(f"Error detected at bit position: {error_pos}")
        # Correct the error
        hamming_code[error_pos - 1] ^= 1
        print("Error corrected.")
        
        # Recalculate parity bits to check for multiple errors
        recalculated_parity_bits = calculate_parity_bits(hamming_code)
        if any(recalculated_parity_bits):
            print("Multiple errors detected. Message discarded.")
            return [], error_pos, True

    # Extract the original message
    original_message = []
    for i in range(1, n + 1):
        if not (i & (i - 1)) == 0:
            original_message.append(hamming_code[i - 1])

    return original_message, error_pos, False

# Código para Viterbi.

def hamming_distance(x, y):
    return sum(el1 != el2 for el1, el2 in zip(x, y))

def decodificarMensaje(mensajeCodificado, rate):
    n = len(mensajeCodificado) // rate
    trellis = [{} for _ in range(n + 1)]
    trellis[0][0] = (0, '')

    for i in range(n):
        for state in trellis[i]:
            for bit in [0, 1]:
                new_state = (((state << 1) | bit) & 1)
                salida1 = (state & 1) ^ bit
                salida2 = ((state >> 1) & 1) ^ bit
                nueva_salida = [salida1, salida2]
                new_path = trellis[i][state][1] + str(bit)
                metrica = trellis[i][state][0] + hamming_distance([mensajeCodificado[2*i], mensajeCodificado[2*i+1]], nueva_salida)
                
                if new_state not in trellis[i + 1] or metrica < trellis[i + 1][new_state][0]:
                    trellis[i + 1][new_state] = (metrica, new_path)

    best_path = min(trellis[-1].values(), key=lambda x: x[0])[1]
    return best_path

def bin_to_text(binary_string):
    n = int(binary_string, 2)
    return n.to_bytes((n.bit_length() + 7) // 8, 'big').decode('utf-8', 'ignore') or '\0'

# Código para CRC-32

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


# Función Main.

def main():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(("127.0.0.1", 9090))
    server_socket.listen(1)

    while True:
        print("Esperando conexión...")
        conn, addr = server_socket.accept()
        data = conn.recv(4096).decode()
        algoritmo, mensajeCodificadoStr, mensajeConRuidoStr = data.split('|')

        if algoritmo == 'Hamming':
            mensajeCodificado = [int(bit) for bit in mensajeCodificadoStr]
            mensajeConRuido = [int(bit) for bit in mensajeConRuidoStr]

            print(f"Algoritmo utilizado: {algoritmo}")
            print("Mensaje codificado recibido.\n")

            original_message, error_pos, multiple_errors = check_and_correct_hamming_code(mensajeConRuido)
            
            if error_pos == 0:
                print("No errors detected.")
            elif not multiple_errors:
                print("Errors detected and corrected.")
            else:
                print("Multiple errors detected. Message discarded.")
            
            if not multiple_errors:
                mensajeTexto = bin_to_text(''.join(map(str, original_message)))
                print("Mensaje decodificado en texto:", mensajeTexto)

        if algoritmo == 'Viterbi':

            mensajeCodificado = [int(bit) for bit in mensajeCodificadoStr]
            mensajeConRuido = [int(bit) for bit in mensajeConRuidoStr]
            rate = 2

            print(f"Algoritmo utilizado: {algoritmo}")
            print("Mensaje codificado recibido.\n")
            #print(mensajeCodificadoStr)

            mensajeDecodificado = decodificarMensaje(mensajeConRuido, rate)
            
            if mensajeCodificado == mensajeConRuido:
                print("No se detectaron errores. Mensaje original:\n")
                #print(str(mensajeDecodificado))
            else:
                print("Se detectaron errores. Mensaje corregido:\n")
                #print(str(mensajeDecodificado))
            
            mensajeTexto = bin_to_text(''.join(mensajeDecodificado))
            print("Mensaje decodificado en texto:", mensajeTexto)
        if algoritmo == 'CRC-32':
            mensajeCodificado = [int(bit) for bit in mensajeCodificadoStr]
            resultado = crc32_receptor(mensajeCodificadoStr)
            print(f"Algoritmo utilizado: {algoritmo}")
            print("Mensaje codificado recibido.\n")
            print(resultado)

        conn.close()

if __name__ == "__main__":
    main()
