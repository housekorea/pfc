import serial
import time
import sys
import os
import termios
from datetime import datetime

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from pfc_connection_arduino import pfc_connection_arduino

class serial_reset:
	SERIAL = None

	def __init__(self):
		self.connect()

	def connect(self):
		cont_ad = pfc_connection_arduino()
		port = cont_ad.get_USB_PORT()
		BRATE = cont_ad.get_BAUD_RATE()
		ser = serial.Serial(port,BRATE)
		# if ser.isOpen():
			# print("opend")
		# ser.close()
		while ser.isOpen():
			print("opened")
			print("IS CLOSED : " + str(ser.isOpen()))
			ser.close()
			time.sleep(1)


serial_reset = serial_reset()
# serial_reset.connect()
