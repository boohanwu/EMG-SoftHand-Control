// Create an IntervalTimer object
IntervalTimer myTimer;

// Declare Variable
const int sensorPin_1 = A0;
const int sensorPin_2 = A1;
const int sensorPin_3 = A2;
const int sensorPin_4 = A3;
//const int sensorPin_5 = A4;
float sensorValue_Read_1 = 0;
float sensorValue_Read_2 = 0;
float sensorValue_Read_3 = 0;
float sensorValue_Read_4 = 0;
//float sensorValue_Read_5 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myTimer.begin(sensorRead, 1000); // Run every 0.01s = 10000
}

void loop() {
  // put your main code here, to run repeatedly:

}

void sensorRead(){
  sensorValue_Read_1 = analogRead(sensorPin_1);
  sensorValue_Read_2 = analogRead(sensorPin_2);
  sensorValue_Read_3 = analogRead(sensorPin_3);
  sensorValue_Read_4 = analogRead(sensorPin_4);
//  sensorValue_Read_5 = analogRead(sensorPin_5);
  double sensorValue_1 = double(sensorValue_Read_1); 
  double sensorValue_2 = double(sensorValue_Read_2); 
  double sensorValue_3 = double(sensorValue_Read_3); 
  double sensorValue_4 = double(sensorValue_Read_4); 
//  double sensorValue_5 = double(sensorValue_Read_5);  
  Serial.print(sensorValue_1);
  Serial.print(",");
  Serial.print(sensorValue_2);
  Serial.print(",");
  Serial.print(sensorValue_3);
  Serial.print(",");
  Serial.print(sensorValue_4);
//  Serial.print(",");
//  Serial.print(sensorValue_5);
  Serial.print("\n");
}
