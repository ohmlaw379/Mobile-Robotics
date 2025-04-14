# -*- coding: utf-8 -*-
"""
Created on Wed Feb 19 16:53:32 2025

@author: omshe
"""

import serial
import webbrowser

# Replace 'COM3' with your ESP32 port (Check Arduino Serial Monitor)
ser = serial.Serial('COM3', 115200, timeout=5)

while True:
    line = ser.readline().decode('utf-8').strip()
    if "Image URL:" in line:
        url = line.split("Image URL: ")[1]
        print(f"Opening {url} in browser...")
        webbrowser.open(url)  # Opens image in default browser
