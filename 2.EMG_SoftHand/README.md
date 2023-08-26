# EMG-SOFTHAND-CONTROL
This is soft hand controller via EMG signal.
## hand_control.py
Send the command to the syringe pump mechanism using python.
Follow the command format: **(Degree of Motor1, Degree of Motor2, Degree of Motor3, Degree of Motor4, Degree of Motor5, Running Time)**
## emg_hand_control.py
Send the command to the syringe pump mechanism using gesture classification.
## Mygesture_classification.py
Send the gesture classification result to the C# GUI (EMG_gui file).
This Python code functions as the "Server," while the C# GUI code serves as the "Client." They communicate with each other using sockets.
Excute this python file first, then excute the C# file to open GUI, press the "Start" button to show the gesture classification result.