#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiAP.h>
#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include <SPI.h>

// Pines para la comunicación SPI con MPU-9250
#define PIN_CS 5     // Pin de selección del chip para MPU-9250
#define PIN_SCK 18   // Pin de reloj serial
#define PIN_MISO 19  // Pin de entrada de datos del maestro
#define PIN_MOSI 23  // Pin de salida de datos del maestro

// Direcciones de registro del MPU-9250
#define GYRO_XOUT_H 0x43   // Primer registro de datos del giroscopio
#define ACCEL_XOUT_H 0x3B  // Primer registro de datos del acelerómetro
#define PWR_MGMT_1 0x6B    // Registro de administración de energía
#define WHO_AM_I 0x75      // Registro que devuelve la identificación del dispositivo (debe ser 0x71)


//Definiciones giroscopio I2C
//#define MPU9250_ADDRESS 0x68 // Dirección por defecto del MPU-9250 en el bus I2C
//#define PWR_MGMT_1      0x6B // Registro de gestión de energía
#define GYRO_CONFIG 0x1B   // Configuración del giroscopio
#define ACCEL_CONFIG 0x1C  // Configuración del acelerómetro
//#define CONFIG          0x1A // Configuración general
//#define MPU9250_ADDRESS     0x68 // Dirección I2C del MPU-9250
//#define ACCEL_XOUT_H        0x3B // Dirección del primer registro de datos del acelerómetro
//#define GYRO_XOUT_H         0x43 // Dirección del primer registro de datos del giroscopio
//#define RESOLUTION_REGISTER 0xE6 // Dirección del registro de resolución del Si7021


// Dirección I2C del Si7021
#define SI7021_ADDR 0x40
// Comando para leer el registro de usuario
#define READ_USER_REG 0xE7
// Comando para escribir en el registro de usuario
#define WRITE_USER_REG 0xE6
// Comando de reinicio suave
#define SOFT_RESET 0xFE


char codigo = '0';
String x = "0";
String trama = "0";
String respuesta = "0";
char led = '0';
char accion = '0';

bool automatico = false;
bool led1_encendido = false;
bool led2_encendido = false;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
// intervalos
int interval1 = 100;
int interval2 = 100;
int ledState1 = LOW;
int ledState2 = LOW;

int TRANSACTION_ID = 0;
char hexa_freq[5] = { '0', '0', '0', '0', '\0' };
char hexa_freq2[5] = { '0', '0', '0', '0', '\0' };
char hexa_transaction[5] = { '0', '0', '0', '0', '\0' };
char hexa_led1[5] = { '0', '0', '0', '0', '\0' };
char hexa_led2[5] = { '0', '0', '0', '0', '\0' };
char hexa_accX[5] = { '0', '0', '0', '0', '\0' };
char hexa_accY[5] = { '0', '0', '0', '0', '\0' };
char hexa_accZ[5] = { '0', '0', '0', '0', '\0' };
char hexa_gyroX[5] = { '0', '0', '0', '0', '\0' };
char hexa_gyroY[5] = { '0', '0', '0', '0', '\0' };
char hexa_gyroZ[5] = { '0', '0', '0', '0', '\0' };
char hexa_Temperature[5] = { '0', '0', '0', '0', '\0' };
char hexa_Humidity[5] = { '0', '0', '0', '0', '\0' };
char hexa_encendido[5] = { '0', '0', '0', '0', '\0' };
char hexa_SI7021_Res[5] = { '0', '0', '0', '0', '\0' };
char hexa_MPU9250_Res[5] = { '0', '0', '0', '0', '\0' };

String PROTOCOL_ID = "0000";

String TAM_1 = "0006";
String TAM_2 = "000C";
String TAM_3 = "";
String UNIT_ID = "01";

//PIN LED
const int ledPin = 16;
const int ledPin_2 = 17;
int freq = 0;
int freq2 = 0;
int encendido = 0;
int SI7021_Res = 0;
int MPU9250_Res = 0;
int16_t accX, accY, accZ, gyroX, gyroY, gyroZ;
//accX, accY, accZ, gyroX, gyroY, gyroZ = 0;
float temperature, humidity;

