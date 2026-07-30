# simple uart send/receive app without sophisticated GUI,just to be able to enchance it with some gui one day
# currently based on:
# https://forbot.pl/forum/topic/17581-python-i-komunikacja-uart/

import serial
import select
import sys

stm32Uart2 = serial.Serial("/dev/ttyACM1", 115200, timeout=0.1)

while True:
    # Read uart
    data = stm32Uart2.readline()
    if data:
        data = data.decode()
        print(data)

    # Read keyboard without blocking
    if select.select([sys.stdin], [], [], 0)[0]:
        line = sys.stdin.readline()

        if line.strip() == "exit":
            break

        stm32Uart2.write(line.encode())
