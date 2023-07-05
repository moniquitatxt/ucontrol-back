#include <SoftwareSerial.h>

SoftwareSerial espSerial(6, 5);

int temt6000Pin = A0;
float light;
int light_value;
float volt;

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

  //volt conversion
  int mVolt = map(light_value, 0, 1023, 0, 5000); 
  volt=(double)mVolt/1000;

  if (light <= 1.0 || volt <=0) {
    str = String("OFF. ") + String(light) + String("% ")+String(volt,3)+String("V");
    Serial.println(str);
  } else if (light > 1.0 || volt > 0){
    str = String("ON. ") + String(light) + String("% ")+String(volt,3)+String("V");
    Serial.println(str);
  }



  // str=String("luisa");
  // espSerial.println(str);
  delay(2000);
}
