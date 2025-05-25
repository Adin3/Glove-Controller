import serial
import time
import re
from mouse_movement import MouseMovement 

COM_PORT = 'COM9'
BAUD_RATE = 9600

# Regex
pattern = (
    r"A:\s*(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+g\s+\|\s+"
    r"G:\s*(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+dps\s+\|\s+"
    r"M:\s*(-?\d+)\s+(-?\d+)\s+(-?\d+)"
)

def main_loop(driver):
    print(f"Connecting to {COM_PORT} with {BAUD_RATE} baud...")
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    time.sleep(4)
    print("Connected")

    while driver.is_close_app == False:

        response = ser.readline().decode().strip()
        if response:
            match = re.search(pattern, response)
            if match:
                ax, ay, az, gx, gy, gz = map(float, match.groups()[:6])
                mx, my, mz = map(int, match.groups()[6:])
                print("Accel:", ax, ay, az)
                print("Gyro:", gx, gy, gz)
                print("Mag:", mx, my, mz)

                if mx > 1500:
                    print("Magnitude difference:", mx)
                    driver.click()

                if (ax < -0.25) or (ax > 0.25):
                    driver.move(-20 * ax, 0)
                
                if (ay < -0.35) or (ay > 0.25):
                    driver.move(0, -20 * ay)
                
    else:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Connection closed.")


def main():
    mouse_driver = MouseMovement()
    main_loop(mouse_driver)   

if __name__ == "__main__":
    main()
