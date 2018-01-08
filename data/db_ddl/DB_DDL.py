# -*- coding: utf-8 -*-
"""
Database Schema DDL(Data Definition Language)
Table List
1. PFC_model
2. model_meta
3. system_status
4. order
5. order_result
6. participant
7. participant_meta
8. actuator
9. actuator_activity
10. actuator_meta
11. sensor
12. sensor_meta
13. sensor_data
14. sensor_data_meta
15. image
16. image_process
"""
import time
import sys
import os
import MySQLdb


sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from configure import pfc_conf

class DB_DDL:
	TABLE_LSIT=(
		'PFC_model',
		'model_meta',
		'system_status',
		'order',
		'order_result',
		'participant',
		'participant_meta',
		'actuator',
		'actuator_activity',
		'actuator_meta',
		'sensor',
		'sensor_meta',
		'sensor_data',
		'sensor_data_meta',
		'image',
		'image_process'
	)
	def __init__(
			self,
			model_id=None,
			mac_addr=None,
			db_host=None,
			db_port=None,
			db_id=None,
			db_pw=None
		):
		None

	def create_PFC_model(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `PFC_model`
(
	model_id UNSIGNED INT(10) NOT NULL AUTO_INCREMENT,
	mac_address VARCHAR(255),
	PRIMARY KEY(model_id)
)
		"""
	def create_model_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `model_meta`
(
	meta_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	model_id UNSIGNED INT(10) NOT NULL,
	meta_key 	VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255),
	PRIMARY KEY(meta_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id)
)
		"""
	def create_system_status(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `system_status`
(
	status_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	model_id UNSIGNED INT(10) NOT NULL,
	cpu_usage INT(10),
	memory_usage INT(15),
	momory_avail INT(15),
	cron_status VARCHAR(255),
	ros_avail VARCHAR(255),
	brain_status VARCHAR(255),
	datetime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY(status_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id)
)
		"""
	def create_order(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `order`
(
	order_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	model_id UNSIGNED INT(10) NOT NULL,
	order VARCHAR(500) NOT NULL,
	order_type VARCHAR(255),
	participant_id UNSIGNED INT(10),
	datetime DATETIME DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY(order_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id),
	FOREIGN KEY(participant_id) REFERENCES participant(participant_id)
)
		"""
	def create_order_result(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `order_result`
(
	order_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	result VARCHAR(255),
	datetime DATETIME DEFAULT CURRENT_TIMESTAMP,
	FOREIGN KEY(order_id) REFERENCES order(order_id)
)
		"""
	def create_participant(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `participant`
(
	participant_id UNSIGNED INT(10) NOT NULL AUTO_INCREMENT,
	type VARCHAR(255),
	PRIMARY KEY(participant_id)
)
		"""
	def create_participant_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `participant_meta`
(
	meta_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	participant_id UNSIGNED INT(10) NOT NULL,
	meta_key VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255) NOT NULL,
	PRIMARY KEY(meta_id),
	FOREIGN KEY(participant_id) REFERENCES participant(participant_id)
)
		"""
	def create_actuator(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `actuator`
(
	actuator_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	model_id UNSIGNED INT(10) NOT NULL,
	PRIMARY KEY(actuator_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id)
)
		"""
	def create_actuator_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `actuator_meta`
(
	meta_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	actuator_id UNSIGNED INT(15) NOT NULL,
	meta_key VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255),
	PRIMARY KEY(meta_id),
	FOREIGN KEY(actuator_id) REFERENCES actuator(actuator_id)
)
		"""
	def create_actuator_activity(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `actuator_activity`
(
	activity_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	actuator_id UNSIGNED INT(15) NOT NULL,
	order_id UNSIGNED INT(15) NOT NULL,
	status VARCHAR(255),
	datetime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY(activity_id),
	FOREIGN KEY(actuator_id) REFERENCES actuator(actuator_id),
	FOREIGN KEY(order_id) REFERENCES order(order_id)
)
		"""
	def create_sensor(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `sensor`
(
	sensor_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	model_id UNSIGNED INT(10) NOT NULL,
	PRIMARY KEY(sensor_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id)
)
		"""
	def create_sensor_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `sensor_meta`
(
	meta_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	sensor_id UNSIGNED INT(15) NOT NULL,
	meta_key VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255),
	PRIMARY KEY(meta_id),
	FOREIGN KEY(sensor_id) REFERENCES sensor(sensor_id) NOT NULL
)
		"""
	def create_sensor_data(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `sensor_data`
(
	data_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	sensor_id UNSIGNED INT(15) NOT NULL,
	order_id UNSIGNED INT(15) NOT NULL,
	data_key VARCHAR(255) NOT NULL,
	data_value VARCHAR(255),
	datetime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY(data_id),
	FOREIGN KEY(sensor_id) REFERENCES sensor(sensor_id),
	FOREIGN KEY(order_id) REFERENCES order(order_id)
)
		"""
	def create_sensor_data_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `sensor_data_meta`
(
	meta_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	data_id UNSIGNED INT(15) NOT NULL,
	meta_key VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255),
	PRIMARY KEY(meta_id),
	FOREIGN KEY(data_id) REFERENCES sensor_data(data_id)
)
		"""
	def create_image(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `image`
(
	image_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	sensor_id UNSIGNED INT(15) NOT NULL,
	file_path VARCHAR(255) NOT NULL,
	datetime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY(image_id),
	FOREIGN KEY(sensor_id) REFERENCES sensor(sensor_id)
)
		"""
	def create_image_process(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `image_process`
(
	process_image_id UNSIGNED INT(15) NOT NULL AUTO_INCREMENT,
	image_id UNSIGNED INT(15) NOT NULL,
	process_type VARCHAR(55) NOT NULL,
	file_path VARCHAR(255) NOT NULL,
	datetime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY(process_image_id),
	FOREIGN KEY(image_id) REFERENCES image(image_id)
)
		"""
	def check_exists_table(self):
		None
	def create_all_db(self):
		None

	def create_dummy_db(self):
		None


if __name__ == '__main__':
	PFC_MODEL_ID = pfc_conf.PFC_MODEL_ID
	PFC_MAC_ADDR = pfc_conf.PFC_MAC_ADDR
	PFC_DB_HOST = pfc_conf.PFC_DB_HOST
	PFC_DB_PORT = pfc_conf.PFC_DB_PORT
	PFC_DB_ID = pfc_conf.PFC_DB_ID
	PFC_DB_PW = pfc_conf.PFC_DB_PW

	DB_DDL = DB_DDL(
										model_id=PFC_MODEL_ID,
										mac_addr=PFC_MAC_ADDR,
										db_host=PFC_DB_HOST,
										db_port=PFC_DB_PORT,
										db_id =PFC_DB_ID,
										db_pw =PFC_DB_PW
									)
	DB_DDL.check_exists_table()
	DB_DDL.create_all_db()


















