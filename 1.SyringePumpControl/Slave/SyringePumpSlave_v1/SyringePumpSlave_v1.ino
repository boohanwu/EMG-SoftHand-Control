// 0414 add fsr and flex sensor
// 0419 add valve control
#define mpwm 4
#define dir 3
#define maxspeed 4000
#define minspeed 0
// analog read pin
#define PotPin 18  // Potentiometer 1k ohm
#define Pressure_PIN 17   // Pressure sensor
#define FLEX_PIN 16   // Flex sensor
#define FSR_PIN 15    // Force sensitive resistor
#define Valve_PIN 14  // Valve control

IntervalTimer PIDTimer;

float kp = 100;
float ki = 0.1;
float kd = 0;
float integral = 0;  // 累計誤差
float error = 0;
float diff_error = 0;  // Difference of error
float old_error = 0;   // Previous error

float output_speed = 0;
int count = 0;
int detect_num = 0; // 偵測時間
int acc_num = 0;
float runTime; // value[1]
float initialPos = 0;
float targetPos;  // value[0]
float uplimitAngle = 100;
float downlimitAngle = -10;
float displacement = 0;
long position = 0;

int period = 1;
long time1 = 0;
long time2 = 0;
bool record = 0;

// Potentiometer setup
float potVal = 0;   // Potentiometer value

// Pressure sensor setup
const float VCC = 3.3;
const float offset = 0.53;
const float InitialPressure = 0.0;
const float MaxPressure = 50000.0;
float PressureVal;

// Flex sensor setup
const float FlexResDiv = 987.0; //change it!
const float FlexStraightRes = 10000.0;
const float FlexBendRes = 40000.0;
float FlexVal;
float FlexAngle = 0;
float FlexR_last = 0;

// FSR setup
const float ForceResDiv = 987.0; //change it!
float ForceVal;
float Force;

void setup() {
  // DC Motor inital setup
  Serial.begin(115200);
  Serial.setTimeout(500);
  // analogReadResolution(12);    // 用12bit要改成4096
  analogWriteResolution(12);
  pinMode(mpwm, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(Pressure_PIN, INPUT);
  pinMode(FSR_PIN, INPUT);
  pinMode(FLEX_PIN, INPUT);
  pinMode(Valve_PIN, OUTPUT);

  analogWrite(mpwm, 0);
  digitalWrite(dir, HIGH);
  digitalWrite(Valve_PIN, LOW);

  PIDTimer.begin(pos, 1000);
  delay(10);
}

void loop() {
  if (Serial.available() >= 0){
    char firstChar = Serial.read();
    if(firstChar=='R'){
      digitalWrite(Valve_PIN, HIGH);
      delay(1000);
      digitalWrite(Valve_PIN, LOW);
    }else if (firstChar=='A') {
      targetPos = Serial.parseFloat();
      runTime = Serial.parseFloat();

      if(targetPos >= downlimitAngle && targetPos <= uplimitAngle){
        initialPos = position;
        integral = 0;
        count = 0;
        time1 = millis();
        record = 1;
        displacement = targetPos - initialPos;

        detect_num = runTime / period;
        acc_num = detect_num / 6;
      }
      else{
        Serial.print("Error");
      }
    }
  }

  static uint32_t prev_ms = millis();
  if (millis() > prev_ms + 1) {
    if (record == 1) {
      // Serial.print("Steady Time: ");
      Serial.print(time2 - time1);
      Serial.print(',');
      // Serial.print(" Position: ");
      Serial.print(position);
      Serial.print(',');
      // Serial.print(" Speed: ");
      Serial.print(output_speed / 10); //output_speed
      Serial.print(',');
      // Serial.print(" Error: ");
      Serial.print(error);
      Serial.print(',');

      // pressure sensor measure
      // Read the ADC, and calculate voltage and resistance from it
      PressureVal = Filter_pressure();       // 獲得濾波器輸出值   
      // PressureVal = analogRead(Pressure_PIN);
      float press = (PressureVal * VCC / 1023.0) - offset;
      float pressure = map(press, 0, VCC, InitialPressure, MaxPressure);
      // Serial.print(" Pressure: ");
      Serial.print(pressure);
      Serial.print(',');

      // //======================Flex and FSR block=================================
      // Flex sensor measure
      FlexVal = Filter_angle();
      // FlexVal = analogRead(FLEX_PIN);
      float FlexV = FlexVal * VCC / 1023.0; // 量測到的電壓值
      float FlexR = FlexResDiv * (VCC / FlexV - 1.0);
      // Use the calculated resistance to estimate the sensor's bend angle
      if (FlexR < FlexBendRes){
        FlexAngle = map(FlexR, FlexStraightRes, FlexBendRes, 0.0, 270.0);
      }
      Serial.print(FlexAngle);
      Serial.print(",");

      // // FSR calculation
      // ForceVal = analogRead(FSR_PIN);
      // float ForceV = ForceVal * VCC / 1023.0;
      // float ForceR = ForceResDiv * (VCC / ForceV - 1.0);
      // float ForceG = 1.0 / ForceR;  // Calculate conductance
      
      // // Break the calibration curve to two linear slopes:
      // if (ForceR <= 1000){
      //   Force = (ForceG - 0.00075) / 0.000000032639;
      // }
      // else{
      //   Force = ForceG / 0.0000000642857;
      // }
      // Force = Force - 210;    // force - offset
      // if (Force < 0)
      //   Force = 0;

      // Serial.print(Force);
      // //======================Flex and FSR block=================================
      Serial.print('\n');

    }
    prev_ms = millis();
  }
  //  delay(10);
}

void pos()
{  //(pulse, millisecond)
  // potVal = (float)analogRead(PotPin);
  potVal = (float)Filter_pos();
  position = 180 - map(potVal, 0, 1023, 40, 320);  // Potentiometer rotate range:280 deg

  if (count <= detect_num)
  {
    if (count <= detect_num / 6)
    {
      //    output_speed = 0;
      error = displacement * 3.6 * count * count / detect_num / detect_num + initialPos - position;
    }
    else if (count > detect_num / 6 && count <= detect_num * 5 / 6)
    {
      error = 0.1 * displacement + 0.8 * displacement * (count - detect_num / 6) / (detect_num * 4 / 6) + initialPos - position;
    }
    else
    {
      error = 0.9 * displacement + (0.1 * displacement - displacement * 3.6 * (detect_num - count) * (detect_num - count) / detect_num / detect_num) + initialPos - position;
    }
    time2 = millis();
    count++;
  }
  else
  {
    error = integral = old_error = 0;
    //    digitalWrite(10, LOW);
    //    output_speed = 0;
    //    analogWrite(mpwm, output_speed);
    //    time2 = millis();
    if (record == 1 && error == 0)
    {
      time2 = millis();
      // Serial.println(position);
      // Serial.print(time2 - time1);
      // Serial.print(',');
      // Serial.print(position);
      // Serial.print(',');
      // Serial.print(error);
      // Serial.print(',');
      // Serial.println(targetPos - position);  //output_speed
      record = 0;
    }
  }
  integral += error;
  if (integral > 100)
  {
    integral = 100;
  }
  else if(integral < -100){
    integral = -100;
  }
  diff_error = error - old_error;
  old_error = error;
  output_speed = error * kp + ki * integral + kd * diff_error;

  if (output_speed < 0)
  {
    digitalWrite(dir, LOW);
    output_speed = abs(output_speed);
  }
  else
    digitalWrite(dir, HIGH);

  //  if(output_speed >= 100000){
  //    output_speed = 0;
  //  }

  if (minspeed < output_speed && output_speed < maxspeed)
  {
    analogWrite(mpwm, output_speed);
  }
  else if (output_speed > maxspeed)
  {
    output_speed = maxspeed;
    analogWrite(mpwm, output_speed);
  }
  else
  {
    output_speed = minspeed;
    analogWrite(mpwm, output_speed);
  }
}

// Weighted moving average filter for position
#define FILTER_N_1 12
int coe_1[FILTER_N_1] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};    // 加權係數表
int sum_coe_1 = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12; // 加權係數和
int filter_buf_1[FILTER_N_1 + 1];

