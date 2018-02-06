# -*- coding: utf-8 -*-
import paho.mqtt.client as mqtt
import sys
import os

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf
from datetime import datetime
import time

class pfc_log_publisher:
	log_path = '/var/og/logs/log.log'
	log_path = '/Users/house/Desktop/pfc_milesstone.pdf'
	publish_topic= 'ezfarm/pfc/baselog'
	def __init__(self):
		self.client = mqtt.Client()
		self.client.on_publish = self.on_publish
	def on_publish(self,client,obj,mid):
		print("mid : " + str(mid))
		print("ojb : " + str(obj))
		print("client : " + str(client))

	def publish_log(self):
		pfc_conf.PFC_BROKER_HOST = 'iot.eclipse.org'
		self.client.connect(pfc_conf.PFC_BROKER_HOST, pfc_conf.PFC_BROKER_PORT, pfc_conf.PFC_BROKER_KEEPALIVE)
		f = open(self.log_path)
		log_f = f.read()
		log_bytearray = bytearray(log_f)
		f.close()
		self.client.publish(self.publish_topic,payload=log_bytearray,qos=2,retain=False)
		# self.client.publish(self.publish_topic,"hello from the mac hahahaha")
		print("Send " + str(self.log_path) + "/" + str(datetime.now()))
		# self.client.disconnect()


if __name__ == '__main__':
	pfc_log_publisher = pfc_log_publisher()
	while True:
		pfc_log_publisher.publish_log()
		time.sleep(10)
