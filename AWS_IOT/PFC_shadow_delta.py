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


class PFC_shadow_delta:
	is_delay_iot_shadow = False
	PFC_SHADOW = None
	PFC_SHADOW_DELTA = None
	actuators = {
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
	def __init__(self):
		self.init_AWS_IOT()

	def connection_AWS_IOT(self):
		myAWSIoTMQTTShadowClient = AWSIoTMQTTShadowClient(pfc_mqtt_topic.AWS_SHADOW_THING_NAME)
		myAWSIoTMQTTShadowClient.configureEndpoint(pfc_mqtt_topic.AWS_ENDPOINT, 8883)
		myAWSIoTMQTTShadowClient.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)
		myAWSIoTMQTTShadowClient.configureConnectDisconnectTimeout(30)
		myAWSIoTMQTTShadowClient.configureMQTTOperationTimeout(30)
		myAWSIoTMQTTShadowClient.connect()
		mc = myAWSIoTMQTTShadowClient.getMQTTConnection()
		mc.configureOfflinePublishQueueing(-1)

		self.IOT_SHADOW_CLIENT = myAWSIoTMQTTShadowClient
		self.PFC_SHADOW = myAWSIoTMQTTShadowClient.createShadowHandlerWithName(pfc_mqtt_topic.AWS_SHADOW_THING_NAME, True)
		self.PFC_SHADOW.shadowRegisterDeltaCallback(self.shadowDeltaCallback)

	def init_AWS_IOT(self):
		try :
			self.connection_AWS_IOT()
		except Exception, e:
			print(str(e))
			print "Exception occured in connection to AWS IoT"
			print " = > Script ShudtDown"
			sys.exit()

		# while True:
		# 	if self.is_delay_iot_shadow == True:
		# 		self.syncActuator()
		# 		break;

	def shadowDeltaCallback(self, payload, responseStatus, token):
		if type(payload) is dict:
			delta_status = payload
		elif payload is not None:
			delta_status = json.loads(payload)
		print(json.dumps(delta_status,indent=4, sort_keys=True))

		is_actuator_order_exists = False
		for o_name in delta_status['state']:
			if o_name in self.actuators:
				is_actuator_order_exists = True
				break;

		if is_actuator_order_exists == False:
			return

		self.PFC_SHADOW_DELTA=delta_status
		print("\n\n self.PFC_SHADOW_DELTA")
		print(self.PFC_SHADOW_DELTA)
		time.sleep(0.1)
		self.syncActuator()


	def syncActuator(self):
		kill_proc = lambda p : p.kill()


		if self.PFC_SHADOW is not None:
			for pfc_n in self.PFC_SHADOW_DELTA['state']:
				pfc_v = self.PFC_SHADOW_DELTA['state'][pfc_n]
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
						except Exception, e:
							print("Execution Acutator Suprocess Exception Occured")
							print(str(e))
						finally :
							None

			self.PFC_SHADOW_DELTA['state']['reported'] = {}
			for act_name in self.actuators:
				if self.actuators[act_name]['sync'] is not True:
					print("Not Executed to " + act_name)
				else :
					if self.actuators[act_name]['value'] in ["ON","OFF"]:
						self.PFC_SHADOW_DELTA['state']['reported'][act_name] = self.actuators[act_name]['value']
						# Important!
						#	In the Peristaltic Pump case , "python scirpt of the pump" has a automatically off the pump after delay to supply liquid. So Finally, this pump's status is the "OFF" is real.
						#	Following codes change the "OFF" state in this case.
						#
						if (act_name in ['SOLUTION_A_PUMP','SOLUTION_B_PUMP','PH_PLUS_PUMP','PH_MINUS_PUMP']) and self.actuators[act_name]['value'] == "ON" :
							self.PFC_SHADOW_DELTA['state']['reported'][act_name] = "OFF"
			# print(json.dumps(self.PFC_SHADOW_STATUS,indent=4, sort_keys=4))
			#Report to the AWS_IOT Shadow Thing
			try :
				print(">>>>>>>>> Final Result")
				print('{"state" : {"reported" :'+json.dumps(self.PFC_SHADOW_DELTA['state']['reported']) +'}}')
				print("length : " + str(len(self.PFC_SHADOW_DELTA['state']['reported'])))
				if len(self.PFC_SHADOW_DELTA['state']['reported']) > 0 :
					return_code = self.PFC_SHADOW.shadowUpdate('{"state" : {"reported" :'+json.dumps(self.PFC_SHADOW_DELTA['state']['reported']) +'}}', self.reportShadow, 60)
				else :
					print("Not Report(Any Acutator doesn't exeucted)")
			except Exception,e:
				print("[AWS IOT]occured Error on the reported process")
				print(str(e))
			# print(json.dumps(self.PFC_SHADOW_STATUS,indent=4,sort_keys=True))
			# self.reportShadow()

	def reportShadow(self, payload, responseStatus, token):
		if responseStatus == 'timeout':
			print("Update request " + token + " time out!")
			return -1
		elif responseStatus == 'accepted':
			print("Update request " + token + " accepted!")
			return 1
		elif responseStatus == 'rejected':
			print("Update request " + token + " rejected!")
			return 0

#Custom Shadow Callback
# def customShadowCallback_delta(payload, responseStatus, token):
# 	print(responseStatus)
# 	payloadDict = json.loads(payload)
# 	print("+++++++++++++ Listen Shadow Delta +++++++++++++")
# 	print(json.dumps(payloadDict,indent=4, sort_keys=True))
# 	print('version : ' + str(payloadDict['version']))
# 	print("++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n")

# myAWSIoTMQTTShadowClient = AWSIoTMQTTShadowClient(pfc_mqtt_topic.AWS_SHADOW_THING_NAME)
# myAWSIoTMQTTShadowClient.configureEndpoint(pfc_mqtt_topic.AWS_ENDPOINT, 8883)
# myAWSIoTMQTTShadowClient.configureCredentials(pfc_conf.CA_PATH, pfc_conf.PRIVATE_KEY_PATH, pfc_conf.CERTIFICATE_PATH)
# myAWSIoTMQTTShadowClient.configureConnectDisconnectTimeout(10)
# myAWSIoTMQTTShadowClient.configureMQTTOperationTimeout(5)
# myAWSIoTMQTTShadowClient.connect()
# myDeviceShadow = myAWSIoTMQTTShadowClient.createShadowHandlerWithName(pfc_mqtt_topic.AWS_SHADOW_THING_NAME,True)
# myDeviceShadow.shadowGet(customShadowCallback_delta,5)
# myDeviceShadow.shadowRegisterDeltaCallback(customShadowCallback_delta)
# print(myDeviceShadow)

# while True:
# 	time.sleep(1)


if __name__ == '__main__':
	PFC_shadow_delta = PFC_shadow_delta()
	while True:
		time.sleep(1)














