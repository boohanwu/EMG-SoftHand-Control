# Syringe Pump Controller
This is an arduino project code for syringe pump machanism.
There are two parts: Master and Slave folders.
## Master
Inside the "Master" folder. Open the code with arduino and burning the program into a microchip (Teensy 4.0).
## Slave
Inside the "Slave" folder. Open the code with arduino and burning the program into a microchip (Teensy 4.0).
The syringe pump machanism have five slaves, you should burning the program into five different Teensy chips, and the slave ID starts from 1 to 5. 
Change the ID of "rxdata.motor_xx_CMD_High_Byte", xx is the ID you set.
## The command format
**(Degree of Motor1, Degree of Motor2, Degree of Motor3, Degree of Motor4, Degree of Motor5, Running Time)**
The degree of motor range starts from 90 to 0, 90 degree is the inital position, and 0 is the maximum position where the syringe can reach, which can apply the maximum air pressure to the soft finger.
The recommend range of running time is 1000~3000(ms) 
For example: (90,0,0,10,50,1500)
