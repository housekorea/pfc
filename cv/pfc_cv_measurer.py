# -*- coding: utf-8 -*-
from imutils import perspective
from imutils import contours
import numpy as np
import argparse
import imutils
import sys
import cv2
from matplotlib import pyplot as plt
from pprint import pprint
import time
from scipy.spatial import distance as dist
import numpy as np
from pprint import pprint
# np.set_printoptions(threshold='nan')

class pfc_cv_measurer:
	COIN_PX = None	# 동전의 PX 사이즈 (지름)
	COIN_MM = None	# 동전의 MM 사이즈(지름)
	MAX_CONTOURS = None # Contuor 의 최대 갯수
	ORIGINAL_FILE_PATH = None # 원본 이미지 경로
	IMAGES = {
							'ORIGINAL' : None,	# 오리지널 이미지
							'KCLUSTER' : None,	# K Clustering 을 통한 Color Qunatization
							'COLOR' : None,	# Color 추출 후
							'GRAY_IMG': None,	#	그레이스케일 변환
							'GAUSSIAN_IMG' : None,	#	가우시안 필터 적용
							'CANNY_IMG' : None,	# 캐니 필터 적용
							'DILATE_IMG' : None,	# Dilate 필터 적용
							'ERODE_IMG' : None,	# Erode 필터 적용
							'CV_IMG' : None	# 최종 OpenCV 프로세싱된 이미지 결과물
						}
	THRESHOLD_CONTOUR_AREA = 200	# 임계점, 콘투어의 면적이 200이상일때만 오브젝트로 인정
	THRESHOLD_MINIMUM_SIDE = 30	# 임계점, 콘투어의 한 변의 길이가 30 이상 일때만 오브젝트로 인정
	PX_MM_RATIO = 0	# 픽셀과 MM 단위 변환비(RATE)
	COLOR_DIFF = 30	# 색의 RANGE 차이(특정 RGB 영역값이 +- Color Diff)
	PLANT_COLOR = (198,228,82)
	PLANT_COLOR = (102,7,111)
	PLANT_COLOR = (97,111,33)
	PLANT_COLOR = (70,90,20) # 추출하고자 하는 색의 RGB 값
	BOUNDARIES = []


	def __init__(self,coin_px=10,coin_mm=10,max_contours=20,opath=None,carea=200,min_side=30,carea_max=2000):
		# 클래스 생성자 매개변수(Class Constructor Prarameter)
		if opath == None:
			return False
			sys.exit()



		# 색상추출시의 Minimum RGB ~ Maximum RGB 범위를 구하는 연산, 최저점은 0, 최고점은 255 RGB 값으로 설정
		self.BOUNDARIES = [
			(self.color_rgb_to_gbr(self.PLANT_COLOR[0]-self.COLOR_DIFF, self.PLANT_COLOR[1]-self.COLOR_DIFF, self.PLANT_COLOR[2] - self.COLOR_DIFF)),
			(self.color_rgb_to_gbr(self.PLANT_COLOR[0]+self.COLOR_DIFF, self.PLANT_COLOR[1]+self.COLOR_DIFF, self.PLANT_COLOR[2]+self.COLOR_DIFF))
			]
		for i,colo_set in enumerate(self.BOUNDARIES):
			for j,colo_val in enumerate(colo_set):
				if colo_val > 255:
					self.BOUNDARIES[i][j] = 255
				elif colo_val < 1:
					self.BOUNDARIES[i][j] = 0

		# print("Palnt Color Range setted by Hardcode. lower / upper")
		# print(self.BOUNDARIES)


		self.COIN_PX = coin_px
		self.COIN_MM = coin_mm
		self.MAX_CONTOURS = max_contours
		self.ORIGINAL_FILE_PATH = opath
		self.THRESHOLD_CONTOUR_AREA = carea
		self.THRESHOLD_MINIMUM_SIDE = min_side
		self.THRESHOLD_CONTOUR_AREA_MAX = carea_max
		self.PX_MM_RATIO = self.calc_PxForMmRatio(self.COIN_PX,self.COIN_MM)	# 1 픽셀당 MM의 비율 구하기


		self.IMAGES['ORIGINAL'] = cv2.imread(self.ORIGINAL_FILE_PATH) # 경로의 이미지 파일 읽기

		self.get_kcluster(K=8)	# K Clustering 을 통한 "Color Quantization" 수행
		self.color_detection() # 특정 색상 추출
		self.transition_images() # 이미지 변환 프로세스 수행
		self.find_contours() # Contour 탐색 및 추출


		# If you want save all of image on the transition process, set debug_save = True
		self.save_images(debug_save=True) # 이미지 프로세스단계별 저장

	# CV에 의해 식별된 Contour의 각 좌표를 순회하며, 해당 좌표가 대표하는 위치를 찾는다. 아래의 4개 위치 식별.
	# 1.TOP LEFT
	# 2.TOP RIGHT
	# 3.BOTTOM LEFT
	# 4.BOTTOM RIGHT
	# 위 식별된 4개의 점을 기준으로 관측된 식물의 Height / Widht 의 위치를 추론한다.
	def get_order_points(self,pts):
		xSorted = pts[np.argsort(pts[:,0]),:]
		leftMost = xSorted[:2,:]
		rightMost = xSorted[2:,:]

		leftMost = leftMost[np.argsort(leftMost[:,1]),:]
		(tl,bl) = leftMost

		D = dist.cdist(tl[np.newaxis], rightMost, "euclidean")[0]
		(br,tr) = rightMost[np.argsort(D)[::-1],:]

		return np.array([tl,tr,br,bl], dtype="float32")


	# 주어진 두 점의 좌표의 가운데 좌표를 반환한다.(좌표평면 기준)
	def get_midpoint(self,ptA,ptB):
		return ((ptA[0] + ptB[0]) * 0.5, (ptA[1] + ptB[1]) * 0.5)

	# OpenCV 에서는 RGB가 아닌 GBR 로 매개변수를 전달해야 하기 때문에, 변환 함수 정의
	def color_rgb_to_gbr(self,r,g,b):
		return [b,g,r]


	# Color Quantization 수행
	def get_kcluster(self,K=8):
		self.IMAGES['KCLUSTER'] = self.IMAGES['ORIGINAL'].copy()
		Z = self.IMAGES['KCLUSTER'].reshape((-1,3))

		Z = np.float32(Z)
		criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER,10,1.0)
		ret, label, center = cv2.kmeans(Z,K,None,criteria,10,cv2.KMEANS_RANDOM_CENTERS)
		print("Extracted COLOR code : BGR ")
		print(center)
		print(np.bincount(label.flatten()))

		center = np.uint8(center)
		process_image = center[label.flatten()]
		self.IMAGES['KCLUSTER'] = process_image.reshape((self.IMAGES['KCLUSTER'].shape))

		# cv2.imshow(str(K) + " cluster_image", self.IMAGES['KCLUSTER'])
		# cv2.waitKey(0)
		# cv2.destroyAllWindows()

	def color_detection(self):

		(lower, upper) = self.BOUNDARIES
		# create NumPy arrays from the boundaries
		lower = np.array(lower, dtype = "uint8")
		upper = np.array(upper, dtype = "uint8")


		# find the colors within the specified boundaries and apply
		# the mask
		mask = cv2.inRange(self.IMAGES['KCLUSTER'], lower, upper)
		# print("Find Mask Range : " )
		# pprint(mask)

		# output = cv2.bitwise_and(self.IMAGES['ORIGINAL'], self.IMAGES['ORIGINAL'] , mask = mask)
		output = cv2.bitwise_and(self.IMAGES['ORIGINAL'], self.IMAGES['ORIGINAL'] , mask = mask)
		self.IMAGES['COLOR'] = output
		# show the images
		# cv2.imshow("images", np.hstack([self.IMAGES['ORIGINAL'],output]))
		# cv2.waitKey(0)




	# 주어진 이미지의 GRAY/CSIAN/CANNY/DILATE/ERODE 의 각 전처리를 수행한다.
	def transition_images(self):

		# 프로세싱처리가 필요한 오리지널 이미지 읽기
		# self.IMAGES['ORIGINAL'] = cv2.imread(self.ORIGINAL_FILE_PATH)
		self.IMAGES['CV_IMG'] = self.IMAGES['ORIGINAL'].copy()
		# self.IMAGES['GRAY_IMG'] = cv2.cvtColor(self.IMAGES['ORIGINAL'], cv2.COLOR_BGR2GRAY)
		# self.IMAGES['GRAY_IMG'] = cv2.cvtColor(self.IMAGES['COLOR'], cv2.COLOR_BGR2GRAY)
		self.IMAGES['GRAY_IMG'] = cv2.cvtColor(self.IMAGES['COLOR'], cv2.COLOR_BGR2GRAY)
		self.IMAGES['GAUSSIAN_IMG'] = cv2.GaussianBlur(self.IMAGES['GRAY_IMG'], (1,1),0)
		self.IMAGES['CANNY_IMG'] = cv2.Canny(self.IMAGES['GAUSSIAN_IMG'],30,120)
		self.IMAGES['DILATE_IMG'] = cv2.dilate(self.IMAGES['CANNY_IMG'],None,iterations=2)
		self.IMAGES['ERODE_IMG'] = cv2.erode(self.IMAGES['DILATE_IMG'],None,iterations=1)



	# 주어진 PX(픽셀)과 MM(밀리미터)의 비율을 반환한다.
	def calc_PxForMmRatio(self,px, mm):
		mm = float(mm)
		px = float(px)
		return mm/px

	# 이미지의 Contour 를 찾는다.
	def find_contours(self):
		copy_image = self.IMAGES['ERODE_IMG'].copy()
		f_contours = cv2.findContours(copy_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

		f_contours = f_contours[0] if imutils.is_cv2() else f_contours[1]

		if len(f_contours) == 0:
			# print("Print f_contour : " + str(len(f_contours)))
			self.save_images(debug_save=True)
			sys.exit()


		(f_contours,_) = contours.sort_contours(f_contours)

		detect_object_cnt = 0
		object_stack = []
		for (i,c) in enumerate(f_contours):
			box = cv2.minAreaRect(c)
			box = cv2.cv.BoxPoints(box) if imutils.is_cv2() else cv2.boxPoints(box)
			box = self.get_order_points(box)

			(tl,tr,br,bl) = box
			if dist.euclidean(tl,tr) < self.THRESHOLD_MINIMUM_SIDE or dist.euclidean(tl,bl) < self.THRESHOLD_MINIMUM_SIDE:
				continue
			if cv2.contourArea(c) < self.THRESHOLD_CONTOUR_AREA :
				continue
			elif cv2.contourArea(c) > self.THRESHOLD_CONTOUR_AREA_MAX :
				continue


			# 주어진 조건을 통과한 Contour 이미지 삽입
			cv2.drawContours(self.IMAGES['CV_IMG'], [box.astype("int")], -1, (144,43,120),2)
			# 박스의 각 모서리 위치의 좌표에 원형 이미지 삽입
			for i, (x,y) in enumerate(box):
				cv2.circle(self.IMAGES['CV_IMG'],(int(x), int(y)), 2, (0,0,255), -1)

			(tl,tr,br,bl) = box
			(tltrX, tltrY) = self.get_midpoint(tl,tr)
			(blbrX, blbrY) = self.get_midpoint(bl,br)
			(tlblX, tlblY) = self.get_midpoint(tl,bl)
			(trbrX, trbrY) = self.get_midpoint(tr,br)

			cv2.circle(self.IMAGES['CV_IMG'], (int(tltrX), int(tltrY)),3,(255,0,0),-1)
			cv2.circle(self.IMAGES['CV_IMG'], (int(blbrX), int(blbrY)),3,(255,0,0),-1)
			cv2.circle(self.IMAGES['CV_IMG'], (int(tlblX), int(tlblY)),3,(255,0,0),-1)
			cv2.circle(self.IMAGES['CV_IMG'], (int(trbrX), int(trbrY)),3,(255,0,0),-1)

			cv2.line(self.IMAGES['CV_IMG'], (int(tltrX),int(tltrY)), (int(blbrX),int(blbrY) ), (255,0,0),2)
			cv2.line(self.IMAGES['CV_IMG'], (int(tlblX), int(tlblY)),(int(trbrX), int(trbrY)), (255,0,0),2)

			# 측정된 PX 길이 측정 및 mm(밀리미터) 단위 변환
			dA = dist.euclidean((tltrX, tltrY), (blbrX, blbrY))
			dB = dist.euclidean((tlblX, tlblY), (trbrX, trbrY))

			# print(dA)
			# print(dB)
			# print('=========')
			mid_point_dot = tuple(map(lambda x: int(x), self.get_midpoint((tlblX, tlblY), (trbrX, trbrY))))

			cv2.putText(self.IMAGES['CV_IMG'], str(round(self.PX_MM_RATIO * dA,3)) + "mm", (mid_point_dot[0]-60,mid_point_dot[1]-60), cv2.FONT_HERSHEY_SIMPLEX,1, (51,134,255),2)
			cv2.putText(self.IMAGES['CV_IMG'], str(round(self.PX_MM_RATIO * dB,3)) + "mm", (mid_point_dot[0]+30,mid_point_dot[1]+30),cv2.FONT_HERSHEY_SIMPLEX,1, (51,134,255),2)


			detect_object_cnt+=1
			# print(str(detect_object_cnt) + "/" + str(round(self.PX_MM_RATIO * dA,3)) + "mm" + "/" + str(round(self.PX_MM_RATIO * dB,3)) + "mm")
			object_stack.append((round(self.PX_MM_RATIO * dA,3),round(self.PX_MM_RATIO * dB,3)))
			# cv2.putText(self.IMAGES['CV_IMG'], str(round(dA,2)) + "px", (mid_point_dot[0]-40,mid_point_dot[1]-40), cv2.FONT_HERSHEY_SIMPLEX,1, (102,255,255),3)
			# cv2.putText(self.IMAGES['CV_IMG'], str(round(dB,2)) + "px", (mid_point_dot[0]+40,mid_point_dot[1]+40),cv2.FONT_HERSHEY_SIMPLEX,1, (102,255,255),3)
		#print object stack which detected by algorithms
		# pprint(object_stack)


	# 최종작업을 완료하고 모든 이미지들을 저장한다.
	def save_images(self,debug_save=False):

		splt_image_file_name = self.ORIGINAL_FILE_PATH.split('.')
		if debug_save == False :
			cv2.imwrite(splt_image_file_name[0] + "_cv." + splt_image_file_name[1], self.IMAGES['CV_IMG'])
		elif debug_save == True :
			for (i,k) in enumerate(list(self.IMAGES)):
				cv2.imwrite(splt_image_file_name[0] + "_" + str(i)+"_"+str(k) + "." + splt_image_file_name[1], self.IMAGES[k])
		print ("Saved Images : " + splt_image_file_name[0] + "_" + str(i)+"_"+str(k) + "." + splt_image_file_name[1])

if __name__ == '__main__':
	pfc_cv_measurer = pfc_cv_measurer(coin_px=92, coin_mm=24, max_contours=10, opath="/Users/house/DEV/pfc_v2/cv/ex_imgs/20180405_162719.jpg",carea=200,carea_max=80000, min_side=35)
