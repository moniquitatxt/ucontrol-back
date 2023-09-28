#include <SoftwareSerial.h>


int xpin = A1;
int ypin = A2;
int zpin = A3;
int relayPin = 3;

/*variables*/
int xsample = 0;
int ysample = 0;
int zsample = 0;
long start;
int buz = 0;

const int PIRPin = 2;


const int AirValue = 950;
const int WaterValue = 20;
int intervals = (AirValue - WaterValue) / 3;
const int SensorPin = A4;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

SoftwareSerial Arduino_SoftSerial(10, 11);
String str;
char c;
String dataFromNodeMCU;

void ReceiveData() {

  while (Arduino_SoftSerial.available() > 0) {

    c = Arduino_SoftSerial.read();
    if (c == '\n') {
      break;
    } else {
      dataFromNodeMCU += c;
    }
  }

  if (c == '\n') {
    Serial.println(dataFromNodeMCU);
    if (dataFromNodeMCU.indexOf("1") >= 0) {
      str = String("1");
      digitalWrite(relayPin, HIGH);
      String espStr = str + String('\n');
      Arduino_SoftSerial.print(espStr);

    } else if (dataFromNodeMCU.indexOf("0") >= 0) {
      str = String("0");
      digitalWrite(relayPin, LOW);
      String espStr = str + String('\n');
      Arduino_SoftSerial.print(espStr);
    }
    c = 0;
    dataFromNodeMCU = "";
  }
}



void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);
  pinMode(PIRPin, INPUT);

  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(zpin, INPUT);

  pinMode(relayPin, OUTPUT);

  Arduino_SoftSerial.begin(115200);
  delay(10000);
}

void loop() {


  int PIRvalue = digitalRead(PIRPin);


  soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  Serial.println(soilmoisturepercent);
  if (soilmoisturepercent > 0) {
    str = String(soilmoisturepercent);
    String espStr = str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(2000);
    str = "";
  }


  int x = analogRead(xpin);  // Leemos el valor de la tensión en el pin x

  delay(1);  // Esperamos 1 ms a leer en el próximo pin

  int y = analogRead(ypin);  // Leemos el valor de la tensión en el pin y

  delay(1);  // Esperamos 1 ms a leer en el próximo pin

  int z = analogRead(zpin);

  // Una conversión analogo a digital va de 0 a 1023, siendo 512
  // la mitad del rango y por lo tanto el 0
  float zero_G = 512.0;

  float escala = 102.3;

  float xValue = ((float)x - zero_G) / escala;
  float yValue = ((float)y - zero_G) / escala;
  float zValue = ((float)z - zero_G) / escala;

  Serial.print(xValue);
  Serial.print("\t");

  Serial.print(yValue);
  Serial.print("\t");

  Serial.print(zValue);
  Serial.print("\n");

  if ((xValue > 2.5) || (yValue > 2.5) || (zValue > 0.6)) {
    Serial.println("Vibracion detectada");
    str = String("1");
    String espStr = "Vibracion " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(2000);
    str = "";
  }

  if (PIRvalue == HIGH) {
    Serial.println("Motion detected!");
    str = String("1");
    String espStr = "Movimiento " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(2000);
    str = "";
    delay(5000);
  } else {
    Serial.println("NO Motion detected!");
  }
  if (digitalRead(relayPin) == 1) {
    str = String("1");
    digitalWrite(relayPin, HIGH);
    String espStr = "Luz " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(2000);
    str = "";
  }
  else if (digitalRead(relayPin) == 0) {
    str = String("0");
    digitalWrite(relayPin, LOW);
    String espStr = "Luz " + str + String('\n');
    Arduino_SoftSerial.print(espStr);
    delay(2000);
    str = "";
  }

  delay(900000);
  ReceiveData();
}