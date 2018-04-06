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
logger.setLevel(logging.WARNING)
streamHandler = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
streamHandler.setFormatter(formatter)
logger.addHandler(streamHandler)


delta_status = None

#Delete Callback to delete JSON DOC
def customShadowCallback_Delete(payload, responseStatus, token):
	if responseStatus == "timeout":
		print("Delete request " + token + " time out!")
	if responseStatus == "accepted":
		print("~~~~~~~~~~~~~~~~~~~~~~~")
		print("Delete request with token: " + token + " accepted!")
		print("~~~~~~~~~~~~~~~~~~~~~~~\n\n")
	if responseStatus == "rejected":
		print("Delete request " + token + " rejected!")


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
			print("payload is the string")
			delta_status = json.loads(payload)
		elif type(payload) is dict :
			print("payload is the dict")
			delta_status = payload
		print(payload)
		print(token)
		print("~~~~~~~~~~~~~~~~~~~~~~~")
		print("Update request with token: " + token + " accepted!")
		# print("property: " + str(payloadDict["state"]["desired"]["property"]))
		print("~~~~~~~~~~~~~~~~~~~~~~~\n\n")
	if responseStatus == "rejected":
		print("Update request " + token + " rejected!")
	myAWSIoTMQTTShadowClient.disconnect()



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
# myDeviceShadow.shadowGet(customShdadowCallback_delta, 5)
# myDeviceShadow.shadowUpdate(myJSONPayload, customShadowCallback_delta, 5)
# myDeviceShadow.shadowDelete(customShadowCallback_delta, 5)
# myDeviceShadow.shadowRegisterDeltaCallback(customShadowCallback_delta)
# myDeviceShadow.shadowUnregisterDeltaCallback()
print(myDeviceShadow)


json_fopen = open('./device_init_state.json','r')
json_str = json_fopen.read()
device_init_state_json = json.loads(json_str)
# device_init_state_json['state']['desired']['PH'] = 30
# device_init_state_json['state']['desired']['EC'] = 30
# device_init_state_json['state']['desired']['WATER_TEMP']= 25

device_init_state_json['state']['desired']['AIR_FAN'] = "ON"
device_init_state_json['state']['desired']['AIR_PUMP'] = "ON"
device_init_state_json['state']['desired']['LED'] = "ON"
device_init_state_json['state']['desired']['VENTIL_FAN'] = "ON"
device_init_state_json['state']['desired']['WATER_PUMP'] = "OFF"
device_init_state_json['state']['desired']['SOLUTION_A_PUMP'] = "OFF"
device_init_state_json['state']['desired']['SOLUTION_B_PUMP'] = "OFF"
device_init_state_json['state']['desired']['PH_PLUS_PUMP'] = "OFF"
device_init_state_json['state']['desired']['PH_MINUS_PUMP'] = "OFF"

# device_init_state_json['state']['reported']['PH']= 6.5
# device_init_state_json['state']['reported']['EC']= 7
# device_init_state_json['state']['reported']['WATER_TEMP']= 7
# print(json.dumps(device_init_state_json))

loopCount = 0
# Delte Json Document on the AWS THING SHADOW
# myDeviceShadow.shadowDelete(customShadowCallback_Delete, 5)

# Json to String
# delta_status = json.dumps(device_init_state_json)
print('{"state" : {"reported" :'+json.dumps(device_init_state_json['state']['desired']) +'}}')
myDeviceShadow.shadowUpdate('{"state" : {"desired" :'+json.dumps(device_init_state_json['state']['desired']) +'}}', customShadowCallback_Update, 30)




# while True:
# 	# JSONPayload = '{"state":{"desired":{"property":' + str(loopCount) + '}}}'
# 	# JSONPayload = device_init_state_json
# 	print(delta_status)
# 	# JSONPayload = json_str
# 	if type(delta_status) is dict:
# 		# delta_status['state'] = device_init_state_json['state']
# 		delta_status = json.dumps(device_init_state_json)
# 		myDeviceShadow.shadowUpdate(delta_status, customShadowCallback_Update, 5)
# 	loopCount += 1
# 	print("LOOP COUNT : " + str(loopCount))
# 	time.sleep(5)



