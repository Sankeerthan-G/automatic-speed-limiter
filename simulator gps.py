import serial
import time

def send_gps(serial_port, lat, lng, speed):
    data = f"{lat},{lng},{speed}\n"
    serial_port.write(data.encode('utf-8'))
    print(f"Sent GPS: {data.strip()}")

if __name__ == "__main__":
    port_name = 'COM5'  # Change to your Arduino port
    baud_rate = 9600

    try:
        ser = serial.Serial(port_name, baud_rate, timeout=1)
        time.sleep(2)  # Arduino init delay

        # Simulate a route with many points approaching accident zone
        route_points = [
            (12.9680, 77.5880, 25.0),
            (12.9690, 77.5890, 26.0),
            (12.9700, 77.5900, 27.0),
            (12.9705, 77.5915, 27.5),
            (12.9710, 77.5930, 29.0),
            (12.9714, 77.5940, 30.0),
            (12.9715, 77.5943, 22.0),  # Nearing accident zone warning start (~200m)
            (12.97155, 77.5945, 18.0),
            (12.9716, 77.5946, 35.0),  # Inside accident zone, overspeed
            (12.9716, 77.5946, 18.0),  # Inside accident zone, speed reduced
            (12.9717, 77.5947, 18.0),
            (12.9720, 77.5950, 20.0),  # Leaving accident zone
        ]

        while True:
            for lat, lng, speed in route_points:
                send_gps(ser, lat, lng, speed)
                time.sleep(1)  # 1 second interval

    except serial.SerialException:
        print(f"Could not open serial port {port_name}")