WiFiServer servidor(502);  // Puerto en el que el ESP32 está escuchando
WiFiClient cliente;



void setup() {
  Serial.begin(115200);
  WiFi.begin("PALACIO_ORTIZ", "Palacioynoraida1");
  pinMode(PIN_CS, OUTPUT);  // Configura el pin CS como salida

  // Inicializa el bus SPI
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);
  // Configura el MPU-9250 para despertar y establecer el rango del giroscopio y acelerómetro
  writeRegister(PWR_MGMT_1, 0x01);  // Establece el reloj al oscilador PLL

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a la red WiFi");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());

  // Iniciar el servidor Modbus TCP/IP
  servidor.begin();
  Serial.println("Servidor Modbus iniciado");

  //mb.server();

  pinMode(ledPin, OUTPUT);
  pinMode(ledPin_2, OUTPUT);



  digitalWrite(ledPin, LOW);
  digitalWrite(ledPin_2, LOW);

  Wire.begin();
  //setupMPU9250(0x00, 0x00);// Configura el MPU-9250 para usar el acelerómetro y el giroscopio
  //Serial.println("MPU-9250 inicializado para acelerómetro y giroscopio.");
  Serial.print("Probe SI7021: ");
  delay(1000);
  initializeSi7021();
  delay(1000);
  Serial.println("configurado");
}

void loop() {

  cliente = servidor.available();
  if (cliente) {
    Serial.println("Cliente conectado");
    TRANSACTION_ID = 0;

    // Leer datos del cliente
    while (cliente.connected()) {
      sprintf(hexa_led1, "%04X", int(led1_encendido));
      sprintf(hexa_led2, "%04X", int(led2_encendido));
      sprintf(hexa_freq, "%04X", int(freq));

      if (cliente.available()) {
        // Leer la trama Modbus recibida
        codigo = trama[15];
        trama = cliente.readStringUntil('\n');
        Serial.println("Trama Modbus recibida: " + trama);
        TRANSACTION_ID++;
        sprintf(hexa_transaction, "%04X", TRANSACTION_ID);
        actualizarVariablesEstado();
        handleModbusRequest(trama);
      }
      actualizarLEDs();
      actualizarVariablesEstado();
      if (automatico){
        temperature= readTemperature();
        humidity = readHumidity();
        delay(1000);
      }
    }

    // Cerrar la conexión con el cliente
    cliente.stop();
    Serial.println("Cliente desconectado");
  }
}

