# -*- coding: utf-8 -*-
#!/bin/bash
import os
import sys
from datetime import datetime
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import time
import json
import argparse
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf
from configure import pfc_mqtt_topic

class publisher_datalake_data:
	IOT_MQTT_CLIENT = None
	QOS_LEVEL = 1
	def __init__(self, QOS_LEVEL = 1):
		self.QOS_LEVEL = QOS_LEVEL
		self.IOT_MQTT_CLIENT = AWSIoTMQTTClient(pfc_conf.PFC_AWS_IOT_CLIENT_ID)
		self.IOT_MQTT_CLIENT.configureEndpoint(pfc_mqtt_topic.AWS_ENDPOINT,8883)
		self.IOT_MQTT_CLIENT.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)
		self.IOT_MQTT_CLIENT.configureAutoReconnectBackoffTime(1, 32, 20)
		self.IOT_MQTT_CLIENT.configureOfflinePublishQueueing(-1)
		self.IOT_MQTT_CLIENT.configureDrainingFrequency(2)
		self.IOT_MQTT_CLIENT.configureConnectDisconnectTimeout(10)
		self.IOT_MQTT_CLIENT.configureMQTTOperationTimeout(20)


	def publish_mqtt_broker(self, totpic, messageJson):
		if messageJson == None:
			print("Message is none.")
			sys.exit()
		elif "PFC_SERIAL" not in messageJson or "DEVICE_DT" not in messageJons:
			print("PFC_SERIAL, DEVICE_DT is a demandable.")
			sys.exit()

		mqtt_topic = pfc_mqtt_topic.PUBLISH_DATALAKE

		self.IOT_MQTT_CLIENT.connect()
		self.IOT_MQTT_CLIENT.publish(mqtt_topic, messageJson, self.QOS_LEVEL)
		self.IOT_MQTT_CLIENT.disconnect()
		print("Published MQTT topic : " + str(topic))


if __name__ == '__main__':
	parser = argparse.AgrumentParser()
	parser.add_argument("-t", "--topic", action="store", required=True, dest="topic", help="MQTT message `Topic` name")
	parser.add_argument("-m", "--message", action="store", required=True, dest="message", help="MQTT message data")
	parser.add_argument("-q", "--qos_level", action="store", dest="qos_level", help="MQTT QOS_LEVEL", default = 1)

	args = parser.parse_args()
	topic = args.topic
	qos_level = args.qos_level
	messageJson = args.message

	if "PFC_SERIAL" not in messageJson or "DEVICE_DT" not in messageJson:
		print ("PFC_SERIAL, DEVICE_DT is a demandable")
		sys.exit()

	publisehr_dl = publisher_sensor_data(QOS_LEVEL = qos_level)
	publisher_dl.publish_mqtt_broker(topic, messageJson)






















