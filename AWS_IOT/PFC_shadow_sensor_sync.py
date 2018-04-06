# -*- coding: utf-8 -*-
#!/bin/bash
import os
import sys
import re
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
logger.setLevel(logging.WARNING)
streamHandler = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
streamHandler.setFormatter(formatter)
logger.addHandler(streamHandler)



class PFC_shadow_sensor_sync:
	is_disconnect_iot_shadow = False
	PFC_SHADOW = None
	PFC_SHADOW_STATUS = {}

	def __init__(self):
		# self.init_AWS_IOT()

		self.syncSensor()

	def connection_AWS_IOT(self):
		shadow_host = "a1wxijaxbxg469.iot.ap-northeast-2.amazonaws.com"
		myAWSIoTMQTTShadowClient = AWSIoTMQTTShadowClient(pfc_conf.PFC_AWS_IOT_CLIENT_ID)
		myAWSIoTMQTTShadowClient.configureEndpoint(shadow_host, 8883)
		myAWSIoTMQTTShadowClient.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)
		myAWSIoTMQTTShadowClient.configureConnectDisconnectTimeout(30)
		myAWSIoTMQTTShadowClient.configureMQTTOperationTimeout(30)
		myAWSIoTMQTTShadowClient.connect()
		mc = myAWSIoTMQTTShadowClient.getMQTTConnection()
		mc.configureOfflinePublishQueueing(-1)
		self.IOT_SHADOW_CLIENT = myAWSIoTMQTTShadowClient
		self.PFC_SHADOW = myAWSIoTMQTTShadowClient.createShadowHandlerWithName("PFC_v_0001", True)

	def init_AWS_IOT(self):
		try :
			None
			self.connection_AWS_IOT()
		except :
			print "exception occured in connection to AWS IOT"
			print "Script ShutDown"
			sys.exit()
		# self.PFC_SHADOW.shadowGet(self.shadowGet, 60)

		while True:
			if self.is_disconnect_iot_shadow == True:
				self.syncSensor()
				break;
	def shadowGet(self, payload, responseStatus, token):
		print(">>>>>>>>>>>>> ShadowGet Handler")
		print(responseStatus)

		if responseStatus == "accepted":
			self.PFC_SHADOW_STATUS = json.loads(payload)
			self.IOT_SHADOW_CLIENT.disconnect()
			self.is_disconnect_iot_shadow = True


			print(json.dumps(self.PFC_SHADOW_STATUS, indent=4, sort_keys=True))


		elif responseStatus == "rejected":
			print("ShadowGet rejected")
		elif responseStatus == "timeout":
			print("ShadowGet timeout")
		else :
			print("[Error Occured] ShadowGet Callback Function")


	def syncSensor(self):
		kill_proc = lambda p : p.kill()
		self.PFC_SHADOW_STATUS['state'] ={}
		self.PFC_SHADOW_STATUS['state']['reported'] = {}
		# if self.PFC_SHADOW is not None:
		try :
			command_pfc_sensor = command_mapper.SENSOR_DIR_PATH + command_mapper.SENSOR['ALL_SENSOR']['GET']
			sensor_proc = subprocess.Popen(shlex.split("python " + command_pfc_sensor) , stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			stdout, stderr = sensor_proc.communicate()
			sensor_proc.wait()
			print("[PID]" +str(sensor_proc.pid)+". " + str(command_pfc_sensor) + " : " +stdout.rstrip() + "|| SubProcess return code : " +str(sensor_proc.returncode))

			result_sen = re.findall('[^order\=get_all_sensors\/result\=|,]+', stdout)
			get_all_sensor_list = [
																'AIR_TEMP',
																'AIR_HUMIDITY',
																'BRIGHTNESS',
																'CO2',
																'WATER_TEMP',
																'EC',
																'PH',
																'DATETIME'
															]

			if result_sen is not None and len(result_sen) == 8:
				for sen_i, sen_v in enumerate(result_sen):
					# print(get_all_sensor_list[sen_i] + " : "+ str(sen_v))
					if get_all_sensor_list[sen_i] is not "DATETIME":
						self.PFC_SHADOW_STATUS['state']['reported'][get_all_sensor_list[sen_i]] = sen_v
			else :
				print("Read the entire sensor data Failed, Read String : " + str(result_sen))
				print("Script ShutDown")
				sys.exit()

		except Exception, e:
			print("Exception Occured on Execution SubProcess")
			print(str(e))
			sys.exit()
		finally :
			None

		try :
			self.connection_AWS_IOT()
			print(">>>>>>>>> Final Result")
			print('{"state" : {"reported" :'+json.dumps(self.PFC_SHADOW_STATUS['state']['reported']) +'}}')
			return_code = self.PFC_SHADOW.shadowUpdate('{"state" : {"reported" :'+json.dumps(self.PFC_SHADOW_STATUS['state']['reported']) +'}}', self.reportShadow, 60)
		except :
			print("[AWS IOT]occured Error on the reported process")


	def reportShadow(self,payload,responseStatus,token):
		if responseStatus == 'timeout':
			print("Update request " + token + " time out!")
			return -1
		elif responseStatus == 'accepted':
			print("Update request " + token + " accepted!")
			self.IOT_SHADOW_CLIENT.disconnect()
			return 1
		elif responsestatus == 'rejected':
			print("Update request " + token + " rejected")
			self.IOT_SHADOW_CLIENT.disconnect()
			return 0



if __name__ == '__main__':
	if len(sys.argv) == 2:
		delay = sys.argv[1]
		print("DELAY : " + str(delay) + " seconds")
		time.sleep(float(delay))

	PFC_SHADOW = PFC_shadow_sensor_sync()


						# if result_sen is not None:
							# if result_sen.group(1)











































