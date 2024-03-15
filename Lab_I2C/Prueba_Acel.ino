#include <Wire.h>

#define MPU9250_ADDRESS 0x68  // Dirección I2C del sensor MPU9250

void setup() {
  Serial.begin(9600);  // Inicializar el monitor serial
  Wire.begin();  // Inicializar el bus I2C
  // Inicializar el sensor MPU9250
  Wire.beginTransmission(MPU9250_ADDRESS);
  Wire.write(0x6B);  // Dirección del registro de configuración
  Wire.write(0x00);  // Valor para desactivar el modo de suspensión
  Wire.endTransmission();
}

void loop() {
  // Leer los datos del acelerómetro
  int16_t x = readRegister16(0x3B);
  int16_t y = readRegister16(0x3D);
  int16_t z = readRegister16(0x3F);

  // Mostrar los datos en el monitor serial
  Serial.print("Aceleración en X: ");
  Serial.print(x / 16384.0);
  Serial.println(" g");
  Serial.print("Aceleración en Y: ");
  Serial.print(y / 16384.0);
  Serial.println(" g");
  Serial.print("Aceleración en Z: ");
  Serial.print(z / 16384.0);
  Serial.println(" g");

  delay(1000);  // Esperar 1 segundo antes de la próxima lectura
}

int16_t readRegister16(uint8_t reg) {
  // Leer 2 bytes de un registro del sensor
  Wire.beginTransmission(MPU9250_ADDRESS);
  Wire.write(reg);  // Dirección del registro a leer
  Wire.endTransmission();
  Wire.requestFrom(MPU9250_ADDRESS, 2);
  if (Wire.available() == 2) { // 
    uint8_t low = Wire.read();
    uint8_t high = Wire.read();
    return (high << 8) | low;
  } else {
    Serial.println("Error: No se pudo leer el registro");
    return -1;
  }
}

// hacer una función de configuración con 6B, 6A y 24 para configurar el sensor

config_Sensor(){
    Wire.beginTransmission(MPU9250_ADDRESS);
    Wire.write(0x6B);
    Wire.write(
    Wire.endTransmission();

}

