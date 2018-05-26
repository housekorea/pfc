import json
import boto3
from croniter import croniter
import datetime
from pytz import timezone
import sys



def lambda_handler(event, context):
    # # Maintainence
    # publish_local_ip_cr = "*/15 * * * *"

    seoul_tz = timezone("Asia/Seoul")
    # # Actuator
    # led_on_cr = "15 4-24 * * *"
    # led_off_cr = "15 0-3 * * *"
    # air_pump_on_cr = "30,32 * * * *"
    # air_pump_off_cr = "50,52 * * * *"
    # ventil_fan_on_cr = "20,22 * * * *"
    # ventil_fan_off_cr = "50,52 * * * *"
    # air_fan_on_cr = "10,30,50 * * * *"
    # air_fan_off_cr = "0,20,40 * * * * *"


    # # Sensor
    # usb_cam_capture_cr = "*/5 * * * *"
    # all_sensors_cr = "*/5 * * * *"

    sches = {
        # "publish_local_ip": ["55 * * * *","LOCAL_IP","LOCAL_IP","LOCAL_IP"],
        "led_on":["*/10 4-23 * * *","ON","LED","ACTUATOR"],
        "led_off":["*/10 0-3 * * *","OFF","LED","ACTUATOR"],
        "air_pump_on":["40 * * * *","ON","AIR_PUMP","ACTUATOR"],
        "air_pump_off":["50 * * * *","OFF","AIR_PUMP","ACTUATOR"],
        # "ventil_fan_on":["20,22 * * * *","ON","VENTIL_FAN","ACTUATOR"],
        # "ventil_fan_off":["50,52 * * * *","OFF","VENTIL_FAN","ACTUATOR"],
        "ventil_fan_on":["10,12 * * * *","ON","VENTIL_FAN","ACTUATOR"],
        "ventil_fan_off":["20,22 * * * *","OFF","VENTIL_FAN","ACTUATOR"],
        # "air_fan_on":["10,30,50 * * * *","ON","AIR_FAN","ACTUATOR"],
        # "air_fan_off":["0,20,40 * * * * ","OFF","AIR_FAN","ACTUATOR"],

        "air_fan_on":["*/2 * * * *","ON","AIR_FAN","ACTUATOR"],
        "air_fan_off":["1-59/2 * * * * ","OFF","AIR_FAN","ACTUATOR"],

        "water_pump_on" : ["5 */2 * * *", "ON", "WATER_PUMP","ACTUATOR"],
        "water_pump_off" : ["8 */2 * * *", "OFF", "WATER_PUMP","ACTUATOR"],
        # "usb_cam":["*/10 * * * *","CAPTURE","USB_CAM_TOP","SENSOR"],
        # "all_sensor":["*/10 * * * *","GET","ALL_SENSOR","SENSOR"],
        # "s3_img_uploader":["5,45 */12 * * *", "UPLOAD", "S3_UPLOAD","DATALAKE" ]
    }
    current_dt = datetime.datetime.now(seoul_tz)
    client = boto3.client('iot-data', region_name='ap-northeast-2')
    thing_shadow = client.get_thing_shadow(thingName='PFC_v_0001')
    # print(str(current_dt))
    update_jsonState = {'state' : { 'desired' : {}}}

    for key in sches:
        temp_cron = croniter(str(sches[key][0]),current_dt)
        temp_prev = temp_cron.get_prev(datetime.datetime)
        print(str(key) + "/" + str(temp_prev))
        is_reserved = (abs(temp_prev - current_dt) < datetime.timedelta(minutes=2))
        if is_reserved == True:
            TYPE = sches[key][3]
            TARGET = sches[key][2]
            ORDER = sches[key][1]
            update_jsonState['state']['desired'][TARGET] = ORDER

    print("Shadow Update")
    print(">>>>>>> " + json.dumps(update_jsonState))
    client.update_thing_shadow(
        thingName = 'PFC_v_0001',
        payload=json.dumps(update_jsonState)
    )








