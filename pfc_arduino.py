import serial
import time
ser = serial.Serial('/dev/ttyACM0', 19200)


# while True:
# 	ser.write("get_all_sensors")

# 	time.sleep(3)
# 	arduino_reply = ser.readline()
# 	print(arduino_reply)
# 	time.sleep(3)

while True:
	user_order = raw_input("ORDER to PFC :  ");
	ser.write(user_order);
	time.sleep(0.5);
	arduino_reply = ser.readline();

	print(arduino_reply);
	print("\n");


