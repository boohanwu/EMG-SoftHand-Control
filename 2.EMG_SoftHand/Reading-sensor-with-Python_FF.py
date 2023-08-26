import os
import serial
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import timeit
import csv

# Set up the serial line
ser = serial.Serial('COM3', 115200) # Arduino COM and Baudrate
time.sleep(1)

# Start counting time
timer_start = timeit.default_timer()
# fd_number=input("Male: ")
# folder="Male_{}.csv".format(fd_number)
# if not os.path.exists(folder):
#     os.makedirs(folder)

while(True):
    input_number=input("input:\t")
    name_file = f"Three_{input_number}.csv"
    
    # Start
    print("Start")
    # Prepare
    time.sleep(5)
    # Start 
    print("Collecting...")
    
    # Prepare for actually start
    # time.sleep(0.5)
    # Read and record the data
    # name="TurnLeft_27.csv"
    
    SaveData =[]                                # Empty list to store the date
    with open(name_file, "w+",newline ="") as csvfile:     # Open csv file, If not create csv file
        w = csv.writer(csvfile)   
        w.writerow(["CH1", "CH2", "CH3", "CH4"])
        for i in range(15000):         
            EMGval = ser.readline()                  # Read a byte string
            string_n = EMGval.decode("UTF-8")        # Decode byte string to Unicode
            string_EMGval = string_n.rstrip()          # Remove \n and \r
            # print(EMGval)
            EMG1,EMG2,EMG3,EMG4 = string_EMGval.split(",")
            Float_EMG1,Float_EMG2,Float_EMG3,Float_EMG4 = float(EMG1),float(EMG2),float(EMG3),float(EMG4)     # Convert string to float
            # print(flt_EMG)
            SaveData = [Float_EMG1,Float_EMG2,Float_EMG3,Float_EMG4]                    # Add to the end of data list
            # with open(name_file, "a+",newline ="") as csvfile:
                # w = csv.writer(csvfile)
                # w.writerow(SaveData)
            w.writerow(SaveData)
    print("Done {}".format(name_file))
        #time.sleep(0.001)                  # wait  (sleep) 0.1 seconds

    # Stop counting time
    timer_stop = timeit.default_timer()   

    # # Start counting time
    # timer_start = timeit.default_timer()
    # # Read data
    # read_Data()
    # ser.close()
    # # Stop counting time
    # timer_stop = timeit.default_timer()

    # Show the data
    #or line in data:
        #print(line)

    # Timeout
    timeout = timer_stop - timer_start
    print(timeout)

    # Plot the data
    # plt.plot(SaveData)
    # plt.title('Arduino data')
    # plt.xlabel('Data Point')
    # plt.ylabel('Analog Reading vs Time')
    # plt.show()

    # Animation
    #fig = plt.figure()
    #plt.axes(xlim =(-2,100), ylim = (0, 1023))
    #streaming = animation.FuncAnimation(fig, read_Data, interval = 50)
    #plt.show()

    # print(EMGval)
    # EMG1,EMG2,EMG3,EMG4 = EMGval.split("/")       #以 / 作為字元分割切割字串
    # Float_EMG1,Float_EMG2,Float_EMG3,Float_EMG4 = float(EMG1),float(EMG2),float(EMG3),float(EMG4)


