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
	publish_topic= 'ezfarm/pfc/base_log'
	def __init__(self):
		self.client = mqtt.Client()


	def publish_log(self):
		self.client.connect(pfc_conf.PFC_BROKER_HOST, pfc_conf.PFC_BROKER_PORT, pfc_conf.PFC_BROKER_KEEPALIVE)
		f = open(self.log_path)
		log_f = f.read()
		log_bytearray = bytearray(log_f)
		self.client.publish(self.publish_topic,payload=log_bytearray)
		print("Send " + str(self.log_path) + "/" + str(datetime.now()))
		self.client.disconnect()


if __name__ == '__main__':
	pfc_log_publisher = pfc_log_publisher()
	pfc_log_publisher.publish_log()
