#include <SoftwareSerial.h>


int relayPin = 3;
int temt6000Pin = A0;
float light;
int light_value;
float volt;

SoftwareSerial Arduino_SoftSerial(10, 11);


#define outPin 6
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
    if (dataFromNodeMCU.equals("ON")) {
      str = String("1");
      digitalWrite(relayPin, HIGH);
      String espStr = str + String('\n');
      Arduino_SoftSerial.print(espStr);

    } else if (dataFromNodeMCU.equals("OFF")) {
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

  Arduino_SoftSerial.begin(115200);

  pinMode(relayPin, OUTPUT);
  pinMode(temt6000Pin, INPUT);  //data pin for ambientlight sensor
  pinMode(outPin, OUTPUT);
}

void loop() {
  int light_value = analogRead(temt6000Pin);
  light = light_value * 0.0976;  // percentage calculation

  //volt conversion
  int mVolt = map(light_value, 0, 1023, 0, 5000);
  volt = (double)mVolt / 1000;

  if (digitalRead(relayPin) == 1) {
    str = String("1");
    digitalWrite(relayPin, HIGH);
    String espStr = str + String('\n');
    Arduino_SoftSerial.print(espStr);
  } else if (digitalRead(relayPin) == 0) {

    str = String("0");
    digitalWrite(relayPin, LOW);
    String espStr = str + String('\n');
    Arduino_SoftSerial.print(espStr);
  }
  delay(5000);
  ReceiveData();
}