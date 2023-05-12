#!/usr/bin/env python3

import serial

ser = serial.Serial(
        port='COM21',
        baudrate=9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)

ser.flush()

def readFile(uuid):
    print(f"[py:log] Checking for UUID: |{uuid}|")
    with open("./tags.txt", "r") as f:
        for line in f.readlines():
            if line.rstrip().lstrip() == uuid:
                return True
        
    return False
            
    


if __name__ == '__main__':
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)
            if line[0:14] == "[check-access]":
                print("Checking Access")
                isAllowed = readFile(line[14:].rstrip().lstrip())
                response = "granted" if isAllowed else "denied"
                ser.write(response.encode())