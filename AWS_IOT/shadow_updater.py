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
	print("+++++++++++DELTA++++++++++")
	print(str(payloadDict))
	# print("property : " + str(payloadDict["state"]["property"]))
	print("version : " + str(payloadDict["version"]))
	print("++++++++++++++++++++++++++\n\n")


#Custom Shadow Callback
def customShadowCallback_Update(payload, responseStatus, token):
	if responseStatus == "timeout":
		print("Update request " + token + " time out!")
	if responseStatus == "accepted":
		payloadDict = json.loads(payload)
		print("~~~~~~~~~~~~~~~~~~~~~~~")
		print("Update request with token: " + token + " accepted!")
		print("property: " + str(payloadDict["state"]["desired"]["property"]))
		print("~~~~~~~~~~~~~~~~~~~~~~~\n\n")
	if responseStatus == "rejected":
		print("Update request " + token + " rejected!")



shadow_host = "a1wxijaxbxg469.iot.ap-northeast-2.amazonaws.com"
myAWSIoTMQTTShadowClient = AWSIoTMQTTShadowClient(pfc_conf.PFC_AWS_IOT_CLIENT_ID)
myAWSIoTMQTTShadowClient.configureEndpoint(shadow_host, 8883)
# myAWSIoTMQTTShadowClient.configureCredentials(rootCAPath, privateKeyPath, certificatePath)
myAWSIoTMQTTShadowClient.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)


myAWSIoTMQTTShadowClient.configureConnectDisconnectTimeout(10)
myAWSIoTMQTTShadowClient.configureMQTTOperationTimeout(5)


myAWSIoTMQTTShadowClient.connect()



print(myAWSIoTMQTTShadowClient)
myDeviceShadow = myAWSIoTMQTTShadowClient.createShadowHandlerWithName("PFC_v_0001", True)
# Shadow operations
myDeviceShadow.shadowGet(customShadowCallback_delta, 5)
# myDeviceShadow.shadowUpdate(myJSONPayload, customShadowCallback_delta, 5)
# myDeviceShadow.shadowDelete(customShadowCallback_delta, 5)
# myDeviceShadow.shadowRegisterDeltaCallback(customShadowCallback_delta)
# myDeviceShadow.shadowUnregisterDeltaCallback()
print(myDeviceShadow)

loopCount = 0
while True:
	JSONPayload = '{"state":{"desired":{"property":' + str(loopCount) + '}}}'
	myDeviceShadow.shadowUpdate(JSONPayload, customShadowCallback_Update, 5)
	loopCount += 1
	time.sleep(1)