void handleModbusRequest(const String &trama) {
  char functionCode = trama[15];
  char ledCode = trama[19];
  char action = trama[23];

  actualizarVariablesEstado();

  switch (functionCode) {
    case '3':  // Read Holding Register
      // Construir y enviar la respuesta con los valores de los registros solicitados
      actualizarVariablesEstado();
      //respuesta = hexa_encendido + "," + hexa_led1 + "," + hexa_led2 + "," + hexa_freq + "," + hexa_freq2 + "," + hexa_SI7021_Res + "," + hexa_Temperature + "," + hexa_Humidity + "," + hexa_MPU9250_Res + "," + hexa_accX + "," + hexa_accY + "," + hexa_accZ + "," + hexa_gyroX + "," + hexa_gyroY + "," + hexa_gyroZ;
      //respuesta = hexa_transaction + PROTOCOL_ID + TAM_2 + UNIT_ID + x + functionCode + hexa_encendido + hexa_led1 + hexa_led2 + hexa_freq + hexa_freq2 + hexa_SI7021_Res + hexa_Temperature + hexa_Humidity + hexa_MPU9250_Res + hexa_accX + hexa_accY + hexa_accZ + hexa_gyroX + hexa_gyroY + hexa_gyroZ;
      respuesta = String("0x00") + ":" + String(hexa_encendido) + ", " + String("0x01") + ":" + String(hexa_led1) + ", " + String("0x02") + ":" + String(hexa_led2) + ", " + String("0x03") + ":" + String(hexa_freq) + ", " + String("0x04") + ":" + String(hexa_freq2) + ", " + String("0x05") + ":" + String(hexa_SI7021_Res) + ", " + String("0x06") + ":" + String(hexa_Temperature) + ", " + String("0x07") + ":" + String(hexa_Humidity) + ", " + String("0x08") + ":" + String(hexa_MPU9250_Res) + ", " + String("0x09") + ":" + String(hexa_accX) + ", " + String("0x0A") + ":" + String(hexa_accY) + ", " + String("0x0B") + ":" + String(hexa_accZ) + ", " + String("0x0C") + ":" + String(hexa_gyroX) + ", " + String("0x0D") + ":" + String(hexa_gyroY) + ", " + String("0x0E") + ":" + String(hexa_gyroZ);

      cliente.println(respuesta);
      Serial.println("Respuesta Modbus enviada: " + respuesta);
      mostrarTablaRegistros();
      break;

    case '6':  // Write Single Register
      // Cambiar el estado de los LEDs según los comandos recibidos
      if (ledCode == '0') {
        if (action == '1') {
          temperature = readTemperature();
          humidity = readHumidity();
          // Lee los datos del acelerómetro y giroscopio
          readMPU9250Data(ACCEL_XOUT_H, accX, accY, accZ);
          readMPU9250Data(GYRO_XOUT_H, gyroX, gyroY, gyroZ);
          // Imprime los valores leídos
          Serial.print("Acelerómetro: X=");
          Serial.print(accX);
          Serial.print(", Y=");
          Serial.print(accY);
          Serial.print(", Z=");
          Serial.println(accZ);

          Serial.print("Giroscopio: X=");
          Serial.print(gyroX);
          Serial.print(", Y=");
          Serial.print(gyroY);
          Serial.print(", Z=");
          Serial.println(gyroZ);
          encendido = 1;
          Serial.print("Temperatura: ");
          Serial.print(temperature);
          Serial.println(" °C");
          Serial.print("Humedad: ");
          Serial.print(humidity);
          Serial.println("");
        } else if (action == '0') {
          accX, accY, accZ = 0;
          gyroX, gyroY, gyroZ = 0;
          encendido = 0;
        }else if (action == '2') {
          automatico = true;
        }

        actualizarVariablesEstado();
        respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + functionCode + String("0000") + hexa_encendido;

      } else if (ledCode == '1') {
        led1_encendido = (action == '1');
        if (!led1_encendido) digitalWrite(ledPin, LOW);
        actualizarVariablesEstado();
        respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + functionCode + String("0001") + hexa_led1;
        // Construir y enviar respuesta
        actualizarVariablesEstado();
        respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + functionCode + String("0001") + hexa_led1;
      } else if (ledCode == '2') {
        led2_encendido = (action == '1');
        if (!led2_encendido) digitalWrite(ledPin_2, LOW);
        actualizarVariablesEstado();
        respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + functionCode + String("0002") + hexa_led2;
        // Construir y enviar respuesta
        actualizarVariablesEstado();
        respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + functionCode + String("0002") + hexa_led2;
      } else if (ledCode == '3') {
        if (action == '1') {

          interval1 = 500;
          freq = interval1;
        } else if (action == '2') {
          interval1 = 1000;
          freq = interval1;
        } else if (action == '3') {
          interval1 = 1500;
          freq = interval1;
        }
        actualizarVariablesEstado();
        respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0003") + hexa_freq;
      } else if (ledCode == '4') {
        if (action == '1') {
          interval2 = 500;
          freq2 = interval2;
        } else if (action == '2') {
          interval2 = 1000;
          freq2 = interval2;
        } else if (action == '3') {
          interval2 = 1500;
          freq2 = interval2;
        }
        actualizarVariablesEstado();
        respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0004") + hexa_freq2;
      } else if (ledCode == '5') {
        if (action == '0') {
          setSi7021Resolution(0b00);  // Alta resolución: 12 bits para humedad, 14 bits para temperatura
          SI7021_Res = 0;
        } else if (action == '1') {
          setSi7021Resolution(0b01);  // Baja resolución: 11 bits para humedad, 11 bits para temperatura
          SI7021_Res = 1;
        } else if (action == '2') {
          setSi7021Resolution(0b10);  // Resolución media: 10 bits para humedad, 13 bits para temperatura
          SI7021_Res = 2;
        } else if (action == '3') {
          setSi7021Resolution(0b11);  // Resolución baja: 8 bits para humedad, 12 bits para temperatura
          SI7021_Res = 3;
        }
        actualizarVariablesEstado();
        respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0005") + hexa_SI7021_Res;
      } else if (ledCode == '8') {  //lets configure the MPU sensor resolution in SPI
        if (action == '0') {
          writeRegister(GYRO_CONFIG, 0x00);  // Configura el rango del giroscopio a ±250 grados/seg, y el rango del acelerómetro a ±2g
          writeRegister(ACCEL_CONFIG, 0x00);
          MPU9250_Res = 0;
        } else if (action == '1') {
          writeRegister(GYRO_CONFIG, 0x08);  // Configura el rango del giroscopio a ±500 grados/seg, y el rango del acelerómetro a ±4g
          writeRegister(ACCEL_CONFIG, 0x00);
          MPU9250_Res = 1;
        } else if (action == '2') {
          writeRegister(GYRO_CONFIG, 0x10);  // Configura el rango del giroscopio a ±1000 grados/seg, y el rango del acelerómetro a ±8g
          writeRegister(ACCEL_CONFIG, 0x08);
          MPU9250_Res = 2;
        } else if (action == '3') {
          writeRegister(GYRO_CONFIG, 0x18);  // Configura el rango del giroscopio a ±2000 grados/seg, y el rango del acelerómetro a ±16g
          writeRegister(ACCEL_CONFIG, 0x10);
          MPU9250_Res = 3;
        }
        actualizarVariablesEstado();
        respuesta = hexa_transaction + PROTOCOL_ID + TAM_1 + UNIT_ID + x + codigo + String("0006") + hexa_MPU9250_Res;
      } else {

        Serial.println("Acción no válida");
      }
      cliente.println(respuesta);
      Serial.println("Respuesta Modbus enviada: " + respuesta);
      break;
  }
}


