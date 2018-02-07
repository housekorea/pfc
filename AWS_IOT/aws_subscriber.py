# -*- coding: utf-8 -*-
import os
import sys
from datetime import datetime
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import time


CA_PATH = "/Users/house/MAC_IOT_CREDENTIAL/VeriSign-Class 3-Public-Primary-Certification-Authority-G5.pem"
PRIVATE_KEY_PATH ="//Users/house/MAC_IOT_CREDENTIAL/9fbd747e09-private.pem.key"
CERTIFICATE_PATH ="/Users/house/MAC_IOT_CREDENTIAL/9fbd747e09-certificate.pem.crt"
def test_cb(self, params, packet):
	print(packet.payload)
	f = open('/Users/house/Desktop/cert_pfc_0001/log.log','a+')
	f.write(packet.payload + '  /'+str(datetime.now()))
	f.write('\n')
	f.close()

myMQTTClient = AWSIoTMQTTClient("MY_MAC_CLIENT")

myMQTTClient.configureEndpoint("a1wxijaxbxg469.iot.ap-northeast-2.amazonaws.com",8883)
myMQTTClient.configureCredentials(CA_PATH,PRIVATE_KEY_PATH,CERTIFICATE_PATH)




# AWSIoTMQTTClient connection configuration
myMQTTClient.configureAutoReconnectBackoffTime(1, 32, 20)
myMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec

print("Connect")
myMQTTClient.connect()
myMQTTClient.subscribe("pfc/test", 1, test_cb)

#class ip_notifier:
while True:
	time.sleep(1)

