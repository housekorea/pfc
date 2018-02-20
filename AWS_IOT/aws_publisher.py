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

class aws_publisher:
	IOT_MQTT_CLIENT = None
	QOS_LEVEL = 1
	def __init__(self,QOS_LEVEL=1):
		self.QOS_LEVEL = QOS_LEVEL
		self.IOT_MQTT_CLIENT = AWSIoTMQTTClient(pfc_conf.PFC_AWS_IOT_CLIENT_ID)
		self.IOT_MQTT_CLIENT.configureEndpoint(pfc_mqtt_topic.AWS_ENDPOINT,8883)
		self.IOT_MQTT_CLIENT.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)
		self.IOT_MQTT_CLIENT.configureAutoReconnectBackoffTime(1, 32, 20)
		self.IOT_MQTT_CLIENT.configureOfflinePublishQueueing(-1)
		self.IOT_MQTT_CLIENT.configureDrainingFrequency(2)
		self.IOT_MQTT_CLIENT.configureConnectDisconnectTimeout(10)
		self.IOT_MQTT_CLIENT.configureMQTTOperationTimeout(20)

	def publish_mqtt_broker(self,topic,messageJson):
		if messageJson == None:
			print("message is none.")
			sys.exit()
		elif "PFC_SERIAL" not in messageJson or "DEVICE_DT" not in messageJson:
			print("PFC_SERIAL, DEVICE_DT is a demandable.")
			sys.exit()

		self.IOT_MQTT_CLIENT.connect()
		self.IOT_MQTT_CLIENT.publish(topic, messageJson, self.QOS_LEVEL)
		self.IOT_MQTT_CLIENT.disconnect()
		print("Pbulished MQTT topic: " + str(topic))

if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument("-t","--topic",action="store",required=True,dest="topic", help="MQTT message `Topic` name")
	parser.add_argument("-m","--message",action="store",required=True,dest="message", help="MQTT message data")
	parser.add_argument("-q","--qos_level",action="store",dest="qos_level", help="MQTT QOS_LEVEL", default=1)

	qos_level = 1
	topic = 'EZFARM/PFC/V1/DEV/00000001/order_subscribe'
	message = {
  "PFC_SERIAL": "00000000",
  "DEVICE_DT" : "",
  "ORDER" : "ON",
  "TARGET" : "VENTIL_FAN",
  "TYPE" : "ACTUATOR",
  "ORDER_DT" : ""
	}


	message['ORDER_DT'] = str(datetime.now())
	messageJson = json.dumps(message)


	publisher_aws = aws_publisher(QOS_LEVEL = qos_level)
	publisher_aws.publish_mqtt_broker(topic,messageJson)



