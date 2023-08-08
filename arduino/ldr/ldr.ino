int LDR = A0;
int AnalogValue;

void setup() {
  Serial.begin(9600);
}

void loop() {
  AnalogValue = analogRead(LDR);
  if (AnalogValue >= 1000) {
    Serial.print("OFF. Intensity: ");
    Serial.println(AnalogValue);

  } else {
    Serial.print("ON. Intensity: ");
    Serial.println(AnalogValue);
  }

  delay(2000);
}
