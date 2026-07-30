# simple uart send/receive app without sophisticated GUI,just to be able to enchance it with some gui one day
# currently based on:
# https://forbot.pl/forum/topic/17581-python-i-komunikacja-uart/

import serial
import select
import sys

uart = serial.Serial("/dev/ttyACM1", 115200, timeout=0.1)


def process_read():
    data = uart.readline()
    if data:
        data = data.decode()
        print(data)


def process_keyboard():
    if select.select([sys.stdin], [], [], 0)[0]:
        line = sys.stdin.readline()
        uart.write(line.encode())


while True:
    process_read()
    process_keyboard()
