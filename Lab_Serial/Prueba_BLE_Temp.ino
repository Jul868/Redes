#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Wire.h>

#define SI7021_ADDRESS 0x40  // Dirección I2C del sensor Si7021

BLEServer* pServer = nullptr; 
BLECharacteristic* pTemperatureCharacteristic = nullptr;
BLECharacteristic* pHumidityCharacteristic = nullptr;
BLECharacteristic* pLedStateCharacteristic = nullptr; // Característica para el estado del LED

void setup() {
  Serial.begin(9600);
  Wire.begin();  // Inicializar el bus I2C para el sensor SI7021

  BLEDevice::init("SI7021 Sensor");

  pServer = BLEDevice::createServer();

  // Crear un servicio para los sensores
  BLEService* pSensorService = pServer->createService("1809"); // UUID genérico para servicios ambientales

  // Característica para la temperatura
  pTemperatureCharacteristic = pSensorService->createCharacteristic(
                                                      "2A1C", // UUID estándar para la temperatura
                                                      BLECharacteristic::PROPERTY_READ |
                                                      BLECharacteristic::PROPERTY_NOTIFY
                                                    );

  // Característica para la humedad
  pHumidityCharacteristic = pSensorService->createCharacteristic(
                                                      "2A1E", // UUID estándar para la humedad
                                                      BLECharacteristic::PROPERTY_READ |
                                                      BLECharacteristic::PROPERTY_NOTIFY
                                                    );

  // Crear un servicio personalizado para el LED
  BLEService* pLedService = pServer->createService("12345678-1234-1234-1234-123456789abc"); // UUID personalizado para el servicio del LED

  // Característica para el estado del LED
  pLedStateCharacteristic = pLedService->createCharacteristic(
                                                      "abcd1234-ab12-cd34-ef56-abcdef123456", // UUID personalizado para la característica del estado del LED
                                                      BLECharacteristic::PROPERTY_READ | 
                                                      BLECharacteristic::PROPERTY_NOTIFY
                                                    );

  pSensorService->start();
  pLedService->start(); // Iniciar el servicio del LED

  // Iniciar publicidad del servidor BLE
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pSensorService->getUUID());
  pAdvertising->addServiceUUID(pLedService->getUUID()); // Anunciar el servicio del LED
  pAdvertising->start();
}

void loop() {
  float temperature = readTemperature();
  float humidity = readHumidity();

  char tempBuffer[32];
  char humidityBuffer[32];

  // Convertir los valores de temperatura y humedad a cadenas de texto
  sprintf(tempBuffer, "%.2f C", temperature);
  sprintf(humidityBuffer, "%.2f %%", humidity);

  // Print en el monitor serial para depuración
  Serial.print("Temperatura: ");
  Serial.println(tempBuffer);
  Serial.print("Humedad: ");
  Serial.println(humidityBuffer);

  // Actualizar y notificar la temperatura
  pTemperatureCharacteristic->setValue((uint8_t*)tempBuffer, strlen(tempBuffer));
  pTemperatureCharacteristic->notify();

  // Actualizar y notificar la humedad
  pHumidityCharacteristic->setValue((uint8_t*)humidityBuffer, strlen(humidityBuffer));
  pHumidityCharacteristic->notify();
  // Leer entrada del monitor serial para controlar el LED
  if (Serial.available() > 0) {
    String input = Serial.readString();
    input.trim(); // Eliminar espacios en blanco y nuevas líneas

    if (input.equals("1") || input.equals("0")) {
      pLedStateCharacteristic->setValue((uint8_t*)(input.c_str()), input.length());
      Serial.print("LED actualizado a: ");
      Serial.println(input.equals("1") ? "Encendido" : "Apagado");
      pLedStateCharacteristic->notify(); // Notificar cambio a los dispositivos BLE conectados
    } else {
      Serial.println("Entrada inválida. Escriba '1' para encender o '0' para apagar el LED.");
    }
  }

  delay(100); // Pequeño delay para estabilidad
}

float readTemperature() {
  Wire.beginTransmission(SI7021_ADDRESS);
  Wire.write(0xE3);
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(SI7021_ADDRESS, 2);
  if (Wire.available() == 2) {
    uint16_t raw = (Wire.read() << 8) | Wire.read();
    return ((175.72 * raw) / 65536) - 46.85;
  }
  return -100; // Error de lectura
}

float readHumidity() {
  Wire.beginTransmission(SI7021_ADDRESS);
  Wire.write(0xE5);
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(SI7021_ADDRESS, 2);
  if (Wire.available() == 2) {
    uint16_t raw = (Wire.read() << 8) | Wire.read();
    return ((125 * raw) / 65536) - 6;
  }
  return -100; // Error de lectura
}