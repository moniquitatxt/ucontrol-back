#include <SoftwareSerial.h>


int relayPin = 2;
int temt6000Pin = A0;
float light;
int light_value;
float volt;

SoftwareSerial Arduino_SoftSerial (10,11);


#define outPin 6
String str;

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

  if (light <= 1.0 || volt <= 0) {
    str = String("OFF. ") + String(light) + String("% ") + String(volt, 3) + String("V");
    Serial.println(str);

    digitalWrite(relayPin, HIGH);
  } else if (light > 1.0 || volt > 0) {
    str = String("ON. ") + String(light) + String("% ") + String(volt, 3) + String("V");
    Serial.println(str);
    digitalWrite(relayPin, LOW);
  }
 String espStr=str+String('\n');

  Arduino_SoftSerial.print(espStr);

  // str=String("luisa");
  // espSerial.println(str);
  delay(2000);

}