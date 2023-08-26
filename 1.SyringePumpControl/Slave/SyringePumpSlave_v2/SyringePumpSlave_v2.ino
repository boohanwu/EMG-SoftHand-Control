#include <EasyTransfer.h>
#include <SPI.h>

// RS485 DECLARATION
EasyTransfer RS485_RX, RS485_TX;
#define SSerialTxControl 2   //RS485 Direction control
#define RS485Transmit    HIGH
#define RS485Receive     LOW
int TxRx_Flag = 0;

byte motor_ctrl[2];
byte running_Time[2];
// int16_t target_Pos;
// int16_t motor_running_Time;

// --- MOTOR CONTROL VARIABLE ----
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
float ki = 1;
float kd = 0;
float integral = 0;  // 累計誤差
float error = 0;
float diff_error = 0;  // Difference of error
float old_error = 0;   // Previous error

float output_speed = 0;
int count = 0;
int detect_num = 0; // 偵測時間
int acc_num = 0;
int16_t targetPos;  // value[0]
int16_t runTime; // value[1]
float initialPos = 0;
float uplimitAngle = 100;
float downlimitAngle = -10;
float displacement = 0;
long position = 0;

int period = 1;
long time1 = 0;
long time2 = 0;
bool record = 0;

struct SEND_DATA_STRUCTURE
{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  byte motor_1_CMD_High_Byte;
  byte motor_1_CMD_Low_Byte ;
  byte motor_2_CMD_High_Byte;
  byte motor_2_CMD_Low_Byte ;
  byte motor_3_CMD_High_Byte;
  byte motor_3_CMD_Low_Byte ;
  byte motor_4_CMD_High_Byte;
  byte motor_4_CMD_Low_Byte ;
  byte motor_5_CMD_High_Byte;
  byte motor_5_CMD_Low_Byte ;

  byte running_Time_High_Byte;
  byte running_Time_Low_Byte;
};

struct RECEIVE_DATA_STRUCTURE
{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  //  char received_Data;
//  byte mode_Control;
//  byte MOTOR_ID;
  byte motor_1_CMD_High_Byte;
  byte motor_1_CMD_Low_Byte;
  byte motor_2_CMD_High_Byte;
  byte motor_2_CMD_Low_Byte;
  byte motor_3_CMD_High_Byte;
  byte motor_3_CMD_Low_Byte;
  byte motor_4_CMD_High_Byte;
  byte motor_4_CMD_Low_Byte;
  byte motor_5_CMD_High_Byte;
  byte motor_5_CMD_Low_Byte;

  byte running_Time_High_Byte;
  byte running_Time_Low_Byte;
};

//give a name to the group of data
SEND_DATA_STRUCTURE txdata;
RECEIVE_DATA_STRUCTURE rxdata;

// Potentiometer setup
float potVal = 0;   // Potentiometer value

// Pressure sensor setup
const float VCC = 3.28;
const float offset = 0.53;
const float InitialPressure = 0.0;
const float MaxPressure = 50000.0;
float Filter_Value;

// Flex sensor setup
const float FlexResDiv = 987.0; //change it!
const float Vcc = 3.25;
const float FlexStraightRes = 8370.0;
const float FlexBendRes = 36000.0;
float FlexVal;
float FlexAngle;

// FSR setup
const float ForceResDiv = 987.0; //change it!
float ForceVal;
float Force;

void setup() {
  // DC Motor inital setup
  Serial.begin(115200);
  // Serial.setTimeout(500);

  Serial1.begin(115200);
  // Serial1.setTimeout(500);

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

  // -- RS485
  RS485_RX.begin(details(rxdata), &Serial1); // start easytransfer with Serial1
  RS485_TX.begin(details(txdata), &Serial1 ); // start easytransfer with Serial1
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, RS485Receive);
}

void loop() {
  if (RS485_RX.receiveData()){  // && TxRx_Flag == 0
    motor_ctrl[0] = rxdata.motor_1_CMD_High_Byte;   // CHANGING PARAMETER SUITABLE WITH MOTOR ID
    motor_ctrl[1] = rxdata.motor_1_CMD_Low_Byte;

    running_Time[0] = rxdata.running_Time_High_Byte;
    running_Time[1] = rxdata.running_Time_Low_Byte;

    Serial.print("Motor Control Command in HEX: ");
    Serial.print(motor_ctrl[0], HEX);
    Serial.print(" ");                              
    Serial.println(motor_ctrl[1], HEX);

    Serial.print("Motor running time in HEX: ");
    Serial.print(running_Time[0], HEX);
    Serial.print(" ");
    Serial.println(running_Time[1], HEX);

    //---------- CONVERT motor control message to DEC ------------------
    targetPos =  (int16_t) motor_ctrl[0] << 8 | (int16_t) motor_ctrl[1];
    //---------- CONVERT Motor Running TIME message to DEC ------------------
    runTime =  (int16_t) running_Time[0] << 8 | (int16_t) running_Time[1];

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

  // // pressure sensor measure
  // // Read the ADC, and calculate voltage and resistance from it
  // Filter_Value = analogRead(Pressure_PIN);       // 獲得濾波器輸出值   
  // float press = (Filter_Value * VCC / 1023.0) - offset;
  // float pressure = map(press, 0, VCC, InitialPressure, MaxPressure);

  // // Flex sensor measure
  // FlexVal = analogRead(FLEX_PIN);
  // float FlexV = FlexVal * Vcc / 1023.0; // 量測到的電壓值
  // float FlexR = FlexResDiv * (Vcc / FlexV - 1.0);
  // // Use the calculated resistance to estimate the sensor's bend angle
  // float FlexAngle = map(FlexR, FlexStraightRes, FlexBendRes, 0.0, 180.0);

  // // FSR calculation
  // ForceVal = analogRead(FSR_PIN);
  // float ForceV = ForceVal * Vcc / 1023.0;
  // float ForceR = ForceResDiv * (Vcc / ForceV - 1.0);  // Calculate resistor of FSR
  // float ForceG = 1.0 / ForceR;  // Calculate conductance
  // // 用量測的??
  // if (ForceR <= 1000){
  //   Force = (ForceG - 0.00075) / 0.000000032639;
  // }
  // else{
  //   Force = ForceG / 0.0000000642857;
  // }

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
      // Serial.print(output_speed); //output_speed
      // Serial.print(',');
      // Serial.print(" Error: ");
      // Serial.print(error);
      // Serial.print(',');

      // Serial.print(pressure);
      // Serial.print(',');

      // Serial.print(FlexAngle);
      // Serial.print(",");

      // Serial.print(Force);
      // Serial.print('\n');
    }
  } 
}

void pos()
{  //(pulse, millisecond)
  potVal = (float)analogRead(PotPin);
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

// -------------------12-point weighted moving average filter------------------------- 
// #define FILTER_N 12
// int coe[FILTER_N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};    // 加權係數表
// int sum_coe = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12; // 加權係數和
// int filter_buf[FILTER_N + 1];

// float Filter() {
//   int i;
//   float filter_sum = 0;
//   filter_buf[FILTER_N] = analogRead(Pressure_PIN);
//   for(i = 0; i < FILTER_N; i++) {
//     filter_buf[i] = filter_buf[i + 1]; // 所有數據左移，低位扔掉
//     filter_sum += filter_buf[i] * coe[i];
//   }
//   filter_sum /= sum_coe;
//   return filter_sum;
// }
