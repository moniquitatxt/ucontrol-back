#include <SoftwareSerial.h>

// Vibraciones
int xpin = A1;
int ypin = A2;
int zpin = A3;
int xsample = 0;
int ysample = 0;
int zsample = 0;
const float zero_G = 512.0;
const float escala = 102.3;
bool vibrationDetected = false;

// Movimiento
const int PIRPin = 2;
bool motionDetected = false;

// Humedad del suelo
const int AirValue = 650;
const int WaterValue = 210;
int intervals = (AirValue - WaterValue) / 3;
const int SoilSensorPin = A4;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

// Agua
#define POWER_PIN 7
const int WaterSensorPin = A0;
bool waterDetected = false;

SoftwareSerial Arduino_SoftSerial(10, 11);
String str;


void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);

  pinMode(PIRPin, INPUT);

  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(zpin, INPUT);

  pinMode(WaterSensorPin, INPUT);
  pinMode(POWER_PIN, OUTPUT);    // configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW);  // turn the sensor OFF


  Arduino_SoftSerial.begin(115200);
  delay(10000);
}

void loop() {
  // Movimiento
  int PIRvalue = digitalRead(PIRPin);

  // Vibraciones
  int x = analogRead(xpin);
  delay(1);
  int y = analogRead(ypin);
  delay(1);
  int z = analogRead(zpin);

  float xValue = ((float)x - zero_G) / escala;
  float yValue = ((float)y - zero_G) / escala;
  float zValue = ((float)z - zero_G) / escala;

  // Detección de vibraciones
  if ((xValue > 2.6) || (yValue > 2.7) || (zValue > 1.6)) {
    if (!vibrationDetected) {
      vibrationDetected = true;
      sendToESP("Vibracion", "1");
    }
  } else {
    if (vibrationDetected) {
      vibrationDetected = false;
      sendToESP("Vibracion", "0");
    }
  }

  // Detección de movimiento
  if (PIRvalue == HIGH) {
    if (!motionDetected) {
      motionDetected = true;
      sendToESP("Movimiento", "1");
    }
  } else {
    if (motionDetected) {
      motionDetected = false;
      sendToESP("Movimiento", "0");
    }
  }

  // Detección de humedad del suelo (sin cambios significativos detectados)
  sendToESP("Soil", "");
  
  // Detección de agua
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);
  int waterLevel = analogRead(WaterSensorPin);
  digitalWrite(POWER_PIN, LOW);  // turn the sensor OFF

  //Serial.println(waterLevel);
  if (waterLevel > 250) {
    if (!waterDetected) {
      waterDetected = true;
      sendToESP("Agua", "1");
    }
  } else {
    if (waterDetected) {
      waterDetected = false;
      sendToESP("Agua", "0");
    }
  }

  delay(3000);
}

void sendToESP(String sensor, String value) {
  String espStr = sensor + " " + value + "\n";
  Arduino_SoftSerial.print(espStr);
  Serial.println(espStr);
  delay(1000);
}
