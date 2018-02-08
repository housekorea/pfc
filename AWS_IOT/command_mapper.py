# -*- coding: utf-8 -*-
#!/bin/bash
import sys
import os

class command_mapper:
	SENSOR_DIR_PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + '/sensors/'

	ACTUATOR_DIR_PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + '/actuators/'
	SENSOR = {
		"PH" : {
			"GET" : "ph.py get"
		},
		"EC" : {
			"GET" : "ec.py get"
		},
		"CO2" : {
			"GET" : "co2.py get"
		},
		"DHT11_TEMP": {
			"get" : "dht11_temp.py get"
		},
		"DHT11_HUM": {
			"GET" : "dht11_hum.py get"
		},
		"DS18BS20_TEMP":{
			"GET" : "ds18b20_temp.py get"
		},
		"LDR" : {
			"GET" : "ldr.py get"
		},
		"USB_CAM_TOP" : {
			"CAPTURE" : "usb_cam.py capture"
		},
		"USB_CAM_SIDE" : {
			"CAPTURE" : "usb_cam.py capture"
		}
	}
	ACTUATOR = {
		"air_fan" : {
			"ON" : "air_fan.py on",
			"OFF" : "air_fan.py off"
		},
		"air_fan_in" : {
			"ON" : "air_fan_in.py on",
			"OFF" : "air_fan_in.py off"
		},
		"air_fan_out" : {
			"ON" : "air_fan_out.py on",
			"OFF" : "air_fan_out.py off"
		},
		"air_pump" : {
			"ON" : "air_pump.py on",
			"OFF" : "air_pump.py off"
		},
		"led" : {
			"ON" : "led.py on",
			"OFF" : "led.py off"
		},
		"ph_minus_pump" : {
			"ON" : "ph_minus_pump.py on",
			"OFF" : "ph_minus_pump.py off"
		},
		"ph_plus_pump" : {
			"ON" : "ph_plus_pump.py on",
			"OFF" : "ph_plus_pump.py off"
		},
		"solution_a_pump" : {
			"ON" : "solution_a_pump.py on",
			"OFF" : "solution_a_pump.py off"
		},
		"solution_b_pump" : {
			"ON" : "solution_b_pump.py on",
			"OFF" : "solution_b_pump.py off"
		},
		"ventil_fan" : {
			"ON" : "ventil_fan.py on",
			"OFF" : "ventil_fan.py off"
		},
		"water_pump" : {
			"ON" : "water_pump.py on",
			"OFF" : "water_pump.py off"
		},
		"humidifier" : {
			"ON" : "humidifier.py on",
			"OFF" : "humidifier.py off"
		}

	}
	HEARTBEAT = {
		"heartbeat" : {
			"BEATING" : "publisher_heartbeat.py"
		}
	}
	LOCAL_IP = {
		"LOCAL_IP" : {
			"LOCAL_IP" : "publisher_local_ip.py"
		}
	}



