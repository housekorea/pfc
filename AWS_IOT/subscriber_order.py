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

class subscrbier_order:
	iot_mqtt_client = None
	def __init__(self):
		self.iot_mqtt_client = AWSIoTMQTTClient(pfc_conf.PFC_AWS_IOT_CLIENT_ID)
		self.iot_mqtt_client.configureEndpoirnt(pfc_mqtt_topic.AWS_ENDPOINT,8883)
		self.iot_mqtt_client.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)

		self.iot_mqtt_client.configureDrainingFrequency(2)
		self.iot_mqtt_client.configureConnectDisconnectTimeout(10)
		self.iot_mqtt_client.configureMQTTOperationTimeout(5)



	def msg_callback(self, client, userdata, message):
		mes_pld = message.payload
		mes_tpc = message.topic
		f = open('/Users/house/Desktop/cert_pfc_0001/log.log','a+')
		f.write(mes_tpc + ' => ' + mes_pld + str(datetime.now()))
		f.write('\n')
		f.close()



	def subscribe_mqtt_broker(self):
		self.iot_mqtt_client.connect()
		self.iot_mqtt_client.subscribe(pfc_mqtt_topic.SUBSCRIBE_ORDER)
	def logging(self):
		None


if __name__ == '__main__':
	# sys.argv
	iot_subscriber = subscribe_order()
	iot_subscriber.subscribe_mqtt_broker()









