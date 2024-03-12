#include <Wire.h>

#define SI7021_ADDRESS 0x40  // Dirección I2C del sensor Si7021

void setup() {
  Serial.begin(9600);  // Inicializar el monitor serial
  Wire.begin();  // Inicializar el bus I2C
}

void loop() {
  // Leer la temperatura
  float temperature = readTemperature();
  float humidity = readHumidity();

  // Mostrar la temperatura en el monitor serial
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.println("");

  delay(1000);  // Esperar 1 segundo antes de la próxima lectura
}

float readTemperature() {
  // Enviar comando de lectura de temperatura al sensor
  Wire.beginTransmission(SI7021_ADDRESS);
  Wire.write(0xE3);  // Comando para leer temperatura (Hold master mode)
  Wire.endTransmission();

  // Esperar un tiempo suficiente para que se realice la conversión
  delay(100);

  // Leer 2 bytes de datos de temperatura del sensor
  Wire.requestFrom(SI7021_ADDRESS, 2);
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
float readHumidity() {
  // Enviar comando de lectura de temperatura al sensor
  Wire.beginTransmission(SI7021_ADDRESS);
  Wire.write(0xE5);  // Comando para leer temperatura (Hold master mode)
  Wire.endTransmission();

  // Esperar un tiempo suficiente para que se realice la conversión
  delay(100);

  // Leer 2 bytes de datos de temperatura del sensor
  Wire.requestFrom(SI7021_ADDRESS, 2);
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
