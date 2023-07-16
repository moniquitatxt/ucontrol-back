#include <SoftwareSerial.h>


// Sensor pins
#define sensorPower 7
#define sensorPin A0

// Value for storing water level
int val = 0;
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
  // Set D7 as an OUTPUT
  pinMode(sensorPower, OUTPUT);

  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);

  Serial.begin(9600);


  Arduino_SoftSerial.begin(9600);
}

void loop() {
  //get the reading from the function below and print it
  int level = readSensor();

  Serial.print("Water level: ");
  Serial.println(level);
  Arduino_SoftSerial.print(String(level) + String('\n'));
  delay(5000);
}

//This is a function used to get the reading
int readSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(10);                        // wait 10 milliseconds
  val = analogRead(sensorPin);      // Read the analog value form sensor
  digitalWrite(sensorPower, LOW);   // Turn the sensor OFF
  return val;                       // send current reading
}