void actualizarLEDs() {
  unsigned long currentMillis = millis();
  if (led1_encendido && currentMillis - previousMillis1 >= interval1) {
    previousMillis1 = currentMillis;
    ledState1 = !ledState1;
    actualizarVariablesEstado();
    digitalWrite(ledPin, ledState1);
  }
  if (led2_encendido && currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;
    ledState2 = !ledState2;
    actualizarVariablesEstado();
    digitalWrite(ledPin_2, ledState2);
  }
}

void writeRegister(uint8_t registerAddress, uint8_t data) {
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(registerAddress);
  SPI.transfer(data);
  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();
}

void readMPU9250Data(uint8_t registerAddress, int16_t &dataX, int16_t &dataY, int16_t &dataZ) {
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(registerAddress | 0x80);  // Specify the starting register address
  for (int i = 0; i < 6; ++i) {
    rawData[i] = SPI.transfer(0x00);  // Read the data
  }
  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();

  // Convertir los datos en bruto a valores de 16 bits
  dataX = ((int16_t)rawData[0] << 8) | rawData[1];
  dataY = ((int16_t)rawData[2] << 8) | rawData[3];
  dataZ = ((int16_t)rawData[4] << 8) | rawData[5];
}

// void setupMPU9250(byte gScale, byte aScale) {
//   Wire.begin(); // Inicializa la comunicación I2C
//   Wire.beginTransmission(MPU9250_ADDRESS); // Comienza transmisión al dispositivo
//   Wire.write(PWR_MGMT_1); // Selecciona el registro PWR_MGMT_1
//   Wire.write(0x00); // Escribe 0x00 para despertar el dispositivo
//   Wire.endTransmission(true);

//   // Configura el rango del giroscopio a ±250 grados/seg
//   Wire.beginTransmission(MPU9250_ADDRESS);
//   Wire.write(GYRO_CONFIG);
//   Wire.write(gScale); // Configura el GYRO_CONFIG a 0x00
//   Wire.endTransmission(true);

