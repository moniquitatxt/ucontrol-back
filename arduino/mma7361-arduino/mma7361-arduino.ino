#include <math.h>

int xVal = 0;
int yVal = 0;
int zVal = 0;

double angleYZ = 0;
double angleXZ = 0;
double i = 0;

void setup() {
  analogReference(EXTERNAL);
  Serial.begin(9600);
}

void loop() {
  xVal = analogRead(0);
  yVal = analogRead(1);
  zVal = analogRead(2);
  xVal = map(xVal, 0, 1023, -500, 500);
  yVal = map(yVal, 0, 1023, -500, 500);
  zVal = map(zVal, 0, 1023, -500, 500);
  angleYZ = atan((double)yVal / (double)zVal);
  angleYZ = angleYZ*(57.2958);
  angleXZ = atan((double)xVal / (double)zVal);
  angleXZ = angleXZ*(57.2958);
 // acceleration=((0.000001)*sen(2*3.1416*))
  Serial.write("yz:");
  Serial.print(angleYZ);
  Serial.write("\n");
  Serial.write("xz:");
  Serial.print(angleXZ);
  Serial.write("\n");
  Serial.print(i);
  Serial.println("-----------------");
  i++;
  delay(1000);
}