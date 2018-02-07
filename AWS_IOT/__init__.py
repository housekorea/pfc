# -*- coding: utf-8 -*-
import os
import sys
from datetime import datetime
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient

CA_PATH = "/Users/house/Desktop/cert_pfc_0001/VeriSign-Class 3-Public-Primary-Certification-Authority-G5.pem"
PRIVATE_KEY_PATH ="/Users/house/Desktop/cert_pfc_0001/83f2f5c142-private.pem.key"
CERTIFICATE_PATH ="/Users/house/Desktop/cert_pfc_0001/83f2f5c142-certificate.pem.crt"

def test_cb(self, params, packet):
	print(packet.payload)
	f = open('/Users/house/Desktop/copy/temp_copy_'+str(time.time()) + '.log','w')
	f.write(packet.payload)
	f.close()

myMQTTClient = AWSIoTMQTTClient("myClientID")

myMQTTClient.configureEndpoint("a1wxijaxbxg469.iot.ap-northeast-2.amazonaws.com",8883)
myMQTTClient.configureCredentials(CA_PATH,PRIVATE_KEY_PATH,CERTIFICATE_PATH)




# AWSIoTMQTTClient connection configuration
myMQTTClient.configureAutoReconnectBackoffTime(1, 32, 20)
myMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec


myMQTTClient.connect()
myMQTTClient.subscribe("pfc/test", 1, test_cb)




#class ip_notifier:

