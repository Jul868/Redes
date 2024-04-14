import socket
import time
from prettytable import PrettyTable

# Configuración del microcontrolador (IP y puerto)
microcontrolador_ip = '192.168.0.103'  # Cambia esto a la IP del ESP32
microcontrolador_puerto = 502  # Puerto en el que el ESP32 está escuchando

# Crear un socket TCP/IP
cliente = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
cliente.settimeout(5)


# Conectar el socket al servidor
cliente.connect((microcontrolador_ip, microcontrolador_puerto))

bandera = 0


# Constantes Modbus
TRANSACTION_ID = 0
PROTOCOL_ID = "0000"
UNIT_ID = "01"
# Función para enviar trama Modbus
def enviar_trama(funcion):
    global cliente, TRANSACTION_ID, PROTOCOL_ID, UNIT_ID, bandera
    TRANSACTION_ID += 1
    CODE = "{:04x}".format(TRANSACTION_ID) + PROTOCOL_ID
    mensaje = CODE + "0006" + UNIT_ID + "0" + funcion + "0000" + "000D"
    cliente.sendall(mensaje.encode())
    print(mensaje)
    if bandera == 1:
        print("hola")
        recibir_respuesta()
        bandera = 0

    
def enviar_trama_single_register(valor, registro):
    global cliente, TRANSACTION_ID, PROTOCOL_ID, UNIT_ID
    TRANSACTION_ID += 1
    CODE = "{:04x}".format(TRANSACTION_ID) + PROTOCOL_ID
    mensaje = CODE + "0006" + UNIT_ID + "06" + "{:04X}".format(int(registro)) + "{:04X}".format(int(valor))
    cliente.sendall(mensaje.encode())
    print(mensaje)
    recibir_respuesta_2()

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
    recibir_respuesta()

def recibir_respuesta():
    global bandera
    try:
        mensaje_esp = cliente.recv(1024)
        if mensaje_esp:
            print("Mensaje recibido de la ESP32:", mensaje_esp.decode())

            # Crear una tabla con PrettyTable
            table = PrettyTable()
            table.field_names = ["Registro", "Valor"]

            # Supongamos que cada línea recibida es un registro y su valor, separados por comas
            registros = mensaje_esp.decode().strip().split(',')
            for reg in registros:
                if reg:  # Asegurar que no es una cadena vacía
                    registro, valor = reg.split(':')  # Asumiendo que el formato es registro:valor
                    table.add_row([registro, valor])
            print(table)
        else:
            print("No se recibió respuesta del microcontrolador.")
    except socket.timeout:
        print("Tiempo de espera agotado, no se recibió respuesta del microcontrolador.")
    except Exception as e:
        print(f"Error al recibir datos: {e}")
        
def recibir_respuesta_2():
    mensaje_esp = cliente.recv(1024)
    time.sleep(0.5)
    print("Mensaje recibido de la ESP32:", mensaje_esp.decode())

def main():
    global TRANSACTION_ID, bandera
    while True:
        opcion = input("¿Qué acción deseas realizar?\n1. Read Coils\n2. Read Discret Inputs\n3. Read Holding Register\n5. Write Single Coil\n6. Write Register\n15. Write Multiple Coils\n16. Write Multiple Registers\nOtro. Salir\nOpción: ")
        
        if opcion in ["1", "2", "3", "5", "15"]:
            bandera = 1           
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
        recibir_respuesta()
        # Espera antes de la siguiente acción
        time.sleep(0.5)
        recibir_respuesta()

    cliente.close()

if __name__ == "__main__":
    main()