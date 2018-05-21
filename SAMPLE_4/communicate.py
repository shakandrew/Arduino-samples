from serial import Serial
import threading
import time
import sys


def listen(interval):
    while ser.isOpen():
        msg = ser.readline().decode("utf-8", "ignore")
        msg = msg.replace("\n", "")
        msg = msg.replace("\r", "")
        if msg is not "":
            print(msg)

ser = Serial(sys.argv[1], int(sys.argv[2]))
running = True
thread1 = threading.Thread(target=listen, args=(15,))
thread1.start()

time.sleep(4)

curr_time ="T"+str( int( time.time() ) )
ser.write(curr_time.encode('ascii'))
print(curr_time)

while True:
    msg = input()
    # if msg == "exit":
    #     print(msg.encode('ascii'))
    #     thread1.join()
    if msg != "":
        ser.write(msg.encode('ascii'))
