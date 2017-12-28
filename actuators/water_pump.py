import serial
import time
import sys
import os
import termios

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from pfc_connection_arduino import pfc_connection_arduino

class water_pump:
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
		self.SERIAL = serial.Serial(port, cont_ad.get_BAUD_RATE());

	def on(self):
		time.sleep(0.7)
		self.SERIAL.write("on_water_pump")
	def off(self):
		time.sleep(0.7)
		self.SERIAL.write("off_water_pump")


if __name__ == '__main__':
	water_pump = water_pump()

	if len(sys.argv) == 1:
		exit()
	elif len(sys.argv) == 2:
		order = sys.argv[1]
		delay = 0
	elif (len(sys.argv) == 3 ) and sys.argv[2].isdigit():
		order = sys.argv[1]
		delay = sys.argv[2]

	time.sleep(float(delay))

	if order == 'on':
		water_pump.on()
	elif order == 'off':
		water_pump.off()
