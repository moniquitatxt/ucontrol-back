#include <SoftwareSerial.h>


int relayPin = 3;

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
    if (dataFromNodeMCU.indexOf("1")>=0) {
      str = String("1");
      digitalWrite(relayPin, HIGH);
      String espStr = str + String('\n');
      Arduino_SoftSerial.print(espStr);

    } else if (dataFromNodeMCU.indexOf("0")>=0) {
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

  pinMode(relayPin, OUTPUT); //data pin for ambientlight sensor
  pinMode(outPin, OUTPUT);
}

void loop() {

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
  delay(30000);
  ReceiveData();
}