# -*- coding: utf-8 -*-
#!/bin/bash
import os
import sys
from datetime import datetime
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import time
import json
import socket
import fcntl
import struct

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf
from configure import pfc_mqtt_topic

class publisher_local_ip:
	iot_mqtt_client = None
	QOS_LEVEL = 1
	DEVICE_IP_ADDR = None

	def __init__(self):
		self.iot_mqtt_client = AWSIoTMQTTClient(pfc_conf.PFC_AWS_IOT_CLIENT_ID)
		self.iot_mqtt_client.configureEndpoint(pfc_mqtt_topic.AWS_ENDPOINT,8883)
		self.iot_mqtt_client.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)
		self.iot_mqtt_client.configureAutoReconnectBackoffTime(1, 32, 20)
		self.iot_mqtt_client.configureOfflinePublishQueueing(-1)
		self.iot_mqtt_client.configureDrainingFrequency(2)
		self.iot_mqtt_client.configureConnectDisconnectTimeout(10)
		self.iot_mqtt_client.configureMQTTOperationTimeout(20)


	def publish_mqtt_broker(self,messageJson,interval_sec=0):
		self.iot_mqtt_client.connect()
		if messageJson == None:
			print("Message Json not passed.")
			sys.exit()

		self.iot_mqtt_client.publish(pfc_mqtt_topic.PUBLISH_LOCAL_IP, messageJson, self.QOS_LEVEL)
		self.iot_mqtt_client.disconnect()


	def get_ip_addr(self,network):
		s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.DEVICE_IP_ADDR = socket.inet_ntoa(fcntl.ioctl(
				s.fileno(),
				0x8915,
				struct.pack('256s', network[:15])
			)[20:24])
		return self.DEVICE_IP_ADDR



if __name__ == '__main__':
	#sys.argv
	publisher_ip = publisher_local_ip()
	ip_addr = publisher_ip.get_ip_addr('wlan0')
	message = {
		'PFC_SERIAL' : str(pfc_conf.PFC_AWS_IOT_SERIAL),
		'IP_ADDR' : str(ip_addr),
		'send_dt' : str(datetime.now())
	}
	messageJson = json.dumps(message)
	publisher_ip.publish_mqtt_broker(messageJson)
	print "Published Message Topic : " + pfc_mqtt_topic.PUBLISH_LOCAL_IP + "_" + str(datetime.now());
	print "\n"

