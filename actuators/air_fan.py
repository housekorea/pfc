# -*- coding: utf-8 -*-
#!/bin/bash
import serial
import time
import smbus
import sys
import os
import termios
from datetime import datetime
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from pfc_connection_arduino import pfc_connection_arduino





class air_fan:
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
			if act_key[0] == "air_fan":
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



if __name__ == '__main__':
	air_fan = air_fan()

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
		value = air_fan.on()
	elif order =='off':
		value = air_fan.off()
	else :
		print("It is not correct arugments")
		sys.exit()
	print(value.strip() + "/dt=" + str(datetime.now()))


