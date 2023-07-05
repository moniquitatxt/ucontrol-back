#include <SoftwareSerial.h>

SoftwareSerial espSerial(6, 5);

int temt6000Pin = A0;
float light;
int light_value;

#define outPin 6

String str;

void setup() {
  Serial.begin(9600);

  espSerial.begin(115200);

  pinMode(temt6000Pin, INPUT);  //data pin for ambientlight sensor
  pinMode(outPin, OUTPUT);
}

void loop() {
  int light_value = analogRead(temt6000Pin);
  light = light_value * 0.0976;  // percentage calculation


  if (light <= 1.0) {
    str = String("OFF. ") + String(light) + String("%");
    Serial.println(str);
  } else {
    str = String("ON. ") + String(light) + String("%");
    Serial.println(str);
  }



  // str=String("luisa");
  // espSerial.println(str);
  delay(2000);
}
