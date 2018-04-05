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



class PFC_shadow_actuator_sync:
	is_disconnect_iot_shadow = False
	PFC_SHADOW = None
	PFC_SHADOW_STATUS = None

	def __init__(self):
		self.init_AWS_IOT()

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
			self.connection_AWS_IOT()
		except :
			print "exception occured in connection to AWS IOT"
			print "Script ShudtDown"
			sys.exit()


		self.PFC_SHADOW.shadowGet(self.shadowGet, 60)
		# myAWSIoTMQTTShadowClient.disconnect()
		while True:
			if self.is_disconnect_iot_shadow == True:
				self.syncActuator()
				break;



	def shadowGet(self,payload,responseStatus,token):
		print(">>>>>>>>>>>>> ShadowGet Handler")
		print(responseStatus)

		if responseStatus == "accepted":
			self.PFC_SHADOW_STATUS = json.loads(payload)
			# print(self.PFC_SHADOW_STATUS['state'])
			# print(json.dumps(self.PFC_SHADOW_STATUS['state'], indent=4, sort_keys=True))
			self.IOT_SHADOW_CLIENT.disconnect()
			self.is_disconnect_iot_shadow = True

		elif responseStatus == "rejected":
			print("ShadowGet rejected")
		elif responseStatus == "timeout":
			print("ShadowGet timeout")
		else:
			print("[Error Occured] ShadowGet Callback Function")

	def syncActuator(self):
		kill_proc = lambda p : p.kill()
		self.actuators = {
									'AIR_FAN' : {'value' : None, "sync" : None},
									'AIR_PUMP' : {'value' : None, "sync" : None},
									'PH_MINUS_PUMP' : {'value' : None, "sync" : None},
									'PH_PLUS_PUMP' : {'value' : None, "sync" : None},
									'LED' : {'value' : None, "sync" : None},
									'SOLUTION_A_PUMP' : {'value' : None, "sync" : None},
									'SOLUTION_B_PUMP' : {'value' : None, "sync" : None},
									'VENTIL_FAN' : {'value' : None, "sync" : None},
									'WATER_PUMP' : {'value' : None, "sync" : None}
								}

		if self.PFC_SHADOW is not None:
			for pfc_n in self.PFC_SHADOW_STATUS['state']['desired']:
				pfc_v = self.PFC_SHADOW_STATUS['state']['desired'][pfc_n]
				if pfc_n in self.actuators and pfc_v is not '':
					self.actuators[pfc_n]['value'] = pfc_v
					if pfc_n in command_mapper.ACTUATOR and pfc_v in command_mapper.ACTUATOR[pfc_n]:
						try :
							command_pfc_actuator = command_mapper.ACTUATOR_DIR_PATH + command_mapper.ACTUATOR[pfc_n][pfc_v]
							actuator_proc = subprocess.Popen(shlex.split("python " + command_pfc_actuator), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
							stdout, stderr = actuator_proc.communicate()
							actuator_proc.wait()
							print("[PID]" +str(actuator_proc.pid)+". "+ str(pfc_n) + " : " +stdout.rstrip() + "|| SubProcess return code : " +str(actuator_proc.returncode))
							# print(pfc_v + " / " + str(stdout))
							result_act = re.search('^result=([0,1,2]){1}\/.*$', stdout)
							if result_act is not None:
								if result_act.group(1) == "1" and pfc_v == "ON":
									self.actuators[pfc_n]['sync'] = True
								elif result_act.group(1) == "0" and pfc_v == "OFF":
									self.actuators[pfc_n]['sync'] = True
							else :
									self.actuators[pfc_n]['sync'] = False
							time.sleep(0.2)
						except :
							print("Exception Occured")
						finally :
							None
			# break; #break the for Loop
			for act_name in self.actuators:
				if self.actuators[act_name]['sync'] is not True:
					print("not Synced to " + act_name)
				else :
					self.PFC_SHADOW_STATUS['state']['reported'][act_name] = self.actuators[act_name]['value']



			#Report to the AWS_IOT Shadow Thing
			try :
				self.connection_AWS_IOT()
				return_code = self.PFC_SHADOW.shadowUpdate('{"state" : {"desired" : ' +json.dumps(self.PFC_SHADOW_STATUS['state']['desired'])+', "reported" :'+json.dumps(self.PFC_SHADOW_STATUS['state']['reported']) +'}}', self.reportShadow, 60)
			except :
				print("[AWS IOT]occured Error on the reported process")

			# print(json.dumps(self.PFC_SHADOW_STATUS,indent=4,sort_keys=True))
			# self.reportShadow()


	def reportShadow(self,payload,responseStatus,token):
		if responseStatus == 'timeout':
			print("Update request " + token + " time out!")
			return -1
		elif responseStatus == 'accepted':
			print("Update request " + token + " accepted!")
			self.IOT_SHADOW_CLIENT.disconnect()
			return 1
		elif responseStatus == 'rejected':
			print("Update request " + token + " rejected!")
			self.IOT_SHADOW_CLIENT.disconnect()
			return 0

		# self.PFC_SHADOW.shadowGet(self.shadowGet, 60)


if __name__ == '__main__':
	PFC_shadow = PFC_shadow_actuator_sync()




















