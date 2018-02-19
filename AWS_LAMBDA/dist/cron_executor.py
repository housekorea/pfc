import json
import boto3
from croniter import croniter
import datetime
import sys



def lambda_handler(event, context):
    # Maintainence
    publish_local_ip_cr = "*/15 * * * *"


    # Actuator
    led_on_cr = "15 4-24 * * *"
    led_off_cr = "15 0-3 * * *"
    air_pump_on_cr = "30,32 * * * *"
    air_pump_off_cr = "50,52 * * * *"
    ventil_fan_on_cr = "20,22 * * * *"
    ventil_fan_off_cr = "50,52 * * * *"
    air_fan_on_cr = "10,30,50 * * * *"
    air_fan_off_cr = "0,20,40 * * * * *"


    # Sensor
    usb_cam_capture_cr = "*/5 * * * *"
    all_sensors_cr = "*/5 * * * *"

    sches = {
        "publish_local_ip": ["*/15 * * * *","GET","ALL_SENSOR","SENSOR"],
        # "led_on":["15 4-23 * * *","ON","LED","ACTUATOR"],
        # "led_off":["15 0-3 * * *","OFF","LED","ACTUATOR"],
        "led_on":["1-59/2 * * * *","ON","LED","ACTUATOR"],
        "led_off":["0-58/2 * * * *","OFF","LED","ACTUATOR"],
        "air_pump_on":["30,32 * * * *","ON","AIR_PUMP","ACTUATOR"],
        "air_pump_off":["50,52 * * * *","OFF","AIR_PUMP","ACTUATOR"],
        "ventil_fan_on":["20,22 * * * *","ON","VENTIL_FAN","ACTUATOR"],
        "ventil_fan_off":["50,52 * * * *","OFF","VENTIL_FAN","ACTUATOR"],
        "air_fan_on":["10,30,50 * * * *","ON","AIR_FAN","ACTUATOR"],
        "air_fan_off":["0,20,40 * * * * ","OFF","AIR_FAN","ACTUATOR"],
        "usb_cam":["*/5 * * * *","CAPTURE","USB_CAM","SENSOR"],
        "all_sensor":["*/5 * * * *","GET","ALL_SENSOR","SENSOR"]
    }
    current_dt = datetime.datetime.now()
    client = boto3.client('iot-data', region_name='ap-northeast-2')

    for key in sches:
        temp_cron = croniter(str(sches[key][0]),current_dt)
        temp_prev = temp_cron.get_prev(datetime.datetime)
        is_reserved = (abs(temp_prev - current_dt) < datetime.timedelta(minutes=2))
        if is_reserved == True:
            TYPE = sches[key][3]
            TARGET = sches[key][2]
            ORDER = sches[key][1]

            response = client.publish(
                payload=json.dumps({
                    "PFC_SERIAL" : "None",
                    "DEVICE_DT" : "",
                    "ORDER" : ORDER,
                    "TARGET" : TARGET,
                    "TYPE" : TYPE,
                    "ORDER_DT" : str(datetime.datetime.now()),
                    "DEVICE_DT" : ""
                 }),
                topic = 'EZFARM/PFC/V1/DEV',
                qos=1
                )



    # Change topic, qos and payload
    # response = client.publish(
    #     payload=json.dumps({
    #         "PFC_SERIAL" : "CLOUDWATCH_LAMBDA_ORDER",
    #         "DEVICE_DT" : "",
    #         "ORDER" : "GET",
    #         "TARGET" : "ALL_SENSOR",
    #         "TYPE" : "SENSOR",
    #         "ORDER_DT" : "",
    #         "DEVICE_DT" : "",
    #     }),
    #     topic = 'EZFARM/PFC/V1/DEV/00000001/order_subscribe',
    #     qos=1
    # )


lambda_handler('hello','good')











