# -*- coding: utf-8 -*-
#!/bin/bash
import os
import sys
from datetime import datetime
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import logging
import time
import json
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf
from configure import pfc_mqtt_topic
from command_mapper import command_mapper
import subprocess, shlex
from threading import Timer

# Configure logging
logging.basicConfig(filename=pfc_conf.LOG_DIR_PATH + '/aws_iot_log.log')
logger = logging.getLogger("AWSIoTPythonSDK.core")
logger.setLevel(logging.DEBUG)
streamHandler = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
streamHandler.setFormatter(formatter)
logger.addHandler(streamHandler)


class subscriber_order:
	iot_mqtt_client = None
	QOS_LEVEL = 1
	def __init__(self):
		self.iot_mqtt_client = AWSIoTMQTTClient(pfc_conf.PFC_AWS_IOT_CLIENT_ID)
		self.iot_mqtt_client.configureEndpoint(pfc_mqtt_topic.AWS_ENDPOINT,8883)
		self.iot_mqtt_client.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)
		self.iot_mqtt_client.configureAutoReconnectBackoffTime(1, 32, 20)
		self.iot_mqtt_client.configureOfflinePublishQueueing(-1)
		self.iot_mqtt_client.configureDrainingFrequency(2)
		self.iot_mqtt_client.configureConnectDisconnectTimeout(10)
		self.iot_mqtt_client.configureMQTTOperationTimeout(5)



	def msg_callback(self, client, userdata, message):
		mes_pld = message.payload
		mes_tpc = message.topic

		print("[Get the message] " + str(datetime.now()) + ' / ' + str(mes_tpc))
		f = open(pfc_conf.LOG_DIR_PATH + '/aws_subscribe.log','a+')
		f.write(mes_tpc + ' => ' + mes_pld + str(datetime.now()))
		f.write('\n')
		f.close()
		try :
			messageJson = json.loads(message.payload)
		except :
			print("Throw Message JSON Parse Error.")
			return False

		om_type = messageJson['TYPE']
		om_target = messageJson['TARGET']if 'TARGET' in messageJson else None
		om_order = messageJson['ORDER'] if 'ORDER' in messageJson else None
		self.order_callback(om_type, om_target,om_order)



	def order_callback(self, om_type, om_target, om_order):
		global Timer

		kill_proc = lambda p: p.kill()

		if om_type == 'SENSOR':
			if om_target in command_mapper.SENSOR and om_order in command_mapper.SENSOR[om_target]:
				command_pfc_sensor = command_mapper.SENSOR_DIR_PATH +command_mapper.SENSOR[om_target][om_order]

				print(command_pfc_sensor)
				# Execute get sensor data python process through subprocess
				# It has a timeout setting to prevent permanent blocking
				sensor_proc = subprocess.Popen(shlex.split("python " + command_pfc_sensor), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
				timer = Timer(30, kill_proc,[sensor_proc])
				try :
					timer.start()
					stdout,stderr = sensor_proc.communicate()
				except :
					f = open(pfc_conf.LOG_DIR_PATH + '/aws_subscribe.log','a+')
					f.write("[Sensor-Data-PROCESS] Exception Occured")
					f.write('\n')
					f.close()
				finally:
					timer.cancel()

				# Publish sensor data to AWS IOT DEVICE GATEWAY
				sensor_data = {"DATA" :stdout , "PFC_SERIAL" :str(pfc_conf.PFC_AWS_IOT_SERIAL), "DEVICE_DT" : str(datetime.now())}
				pub_proc = subprocess.Popen(shlex.split("python publisher_sensor_data.py -t '" + pfc_mqtt_topic.PUBLISH_SENSOR+ "' -m '" +json.dumps(sensor_data) + "'"))
				timer = Timer(30,kill_proc, [pub_proc])
				try :
					timer.start()
					stdout,stderr = pub_proc.communicate()
				except :
					f = open(pfc_conf.LOG_DIR_PATH + '/aws_subscribe.log','a+')
					f.write("[Sensor-Data-PROCESS] Exception Occured")
					f.write('\n')
					f.close()

				finally :
					timer.cancel()
			else :
				print("'TARGET' or 'ORDER' is not exists on the command_mapper")
		elif om_type == 'ACTUATOR':
			if om_target in command_mapper.ACTUATOR and om_order in command_mapper.ACTUATOR[om_target]:
				command_pfc_actuator = command_mapper.ACTUATOR_DIR_PATH + command_mapper.ACTUATOR[om_target][om_order]

				print(command_pfc_actuator)
				# Execute get sensor data python process through subprocess
				# It has a timeout setting to prevent permanent blocking
				actuator_proc = subprocess.Popen(shlex.split("python " + command_pfc_actuator), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
				timer = Timer(30, kill_proc,[actuator_proc])
				try :
					timer.start()
					stdout, stderr = actuator_proc.communicate()
				except :
					f = open(pfc_conf.LOG_DIR_PATH + '/aws_subscribe.log','a+')
					f.write("[Actuator-PROCESS] Exception Occured")
					f.write('\n')
					f.close()
				finally :
					timer.cancel()

				actuator_data = {'DATA':stdout, 'PFC_SERIAL': str(pfc_conf.PFC_AWS_IOT_SERIAL), 'DEVICE_DT' : str(datetime.now())}
				pub_proc = subprocess.Popen(shlex.split("python publisher_actuator_data.py -t '"+pfc_mqtt_topic.PUBLISH_ACTUATOR+"' -m '" +json.dumps(actuator_data) + "'"))
				timer = Timer(30,kill_proc, [pub_proc])
				try :
					timer.start()
					stdout,stderr = pub_proc.communicate()
				except :
					f = open(pfc_conf.LOG_DIR_PATH + '/aws_subscribe.log','a+')
					f.write("[Actuator-Data-Process] Exception Occured")
					f.write('\n')
					f.close()

				finally :
					timer.cancel()
			else :
				print("'TARGET' or 'ORDER' is not exists on the command_mapper")
		elif om_type == 'LOCAL_IP' :
			pub_proc = subprocess.Popen(shlex.split("python " + 					command_mapper.LOCAL_IP['LOCAL_IP']['LOCAL_IP']))
			timer = Timer(30,kill_proc, [pub_proc])
			try :
				timer.start()
				stdout,stderr = pub_proc.communicate()
			finally :
				timer.cancel()

		elif om_type == 'HEARTBEAT' :
			pub_proc = subprocess.Popen(shlex.split("python " + command_mapper.LOCAL_IP['HEARTBEAT']['BEATING']))
			timer = Timer(30, kill_proc, [pub_proc])
			try :
				timer.start()
				stdout, stderr = pub_proc.communicate()
			finally :
				timer.cancel()
		elif om_type == 'DATA_LAKE' :
			command_pfc_data_lake = command_mapper.AWS_IOT_DIR_PATH +command_mapper.DATA_LAKE['S3_UPLOAD']['UPLOAD']
			pub_proc = subprocess.Popen(shlex.split("python " + command_pfc_data_lake))
			timer = Timer(600, kill_proc, [pub_proc])
			try :
				timer.start()
				stdout, stderr = pub_proc.communicate()
			finally :
				timer.cancel()


			datalake_data = {'DATA' : stdout, 'PFC_SERIAL' : str(pfc_conf.PFC_AWS_IOT_SERIAL), 'DEVICE_DT' : str(datetime.now())}
			pub_proc = subprocess.Popen(shlex.split("python publisher_datalake_data.py -t '" + pfc_mqtt_topic.PUBLISH_DATALAKE + "' -m '" + json.dumps(datalake_data) + "'"))
			timer = Timer(30, kill_proc, [pub_proc])
			try :
				timer.start()
				stdout,stderr = pub_proc.communicate()
			finally :
				timer.cancel()

	def subscribe_mqtt_broker(self):
		self.iot_mqtt_client.connect()
		self.iot_mqtt_client.subscribe(pfc_mqtt_topic.SUBSCRIBE_ORDER,self.QOS_LEVEL, self.msg_callback)
		print("Subscribing topic : " + str(pfc_mqtt_topic.SUBSCRIBE_ORDER))
		while True:
			time.sleep(1)

	def logging(self):
		None


if __name__ == '__main__':
	# sys.argv
	iot_subscriber = subscriber_order()
	iot_subscriber.subscribe_mqtt_broker()
	# iot_subscriber.order_callback('SENSOR','CO2','GET')







