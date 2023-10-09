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

// Movimiento
const int PIRPin = 2;

// Humedad del suelo
const int AirValue = 650;
const int WaterValue = 210;
int intervals = (AirValue - WaterValue) / 3;
const int SoilSensorPin = A4;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;


//Agua
const int WaterSensorPin = A0;
int waterLevel = 0;

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



  Arduino_SoftSerial.begin(115200);
  delay(10000);
}

void loop() {

  //Movimiento
  int PIRvalue = digitalRead(PIRPin);


  //VIBRACIONES
  int x = analogRead(xpin);

  delay(1);
  int y = analogRead(ypin);

  delay(1);
  int z = analogRead(zpin);

  float xValue = ((float)x - zero_G) / escala;
  float yValue = ((float)y - zero_G) / escala;
  float zValue = ((float)z - zero_G) / escala;

  Serial.print(xValue);
  Serial.print("\t");

  Serial.print(yValue);
  Serial.print("\t");

  Serial.print(zValue);
  Serial.print("\n");

  //Envio vibraciones a la ESP

  if ((xValue > 2.6) || (yValue > 2.7) || (zValue > 1.6)) {
    Serial.println("Vibracion detectada");
    str = String("1");
    String espStr = "Vibracion " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(5000);
    str = "";
  } else {
    Serial.println("NO hay Vibracion detectada");
    str = String("0");
    String espStr = "Vibracion " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(5000);
    str = "";
  }

  //Movimiento a la esp
  if (PIRvalue == HIGH) {
    Serial.println("Motion detected!");
    str = String("1");
    String espStr = "Movimiento " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(5000);
    str = "";
    delay(5000);
  } else {
    Serial.println("NO Motion detected!");
    str = String("0");
    String espStr = "Movimiento " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(5000);
    str = "";
  }


  if (soilmoisturepercent >= 0) {
    String espStr = "Soil" + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(5000);
    str = "";
  }

  // Agua a la esp
  waterLevel = analogRead(WaterSensorPin);
  if (waterLevel > 250) {
    Serial.println("Agua presente");
    str = String("1");
    String espStr = "Agua " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(5000);
    str = "";
  } else {
    Serial.println("No hay agua");
    str = String("0");
    String espStr = "Agua " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(5000);
    str = "";
  }
  Serial.println("----------------");

  delay(900000);
}