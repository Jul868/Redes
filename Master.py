import socket
import time

# Configuración del microcontrolador (IP y puerto)
microcontrolador_ip = '192.168.43.195'  # Cambia esto a la IP del ESP32
microcontrolador_puerto = 502  # Puerto en el que el ESP32 está escuchando

# Crear un socket TCP/IP
cliente = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Conectar el socket al servidor
cliente.connect((microcontrolador_ip, microcontrolador_puerto))

# Constantes Modbus
TRANSACTION_ID = 0
PROTOCOL_ID = "0000"
UNIT_ID = "01"



# Función para enviar trama Modbus
def enviar_trama(funcion):
    global cliente, TRANSACTION_ID, PROTOCOL_ID, UNIT_ID
    TRANSACTION_ID += 1
    CODE = "{:04x}".format(TRANSACTION_ID) + PROTOCOL_ID
    mensaje = CODE + "0006" + UNIT_ID + "0" + funcion + "0000" + "0004"
    cliente.sendall(mensaje.encode())
    print(mensaje)
    
def enviar_trama_single_register(valor, registro):
    global cliente, TRANSACTION_ID, PROTOCOL_ID, UNIT_ID
    TRANSACTION_ID += 1
    CODE = "{:04x}".format(TRANSACTION_ID) + PROTOCOL_ID
    mensaje = CODE + "0006" + UNIT_ID + "06" + "{:04X}".format(int(registro)) + "{:04X}".format(int(valor))
    cliente.sendall(mensaje.encode())
    print(mensaje)

# Función para enviar trama Modbus "Write Multiple Registers"
def enviar_trama_multiple_registers(valores):
    global cliente, TRANSACTION_ID, PROTOCOL_ID, UNIT_ID
    TRANSACTION_ID += 1
    CODE = "{:04x}".format(TRANSACTION_ID) + PROTOCOL_ID
    cantidad = len(valores)
    mensaje = CODE + "0010" + UNIT_ID + "10" + "0000" + "{:04X}".format(cantidad-1) 
    for v in valores:
        mensaje += "{:04X}".format(int(v))
    cliente.sendall(bytes.fromhex(mensaje))
    print(mensaje)

# Función principal

def main():
    global TRANSACTION_ID
    while True:
        opcion = input("¿Qué acción deseas realizar?\n1. Read Coils\n2. Read Discret Inputs\n3. Read Holding Register\n5. Write Single Coil\n6. Write Register\n15. Write Multiple Coils\n16. Write Multiple Registers\nOtro. Salir\nOpción: ")
        
        if opcion in ["1", "2", "3", "5", "15"]:
            enviar_trama(opcion)

        elif opcion == "6":
            registro = input("Digite registro a cambiar: ")
            valor = input("Digite valor a cambiar entre (0 y 9): ")
            
            if valor.isdigit() and registro.isdigit():
                enviar_trama_single_register(valor, registro)

            else:
                print("Valor no válido. Debe ser un número entre 0 y 9.")

        elif opcion == "16":
            valores = input("Digite valores a cambiar separados por coma: ")
            valores = valores.split(',')

            if all(valor.isdigit() for valor in valores):
                enviar_trama_multiple_registers(valores)

            else:
                print("Valores no válidos. Deben ser números entre 0 y 9 separados por coma.")

        else:
            print("Saliendo...")
            break

        mensaje_esp = cliente.recv(1024)
        print("Mensaje recibido de la ESP32:", mensaje_esp.decode())

    cliente.close()

if __name__ == "__main__":
    main()

'''
while True:
    opcion = input("¿Qué acción deseas realizar?\n1. Read Coils\n2. Read Discret Inputs\n3. Read Holding Register\n5. Write Single Coil\n6. Write Register\n15. Write Multiple Coils\n16. Write Multiple Registers\nOtro. Salir\nOpción: ")
            
    if opcion in ["1", "2", "3", "5", "15"]:
        enviar_trama(opcion)

    elif opcion == "6":
        registro = input("Digite registro a cambiar: ")
        valor = input("Digite valor a cambiar entre (0 y 9): ")
                
        if valor.isdigit() and registro.isdigit():
            enviar_trama_single_register(valor, registro)

        else:
            print("Valor no válido. Debe ser un número entre 0 y 9.")

    elif opcion == "16":
        valores = input("Digite valores a cambiar separados por coma: ")
        valores = valores.split(',')

        if all(valor.isdigit() for valor in valores):
            enviar_trama_multiple_registers(valores)

        else:
            print("Valores no válidos. Deben ser números entre 0 y 9 separados por coma.")

    else:
        print("Saliendo...")
        cliente.close()
        break

    time.sleep(0.5)
    mensaje_esp = cliente.recv(1024)
    print("Mensaje recibido de la ESP32:", mensaje_esp.decode())
'''


