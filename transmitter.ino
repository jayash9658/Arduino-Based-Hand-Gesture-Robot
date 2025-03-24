#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//#include <Adafruit_Sensor.h>

#include <Adafruit_ADXL345_U.h>
#include <math.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

RF24 radio(9, 10);
const byte address[6] = "00001";     
int ADXL345 = 0x53;
float x, y, z;
int pos = 0;

// constants for flex sensor
const int FLEX_PIN = A0; // Pin connected to voltage divider output
const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 30000.0; // Measured resistance of 30k resistor
const float STRAIGHT_RESISTANCE = 30000.0; // resistance when straight
const float BEND_RESISTANCE = 90000.0; // resistance at 90 deg

struct Data {
  float x;
  float y;
  float z;
  int angle;
};

void setup() 
{
  pinMode(FLEX_PIN, INPUT);

  // Serial monitor setup
  Serial.begin(9600);

  // for accelerometer
  if (!accel.begin()) {
    Serial.println("No valid sensor found");
    while(1);
  }

  // for NRF24L01 transceiver
  radio.begin();             
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN); 
  radio.stopListening();  
}

void loop()
{
  Data sendData;
  sensors_event_t event;
  accel.getEvent(&event);

  sendData.x = event.acceleration.x;
  sendData.y = event.acceleration.y;
  sendData.z = event.acceleration.z;

  // Read the ADC, and calculate voltage and resistance from it
  int flexADC = analogRead(FLEX_PIN);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);
  float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE, 0, 90.0);
  sendData.angle = angle;

  // Get accelerometer data
  Serial.println("X: " + (String)sendData.x + " g Y: " + (String)sendData.y + " g Z: " + (String)sendData.z + " g Angle: " + String(angle) + " degrees");

  radio.write(&sendData, sizeof(Data));
  delay(10);
}
