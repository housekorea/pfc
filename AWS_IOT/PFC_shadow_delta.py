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
from command_mapper import command_mapper
import subprocess, shlex
from threading import Timer

# Configure logging
logger = logging.getLogger("AWSIoTPythonSDK.core")
logger.setLevel(logging.DEBUG)
streamHandler = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
streamHandler.setFormatter(formatter)
logger.addHandler(streamHandler)

#Custom Shadow Callback
def customShadowCallback_delta(payload, responseStatus, token):
	print(responseStatus)
	payloadDict = json.loads(payload)
	print("+++++++++++++ Listen Shadow Delta +++++++++++++")
	print(str(payloadDcit))
	print('version : ' + str(payloadDcit['version']))
	print("++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n")

myAWSIoTMQTTShadowClient = AWSIoTMQTTShadowClient(pfc_conf.PFC_AWS_IOT_CLIENT_ID)
myAWSIoTMQTTShadowClient.configureEndpoint(pfc_mqtt_topic.AWS_ENDPOINT, 8883)
myAWSIoTMQTTShadowClient.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)
myAWSIoTMQTTShadowClient.configureConnectDisconnectTimeout(10)
myAWSIoTMQTTShadowClient.configureMQTTOperationTimeout(5)
myAWSIoTMQTTShadowClient.connect()

print(myAWSIoTMQTTShadowClient)
myDeviceShadow = myAWSIoTMQTTShadowClient.createShadowHandlerWithName(pfc_mqtt_topic.AWS_SHADOW_THING_NAME,True)
myDeviceShadow.shadowGet(customShadowCallback_delta,5)
myDeviceShadow.shadowRegisterDeltaCallback(customShadowCallback_detla)
print(myDeviceShadow)

while True:
	time.sleep(1)

















