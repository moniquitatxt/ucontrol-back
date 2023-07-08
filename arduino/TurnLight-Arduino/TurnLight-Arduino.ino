#include <SoftwareSerial.h>


int relayPin = 2;
int temt6000Pin = A0;
float light;
int light_value;
float volt;

SoftwareSerial Arduino_SoftSerial(10, 11);


#define outPin 6
String str;
char c;
String dataFromNodeMCU;

void setup() {
  Serial.begin(9600);

  Arduino_SoftSerial.begin(115200);

  pinMode(relayPin, OUTPUT);
  pinMode(temt6000Pin, INPUT);  //data pin for ambientlight sensor
  pinMode(outPin, OUTPUT);
  Serial.println("Indica un comando: ");
}

String input;

void ReceiveData(){

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
      c = 0;
      dataFromNodeMCU = "";
    }

}

void loop() {
  int light_value = analogRead(temt6000Pin);
  light = light_value * 0.0976;  // percentage calculation

  //volt conversion
  int mVolt = map(light_value, 0, 1023, 0, 5000);
  volt = (double)mVolt / 1000;

  if (Serial.available()) {
    input = Serial.readStringUntil('\n');

    if (input.equals("ON")) {
      str = String("ON. ") + String(light) + String("% ") + String(volt, 3) + String("V");
      Serial.println(str);
      digitalWrite(relayPin, HIGH);
      String espStr = str + String('\n');
      Arduino_SoftSerial.print(espStr);

    } else if (input.equals("OFF")) {
      str = String("OFF. ") + String(light) + String("% ") + String(volt, 3) + String("V");
      Serial.println(str);
      digitalWrite(relayPin, LOW);
      String espStr = str + String('\n');
      Arduino_SoftSerial.print(espStr);

    } else {
      Serial.println("Comando no aceptado, solo puede ser ON o OFF, intenta de nuevo");
    }

    // str=String("luisa");
    // espSerial.println(str);

   
    delay(2000);
    ReceiveData();

  }
}