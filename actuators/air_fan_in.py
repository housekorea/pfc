#!/bin/bash
import serial
import time
import sys
import os
import termios

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from pfc_connection_arduino import pfc_connection_arduino


class air_fan_in:
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
		self.SERIAL = serial.Serial(port, cont_ad.get_BAUD_RATE(),timeout=60)
	def on(self):
		time.sleep(0.5)
		self.SERIAL.write("on_air_fan_in")
		time.sleep(0.5)
		value = self.SERIAL.readline()
		return value
	def off(self):
		time.sleep(0.5)
		self.SERIAL.write("off_air_fan_in")
		time.sleep(0.5)
		value = self.SERIAL.readline()
		return value

if __name__ == '__main__':
	air_fan_in = air_fan_in()

	if len(sys.argv) == 1:
		exit()
	elif len(sys.argv) == 2:
		order = sys.argv[1]
		delay = 0
	elif (len(sys.argv) == 3) and sys.argv[2].isdigit():
		order = sys.argv[1]
		delay = sys.argv[2]

	time.sleep(float(delay))

	if order == 'on':
		value = air_fan_in.on()
	elif order =='off':
		value = air_fan_in.off()
	else :
		print("It is not correct arugments")
		sys.exit()
	print(value.strip())

