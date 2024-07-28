# Universidad del Valle de Guatemala
# Redes - Sección: 10
# Mario Antonio Guerra Morales - Carné: 21008
# David Jonathan Aragón Vásquez - Carné: 21053
# Kristopher Javier Alvarado López - Carné: 21188
# Laboratorio 2 - Receptor

import socket

# Código para Hamming.

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
            break
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
            break

        conn.close()

if __name__ == "__main__":
    main()
