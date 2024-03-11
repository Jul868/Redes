#include <Wire.h>

#define SI7021_ADDRESS 0x40
#define RESOLUTION_REGISTER 0xE6

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  // Write resolution value to memory register
  Wire.beginTransmission(SI7021_ADDRESS);
  Wire.write(RESOLUTION_REGISTER);
  Wire.write(0x01); // Set resolution to 12-bit
  Wire.endTransmission();
  delay(1000);

  // Read humidity value
  Wire.beginTransmission(SI7021_ADDRESS);
  Wire.write(0xE5); // Humidity register
  Wire.endTransmission();
  Wire.requestFrom(SI7021_ADDRESS, 2);
  if (Wire.available()) {
    int humidity = Wire.read() << 8 | Wire.read(); //wire.read() read the data recieved from ESP32 pin 21, where is 
    Serial.print("Humidity: ");
    Serial.print(humidity * 125 / 65536.0 - 6);
    Serial.println("%");

    // Send response message to master
    Wire.beginTransmission(SI7021_ADDRESS);
    Wire.write("Humidity measurement complete");
    Wire.endTransmission();
  }
  delay(1000);

  // Read temperature value
  Wire.beginTransmission(SI7021_ADDRESS);
  Wire.write(0xE3); // Temperature register
  Wire.endTransmission();
  Wire.requestFrom(SI7021_ADDRESS, 2);
  if (Wire.available()) { 
    int temperature = Wire.read() << 8 | Wire.read();
    Serial.print("Temperature: ");
    Serial.print(temperature * 175.72 / 65536.0 - 46.85);
    Serial.println("°C");

    // Send response message to master
    Wire.beginTransmission(SI7021_ADDRESS);
    Wire.write("Temperature measurement complete");
    Wire.endTransmission();
  }
  delay(1000);
}
