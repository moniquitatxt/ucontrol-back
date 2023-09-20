#define x A0  // x_out pin of Accelerometer
#define y A1  // y_out pin of Accelerometer
#define z A2  // z_out pin of Accelerometer

/*variables*/
int xsample = 0;
int ysample = 0;
int zsample = 0;
long start;
int buz = 0;


/*Macros*/
#define samples 100
#define maxVal 800    // max change limit
#define minVal -800   // min change limit
#define buzTime 5000  // buzzer on time


void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.println("Calibrando...");
  for (int i = 0; i < samples; i++)  // taking samples for calibration
  {
    xsample += analogRead(x);
    ysample += analogRead(y);
    zsample += analogRead(z);
  }


  buz = 0;

  xsample /= samples;  // taking avg for x
  ysample /= samples;  // taking avg for y
  zsample /= samples;  // taking avg for z

  Serial.println("Calibrado!");
}

void loop() {
  int value1 = analogRead(x);  // reading x out
  int value2 = analogRead(y);  //reading y out
  int value3 = analogRead(z);  //reading z out

  int xValue = xsample - value1;  // finding change in x
  int yValue = ysample - value2;  // finding change in y
  int zValue = zsample - value3;  // finding change in z

 /* comparing change with predefined limits*/
  if (xValue < minVal || xValue > maxVal || yValue < minVal || yValue > maxVal || zValue < minVal || zValue > maxVal) {  // timer start
    Serial.println("Vibración detectada ");
  }

  Serial.print("x= ");
  Serial.println(xValue);
  Serial.print("y= ");
  Serial.println(yValue);
  Serial.print("z= ");
  Serial.println(zValue);
  Serial.println("");


delay(30000);
 


}
