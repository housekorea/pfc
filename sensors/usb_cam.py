import time
import sys
import os
import termios
from datetime import datetime
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from pfc_connection_arduino import pfc_connection_arduino
from cv import pfc_cv_measurer

class usb_cam:
	SKIP_FRAME = 2
	PATH = "/var/og/cam_imgs/"
	def __init__(self):
		None



	def capture(self,skip_num=None):
		if skip_num == None:
			skip_num = self.SKIP_FRAME

		fc_name = datetime.now().strftime("%Y%m%d_%H%M%S") + '.jpg'
		os.system("fswebcam -r 640x480 --no-banner %s/%s --skip %d " % (self.PATH, fc_name, skip_num))

		exist_file = self.is_exist_file(self.PATH + '/' + fc_name);

		if exist_file == False:
			print("Capture file not created")
		else :
			print("Capture file created")
			self.call_cv_process(self.PATH +'/' + fc_name)
	def call_cv_process(self,file_name):
		cv_measurer = pfc_cv_measurer.pfc_cv_measurer(coin_px=52, coin_mm=24, max_contours=10, opath=file_name,carea=600, min_side=50)

	def is_exist_file(self, fname):
		return os.path.exists(fname)





if __name__ == '__main__':
	usb_cam = usb_cam()


	if len(sys.argv) == 1:
		exit()
	elif len(sys.argv) == 2:
		order = sys.argv[1]

	elif (len(sys.argv) == 3) and sys.argv[2].isdigit():
		order = sys.argv[1]

	if order == 'capture':
		usb_cam.capture()





