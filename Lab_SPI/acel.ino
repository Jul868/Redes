#include <SPI.h>
 
// Pines para la comunicación SPI con MPU-9250
#define PIN_CS   5  // Pin de selección del chip para MPU-9250
#define PIN_SCK  18 // Pin de reloj serial
#define PIN_MISO 19 // Pin de entrada de datos del maestro
#define PIN_MOSI 23 // Pin de salida de datos del maestro
 
// Direcciones de registro del MPU-9250
#define GYRO_XOUT_H   0x43 // Primer registro de datos del giroscopio
#define ACCEL_XOUT_H  0x3B // Primer registro de datos del acelerómetro
#define PWR_MGMT_1    0x6B // Registro de administración de energía
#define WHO_AM_I      0x75 // Registro que devuelve la identificación del dispositivo (debe ser 0x71)
 
 
void setup() {
  Serial.begin(115200);
  pinMode(PIN_CS, OUTPUT); // Configura el pin CS como salida
 
  // Inicializa el bus SPI
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);
  // Configura el MPU-9250 para despertar y establecer el rango del giroscopio y acelerómetro
  writeRegister(PWR_MGMT_1, 0x01); // Establece el reloj al oscilador PLL
}
 
void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  ax = ay = az = gx = gy = gz = 0;
 
  // Lee los datos del acelerómetro y giroscopio
  readMPU9250Data(ACCEL_XOUT_H, ax, ay, az);
  readMPU9250Data(GYRO_XOUT_H, gx, gy, gz);
 
  // Imprime los valores leídos
  // Serial.print("Acelerómetro: X=");
  // Serial.print(ax);
  // Serial.print(", Y=");
  // Serial.print(ay);
  // Serial.print(", Z=");
  // Serial.println(az);
 
  Serial.print("X ");
  Serial.print(gx);
  Serial.print("Y ");
  Serial.print(gy);
  Serial.print("Z ");
  Serial.print(gz);
  Serial.println("");
 
  delay(100); // Espera un segundo para la próxima lectura
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
  uint8_t rawData[6]; // x/y/z gyro register data stored here
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(registerAddress | 0x80); // Specify the starting register address
  for (int i = 0; i < 6; ++i) {
    rawData[i] = SPI.transfer(0x00); // Read the data
  }
  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();
 
  // Convertir los datos en bruto a valores de 16 bits
  dataX = ((int16_t)rawData[0] << 8) | rawData[1];
  dataY = ((int16_t)rawData[2] << 8) | rawData[3];
  dataZ = ((int16_t)rawData[4] << 8) | rawData[5];
}