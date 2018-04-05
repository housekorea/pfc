# -*- coding: utf-8 -*-
#!/bin/bash
import os
import sys
from datetime import datetime
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTShadowClient
import logging
import json
import time
import argparse
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf
from configure import pfc_mqtt_topic

# Configure logging
logger = logging.getLogger("AWSIoTPythonSDK.core")
logger.setLevel(logging.DEBUG)
streamHandler = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
streamHandler.setFormatter(formatter)
logger.addHandler(streamHandler)


def customShadowCallback_get(payload, responseStatus, token):
	global pfc_status
	print(responseStatus)
	print("Received Payload from the Shadow")
	print(payload)
	print(type(payload))
	pfc_status = json.loads(payload)
	print('pfc_status loaded by the json format')
	print(json.dumps(pfc_status, indent=4, sort_keys=True))
	print(pfc_status['state']['desired'])
	print(pfc_status['state']['delta'])


shadow_host = "a1wxijaxbxg469.iot.ap-northeast-2.amazonaws.com"
myAWSIoTMQTTShadowClient = AWSIoTMQTTShadowClient(pfc_conf.PFC_AWS_IOT_CLIENT_ID)
myAWSIoTMQTTShadowClient.configureEndpoint(shadow_host, 8883)
# myAWSIoTMQTTShadowClient.configureCredentials(rootCAPath, privateKeyPath, certificatePath)
myAWSIoTMQTTShadowClient.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)


myAWSIoTMQTTShadowClient.configureConnectDisconnectTimeout(10)
myAWSIoTMQTTShadowClient.configureMQTTOperationTimeout(5)


myAWSIoTMQTTShadowClient.connect()

mc = myAWSIoTMQTTShadowClient.getMQTTConnection()
mc.configureOfflinePublishQueueing(-1)

# MQTTClient = myShadowClient.getMQTTConnection()
# MQTTClient.configureOfflinePublishQueueing(yourQueueSize, yourDropBehavior)


print(myAWSIoTMQTTShadowClient)
myDeviceShadow = myAWSIoTMQTTShadowClient.createShadowHandlerWithName("PFC_v_0001", True)



myDeviceShadow.shadowGet(customShadowCallback_get,10)






