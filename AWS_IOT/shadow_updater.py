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


delta_status = None
#Custom Shadow Callback
def customShadowCallback_delta(payload, responseStatus, token):
	global delta_status
	print(responseStatus)
	if type(payload) is dict :
		delta_status = payload
	elif payload is not None:
		delta_status = json.loads(payload)

	print("+++++++++++DELTA++++++++++")
	print(str(delta_status))
	# print("property : " + str(payloadDict["state"]["property"]))
	print("version : " + str(delta_status["version"]))
	print("++++++++++++++++++++++++++\n\n")


#Custom Shadow Callback
def customShadowCallback_Update(payload, responseStatus, token):
	if responseStatus == "timeout":
		print("Update request " + token + " time out!")
	if responseStatus == "accepted":
		if type(payload) is str :
			delta_status = json.loads(payload)
		elif type(payload) is dict :
			delta_status = payload
		print("~~~~~~~~~~~~~~~~~~~~~~~")
		print("Update request with token: " + token + " accepted!")
		# print("property: " + str(payloadDict["state"]["desired"]["property"]))
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

mc = myAWSIoTMQTTShadowClient.getMQTTConnection()
mc.configureOfflinePublishQueueing(-1)

# MQTTClient = myShadowClient.getMQTTConnection()
# MQTTClient.configureOfflinePublishQueueing(yourQueueSize, yourDropBehavior)


print(myAWSIoTMQTTShadowClient)
myDeviceShadow = myAWSIoTMQTTShadowClient.createShadowHandlerWithName("PFC_v_0001", True)
# Shadow operations
myDeviceShadow.shadowGet(customShadowCallback_delta, 5)
# myDeviceShadow.shadowUpdate(myJSONPayload, customShadowCallback_delta, 5)
# myDeviceShadow.shadowDelete(customShadowCallback_delta, 5)
# myDeviceShadow.shadowRegisterDeltaCallback(customShadowCallback_delta)
# myDeviceShadow.shadowUnregisterDeltaCallback()
print(myDeviceShadow)


json_fopen = open('./device_init_state.json','r')
json_str = json_fopen.read()
device_init_state_json = json.loads(json_str)

print(type(device_init_state_json))
loopCount = 0
while True:
	# JSONPayload = '{"state":{"desired":{"property":' + str(loopCount) + '}}}'
	# JSONPayload = device_init_state_json
	print(delta_status)
	# JSONPayload = json_str
	if type(delta_status) is dict:
		delta_status['state'] = device_init_state_json['state']
		print(delta_status)

		myDeviceShadow.shadowUpdate(delta_status, customShadowCallback_Update, 5)
	loopCount += 1
	print("LOOP COUNT : " + str(loopCount))
	time.sleep(5)





