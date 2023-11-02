#include <SoftwareSerial.h>

int relayPin = 3;
SoftwareSerial Arduino_SoftSerial(10, 11);

String str;
char c;
String dataFromNodeMCU;
int oldStatus = -1; // Inicializado a un valor no válido

void ReceiveData() {
  int status = digitalRead(relayPin);

  if (status != oldStatus) { // Verifica si el estado del relé ha cambiado
    oldStatus = status;
    str = String(status);
    String espStr = str + String('\n');
    Arduino_SoftSerial.print(espStr);
  }

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
    if (dataFromNodeMCU.indexOf("1") >= 0) {
      digitalWrite(relayPin, HIGH);
    } else if (dataFromNodeMCU.indexOf("0") >= 0) {
      digitalWrite(relayPin, LOW);
    }
    c = 0;
    dataFromNodeMCU = "";
  }
}

void setup() {
  Serial.begin(9600);
  Arduino_SoftSerial.begin(115200);
  pinMode(relayPin, OUTPUT);
}

void loop() {
  ReceiveData();

  // Lee desde la consola
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      digitalWrite(relayPin, HIGH); // Enciende el relé
      Serial.println("Relé encendido.");
    } else if (command == '0') {
      digitalWrite(relayPin, LOW); // Apaga el relé
      Serial.println("Relé apagado.");
    }
  }
  delay(5000); // Pequeña pausa para evitar lecturas continuas
}
