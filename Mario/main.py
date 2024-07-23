# Universidad del Valle de Guatemala
# Redes - Sección: 10
# Mario Antonio Guerra Morales - Carné: 21008
# Laboratorio 2 - Algoritmo de Viterbi (Receptor)

def hamming_distance(x, y):
    #Calcula la distancia de Hamming entre dos listas de bits.
    return sum(el1 != el2 for el1, el2 in zip(x, y))

def decodificarMensaje(mensajeCodificado, rate):
    #Decodifica un mensaje codificado usando el algoritmo de Viterbi.
    n = len(mensajeCodificado) // rate  # Número de bits en el mensaje original
    trellis = [{} for _ in range(n + 1)]
    
    # Inicializar el trellis con el estado inicial
    trellis[0][(0, 0)] = (0, '')

    for i in range(n):
        for state in trellis[i]:
            for bit in [0, 1]:
                # Calcular el nuevo estado
                new_state = ((state[0] << 1) | bit) & 3
                # Calcular las salidas esperadas
                salida1 = (state[0] & 1) ^ bit
                salida2 = ((state[0] >> 1) & 1) ^ bit
                nueva_salida = [salida1, salida2]
                new_path = trellis[i][state][1] + str(bit)

                # Calcular la métrica de Hamming entre la salida esperada y la recibida
                metrica = trellis[i][state][0] + hamming_distance([mensajeCodificado[2*i], mensajeCodificado[2*i+1]], nueva_salida)
                
                # Actualizar el trellis si encontramos un camino con mejor métrica
                if (new_state, state[1]) not in trellis[i + 1] or metrica < trellis[i + 1][(new_state, state[1])][0]:
                    trellis[i + 1][(new_state, state[1])] = (metrica, new_path)

    # Encontrar el mejor camino en el trellis
    best_path = min(trellis[-1].values(), key=lambda x: x[0])[1]
    return best_path

def find_corrected_bits(original, corrected):
    # Encuentra los bits corregidos comparando el mensaje original con el mensaje corregido.
    bitsCorregidos = []
    for i in range(len(original)):
        if original[i] != corrected[i]:
            bitsCorregidos.append(i)
    return bitsCorregidos

def main():
    mensajeCodificado = input("Introduce el mensaje codificado en binario: ")
    rate = 2
    mensajeCodificado = [int(bit) for bit in mensajeCodificado]
    
    # Decodificar el mensaje usando el algoritmo de Viterbi
    mensajeDecodificado = decodificarMensaje(mensajeCodificado, rate)

    bitsCorregidos = find_corrected_bits(mensajeCodificado[:len(mensajeDecodificado)], mensajeDecodificado)
    
    # Aquí podrías incluir lógica para detectar y corregir errores
    if len(bitsCorregidos) == 0:
        print("No se detectaron errores. Mensaje original:", mensajeDecodificado)
    else:
        print("Se detectaron y corrigieron errores. Posiciones de los bits corregidos:", bitsCorregidos)
        print("Se detectaron errores. Mensaje corregido:", mensajeDecodificado)

if __name__ == "__main__":
    main()
