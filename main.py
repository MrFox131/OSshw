#!/usr/sbin/python3
import serial
from time import time, sleep
import datetime
from dateutil import tz
from random import randint
from sys import argv

serial_path = ''

if len(argv) != 3:
    print("Please provide exactly two arguments, path to serial port and baudrate")
    exit(0)

serial_path = argv[1]
baud = int(argv[2])

ser = serial.Serial(serial_path, baud)

print(datetime.datetime(2023, 12, 5, 10, 0, 0, 0, tz.gettz('UTC')).timestamp())

temperature_pairs = []

for i in range(5, 20, 1):
    for j in range(0, 23):
        temperature_pairs += [(int(datetime.datetime(2023, 12, i, j, k, 0, 0, tz.gettz('UTC')).timestamp()), randint(20,30)) for k in range(0, 59)]

print(temperature_pairs)

for pair in temperature_pairs:
    ser.write(f"{pair[1]} {pair[0]}\n".encode('utf-8'))
    print("written")
