import socket
import time
import matplotlib.pyplot as plt
import subprocess

def hamming_distance(x, y):
    return sum(el1 != el2 for el1, el2 in zip(x, y))

def decodificarMensaje(mensajeCodificado, rate):
    n = len(mensajeCodificado) // rate  # Número de bits en el mensaje original
    trellis = [{} for _ in range(n + 1)]
    
    # Inicializar el trellis con el estado inicial
    trellis[0][0] = (0, '')

    for i in range(n):
        for state in trellis[i]:
            for bit in [0, 1]:
                # Calcular el nuevo estado
                new_state = (((state << 1) | bit) & 1)
                # Calcular las salidas esperadas
                salida1 = (state & 1) ^ bit
                salida2 = ((state >> 1) & 1) ^ bit
                nueva_salida = [salida1, salida2]
                new_path = trellis[i][state][1] + str(bit)

                # Calcular la métrica de Hamming entre la salida esperada y la recibida
                metrica = trellis[i][state][0] + hamming_distance([mensajeCodificado[2*i], mensajeCodificado[2*i+1]], nueva_salida)
                
                # Actualizar el trellis si encontramos un camino con mejor métrica
                if new_state not in trellis[i + 1] or metrica < trellis[i + 1][new_state][0]:
                    trellis[i + 1][new_state] = (metrica, new_path)

    # Encontrar el mejor camino en el trellis
    best_path = min(trellis[-1].values(), key=lambda x: x[0])[1]
    return best_path

def simuladorDePruebas(numeroPruebas, probabilidadError):
    errores_detectados = 0
    errores_corregidos = 0

    # Inicializar el receptor
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', 9090))
    server_socket.listen(1)

    for _ in range(numeroPruebas):
        # Ejecutar el emisor en modo de prueba automatizado
        subprocess.run(['emisor.exe', 'test'])
        
        # Asegurarse de esperar el tiempo necesario
        time.sleep(1)

        # Aceptar la conexión y recibir los datos
        conn, addr = server_socket.accept()
        data = conn.recv(4096).decode()  # Aumentar el tamaño del buffer si es necesario
        conn.close()

        # Imprimir los datos recibidos para depuración
        print("Datos recibidos:", data)

        # Procesar los datos recibidos
        try:
            mensajeCodificado = [int(bit) for bit in data if bit in '01']  # Filtrar solo bits válidos
        except ValueError as e:
            print(f"Error al convertir los datos: {e}")
            continue

        rate = 2
        mensajeDecodificado = decodificarMensaje(mensajeCodificado, rate)
        
        original = ''.join(mensajeDecodificado)
        if hamming_distance(mensajeCodificado, [int(bit) for bit in original]) != 0:
            errores_detectados += 1
            errores_corregidos += 1 if hamming_distance(message, original) == 0 else 0

    server_socket.close()

    return errores_detectados, errores_corregidos

def main():
    numeroPruebas = 4
    probabilidadesError = [0.01, 0.05, 0.1]  # Lista de probabilidades de error a probar
    resultados = []

    for probabilidadError in probabilidadesError:
        print(f"Probabilidad de error: {probabilidadError}")
        errores_detectados, errores_corregidos = simuladorDePruebas(numeroPruebas, probabilidadError)
        resultados.append((probabilidadError, errores_detectados, errores_corregidos))

    probabilidadError, errores_detectados, errores_corregidos = zip(*resultados)
    
    plt.figure(figsize=(10, 5))
    plt.plot(probabilidadError, errores_detectados, label='Errores detectados', marker='o')
    plt.plot(probabilidadError, errores_corregidos, label='Errores corregidos', marker='o')
    plt.xlabel('Probabilidad de error')
    plt.ylabel('Número de errores')
    plt.legend()
    plt.title('Errores detectados y corregidos vs. Probabilidad de error')
    plt.xscale('log')  # Opcional: Usa escala logarítmica si los valores de probabilidad varían ampliamente
    plt.show()

if __name__ == "__main__":
    main()
