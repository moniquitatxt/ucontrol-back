#include <SoftwareSerial.h>


SoftwareSerial Arduino_SoftSerial (10,11);

int PIR = 2;			// señal de PIR a pin 2
int ESTADO = 0;			// almacena estado de señal

#define outPin 6
String str;


void setup(){
  Serial.begin(9600);
  Arduino_SoftSerial.begin(9600);

  pinMode(PIR, INPUT);		// pin 2 como entrada
  delay(20000);			// demora para estabilizar el sensor
  Serial.println("PIR Movimiento");
}

void loop(){

  ESTADO = digitalRead(PIR);	// lectura de estado de señal
  if (ESTADO == HIGH){		// si la señal esta en alto indicando movimiento
   Serial.println("Movimiento")	;// activa modulo de rele con nivel bajo
   str=String("1");
  String espStr=str+String('\n');

  Arduino_SoftSerial.print(espStr);
    delay(4000);		// demora de 4 segundos
  } else {
    Serial.println("No hay movimiento");// apaga modulo rele
    str=String("0");
  String espStr=str+String('\n');

  Arduino_SoftSerial.print(espStr);
  }
  

  delay(5000);
}