//   // Configura el rango del acelerómetro a ±2g
//   Wire.beginTransmission(MPU9250_ADDRESS);
//   Wire.write(ACCEL_CONFIG);
//   Wire.write(aScale); // Configura el ACCEL_CONFIG a 0x00
//   Wire.endTransmission(true);

//   // Configura el filtro pasa-bajas y la tasa de muestreo
//   Wire.beginTransmission(MPU9250_ADDRESS);
//   Wire.write(CONFIG);
//   Wire.write(0x01); // Configura el CONFIG a 0x01 para usar el filtro pasa-bajas
//   Wire.endTransmission(true);
// }

// void readGyroscope(int16_t &gyroX, int16_t &gyroY, int16_t &gyroZ) {
//   Wire.beginTransmission(MPU9250_ADDRESS);
//   Wire.write(GYRO_XOUT_H); // Establece el registro desde el cual empezar la lectura
//   Wire.endTransmission(false);
//   Wire.requestFrom(MPU9250_ADDRESS, 6, true); // Solicita los 6 bytes del giroscopio
//   gyroX = Wire.read() << 8 | Wire.read(); // Combina los bytes
//   gyroY = Wire.read() << 8 | Wire.read(); // Combina los bytes
//   gyroZ = Wire.read() << 8 | Wire.read(); // Combina los bytes
// }

//Funcion de leer acelerometro
// void readAccelerometer(int16_t &accX, int16_t &accY, int16_t &accZ) {
//   Wire.beginTransmission(MPU9250_ADDRESS);
//   Wire.write(ACCEL_XOUT_H); // Establece el registro desde el cual empezar la lectura
//   Wire.endTransmission(false);
//   Wire.requestFrom(MPU9250_ADDRESS, 6, true); // Solicita los 6 bytes del acelerómetro
//   accX = Wire.read() << 8 | Wire.read(); // Combina los bytes
//   accY = Wire.read() << 8 | Wire.read(); // Combina los bytes
//   accZ = Wire.read() << 8 | Wire.read(); // Combina los bytes
// }

void actualizarVariablesEstado() {
  sprintf(hexa_encendido, "%04X", encendido);
  sprintf(hexa_led1, "%04X", int(led1_encendido));
  sprintf(hexa_led2, "%04X", int(led2_encendido));
  sprintf(hexa_freq, "%04X", freq);
  sprintf(hexa_freq2, "%04X", freq2);
  sprintf(hexa_accX, "%04X", accX);
  sprintf(hexa_accY, "%04X", accY);
  sprintf(hexa_accZ, "%04X", accZ);
  sprintf(hexa_gyroX, "%04X", gyroX);
  sprintf(hexa_gyroY, "%04X", gyroY);
  sprintf(hexa_gyroZ, "%04X", gyroZ);
  sprintf(hexa_Temperature, "%04X", int(temperature));
  sprintf(hexa_Humidity, "%04X", int(humidity));
  sprintf(hexa_SI7021_Res, "%04X", SI7021_Res);
  sprintf(hexa_MPU9250_Res, "%04X", MPU9250_Res);
}

void initializeSi7021() {
  Wire.beginTransmission(SI7021_ADDR);
  Wire.write(SOFT_RESET);
  if (Wire.endTransmission() != 0) {
    Serial.println("Error al enviar el comando de reinicio suave.");
    return;  // Salir si hay un error al enviar el comando
  }

  delay(50);  // Espera a que el sensor se reinicie

  // Configurar la resolución del sensor
  setSi7021Resolution(0b00);  // Configura a alta resolución: 12 bits para humedad, 14 bits para temperatura
}

void setSi7021Resolution(byte resolution) {
  // Leer el registro de usuario actual
  Wire.beginTransmission(SI7021_ADDR);
  Wire.write(READ_USER_REG);
  Wire.endTransmission();
  Wire.requestFrom(SI7021_ADDR, 1);
  if (Wire.available()) {
    byte userReg = Wire.read();

    // Configurar los bits de resolución
    userReg &= ~(1 << 2);   // Limpia los bits de resolución manteniendo el resto igual
    userReg &= 0x7E;        // Limpia los bits de resolución manteniendo el resto igual
    userReg |= resolution;  // Configura los nuevos bits de resolución

    // Escribir el nuevo valor del registro de usuario
    Wire.beginTransmission(SI7021_ADDR);
    Wire.write(WRITE_USER_REG);
    Wire.write(userReg);
    Wire.endTransmission();
  }
}

