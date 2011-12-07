#!/usr/bin/env python

import serial
import sys
import time

if (len(sys.argv) <= 1):
	print "Usage: deploy filename.s19"
	exit()


ser = serial.Serial('/dev/ttyUSB0', 57600)

f = open(sys.argv[1], 'r')
ser.write('l');
ser.write(f.read());
ser.write('\n');
time.sleep(0.5);
ser.write(chr(16));
time.sleep(0.1);
ser.write("1000");
ser.write("g");
ser.close()

