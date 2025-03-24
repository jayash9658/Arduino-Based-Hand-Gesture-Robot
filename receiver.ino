#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <math.h>

Servo claw; 
Servo arm2;  
Servo arm1;  

RF24 radio(9, 10);
const byte address[6] = "00001";

// motors
// Motor A connections
int enA = 8;
int in1 = A1;
int in2 = A2;
// Motor B connections
int enB = 8;
int in3 = A3;
int in4 = A4;

struct Data {
  float x;
  float y;
  float z;
  int angle;
};

int previousAngle = 0;
int currentAngle = 0;
bool flag1 = true;
bool flag2 = true;

void setup() 
{
  // arm1.attach(6);  // attaches the servo on pin 9 to the servo object
  claw.attach(2);
  arm2.attach(3);
  arm1.attach(4);
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address); 
  radio.setPALevel(RF24_PA_MIN);       
  radio.startListening();              

  // motors
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  claw.write(0);
  arm1.write(135);
}

void loop()
{
  Data sendData;
  if (radio.available()) {
    Serial.print("(" + (String)sendData.x + ", " + (String)sendData.y + ", " + (String)sendData.z + ", " + (String)sendData.angle + " degrees)\t");
    radio.read(&sendData, sizeof(Data));

    // control arm
    if(sendData.angle > 25) {
      stopMotor();
      if(sendData.x > 7 && flag1) {
        for(int i=90; i<=180; i++) {
          arm2.write(i);
          delay(20);
          flag1 = false;
          flag2 = true;
        }
      }

      else if(sendData.x < 4 && flag2) {
        for(int i=180; i>=90; i--) {
          arm2.write(i);
          delay(20);
          flag2 = false;
          flag1 = true;
        }
      }
    }

    else {
      if(sendData.angle > 45) {
        Serial.print("claw closed");
        claw.write(25);
      }
      else {
        Serial.print("claw opened");
        claw.write(0);
      }

      if(sendData.x > 6) {
        Serial.println("> 6");
        forward();
      }
      else if(sendData.x < -6) {
        Serial.println("< -6");
        reverse();
      }

      else if(sendData.y < -6) {
        Serial.println("right");
        right();
      }
      else if(sendData.y > 6) {
        Serial.println("left");
        left();
      }
      else {
        stopMotor();
      }
    }
    Serial.println("");
  }
  else {
    // Serial.println("radio unavailable!");
  }

  delay(10);
}

void controlArm()
{

}

void right() {
  analogWrite(enA, 255);
  analogWrite(enB, 255);

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void left() {
  analogWrite(enA, 255);
  analogWrite(enB, 255);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void forward() {
  analogWrite(enA, 200);
  analogWrite(enB, 200);

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void reverse() {
  analogWrite(enA, 200);
  analogWrite(enB, 200);

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void stopMotor()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW); 
}
