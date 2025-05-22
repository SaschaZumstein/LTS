# -*- coding: utf-8 -*-
"""
Created on Wed Mai 21 16:43:58 2025

@author: Sascha Zumstein (FHGR)

@version 1.0
"""

# Import libraries
import serial
import time

# Create object serial port
portName = "COM7"
baudrate = 115200
ser = serial.Serial(portName,baudrate)

# Select mode
debug = True
fast = False

count = 0
start_time = None
end_time = None

def update():
    global count
    global start_time
    global end_time

    # Search for special frame
    current_char = ser.readline()
    if current_char[:8] == b'Distance':
        if count == 0:
            start_time = time.time()
        count += 1
        if count == 100:
            end_time = time.time()
            duration = (end_time - start_time) / 100
            freq = 1 / duration
            if debug:
                print("Debug Mode")
            elif fast:
                print("Fast Mode")
            else:
                print("Normal Mode")
            print(f"Frequenz: {freq:.2f} Hz")  
            count = 0


### MAIN PROGRAM #####
# select mode
if debug and fast:
    raise Exception("You can't enable debug and fast mode at the same time")
elif debug:
    ser.write(b'd') 
elif fast:
    ser.write(b'f')
else:
    ser.write(b'n') 

# this is a brutal infinite loop calling your realtime data plot
try:
    while True:
        update()

except KeyboardInterrupt:
    print("Programm Stopped")
    ser.close()