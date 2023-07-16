#include <Adafruit_GFX.h>
#include <SoftwareSerial.h>


// Sensor pins
#define sensorPower 7
#define sensorPin A0

SoftwareSerial Arduino_SoftSerial(10, 11);


String str;
char c;
String dataFromNodeMCU;


int soilMoistureValue = 0;
int soilmoisturepercent=0;

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
    // if (dataFromNodeMCU.indexOf("ON")>=0) {
    //   digitalWrite(relayPin, HIGH);

    // } else if (dataFromNodeMCU.indexOf("OFF")>=0) {
    //   digitalWrite(relayPin, LOW);
    // }
    c = 0;
    dataFromNodeMCU = "";
  }
}
void setup() {
  Serial.begin(9600);
}

void loop() {
  soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
  Serial.println(soilMoistureValue);
  Arduino_SoftSerial.print(String(soilMoistureValue)+String('\n'));
	
  delay(5000);
}
