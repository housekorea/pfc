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





class ph_minus_pump:
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
			if act_key[0] == "ph_minus_pump":
				self.ACT_INDICATOR = act_idx
				break;
		self.MESS = [0]*(len(self.ACT_TMPL))


	def on(self):
		self.MESS[self.ACT_INDICATOR] = 1
		self.I2C_BUS.write_i2c_block_data(self.SLAVE_ADDRESS, int(self.ACT_INDICATOR), self.MESS)
		time.sleep(0.5)
		receive_data = self.I2C_BUS.read_i2c_block_data(self.SLAVE_ADDRESS,0)
		res = "";
		for v in receive_data:
			if v == 255:
				break;
			else:
				res = v
		return res


	def off(self):
		self.MESS[self.ACT_INDICATOR] = 0
		self.I2C_BUS.write_i2c_block_data(self.SLAVE_ADDRESS, int(self.ACT_INDICATOR), self.MESS)
		time.sleep(0.5)
		receive_data = self.I2C_BUS.read_i2c_block_data(self.SLAVE_ADDRESS,0)
		res = "";
		for v in receive_data:
			if v == 255:
				break;
			else:
				res = v
		return res


if __name__ == '__main__':
	ph_minus_pump = ph_minus_pump()

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
		value = ph_minus_pump.on()
		time.sleep(3.5)
		value_off = ph_minus_pump.off()
	elif order == 'off':
		value = ph_minus_pump.off()
	else :
		print("It is not correct arugments")
		sys.exit()

	print("result=" + str(value) + "/dt=" + str(datetime.now()))


