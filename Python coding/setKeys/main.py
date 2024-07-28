# python3.11 main.py
# from inputimeout import inputimeout, TimeoutOccurred
import serial.tools.list_ports
import json
import time
from datetime import datetime

_print = print


def print(*args, **kw):
    _print("[%s]" % (datetime.now()), *args, **kw)


BAUD_RATE = 115200
KEY_CHUNK_SIZE = 40


def send_json_to_arduino(comPort):
    keyMapPath = "keyMap.json"
    keyCodePath = "keyCode.json"
    # Open serial port
    ser = serial.Serial(comPort, BAUD_RATE, timeout=1)
    time.sleep(2)  # Wait for the Arduino to reset after establishing serial connection

    # Open the JSON file and load its contents
    with open(keyMapPath, "r") as file:
        keyMapJson = json.load(file)

    with open(keyCodePath, "r") as file:
        keyCodeJson = json.load(file)

    ser.write(f"Size: {KEY_CHUNK_SIZE}\n".encode())

    for i in range(1, 9):

        memAddr = (i - 1) * (KEY_CHUNK_SIZE) * 10
        ser.write(f"Addr: {memAddr}\n".encode())

        modeIdx = format(i, "04b")
        # print(modeIdx)
        keyMap = keyMapJson["mode-" + modeIdx]

        for keyIdx in range(10):
            # keyMode = "1" if keyMap[str(keyIdx)]["mode"] else "0"
            keyMode = str(keyMap[str(keyIdx)]["mode"])
            ser.write(keyMode.encode())

            stringArr = ""
            # print(keyMap[str(keyIdx)])
            """
                - define max str length 26 byte
                - define list length 3  byte
                - one byte for mode
                => 30 bytes for a key in a mode
            """

            packet = bytearray()
            for keyBind in keyMap[str(keyIdx)]["list"]:
                # here assign keyBinds, need to map with actural key code
                if keyBind in keyCodeJson:
                    print("Found special key -> ", end="")
                    print(keyBind)
                    # safeKeybind = ''
                    packet.append(int(keyCodeJson[keyBind], 16))

                elif keyBind == None:
                    packet.append(ord("\xFF"))
                else:
                    packet.append(ord(keyBind))

            ser.write(packet)

            stringArr = keyMap[str(keyIdx)]["str"]
            # print(len(stringArr))
            if len(stringArr) > ((KEY_CHUNK_SIZE - 4)):
                stringArr = stringArr[0 : ((KEY_CHUNK_SIZE - 4))]
            elif len(stringArr) < ((KEY_CHUNK_SIZE - 4)):
                stringArr = f"{stringArr:<{(KEY_CHUNK_SIZE - 4)}}"

            ser.write(stringArr.encode())
            ser.write("\n".encode())

        # print(memAddr)
        # print(dataChunk)
        # print('done dataChunk')

        # Convert JSON data to string and send it to Arduino
        # for data in dataChunk:
        # ser.write(f"{data}\n".encode())

    ser.write("done!".encode())
    # Close serial port
    ser.close()
    print("JSON data sent successfully!")


def select_serial_port():
    # Get a list of available serial ports
    ports = serial.tools.list_ports.comports()

    # Print the list of available ports
    print("Available serial ports:")
    for index, port in enumerate(ports, start=1):
        print(f"{index}. {port.device}")

    # Prompt the user to select a port
    while True:
        try:
            selection = int(
                input("Enter the number corresponding to the desired port: ")
            )
            if 1 <= selection <= len(ports):
                selected_port = ports[selection - 1].device
                print(f"Selected port: {selected_port}")
                return selected_port
            else:
                print("Invalid selection. Please enter a valid number.")
        except ValueError:
            print("Invalid input. Please enter a number.")


if __name__ == "__main__":
    selected_port = select_serial_port()

    # Open the serial port
    ser = serial.Serial(selected_port, BAUD_RATE)

    try:
        # Loop to continuously send data to Arduino and receive response
        while True:
            # Get user input from the Python terminal
            # data_to_send = input("Enter data to send to Arduino: ")
            # Send data to Arduino
            # ser.write(data_to_send.encode())

            # try:
            # c = inputimeout(prompt='hello\n', timeout=0.5)
            # except TimeoutOccurred:
            #     c = 'timeout

            # print(c)

            # Read and print response from Arduino
            response = ser.readline().decode().strip()
            if response:
                print(response)
            if response == "Begin load json from PC...":
                send_json_to_arduino(selected_port)

            """
            Addr: 0
            Addr: 300
            Addr: 600
            Addr: 900
            Addr: 1200
            Addr: 1500
            Addr: 1800
            Addr: 2100
            """

    except KeyboardInterrupt:
        # Close the serial port when the user interrupts the program
        ser.close()
        print("\nSerial port closed.")
