import serial
import time
import sys
import os
import termios
from datetime import datetime


sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from pfc_connection_arduino import pfc_connection_arduino

class all_sensors:
	SERIAL = None
	def __init__(self):
		self.connect()
	def connect(self):
		cont_ad = pfc_connection_arduino()
		port = cont_ad.get_USB_PORT()
		f = open(port)
		attrs = termios.tcgetattr(f)
		attrs[2] = attrs[2] & ~termios.HUPCL
		termios.tcsetattr(f, termios.TCSAFLUSH, attrs)
		f.close()
		self.SERIAL = serial.Serial(port, cont_ad.get_BAUD_RATE(),timeout=10);

	def getValue(self):
		self.SERIAL.write("get_all_sensors")
		time.sleep(3)
		value = self.SERIAL.readline()
		return value
if __name__ == '__main__':
	all_sensors = all_sensors()
	order = sys.argv[1]

	if len(sys.argv) == 1:
		exit()
	elif len(sys.argv) == 2:
		order = sys.argv[1]
		delay = 0
	elif (len(sys.argv) == 3) and sys.argv[2].isdigit():
		order = sys.argv[1]
		delay = sys.argv[2]

	time.sleep(float(delay))



	if order == 'get':
		v = all_sensors.getValue().rstrip()
		print(v+ "," + str(datetime.now()));
