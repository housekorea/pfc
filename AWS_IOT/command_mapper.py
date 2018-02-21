# -*- coding: utf-8 -*-
#!/bin/bash
import sys
import os

class command_mapper:
	SENSOR_DIR_PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + '/sensors/'

	ACTUATOR_DIR_PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + '/actuators/'
	AWS_IOT_DIR_PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + '/AWS_IOT/'

	SENSOR = {
		"ALL_SENSOR" : {
			"GET" : "all_sensors.py get"
		},
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
			"GET" : "dht11_temp.py get"
		},
		"DHT11_HUM": {
			"GET" : "dht11_hum.py get"
		},
		"DS18BS20_TEMP":{
			"GET" : "ds18bs20_temp.py get"
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
		"AIR_FAN" : {
			"ON" : "air_fan.py on",
			"OFF" : "air_fan.py off"
		},
		"AIR_FAN_IN" : {
			"ON" : "air_fan_in.py on",
			"OFF" : "air_fan_in.py off"
		},
		"AIR_FAN_OUT" : {
			"ON" : "air_fan_out.py on",
			"OFF" : "air_fan_out.py off"
		},
		"AIR_PUMP" : {
			"ON" : "air_pump.py on",
			"OFF" : "air_pump.py off"
		},
		"LED" : {
			"ON" : "led.py on",
			"OFF" : "led.py off"
		},
		"PH_MINUS_PUMP" : {
			"ON" : "ph_minus_pump.py on",
			"OFF" : "ph_minus_pump.py off"
		},
		"PH_PLUS_PUMP" : {
			"ON" : "ph_plus_pump.py on",
			"OFF" : "ph_plus_pump.py off"
		},
		"SOLUTION_A_PUMP" : {
			"ON" : "solution_a_pump.py on",
			"OFF" : "solution_a_pump.py off"
		},
		"SOLUTION_B_PUMP" : {
			"ON" : "solution_b_pump.py on",
			"OFF" : "solution_b_pump.py off"
		},
		"VENTIL_FAN" : {
			"ON" : "ventil_fan.py on",
			"OFF" : "ventil_fan.py off"
		},
		"WATER_PUMP" : {
			"ON" : "water_pump.py on",
			"OFF" : "water_pump.py off"
		},
		"HUMIDIFIER" : {
			"ON" : "humidifier.py on",
			"OFF" : "humidifier.py off"
		}

	}
	HEARTBEAT = {
		"HEARTBEAT" : {
			"BEATING" : "publisher_heartbeat.py"
		}
	}
	LOCAL_IP = {
		"LOCAL_IP" : {
			"LOCAL_IP" : "publisher_local_ip.py"
		}
	}
	DATA_LAKE = {
		"S3_UPLOAD" : {
			"UPLOAD" : "aws_s3_img_uploader.py"
		}
	}


