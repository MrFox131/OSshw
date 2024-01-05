#!/usr/sbin/python3
import serial
from time import time, sleep
from random import randint
from sys import argv

serial_path = ''

if len(argv) != 3:
    print("Please provide exactly two arguments, path to serial port and baudrate")
    exit(0)

serial_path = argv[1]
baud = int(argv[2])

ser = serial.Serial(serial_path, baud)

while True:
    ser.write(f"{randint(20,30)} {int(time())}\n".encode('utf-8'))
    sleep(1)
