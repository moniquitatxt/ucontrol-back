
int soilMoistureValue = 0;
const int SoilSensorPin = A4;
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  analogReference(EXTERNAL);
}

void loop() {
  // put your main code here, to run repeatedly:

  soilMoistureValue = analogRead(SoilSensorPin);
  Serial.println(soilMoistureValue);
  
  delay(5000);
  }
