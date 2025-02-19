import serial
import webbrowser
import threading

# Replace 'COM10' with your ESP32 port (Check Arduino Serial Monitor)
ser = serial.Serial('COM10', 115200, timeout=5)

def read_serial():
    while True:
        line = ser.readline().decode('utf-8').strip()
        if "Image URL:" in line:
            url = line.split("Image URL: ")[1]
            print(f"Opening {url} in browser...")
            webbrowser.open(url)  # Opens image in default browser
# Create a separate thread for serial reading and browser opening
serial_thread = threading.Thread(target=read_serial)
serial_thread.daemon = True
serial_thread.start()

# Main thread can do something else if needed
while True:
    pass  # Keep the program running

