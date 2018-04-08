import json
import boto3
from croniter import croniter
import datetime
from pytz import timezone
import sys
import json

def shadow_update_lambda(event, context):

	seoul_tz = timezone("Asia/Seoul")
	current_dt = datetime.datetime.now(seoul_tz)


	print("Shadow Thing Update Event execute lambda function - AWS IoT Rule Engine")
	print(current_dt)
	print(event)

	# print("Desired")
	# print(event['state']['desired'])
	# print("Requried")
	# print(event['state']['required'])

	client = boto3.client('iot-data', region_name='ap-northeast-2')
	response = client.get_thing_shadow(thingName='PFC_v_0001')

	streamingBody = response["payload"]
	jsonState = json.loads(streamingBody.read())
	print("All_SHADOW DATA")
	print(jsonState)
	print(jsonState["state"]["reported"])
	print("Response END")


	#
	#	IF SOLTION & PH PUMP reported state is the same with the desired state, reset the deisred state to "OFF"
	#
	#
	#
	# Update 시각을 의미하는 Timestamp 값을 설정하여, Desired 보다 Reported 가 더 최근인 경우에만 Desired 를 수정하도록 로직 변경 필요
	#
	#
	#
	#
	update_jsonState = {'state' : { 'desired' : {}}}
	if (jsonState['state']['desired']['SOLUTION_A_PUMP'] ==  "ON") and jsonState['state']['reported']['SOLUTION_A_PUMP'] == 'OFF':
		#Switch The State to `OFF` only for desired
		update_jsonState['state']['desired']['SOLUTION_A_PUMP'] = "OFF"
	if (jsonState['state']['desired']['SOLUTION_B_PUMP'] == "ON") and jsonState['state']['reported']['SOLUTION_B_PUMP'] == 'OFF':
		#Switch The State to `OFF` only for desired
		update_jsonState['state']['desired']['SOLUTION_B_PUMP'] = "OFF"
	if (jsonState['state']['desired']['PH_PLUS_PUMP'] == "ON") and jsonState['state']['reported']['PH_PLUS_PUMP'] == 'OFF':
		#Switch The State to `OFF` only for desired
		update_jsonState['state']['desired']['PH_PLUS_PUMP'] = "OFF"
	if (jsonState['state']['desired']['PH_MINUS_PUMP'] == "ON") and jsonState['state']['reported']['PH_MINUS_PUMP'] == 'OFF':
		#Switch The State to `OFF` only for desired
		update_jsonState['state']['desired']['PH_MINUS_PUMP'] = "OFF"

	if len(update_jsonState['state']['desired']) is not 0:
		print("Will New update state of the shadow :")
		print(">>>>>>>>> " + json.dumps(update_jsonState))
		client.update_thing_shadow(
			thingName='PFC_v_0001',
			payload=json.dumps(update_jsonState)
		)
	else :
		print(json.dumps(update_jsonState))














