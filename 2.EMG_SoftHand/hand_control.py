import serial
import time

def resetPos():
    s.write(b'90,90,90,90,90,2000')
    print("Initial position")

def hand_control():
    print("----------start------------")
    print("Enter the key to move, 0:Reset, 1:Hold, 2:Two, 3:Three, 4:Four, 6:Six, 7:Seven")
    key = input("")
    while True:
        print("Enter the key to move, 0:Reset, 1:Hold, 2:Two, 3:Three, 4:Four, 6:Six, 7:Seven")
        if key == "0":
            resetPos()
            key = input("")
            continue
        if key == "1":
            s.write(b'10,10,10,10,10,2000')
            time.sleep(3)
            resetPos()
            key = input("")
            continue
        if key == "2":
            s.write(b'0,90,90,0,0,2000')
            time.sleep(3)
            resetPos()
            key = input("")
            continue
        if key == "3":
            s.write(b'10,90,90,90,10,2000')
            time.sleep(3)
            resetPos()
            key = input("")
            continue
        if key == "4":
            s.write(b'10,90,90,90,90,2000')
            time.sleep(3)
            resetPos()
            key = input("")
            continue
        if key == "6":
            s.write(b'90,10,10,10,90,2000')
            time.sleep(3)
            resetPos()
            key = input("")
            continue
        if key == "7":
            s.write(b'90,90,10,10,10,2000')
            time.sleep(3)
            resetPos()
            key = input("")
            continue
        # if key == "8":
        #     s.write(b'0,90,90,90,90,2000')
        #     time.sleep(2)
        #     s.write(b'0,0,90,90,90,2000')
        #     time.sleep(2)
        #     s.write(b'0,0,0,90,90,2000')
        #     time.sleep(2)
        #     s.write(b'0,0,0,0,90,2000')
        #     time.sleep(2)
        #     s.write(b'0,0,0,0,0,2000')
        #     time.sleep(2)
        #     resetPos()
        #     key = input("")
        #     continue
        else:
            key = input("")
            continue

            
port = 'COM8'   # change the port name as your settings
baud = 115200     # change the port name as your settings
s = serial.Serial(port=port, baudrate=baud)
    
if __name__ == '__main__':
    hand_control()