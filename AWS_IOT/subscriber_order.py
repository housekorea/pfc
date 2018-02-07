# -*- coding: utf-8 -*-
#!/bin/bash
import os
import sys
from datetime import datetime
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import time
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf
from configure import pfc_mqtt_topic

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



	def subscribe_mqtt_broker(self):
		self.iot_mqtt_client.connect()
		self.iot_mqtt_client.subscribe(pfc_mqtt_topic.SUBSCRIBE_ORDER,self.QOS_LEVEL, self.msg_callback)
		print("Subscribing topic : " + str(pfc_mqtt_topic.SUBSCRIBE_ORDER))

		time.sleep(5)
	def logging(self):
		None


if __name__ == '__main__':
	# sys.argv
	iot_subscriber = subscriber_order()
	iot_subscriber.subscribe_mqtt_broker()








