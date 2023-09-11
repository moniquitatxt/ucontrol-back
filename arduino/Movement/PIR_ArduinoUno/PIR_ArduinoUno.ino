#include <SoftwareSerial.h>


SoftwareSerial Arduino_SoftSerial(10, 11);

int PIR = 2;     // señal de PIR a pin 2

#define outPin 6
String str;
char c;
String dataFromNodeMCU;


void setup() {
  Serial.begin(9600);
  Arduino_SoftSerial.begin(115200);

  pinMode(PIR, INPUT);  // pin 2 como entrada
  delay(20000);         // demora para estabilizar el sensor
  Serial.println("PIR Movimiento");
}

void loop() {

int  ESTADO = digitalRead(PIR);      
  if (ESTADO == HIGH) {            // si la señal esta en alto indicando movimiento
    Serial.println("Movimiento");  
    str = String("1");
    String espStr = str + String('\n');

    Arduino_SoftSerial.print(espStr);
  } else {
    Serial.println("No hay movimiento");  
    str = String("0");
    String espStr = str + String('\n');

    Arduino_SoftSerial.print(espStr);
  }


  delay(10000);
  
}