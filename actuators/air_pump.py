# -*- coding: utf-8 -*-
#!/bin/bash
import serial
import smbus
import time
import sys
import os
import termios
from datetime import datetime

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from pfc_connection_arduino import pfc_connection_arduino




class air_pump:
	SLAVE_ADDRESS = pfc_connection_arduino.SLAVE_ADDRESS
	I2C_BUS = None
	ACT_TMPL = pfc_connection_arduino.ACTUATOR_MES_TMPL
	ACT_INDICATOR = None
	MESS = None
	def __init__(self):
		self.I2C_BUS = smbus.SMBus(1)
		self.connect()
	def connect(self):


		for (act_idx,act_key) in enumerate(self.ACT_TMPL):
			if act_key[0] == "air_pump":
				self.ACT_INDICATOR = act_idx
				break;
		self.MESS = [0]*(len(self.ACT_TMPL))


	def on(self):
		self.MESS[self.ACT_INDICATOR] = 1
		self.I2C_BUS.write_i2c_block_data(self.SLAVE_ADDRESS, int(self.ACT_INDICATOR), self.MESS)
		time.sleep(1)


	def off(self):
		self.MESS[self.ACT_INDICATOR] = 0
		self.I2C_BUS.write_i2c_block_data(self.SLAVE_ADDRESS, int(self.ACT_INDICATOR), self.MESS)
		time.sleep(1)
		# receive_data = self.I2C_BUS.read_i2c_block_data(self.SLAVE_ADDRESS,0)
		# return receive_data


























# class air_pump:
# 	SERIAL = None
# 	def __init__(self):
# 		self.connect()
# 	def connect(self):
# 		cont_ad = pfc_connection_arduino()
# 		port = cont_ad.get_USB_PORT()
# 		f = open(port)
# 		attrs = termios.tcgetattr(f)
# 		attrs[2] = attrs[2] & ~termios.HUPCL
# 		termios.tcsetattr(f, termios.TCSAFLUSH, attrs)
# 		f.close()
# 		self.SERIAL = serial.Serial(port, cont_ad.get_BAUD_RATE(),timeout=60)

# 	def on(self):
# 		time.sleep(0.5)
# 		self.SERIAL.write('on_air_pump')
# 		time.sleep(0.5)
# 		value = self.SERIAL.readline()
# 		return value
# 	def off(self):
# 		time.sleep(0.5)
# 		self.SERIAL.write('off_air_pump')
# 		time.sleep(0.5)
# 		value = self.SERIAL.readline()
# 		return value


if __name__ == '__main__':
	air_pump = air_pump()
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
		value = air_pump.on()
	elif order == 'off':
		value = air_pump.off()
	else :
		print("It is not correct arugments")
		sys.exit()
	print(value.strip() + "/dt=" + str(datetime.now()))

