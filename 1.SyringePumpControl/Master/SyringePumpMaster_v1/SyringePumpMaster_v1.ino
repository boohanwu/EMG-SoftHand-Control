#include <EasyTransfer.h>

EasyTransfer RS485_TX, RS485_RX;

String serial_Receive_String;
String str_Component[100];

const char delimiter = ',';
// float delta_Length[18];
int16_t motor_CMD[5];
int16_t running_Time;

/*--------------- Motion control ------------*/
byte motor_CMD_PACKET[10];
byte motor_running_Time[2];

/*------------------- RS485 ------------------  */
#define SSerialTxControl 2   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW

int time_Out = 500;
int TxRx_Flag = 0;

struct SEND_DATA_STRUCTURE
{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
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

struct RECEIVE_DATA_STRUCTURE
{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
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

//------  Give a name to the group of data
SEND_DATA_STRUCTURE txdata;
RECEIVE_DATA_STRUCTURE rxdata;

void setup() {
  Serial.begin(115200); // start usb serial
  Serial1.begin(115200);
  Serial.setTimeout(50);
  RS485_TX.begin(details(txdata), &Serial1); // start easytransfer with Serial1
  RS485_RX.begin(details(rxdata), &Serial1); // start easytransfer with Serial1

  digitalWrite(SSerialTxControl, RS485Receive);
  delay(100);
}

void loop() {
  while (Serial.available() > 0){ // && TxRx_Flag == 0
    serial_Receive_String = Serial.readString();
    for (unsigned int i = 0; i < serial_Receive_String.length() - 1; i++){
      // split data based on point (.), Can also be replaced by comma (,)
      str_Component[i] = getValue(serial_Receive_String, ',', i);
    }

    for ( int i = 0; i < 5; i++){
      motor_CMD[i] = str_Component[i].toInt();
      // Serial.print(delta_Length[i]); Serial.print(" ");
    }
    running_Time = str_Component[5].toInt();

    // --------------- Convert motor control command to HEX --------------
    for (int i = 0 ; i < 10 ; i = i + 2){
      motor_CMD_PACKET[i] = Dec_to_Hex_Upper(motor_CMD[i / 2]);
      motor_CMD_PACKET[i + 1] = Dec_to_Hex_Lower(motor_CMD[i / 2]);
    }

    // --------------- Convert running time command to HEX --------------
    motor_running_Time[0] = Dec_to_Hex_Upper(running_Time);
    motor_running_Time[1] = Dec_to_Hex_Lower(running_Time);

    // --------------- Assign the motor control command to RS485 and arrange the motor ---------
    //    txdata.mode_Control = 0xFE;
    //    txdata.MOTOR_ID = 0x00;
    txdata.motor_1_CMD_High_Byte = motor_CMD_PACKET[0];
    txdata.motor_1_CMD_Low_Byte = motor_CMD_PACKET[1];
    txdata.motor_2_CMD_High_Byte = motor_CMD_PACKET[2];
    txdata.motor_2_CMD_Low_Byte = motor_CMD_PACKET[3];
    txdata.motor_3_CMD_High_Byte = motor_CMD_PACKET[4];
    txdata.motor_3_CMD_Low_Byte = motor_CMD_PACKET[5];
    txdata.motor_4_CMD_High_Byte = motor_CMD_PACKET[6];
    txdata.motor_4_CMD_Low_Byte = motor_CMD_PACKET[7];
    txdata.motor_5_CMD_High_Byte = motor_CMD_PACKET[8];
    txdata.motor_5_CMD_Low_Byte = motor_CMD_PACKET[9];

    txdata.running_Time_High_Byte = motor_running_Time[0];
    txdata.running_Time_Low_Byte = motor_running_Time[1];

    // -------------- RS485 Sending -----------------------------------
    digitalWrite(SSerialTxControl, RS485Transmit); // Direction pin in RS485 Transmit stage
    RS485_TX.sendData(); // Sending Data
    Serial.println("RS485 sending data: DONE.");
  }
  delay(10);
}

byte Dec_to_Hex_Upper(uint16_t Decimal)
{
  byte Upper = Decimal >> 8;
  // byte Lower = Decimal & 0xFF;

  return Upper;
}

byte Dec_to_Hex_Lower(uint16_t Decimal)
{
  // byte Upper = Decimal >> 8;
  byte Lower = Decimal & 0xFF;

  return Lower;
}

String getValue(String data, char delimiter, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == delimiter || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