float readHumidity() {
  // Enviar comando de lectura de temperatura al sensor
  Wire.beginTransmission(SI7021_ADDR);
  Wire.write(0xE5);  // Comando para leer temperatura (Hold master mode)
  Wire.endTransmission();

  // Esperar un tiempo suficiente para que se realice la conversión
  delay(100);

  // Leer 2 bytes de datos de temperatura del sensor
  Wire.requestFrom(SI7021_ADDR, 2);
  if (Wire.available() == 2) {
    uint16_t rawHumidity = (Wire.read() << 8) | Wire.read();
    // Convertir los datos crudos en temperatura
    float humidity = ((125 * rawHumidity) / 65536) - 6;
    return humidity;
  } else {
    Serial.println("Error: No se pudo leer la Humedad");
    return -1.0;  // Retornar un valor negativo en caso de error
  }
}

float readTemperature() {
  // Enviar comando de lectura de temperatura al sensor
  Wire.beginTransmission(SI7021_ADDR);
  Wire.write(0xE3);  // Comando para leer temperatura (Hold master mode)
  Wire.endTransmission();

  // Esperar un tiempo suficiente para que se realice la conversión
  delay(100);

  // Leer 2 bytes de datos de temperatura del sensor
  Wire.requestFrom(SI7021_ADDR, 2);
  if (Wire.available() == 2) {
    uint16_t rawTemperature = (Wire.read() << 8) | Wire.read();
    // Convertir los datos crudos en temperatura
    float temperature = ((175.72 * rawTemperature) / 65536) - 46.85;
    return temperature;
  } else {
    Serial.println("Error: No se pudo leer la temperatura");
    return -1.0;  // Retornar un valor negativo en caso de error
  }
}

void mostrarTablaRegistros() {
  // Cabecera de la tabla
  Serial.println("REGISTRO | VALOR HEX | VALOR DECIMAL");

  // Mostrando cada registro con su valor hexadecimal y decimal
  Serial.print("00x0     | "); Serial.print(hexa_encendido); Serial.print(" | "); Serial.println(encendido);
  Serial.print("00x1     | "); Serial.print(hexa_led1); Serial.print(" | "); Serial.println(led1_encendido);
  Serial.print("00x2     | "); Serial.print(hexa_led2); Serial.print(" | "); Serial.println(led2_encendido);
  Serial.print("00x3     | "); Serial.print(hexa_freq); Serial.print(" | "); Serial.println(freq);
  Serial.print("00x4     | "); Serial.print(hexa_freq2); Serial.print(" | "); Serial.println(freq2);
  Serial.print("00x5     | "); Serial.print(hexa_SI7021_Res); Serial.print(" | "); Serial.println(SI7021_Res);
  Serial.print("00x6     | "); Serial.print(hexa_Temperature); Serial.print(" | "); Serial.println(temperature);
  Serial.print("00x7     | "); Serial.print(hexa_Humidity); Serial.print(" | "); Serial.println(humidity);
  Serial.print("00x8     | "); Serial.print(hexa_MPU9250_Res); Serial.print(" | "); Serial.println(MPU9250_Res);
  Serial.print("00x9     | "); Serial.print(hexa_accX); Serial.print(" | "); Serial.println(accX);
  Serial.print("00xA     | "); Serial.print(hexa_accY); Serial.print(" | "); Serial.println(accY);
  Serial.print("00xB     | "); Serial.print(hexa_accZ); Serial.print(" | "); Serial.println(accZ);
  Serial.print("00xC     | "); Serial.print(hexa_gyroX); Serial.print(" | "); Serial.println(gyroX);
  Serial.print("00xD     | "); Serial.print(hexa_gyroY); Serial.print(" | "); Serial.println(gyroY);
  Serial.print("00xE     | "); Serial.print(hexa_gyroZ); Serial.print(" | "); Serial.println(gyroZ);
  // Agrega más líneas si necesitas mostrar más registros
}