import serial
import time

def send_gps(serial_port, lat, lng, speed):
    data = f"{lat},{lng},{speed}\n"
    serial_port.write(data.encode('utf-8'))
    print(f"Sent GPS: {data.strip()}")

if __name__ == "__main__":
    port_name = 'COM5'  # Change this to your Arduino’s COM port (e.g., /dev/ttyUSB0 on Linux)
    baud_rate = 9600

    try:
        ser = serial.Serial(port_name, baud_rate, timeout=1)
        time.sleep(2)  # Wait for Arduino to initialize

        points = [
            (12.9700, 77.5900, 25.0),
            (12.9705, 77.5920, 28.0),
            (12.9710, 77.5935, 30.0),
            (12.9716, 77.5946, 35.0),  # Accident zone
        ]

        for lat, lng, speed in points:
            send_gps(ser, lat, lng, speed)
            time.sleep(1)  # Wait 1 second between sends

        ser.close()

    except serial.SerialException:
        print(f"Could not open serial port {port_name}")
