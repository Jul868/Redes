#include <Wire.h>


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
}

void loop () {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(1000);
  Wire.requestFrom(0x40, 2);
  if (Wire.available()) {
    int data = Wire.read();
    Serial.println(data);
  }
  delay(1000);
}
 
 The code is not working. I am not able to read the data from the sensor. 
 I have also tried to use the library 