float Filter_pos() {
  int i;
  float filter_sum_1 = 0;
  filter_buf_1[FILTER_N_1] = analogRead(PotPin);
  for(i = 0; i < FILTER_N_1; i++) {
    filter_buf_1[i] = filter_buf_1[i + 1]; // 所有數據左移，低位扔掉
    filter_sum_1 += filter_buf_1[i] * coe_1[i];
  }
  filter_sum_1 /= sum_coe_1;
  return filter_sum_1;
}


// Weighted moving average filter for pressure
#define FILTER_N_2 12
int coe_2[FILTER_N_2] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};    // 加權係數表
int sum_coe_2 = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12; // 加權係數和
int filter_buf_2[FILTER_N_2 + 1];

float Filter_pressure() {
  int j;
  float filter_sum_2 = 0;
  filter_buf_2[FILTER_N_2] = analogRead(Pressure_PIN);
  for(j = 0; j < FILTER_N_2; j++) {
    filter_buf_2[j] = filter_buf_2[j + 1]; // 所有數據左移，低位扔掉
    filter_sum_2 += filter_buf_2[j] * coe_2[j];
  }
  filter_sum_2 /= sum_coe_2;
  return filter_sum_2;
}

// Weighted moving average filter for angle
#define FILTER_N_3 12
int coe_3[FILTER_N_3] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};    // 加權係數表
int sum_coe_3 = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12; // 加權係數和
int filter_buf_3[FILTER_N_3 + 1];

float Filter_angle() {
  int k;
  float filter_sum_3 = 0;
  filter_buf_3[FILTER_N_3] = analogRead(FLEX_PIN);
  for(k = 0; k < FILTER_N_3; k++) {
    filter_buf_3[k] = filter_buf_3[k + 1]; // 所有數據左移，低位扔掉
    filter_sum_3 += filter_buf_3[k] * coe_3[k];
  }
  filter_sum_3 /= sum_coe_3;
  return filter_sum_3;
}





// // Butterworth low-pass filter implementation using Arduino
// #define SAMPLE_RATE 1000 // Sample rate in Hz
// #define CUTOFF_FREQ 50 // Cutoff frequency in Hz
// #define RC 1.0 / (2.0 * PI * CUTOFF_FREQ) // Time constant of the filter

// double filteredValue = 0; // Filtered value
// double alpha = RC / (RC + (1.0 / SAMPLE_RATE)); // Filter coefficient

// double filter(double inputValue) {
//   filteredValue = (1.0 - alpha) * filteredValue + alpha * inputValue;
//   return filteredValue;
// }
