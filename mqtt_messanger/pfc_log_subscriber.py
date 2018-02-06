# -*- coding: utf-8 -*-
import paho.mqtt.client as mqtt
import sys
import os
from datetime import datetime
import time
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf


class pfc_log_subscriber:
	client = None
	subscribe_topic = 'ezfarm/pfc/baselog'

	def __init__(self):
		self.client = mqtt.Client()
		self.client.on_connect = self.on_connect
		self.client.on_message = self.on_message

	def on_connect(self,client, userdata, flags, rc):
		print("Broker Connect Status : " + str(rc))
		client.subscribe(self.subscribe_topic)

	def on_message(self,client,userdata,msg):
		print("Topic : " , msg.topic + " / Message received" + str(datetime.now()))
		print(len(msg.payload))
		f = open('/Users/house/Desktop/copy/temp_copy_'+str(time.time())+'.pdf','wb')
		f.write(msg.payload)
		f.close()

	def client_looping(self):
		pfc_conf.PFC_BROKER_HOST = 'iot.eclipse.org'
		# pfc_conf.PFC_BROKER_HOST = 'localhost'
		self.client.connect(pfc_conf.PFC_BROKER_HOST, pfc_conf.PFC_BROKER_PORT, pfc_conf.PFC_BROKER_KEEPALIVE)
		self.client.loop_forever()




if __name__ == '__main__':
	subscriber = pfc_log_subscriber()
	subscriber.client_looping()
