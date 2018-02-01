# -*- coding: utf-8 -*-
import paho.mqtt.client as mqtt
import sys
import os
import socket
import fcntl
import struct
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf
from datetime import datetime
import time

class ip_notifier:
	client = None
	publish_topic = 'ezfarm/pfc/ip_notifier'
	pfc_alias = pfc_conf.PFC_ALIAS
	ip_addr = None
	def __init__(self):
		self.client = mqtt.Client()


	def get_ip_addr(self,network):
		s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.ip_addr = socket.inet_ntoa(fcntl.ioctl(
				s.fileno(),
				0x8915,
				struct.pack('256s', network[:15])
			)[20:24])

	def publish_ip_addr(self):
		self.get_ip_addr('wlan0')
		self.client.connect(pfc_conf.PFC_BROKER_HOST, pfc_conf.PFC_BROKER_PORT,pfc_conf.PFC_BROKER_KEEPALIVE)
		self.client.publish(self.publish_topic, self.ip_addr + "/" + self.pfc_alias)
		print("Send to IP address of this PFC/" + str(datetime.now()))
		self.client.disconnect()



if __name__ == '__main__':
	notifier = ip_notifier()
	notifier.publish_ip_addr()





