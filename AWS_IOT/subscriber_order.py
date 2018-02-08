# -*- coding: utf-8 -*-
#!/bin/bash
import os
import sys
from datetime import datetime
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import time
import json
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf
from configure import pfc_mqtt_topic
from command_mapper import command_mapper
import subprocess
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
		om_target = messageJson['TARGET']
		om_order = messageJson['ORDER']
		self.order_callback(self, om_type, om_order)


	def order_callback(self, om_type, om_target, om_order):
		om_type = 'SENSOR'
		om_target = 'CO2'
		om_order = 'GET'

		if om_type == 'SENSOR':
			if om_target in command_mapper.SENSOR and om_order in command_mapper.SENSOR[om_target]:
				command_pfc_sensor = command_mapper.SENSOR_DIR_PATH +command_mapper.SENSOR[om_target][om_order]
				SENSOR_DATA = subprocess.check_output(["python", command_pfc_sensor])
				# subprocess.check_output("python " + command_pfc_sensor))

		elif om_type == 'ACTUATOR':
			None
		elif om_type == 'LOCAL_IP' :
			None
		elif om_type == 'HEARTBEAT' :
			None













	def subscribe_mqtt_broker(self):
		self.iot_mqtt_client.connect()
		self.iot_mqtt_client.subscribe(pfc_mqtt_topic.SUBSCRIBE_ORDER,self.QOS_LEVEL, self.msg_callback)
		print("Subscribing topic : " + str(pfc_mqtt_topic.SUBSCRIBE_ORDER))
		while True:
			time.sleep(5)

	def logging(self):
		None


if __name__ == '__main__':
	# sys.argv
	iot_subscriber = subscriber_order()
	# iot_subscriber.subscribe_mqtt_broker()

	om_type = 'SENSOR'
	om_target = 'CO2'
	om_order = 'GET'
	iot_subscriber.order_callback(om_type,om_target,om_order)







