# -*- coding: utf-8 -*-
"""
Created on Wed Apr 29 10:29:25 2020
Last modified on Wed Mai 21 16:43:58 2025

@author: Gion-Pol Catregn (FHGR)
@author: Sascha Zumstein (FHGR)

@version 1.0
"""

# Import libraries
import numpy as np
from numpy import *
from PyQt5 import QtWidgets  
import pyqtgraph as pg
import serial
import csv
import keyboard

# Create object serial port
portName = "COM7"
baudrate = 115200
ser = serial.Serial(portName,baudrate)

filename = "LTS_CurveData.csv"
save_next_frame = False
i = 1

### START QtApp #####
app = QtWidgets.QApplication([]) # you MUST do this once (initialize things)
####################

pg.setConfigOption('background', 'k')
pg.setConfigOption('foreground', 'w')

#Enable antialiasing for prettier plots
pg.setConfigOptions(antialias=True)

win = pg.GraphicsLayoutWidget(title="LTS Data") # creates a window
p = win.addPlot(title="LTS Data")  # creates empty space for the plot in the window
curve = p.plot(pen=pg.mkPen('r', width=2)) # create an empty "plot" (a curve to plot)
#Set the range of the axis
p.setYRange(0, 150, padding=0)
p.setXRange(0, 1023, padding=0)
#Switch on the grid
p.showGrid(x = True, y = True, alpha = 0.8)
#Set the lable of the axis
p.setLabel('left', 'PixelValue', units='DN')
p.setLabel('bottom', 'Pixel')
#Resize the window and set the title
win.setGeometry(200,200, 2000,1000 )
win.setWindowTitle('pyqtgraph example: Plotting LTS Data')
#Activate and show the window
win.show()
win.activateWindow()

def write_csv(current_char):
    global i

    with open(filename, mode='r', newline='') as file:
        reader = csv.reader(file)
        rows = list(reader)

    rows[i][0] = current_char[0:5].decode('utf-8')
    i += 1

    # Die geänderten Daten wieder in die CSV-Datei schreiben
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerows(rows)
     

# Realtime data plot. Each time this function is called, the data display is updated
def update():
    global curve#global variable for the vcurve
    global save_next_frame

    if keyboard.is_pressed('s') and not save_next_frame:
        print("Taste S gedrückt – nächste Messung wird gespeichert.")
        save_next_frame = True
    #Search for the Fram Start ("START")
    current_char = ser.readline()
    if current_char[:8] == b'Distance': # or current_char[:7] == b'Shutter': 
        print(current_char)
    if save_next_frame and current_char[:8] == b'Distance':
        write_csv(current_char[10:])
        print("Messung gespeichert.")
        save_next_frame = False

    # check for the start string
    if current_char[:5] == b'START':
        #Read the whole frame
        reading = ser.read(1024)
        #Change from ASCII to uint8
        readings=np.frombuffer(reading, dtype=np.uint8, count=1024)
        #Define the x axis
        x = np.arange(1024)
        # set the curve with this data
        curve.setData(x,readings)

        
    QtWidgets.QApplication.processEvents()    # you MUST process the plot now

### MAIN PROGRAM #####
ser.write(b'd') 

# this is a brutal infinite loop calling your realtime data plot
try:
	while True: update()

except KeyboardInterrupt:

    print("Programm Stopped")
    ser.close()
    app.quit()
    win.close()